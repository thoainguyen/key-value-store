#include "bnode.h"

void print_bnode(B_node *node)
{
    printf("Pos %d : NumRecord: %d ", node->position, node->num_records);
    for (int i = 0; i < node->num_records; i++)
    {
        printf(" KVPair(%s,%s) ", node->records[i].key, node->records[i].val);
    }
    printf("Child : ");
    for (int i = 0; i <= node->num_records; i++)
    {
        printf("%d ", node->child[i]);
    }
    printf("\n");
}