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

bool checkvalid(char* str);

int main(int args, char *argv[])
{
	int peer_fd, nbytes;
	struct sockaddr_in peer;
	struct hostent *server_addr;
	char buffer[512];

	if ((peer_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket failed \n");
		close(peer_fd);
		exit(1);
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons(55000);

	server_addr = gethostbyname("127.0.0.1"); //localhost

	memcpy(&peer.sin_addr, server_addr->h_addr, server_addr->h_length);

	if (connect(peer_fd, (struct sockaddr *)&peer, sizeof(peer)) < 0)
	{
		perror("connect failed\n");
		close(peer_fd);
		exit(1);
	}

	while (1)
	{
		// printf("enter the message you want to send\nsend: ");
		memset((char *)&buffer, 0, 512);
		
		printf("127.0.0.1:55000> ");
			
		fgets(buffer, 512, stdin);
		
		if(buffer[strlen(buffer)-1]=='\n'){
			buffer[strlen(buffer)-1]='\0';
		}

		if(!strlen(buffer) || !checkvalid(buffer)){
			continue;
		}

		if ((nbytes = send(peer_fd, buffer, sizeof(buffer), 0)) <= 0)
		{
			perror("sending failed \n");
			close(peer_fd);
			exit(1);
		}

		if ((nbytes = recv(peer_fd, buffer, 512, 0)) < 0){
			perror("receive failed");
		}
		else{
			buffer[strcspn(buffer, "\n")] = 0;
			printf("%s\n", buffer);
		}
	}

	close(peer_fd);
	return 0;
}


bool checkvalid(char* str){

	char *new = (char*)malloc(sizeof(char)*strlen(str));
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