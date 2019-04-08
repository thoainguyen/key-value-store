#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "thpool.h"
#include "btree.h"

#ifndef SERVER_H
#define SERVER_H

#define BUF_LEN 100
#define ARG_L 5
#define KEY_L 40
#define VAL_L 50
#define BACKLOG 10

#define THREAD 4
#define SIZE   8192

#define STORAGE_FILE  "data.dat"
#define CONFIG_FILE   "meta.dat"

typedef struct param{
    int fd;
    char* buf;
} param_t;

void handle_client(void* args);

param_t* init_param(int fd, char* buf){
    param_t* p = (param_t*)malloc(sizeof(param_t));
    p->fd = fd;
    p->buf = buf;
    return p;
}

char *process_request(char *);
void thread_safe_insert(B_tree *, char *, char *, char *);
void thread_safe_search(B_tree *, char *, char *);
void thread_safe_delete(B_tree *, char *, char *);

void signal_handler(int);
void init_server(bool);
void close_server();

#endif