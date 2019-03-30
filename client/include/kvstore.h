#ifndef KVSTORE_H
#define KVSTORE_H

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

int  connect_kvstore(char*,int);
void close_kvstore(int);
bool process_kvstore(int,char*);

char *get_kvstore(int, char*);
char *del_kvstore(int, char*);
char *set_kvstore(int, char*,char*);

#endif