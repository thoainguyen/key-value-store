#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdbool.h> 
#include <fcntl.h>
#include <semaphore.h>

#include "btree.h"

#define STORAGE_FILE   "data.dat"
#define CONFIG_FILE    "meta.dat"

#define PORT 55000

B_tree *tree;
sem_t mutex, wrt;
int rcount = 0;

bool handler(char* resquest, char* response);

void init_server(){
	/* tree = create_empty_btree(STORAGE_FILE);*/
	tree = create_loading_btree(CONFIG_FILE);
	sem_init(&mutex,0,1);
  	sem_init(&wrt,0,1);
}

void close_server(){
	sem_destroy(&wrt);
	sem_destroy(&mutex);
	destroy_btree(tree, CONFIG_FILE);
}

int main(int argc, char *argv[])
{
	init_server();

	int server_fd, port = PORT,
		nbytes, client_fd, clientsock[10],
		max_client = 10, select_val,
		max_fd, i, fd, addrlen, new_sock;
	
	if(argc == 1){
		printf("(success) kvs server using port: %d\n", PORT);
	}
	else
	if(argc == 3 && !strcmp(argv[1],"-p")){
		if((port = atoi(argv[2])) > 0){
			printf("(success) kvs server using port: %d\n", port);
		}
		else {
			printf("(error) invalid port");
			close_server();
			return 1;
		}
	}
	else{
		printf("(error) using ./server or ./server -p <port>");
		close_server();
		return 1;
	}

	struct sockaddr_in server;

	char buffer[512]; 
	char sendmsg[512];

	fd_set rfds;

	for (i = 0; i < max_client; i++)
	{
		clientsock[i] = 0;
	}

	/*create socket*/
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd <= 0)
	{
		perror("(error) socket failed \n");
		exit(EXIT_FAILURE);
	}

	memset((char *)&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	int optval = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	/*binding the  socket*/
	if ((bind(server_fd, (struct sockaddr *)&server, sizeof(server))) < 0)
	{
		perror("(error) failed to bind \n");
		exit(EXIT_FAILURE);
	}

	/* listening state  function */
	if (listen(server_fd, 10) < 0)
	{
		perror("(error) failed to listen \n");
		exit(EXIT_FAILURE);
	}
	else
		printf("(success) server is ready...\n");

	fcntl(server_fd, F_SETFL, O_NONBLOCK);
		
	addrlen = sizeof(server);

	/* accept state function */
	while (1)
	{
		FD_ZERO(&rfds);
		FD_SET(server_fd, &rfds);

		max_fd = server_fd;

		for (i = 0; i < max_client; i++) //add new sockets to set rfds
		{
			fd = clientsock[i];
			if (fd > 0)
			{
				FD_SET(fd, &rfds);
			}
			if (fd > max_fd)
				max_fd = fd;
		}

		select_val = select(max_fd + 1, &rfds, NULL, NULL, NULL);

		if (select_val < 0)
			perror("(error) select fd error \n");

		if (FD_ISSET(server_fd, &rfds))
		{
			if ((new_sock = accept(server_fd, (struct sockaddr *)&server, (socklen_t *)&addrlen)) < 0)
			{
				perror("(error) accept failed");
				exit(EXIT_FAILURE);
			}
			printf("(connect) { sock_fd: %d, ip: %s, port: %d }\n" , new_sock , inet_ntoa(server.sin_addr) , ntohs(server.sin_port));
			for (i = 0; i < max_client; i++)
			{
				if (clientsock[i] == 0)
				{
					clientsock[i] = new_sock;
					break;
				}
			}
		}

		for (i = 0; i < max_client; i++)
		{
			fd = clientsock[i];

			if (FD_ISSET(fd, &rfds))
			{
				memset((char *)&buffer, 0, 512);
				memset((char *)&sendmsg, 0, 512);
				if ((nbytes = recv(fd, buffer, sizeof(buffer), 0)) == 0)
				{
					getpeername(fd, (struct sockaddr *)&server, (socklen_t *)&addrlen);
					printf("(disconn) { sock_fd: %d, ip: %s, port: %d } \n",
						fd, inet_ntoa(server.sin_addr), ntohs(server.sin_port));
					//close the socket using close call
					close(fd);
					clientsock[i] = 0;
				}
				// process on database and response back client
				else
				{
					buffer[nbytes] = '\0';

					handler(buffer, sendmsg);
					// printf("sendmsg: `%s`\n",sendmsg);
					sendmsg[strlen(sendmsg)] = '\n';
					send(fd, sendmsg, strlen(sendmsg), 0);
				}
			}
		}
	}

	close_server();
	close(server_fd);
	return 0;
}


bool handler(char* resquest, char* response){
	// printf("request: `%s`\n",resquest);
	char p1[5], p2[128], p3[256];
	sscanf(resquest, "%s %s %s", p1, p2, p3);
	// printf("param: `%s %s %s`\n",p1,p2,p3);
	if(!strcmp(p1,"set")){
		sem_wait(&wrt);
		insert0(tree, p2, p3);
  		sem_post(&wrt);
		strcpy(response,"OK");
		return 1;
	}
	else
	if(!strcmp(p1,"get"))
	{
		sem_wait(&mutex);
  		rcount = rcount + 1;
  		if (rcount == 1)
    		sem_wait(&wrt);
  		sem_post(&mutex);

		Kv_pair* ret = search(tree,p2);

  		sem_wait(&mutex);
  		rcount = rcount - 1;
  		if (rcount == 0)
    		sem_post(&wrt);
  		sem_post(&mutex);

		if(!ret){
			strcpy(response,"(nul)");
		}
		else{
			strcpy(response,ret->val);
		}
		return 1;
	}
	else
	if(!strcmp(p1,"del"))
	{
		char c = '0';
		sem_wait(&wrt);
		if(remove0(tree, p2)) c = '1';
		sem_post(&wrt);
		sprintf(response, "(integer) %c", c);
		return 1;
	}
	
	return 0;
}

