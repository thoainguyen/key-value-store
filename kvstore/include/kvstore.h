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
bool process_kvstore(int,char*);

char *get_kvstore(int,char*);
char *del_kvstore(int,char*);
char *set_kvstore(int,char*,char*);


#endif