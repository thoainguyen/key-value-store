#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifndef KVSTORE_H
#define KVSTORE_H

#define BUF_LEN 100


int  connect_kvstore(char*,char*);
void close_kvstore(int);
bool kvstore_execute(int,char*);

char *kvstore_get(int,char*);
char *kvstore_delete(int,char*);
char *kvstore_set(int,char*,char*);


#endif