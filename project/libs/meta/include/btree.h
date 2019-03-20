#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "bnode.h"


#define GT(a,b) strcmp((a),(b)) > 0
#define EQ(a,b) strcmp((a),(b)) == 0
#define LT(a,b) strcmp((a),(b)) < 0

typedef struct
{
    char fname[20];
    FILE *fp;
    int root;
    int nextPos;
} BTree;

// BNODE FUNCTION HEADER

void BNodeInit(BNode *node, bool isLeaf, BTree *tree);

void writeFile(BTree* t, BNode* p, int pos);
void readFile(BTree* t, BNode* p, int pos);

void dispNode(BNode * node);

// BTREE FUNCTION HEADER

BTree *BTreeInit(char *fname, bool mode);

void BTreeDestroy(BTree *tree);
void splitChild(BTree *tree, BNode *x, int i, BNode *y);

void insertNonFull(BTree *tree, BNode *node, KVPair *record);
void insert(BTree *tree, KVPair *record);
void traverse(BTree *tree, int root);
KVPair *search(BTree *tree, keyType *key);
KVPair *searchRecursive(BTree *tree, keyType *key, BNode *root);
bool removeFromTree(BTree *tree, keyType *key);
void merge(BTree *tree, BNode *node, int idx);
void borrowFromNext(BTree *tree, BNode *node, int idx);
void borrowFromPrev(BTree *tree, BNode *node, int idx);
void fill(BTree *tree, BNode *node, int idx);
KVPair *getSucc(BTree *tree, BNode *node, int idx);
KVPair *getPred(BTree *tree, BNode *node, int idx);
bool removeFromNonLeaf(BTree *tree, BNode *node, int idx);
bool removeFromLeaf(BTree *tree, BNode *node, int idx);
bool removeNode(BTree *tree, BNode *node, keyType *k);
int  findKey(BNode *node, keyType *k);

void deleteRecursive(BTree *tree, keyType *key, BNode *node);
void deleteKey(BTree *tree, keyType *k);



#endif