#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "bnode.h"

#define FNAME_LENGTH 20

#define GT(a,b) strcmp((a),(b)) > 0
#define EQ(a,b) strcmp((a),(b)) == 0
#define LT(a,b) strcmp((a),(b)) < 0

typedef struct
{
    int  root_pos;
    int  next_pos;
    FILE *fp;
    char file_name[FNAME_LENGTH];
} B_tree;


B_tree *create_empty_btree(char *storage);
B_tree *create_loading_btree(char *config);
void destroy_btree(B_tree *tree, char* config);
int  find_key(B_node *node, key_type *k);
void split_child(B_tree *tree, B_node *x, int i, B_node *y);
void insert_non_full(B_tree *tree, B_node *node, Kv_pair *record);
void insert(B_tree *tree, Kv_pair *record);
void insert0(B_tree *btree, key_type *key, val_type *val);
void traverse(B_tree *tree, int root);
Kv_pair *search(B_tree *tree, key_type *key);
Kv_pair *recursive_search(B_tree *tree, key_type *key, B_node *root);
bool remove0(B_tree *tree, key_type *key);
void merge(B_tree *tree, B_node *node, int idx);
void borrow_from_next(B_tree *tree, B_node *node, int idx);
void borrow_from_prev(B_tree *tree, B_node *node, int idx);
void fill(B_tree *tree, B_node *node, int idx);
Kv_pair *get_successor(B_tree *tree, B_node *node, int idx);
Kv_pair *get_predecessor(B_tree *tree, B_node *node, int idx);
bool remove_from_non_leaf(B_tree *tree, B_node *node, int idx);
bool remove_from_leaf(B_tree *tree, B_node *node, int idx);
bool remove_bnode(B_tree *tree, B_node *node, key_type *k);

void create_bnode(B_node *node, bool is_leaf, B_tree *tree);

#endif