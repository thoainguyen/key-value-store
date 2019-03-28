#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>

#include "kvstore.h"

#define PORT 55000;

bool checkvalid(char*);

int main0(int argc, char *argv[])
{
	int peer_fd, nbytes,
		ser_port = PORT;
	bool ret;
	char buffer[512];
	char addr_name[20] = "127.0.0.1";

	if(argc == 3 || argc == 5)
	{
		if(!strcmp(argv[1],"-p")){
			if((ser_port = atoi(argv[2])) <= 0){
				perror("(error) invalid port");
				return 1;
			}
		}
		else{
			perror("(error) invalid syntax");
			return 1;
		}
		if(argc > 3){
			if(!strcmp(argv[3], "-h")){
				strcpy(addr_name, argv[4]);
			}
			else{
				perror("(error) invalid syntax");
				return 1;
			}
		}
	}
	else
	if(argc != 1){
		perror("(error) using ./client or ./client [-p <port>] [-h <host>]");
		return 1;
	}
	peer_fd = connect_kvstore(addr_name, ser_port);
	while (1)
	{
		// printf("enter the message you want to send\nsend: ");
		memset((char *)&buffer, 0, 512);
		
		printf("%s:%d> ", addr_name, ser_port);
			
		fgets(buffer, 512, stdin);
		
		if(!checkvalid(buffer)) {
			continue;
		};

		if(!process_kvstore(peer_fd, buffer)){
			continue;
		};
		
		printf("%s\n",buffer);
	}
	close(peer_fd);
	return 0;
}


bool checkvalid(char* str){

	char *new = (char*)malloc(512);
	strcpy(new, str);

	char delim[] = " ";
	char *ptr = strtok(new, delim);

	char p1[30]; int n = 0;
	strcpy(p1, ptr);

	if(strcmp(p1,"set") != 0 && strcmp(p1,"get") != 0 && strcmp(p1,"del") != 0){
		printf("(error) ERR unknown command `%s`, with args beginning with:", p1);
		while(ptr != NULL)
		{
			printf(" '%s',", ptr);
			ptr = strtok(NULL, delim);
		}
		printf("\n");
		return false;
	}
	else{
		while(ptr != NULL){ ptr = strtok(NULL, delim); n++;};
		if (!strcmp(p1,"set") && n != 3){
			printf("(error) ERR wrong number of arguments for 'set' command\n");
			return false;
		}
		if (!strcmp(p1,"get") && n != 2){
			printf("(error) ERR wrong number of arguments for 'get' command\n");
			return false;
		} 
		if (!strcmp(p1,"del") && n != 2){
			printf("(error) ERR wrong number of arguments for 'del' command\n");
			return false;
		} 
	}
	return true;
}

