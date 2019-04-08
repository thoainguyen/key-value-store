#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifndef BUGDB_H
#define BUGDB_H

#define BUF_LEN 100


int  connect_bugdb(char*,char*);
void close_bugdb(int);
bool process_bugdb(int,char*);

char *get_bugdb(int,char*);
char *del_bugdb(int,char*);
char *set_bugdb(int,char*,char*);


#endif