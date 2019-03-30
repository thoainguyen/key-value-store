#include "kvstore.h"
#include <stdio.h>
#include <pthread.h>

void *thread_set(void *args){
    int fd = connect_kvstore("127.0.0.1", 55000);
    char key[20], value[30];

    for(int i = 0; i < 100; i++){
        sprintf(key, "s_%d_key", i);
        sprintf(value, "v_%d_value",i);
        set_kvstore(fd,key,value);
    }

    // for(int i = 0; i < 100; i++){
    //     sprintf(key, "a_%d_key", i);
    //     printf("%s\n", del_kvstore(fd,key));
    // }

    // for(int i = 0; i < 100; i++){
    //     sprintf(key, "s_%d_key", i);
    //     sprintf(value, "v_%d_value",i);
    //     set_kvstore(fd,key,value);
    // }

    // for(int i = 0; i < 100; i++){
    //     sprintf(key, "a_%d_key", i);
    //     printf("%s\n", del_kvstore(fd,key));
    // }

    printf("exit thread 1");
    pthread_exit(NULL);
} 

void *thread_get(void *args){
    int fd = connect_kvstore("127.0.0.1", 55000);
    char key[20], value[30];
    for(int i = 0; i < 100; i++){
        sprintf(key, "s_%d_key", i);
        printf("`%s`\n", get_kvstore(fd,key));
    }
    printf("exit thread 2");
    pthread_exit(NULL);
}

void *thread_del(void *args){
    int fd = connect_kvstore("127.0.0.1", 55000);
    char key[20], value[30];
    for(int i = 0; i < 100; i++){
        sprintf(key, "s_%d_key", i);
        printf("%s\n", del_kvstore(fd,key));
    }

    // int fd = connect_kvstore("127.0.0.1", 55000);
    // char key[20], value[30];
    // for(int i = 0; i < 50; i++){
    //     sprintf(key, "s_%d_key", i);
    //     sprintf(value, "v_%d_value",i);
    //     set_kvstore(fd,key,value);
    // }

    // for(int i = 50; i < 100; i++){
    //     sprintf(key, "s_%d_key", i);
    //     printf("`%s` `%s`\n",key, get_kvstore(fd,key));
    // }

    printf("exit thread 3");
    pthread_exit(NULL);
}   

int main0(){
    pthread_t thread1;
    // pthread_t thread2;
    pthread_t thread3;
    

    pthread_create(&thread1, NULL, thread_set,NULL);
    // pthread_create(&thread2, NULL, thread_get,NULL);
    pthread_create(&thread3, NULL, thread_del,NULL);

    
    pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}