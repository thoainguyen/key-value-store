
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include "btree.h"

static pthread_mutex_t mutex;
static pthread_mutex_t wrt;
static int rcount = 0;

#define STORAGE_FILE "data.dat"
#define CONFIG_FILE "meta.dat"
#define DATASET_SIZE 100000000

int from = 0;
int to = 0;

B_tree *tree;

void* test_set(void* args)
{
    char key[20], value[30];
    for (int i = 1; i <= DATASET_SIZE; i++)
    {
        sprintf(key, "%08d", i);
        sprintf(value, "v_%05d_value", i);
        // printf("set %s - %s\n", key, value);
        pthread_mutex_lock(&wrt);
        // printf("---\nb_s\n");
        insert0(tree, key, value);
        // printf("e_s\n---\n");
        pthread_mutex_unlock(&wrt);
    }
}

void* test_del(void* args)
{
    char key[20], value[30];
    for (int i = from; i <= to; i++)
    {
        sprintf(key, "%08d", i);
        // printf("Delete %s\n", key);
        pthread_mutex_lock(&wrt);
        // printf("---\nb_d\n");
        remove0(tree, key);
        // printf("e_d\n---\n");
        pthread_mutex_unlock(&wrt);
    }
}

void* test_get(void *args)
{
    char key[20], value[30];
    for (int i = 1; i <= DATASET_SIZE; i++)
    {
        sprintf(key, "%08d", i);
        pthread_mutex_lock(&mutex);
        rcount = rcount + 1;
        if (rcount == 1)
            pthread_mutex_lock(&wrt);
        pthread_mutex_unlock(&mutex);

        // printf("---\nb_g\n");
        Kv_pair *result = search(tree, key);
        if (result != NULL)
            printf("`%s`\n", result->val);
        else
            printf("`(nul)`\n");
        // printf("e_g\n---\n");
        
        pthread_mutex_lock(&mutex);
        rcount = rcount - 1;
        if (rcount == 0)
            pthread_mutex_unlock(&wrt);
        ;
        pthread_mutex_unlock(&mutex);
    }
    // test_trav();
}

void test_trav()
{
    traverse(tree, tree->root_pos);
}

void signal_handler(int num)
{
  destroy_btree(tree, CONFIG_FILE);
  exit(num);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, signal_handler);

    // printf("%d",sizeof(B_node));
    // tree = create_empty_btree(STORAGE_FILE);
    tree = create_loading_btree(CONFIG_FILE);
    
    pthread_t th_set;
    pthread_t th_get;
    pthread_t th_del;

    char op[3];

    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&wrt,NULL);

    

    strcpy(op,argv[1]);

    

    if(!strcmp(op,"s")){
        pthread_create(&th_set, NULL, test_set,NULL);
        pthread_join(th_set, NULL);
    }
    else if (!strcmp(op, "g"))
    {
        pthread_create(&th_get, NULL, test_get,NULL);
        pthread_join(th_get, NULL);
    }
    else if(!strcmp(op,"d")){
        from = atoi(argv[2]);
        to = atoi(argv[3]);
        pthread_create(&th_del, NULL, test_del,NULL);
        pthread_join(th_del, NULL);
    }
    else if (!strcmp(op, "sg"))
    {
        pthread_create(&th_set, NULL, test_set,NULL);
        pthread_create(&th_get, NULL, test_get,NULL);
        pthread_join(th_set, NULL);
        pthread_join(th_get, NULL);
    }
    else if (!strcmp(op,"sd"))
    {
        pthread_create(&th_set, NULL, test_set,NULL);
        pthread_create(&th_del, NULL, test_del,NULL);
        pthread_join(th_set, NULL);
        pthread_join(th_del, NULL);
    }
    else if (!strcmp(op,"gd"))
    {
        pthread_create(&th_get, NULL, test_get,NULL);
        pthread_create(&th_del, NULL, test_del,NULL);
        pthread_join(th_get, NULL);
        pthread_join(th_del, NULL);
    }
    else if(!strcmp(op,"sgd")){
        pthread_create(&th_set, NULL, test_set,NULL);
        pthread_create(&th_get, NULL, test_get,NULL);
        pthread_create(&th_del, NULL, test_del,NULL);
        pthread_join(th_set, NULL);
        pthread_join(th_get, NULL);
        pthread_join(th_del, NULL);
    }
    else
    {
        printf("invalid");
    }

    destroy_btree(tree, CONFIG_FILE);

    return 0;
}
