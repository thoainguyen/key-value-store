
// #include <stdio.h>
// #include <pthread.h>

// #include "btree.h"

// static pthread_mutex_t mutex;
// static pthread_mutex_t wrt;

// static int rcount = 0;

// extern int n_ss;

// #define STORAGE_FILE "data.dat"
// #define CONFIG_FILE "meta.dat"
// #define DATASET_SIZE 1000

// B_tree *tree;

// void *thread_set(void *args)
// {
//     B_tree *fd = (B_tree *)args;
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {

//         sprintf(key, "%d", i);
//         sprintf(value, "v_%d_value", i);
//         printf("set %s - %s\n", key, value);
//         pthread_mutex_lock(&wrt);
//         insert0(fd, key, value);
//         pthread_mutex_unlock(&wrt);
//     }
// }

// void *thread_get(void *args)
// {
//     B_tree *fd = (B_tree *)args;
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {
//         sprintf(key, "%d", i);
//         pthread_mutex_lock(&mutex);
//         rcount = rcount + 1;
//         if (rcount == 1)
//             pthread_mutex_lock(&wrt);
//         pthread_mutex_unlock(&mutex);

//         Kv_pair *result = search(tree, key);
//         if (!result)
//             printf("`%s`\n", result->val);

//         pthread_mutex_lock(&mutex);
//         rcount = rcount - 1;
//         if (rcount == 0)
//             pthread_mutex_unlock(&wrt);
//         ;
//         pthread_mutex_unlock(&mutex);
//     }
// }

// void *thread_del(void *args)
// {
//     B_tree *fd = (B_tree *)args;
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {
//         sprintf(key, "%d", i);
//         printf("Delete %s\n", key);
//         pthread_mutex_lock(&wrt);
//         remove0(fd, key);
//         pthread_mutex_unlock(&wrt);
//     }
// }

// void test_set()
// {
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {
//         sprintf(key, "%d", i);
//         sprintf(value, "v_%d_value", i);
//         printf("set %s - %s\n", key, value);
//         pthread_mutex_lock(&wrt);
//         insert0(tree, key, value);
//         pthread_mutex_unlock(&wrt);
//     }
// }

// void test_del()
// {
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {
//         sprintf(key, "%d", i);
//         printf("Delete %s\n", key);
//         pthread_mutex_lock(&wrt);
//         remove0(tree, key);
//         pthread_mutex_unlock(&wrt);
//     }
// }

// void test_get()
// {
//     char key[20], value[30];
//     for (int i = 1; i <= DATASET_SIZE; i++)
//     {
//         sprintf(key, "%d", i);
//         pthread_mutex_lock(&mutex);
//         rcount = rcount + 1;
//         if (rcount == 1)
//             pthread_mutex_lock(&wrt);
//         pthread_mutex_unlock(&mutex);

//         Kv_pair *result = search(tree, key);
//         if (!result)
//             printf("`%s`\n", result->val);

//         pthread_mutex_lock(&mutex);
//         rcount = rcount - 1;
//         if (rcount == 0)
//             pthread_mutex_unlock(&wrt);
//         ;
//         pthread_mutex_unlock(&mutex);
//     }
// }

// void test_trav()
// {
//     traverse(tree, tree->root_pos);
// }

// int main(int argc, char *argv[])
// {

//     if (argc < 2)
//     {
//         printf("(error) miss parameter");
//         exit(EXIT_FAILURE);
//     }

//     if (strcmp(argv[1], "-s") == 0)
//     {
//         tree = create_empty_btree(STORAGE_FILE);
//         test_set();
//     }
//     else if (strcmp(argv[1], "-d") == 0)
//     {
//         tree = create_loading_btree(CONFIG_FILE);
//         test_del();
//     }
//     else if (strcmp(argv[1], "-r") == 0)
//     {
//         tree = create_loading_btree(CONFIG_FILE);
//         test_trav();
//         printf("%d",n_ss);
//     }

//     destroy_btree(tree, CONFIG_FILE);

//     return 0;
// }
