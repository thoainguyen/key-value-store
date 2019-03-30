#include "kvstore.h"

#include <stdio.h>
#include <pthread.h>

void *thread_run1(void *args){
    int fd = *(int*)args;
    char key[20], value[30];
    for(int i = 0; i < 100; i++){
        sprintf(key, "a_%d_key", i);
        sprintf(value, "a_%d_value",i);
        set_kvstore(fd,key,value);
    }
} 

void *thread_run2(void *args){
    int fd = *(int*)args;
    char key[20], value[30];
    for(int i = 0; i < 100; i++){
        sprintf(key, "a_%d_key", i);
        printf("%s\n", get_kvstore(fd,key));
    }
}   

int main1(){
    pthread_t thread1;
    pthread_t thread2;
    int fd = connect_kvstore("127.0.0.1", 55000);

    pthread_create(&thread1, NULL, thread_run1, (void*)&fd);
    pthread_create(&thread2, NULL, thread_run2, (void*)&fd);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    close_kvstore(fd);
    return 0;
}