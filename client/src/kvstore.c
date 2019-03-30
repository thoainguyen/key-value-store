#include "kvstore.h"

#include <stdio.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h>

static pthread_mutex_t lock;


int connect_kvstore(char* addr_name, int ser_port){
	int peer_fd;
	struct sockaddr_in peer;
	struct hostent *server_addr;

	pthread_mutex_init(&lock, NULL);

	if ((peer_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("(error) socket failed \n");
		close_kvstore(peer_fd);
		exit(1);
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons(ser_port);

	server_addr = gethostbyname(addr_name); // default localhost

	memcpy(&peer.sin_addr, server_addr->h_addr, server_addr->h_length);

	if (connect(peer_fd, (struct sockaddr *)&peer, sizeof(peer)) < 0)
	{
		perror("(error) connect failed, can't connect to server");
		close_kvstore(peer_fd);
		exit(1);
	}	
	printf("(success) connect to kvs on address: %s and port: %d\n", addr_name, ser_port);	
	return peer_fd;
}




bool process_kvstore(int peer_fd, char* buf){
	
	pthread_mutex_lock(&lock);
	if(buf[strlen(buf)-1]=='\n'){
		buf[strlen(buf)-1]='\0';
	}
	if(!strlen(buf)){
		pthread_mutex_unlock(&lock);
		return false;
	}
	if (send(peer_fd, buf, 512, 0) <= 0)
	{
		perror("(error) sending failed \n");
		pthread_mutex_unlock(&lock);
		close_kvstore(peer_fd);
		exit(1);
	}
	if (recv(peer_fd, buf, 512, 0) < 0){
		perror("(error) receive failed");
	} 
	else{
		buf[strcspn(buf, "\n")] = 0;
	}
	pthread_mutex_unlock(&lock);
	return true;
}

void close_kvstore(int fd){
	pthread_mutex_destroy(&lock);
	close(fd);
}

char* get_kvstore(int fd, char* key){
	char* result = (char*)malloc(512);
	sprintf(result, "get %s", key);
	if(process_kvstore(fd, result)){
		return result;
	}
	free(result);
	return NULL;
}

char* del_kvstore(int fd,char* key){
	
	char* result = (char*)malloc(512);
	sprintf(result, "del %s", key);
	if(process_kvstore(fd, result)){
		return result;
	}
	free(result);
	
	return NULL;
}

char* set_kvstore(int fd, char* key, char* val){
	
	char* result = (char*)malloc(512);
	sprintf(result, "set %s %s", key, val);
	if(process_kvstore(fd, result)){
		return result;
	}
	free(result);
	return NULL;
}
