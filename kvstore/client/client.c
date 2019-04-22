#include <stdio.h>
#include "bugdb.h"

#define HOST "127.0.0.1"
#define PORT "9899"
#define DATASET 100

int validate_command(char*);
void user_test();
void test_insert();
void test_delete();
void test_search();


int main(int argc, char *argv[])
{
	char mode[5];
	if(argc == 2 && !strcmp(argv[1],"usertest")){
		user_test();
	}
	else if(argc == 2 && !strcmp(argv[1],"autotest")){
		printf("choose mode for test ...\n");
test:	printf("test insert: -i\n");
		printf("test delete: -d\n");
		printf("test search: -s\n");
		printf("choose: ");
	
		fgets(mode, 100 ,stdin);
		mode[strcspn(mode, "\n")] = 0;


		if(!strcmp(mode, "-i")){
			test_insert();
		}
		else if(!strcmp(mode, "-d")){
			test_delete();
		}
		else if(!strcmp(mode, "-s")){
			test_search();
		}
		else{
			printf("invalid command\n");
			goto test;
		}
	}
	else{
		printf("using `./client usertest` or `./client autotest`\n");
		exit(0);
	}
	return 0;
}


int validate_command(char *str){
	
	if(!strlen(str)) return 0;

	char *new = (char *)malloc(BUF_LEN);
	strcpy(new, str);

	char delim[] = " ";
	char *ptr = strtok(new, delim);

	char p1[30];
	int n = 0;
	strcpy(p1, ptr);

	if (strcmp(p1, "set") != 0 && strcmp(p1, "get") != 0 && strcmp(p1, "del") != 0)
	{
		printf("(error) ERR unknown command `%s`, with args beginning with:", p1);
		while (ptr != NULL)
		{
			printf(" '%s',", ptr);
			ptr = strtok(NULL, delim);
		}
		printf("\n");
		return 0;
	}
	else
	{
		while (ptr != NULL)
		{
			ptr = strtok(NULL, delim);
			n++;
		};
		if (!strcmp(p1, "set") && n != 3)
		{
			printf("(error) ERR wrong number of arguments for 'set' command\n");
			return 0;
		}
		if (!strcmp(p1, "get") && n != 2)
		{
			printf("(error) ERR wrong number of arguments for 'get' command\n");
			return 0;
		}
		if (!strcmp(p1, "del") && n != 2)
		{
			printf("(error) ERR wrong number of arguments for 'del' command\n");
			return 0;
		}
	}
	return 1;
}



void user_test(){
	int sockfd;
	char buf[BUF_LEN];

	if((sockfd = connect_bugdb(HOST, PORT)) < 0){
		printf("(error) failed to connect bugdb\n");
	}

    while(1){
        printf("%s:%s> ", HOST, PORT);
		fgets(buf, BUF_LEN ,stdin);
		buf[strcspn(buf, "\n")] = 0;

		if(!validate_command(buf)){
			continue;
		}
		if(!process_bugdb(sockfd, buf)){
			exit(1);
		}
		printf("%s\n",buf);
	}
	close(sockfd);
}

void test_insert(){
	char buf[100];
	int sockfd;
	if((sockfd = connect_bugdb(HOST, PORT)) < 0){
		printf("(error) failed to connect bugdb\n");
	}
	for(int i = 0; i < DATASET; i ++ ){
		sprintf(buf, "set %05d_key %05d_value", i, i);
		printf("request: %s\n", buf);
		if(process_bugdb(sockfd, buf)){
			printf("response: %s\n",buf);
		}
	}
}
void test_delete(){
	char buf[100];
	int sockfd;
	if((sockfd = connect_bugdb(HOST, PORT)) < 0){
		printf("(error) failed to connect bugdb\n");
	}
	for(int i = 0; i < DATASET; i ++ ){
		sprintf(buf, "del %05d_key", i);
		printf("request: %s\n", buf);
		if(process_bugdb(sockfd, buf)){
			printf("response: %s\n",buf);
		}
	}
}
void test_search(){
	char buf[100];
	int sockfd;
	if((sockfd = connect_bugdb(HOST, PORT)) < 0){
		printf("(error) failed to connect bugdb\n");
	}
	for(int i = 0; i < DATASET; i ++ ){
		sprintf(buf, "get %05d_key", i);
		printf("request: %s\n", buf);
		if(process_bugdb(sockfd, buf)){
			printf("response: %s\n",buf);
		}
	}
}