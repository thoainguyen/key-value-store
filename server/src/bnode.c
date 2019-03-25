#include "bnode.h"


void print_bnode(B_node * node){
    printf("Record : ");
    for(int i = 0; i < node->num_records; i++) {
        printf(" KVPair(%s,%s)", node->records[i].key, node->records[i].val);
    }
    printf("\n Pos: %d Child : ",node->position);
    for(int i = 0; i <= node->num_records; i++) {
        printf("%d ", node->children[i]);
    }
    printf("\n");
}