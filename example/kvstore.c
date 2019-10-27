#include <stdio.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdbool.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#include "kvstore.h"

static pthread_mutex_t lock;

/* get sockaddr, IPv4 or IPv6 */
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int connect_kvstore(char* hostname, char* port){
	int sockfd, rv;  
	struct addrinfo hints, *servinfo, *p;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "(error) getaddrinfo %s\n", gai_strerror(rv));
		return -1;
	}

	/*loop through all the results and connect to the first we can*/
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("(error) socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("(error) connect");
			close(sockfd);
			continue;
		}
		break;
	}

	if (p == NULL) {
		perror("(error) failed to connect");
		return -1;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	/* printf("(success) connecting to %s\n", s); */
	freeaddrinfo(servinfo);
	pthread_mutex_init(&lock, NULL);
	return sockfd;
}


bool kvstore_execute(int sockfd, char* buf){
	int nbytes = 0;
	bool ret = true;
	pthread_mutex_lock(&lock);
	if ((nbytes = send(sockfd, buf, BUF_LEN*sizeof(char), 0)) <= 0){
        perror("(error) send");
        close(sockfd);
	    ret = false;
    }
    else if ((nbytes = recv(sockfd, buf, BUF_LEN*sizeof(char), 0)) <= 0) {
	    perror("(error) recv");
        close(sockfd);
	    ret = false;;
    }
    else{
        buf[nbytes] = '\0';
		ret = true;
    }
	pthread_mutex_unlock(&lock);
	return ret;
}

void close_kvstore(int fd){
	pthread_mutex_destroy(&lock);
	close(fd);
}

char* kvstore_get(int fd, char* key){
	char *buf = (char*)malloc(BUF_LEN*sizeof(char));
	sprintf(buf, "get %s", key);
	if(!kvstore_execute(fd, buf)){
		free(buf);
		buf = NULL;
	}
	return buf;
}

char* kvstore_delete(int fd, char* key){
	char *buf = (char*)malloc(BUF_LEN*sizeof(char));
	sprintf(buf, "del %s", key);
	if(!kvstore_execute(fd, buf)){
		free(buf);
		buf = NULL;
	}
	return buf;
}

char* kvstore_set(int fd, char* key, char* value){
	char *buf = (char*)malloc(BUF_LEN*sizeof(char));
	sprintf(buf, "set %s %s", key, value);
	if(!kvstore_execute(fd, buf)){
		free(buf);
		buf = NULL;
	}
	return buf;
}
