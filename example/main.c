#include <stdio.h>
#include "kvstore.h"

#define HOST "127.0.0.1"
#define PORT "9899"


int main(int argc, char** argv){
    
    int db = connect_kvstore(HOST, PORT);
    if (db < 0){
        return -1;
    }
    char *result = kvstore_set(db, "a", "A");
    printf("Set\t{key: \"%s\", value: \"%s\"}\n-> \"%s\"\n", "a", "A", result);

    char *value = kvstore_get(db, "a");
    printf("Get\t{key: \"%s\"}\n-> \"%s\"\n", "a", value);
    
    result = kvstore_delete(db, "a");
    printf("Delete\t{key: \"%s\"}\n-> \"%s\"\n", "a",  result);
    
    
    free(value);
    free(result);
    
    close_kvstore(db);
    return 0;
}