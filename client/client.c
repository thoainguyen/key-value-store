#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

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
		
		if(!strcmp(buffer,"\n")){
			continue;
		}
			

		if (send(peer_fd, buffer, sizeof(buffer), 0) < 0)
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
