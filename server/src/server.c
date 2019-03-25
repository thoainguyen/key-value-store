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

#include "btree.h"

#define STORAGE_FILE   "data.dat"
#define CONFIG_FILE    "meta.dat"


#define PORT 55000

B_tree *tree;

bool handler(char* resquest, char* response);

int main(int argc, char *argv[])
{
	tree = create_loading_btree(CONFIG_FILE);
	int server_fd,
		nbytes, client_fd, clientsock[10],
		max_client = 10, select_val,
		max_fd, i, fd, addrlen, new_sock;

	struct sockaddr_in server;
	char buffer[512]; 
	char sendmsg[512];

	fd_set rfds, wfds;

	for (i = 0; i < max_client; i++)
	{
		clientsock[i] = 0;
	}

	/*create socket*/
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd <= 0)
	{
		perror("socket failed \n");
		exit(EXIT_FAILURE);
	}

	memset((char *)&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	/*binding the  socket*/
	if ((bind(server_fd, (struct sockaddr *)&server, sizeof(server))) < 0)
	{
		perror("failed to bind \n");
		exit(EXIT_FAILURE);
	}

	/* listening state  function */
	if (listen(server_fd, 7) < 0)
	{
		perror("failed to listen \n");
		exit(EXIT_FAILURE);
	}
	else
		printf("server is ready \n");
		
	addrlen = sizeof(server);

	/* accept state function */
	while (1)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(server_fd, &rfds);
		FD_SET(server_fd, &wfds);

		max_fd = server_fd;

		for (i = 0; i < max_client; i++) //add new sockets to set rfds
		{
			fd = clientsock[i];
			if (fd > 0)
			{
				FD_SET(fd, &rfds);
				// FD_SET(fd, &wfds);
			}
			if (fd > max_fd)
				max_fd = fd;
		}

		select_val = select(max_fd + 1, &rfds, NULL/*&wfds*/, NULL, NULL);

		if (select_val < 0)
			perror("select error \n");

		if (FD_ISSET(server_fd, &rfds))
		{
			if ((new_sock = accept(server_fd, (struct sockaddr *)&server, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept failed");
				exit(EXIT_FAILURE);
			}
			printf("new_connection = { sock_fd : %d, ip : %s, port : %d }\n" , new_sock , inet_ntoa(server.sin_addr) , ntohs(server.sin_port));
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

				if ((nbytes = recv(fd, buffer, sizeof(buffer), 0)) == 0)
				{
					getpeername(fd, (struct sockaddr *)&server, (socklen_t *)&addrlen);
					printf("disconnected_client = { ip: %s, port: %d } \n",
						inet_ntoa(server.sin_addr), ntohs(server.sin_port));
					//close the socket using close call
					close(fd);
					clientsock[i] = 0;
				}
				//echo back the message that came in
				else
				{
					buffer[nbytes] = '\0';
					handler(buffer, sendmsg);
					sendmsg[strlen(sendmsg)] = '\n';
					send(fd, sendmsg, strlen(sendmsg), 0);
				}
			}
		}
	}

	destroy_btree(tree, CONFIG_FILE);
	close(server_fd);
	return 0;
}


bool handler(char* resquest, char* response){
	char p1[5], p2[128], p3[256];
	sscanf(resquest, "%s %s %s", p1, p2, p3);
	if(!strcmp(p1,"set")){
		insert0(tree, p2, p3);
		strcpy(response,"OK");
		return 1;
	}
	else
	if(!strcmp(p1,"get"))
	{
		Kv_pair* ret = search(tree,p2);
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
		if(remove0(tree, p2)) c = '1';
		sprintf(response, "(integer) %c", c);
		return 1;
	}
	return 0;
}