
#include <stdio.h>
#include <stdbool.h>

#ifndef BNODE_H
#define BNODE_H

#define KEY_SIZE 300
#define VAL_SIZE 512

#define keyType char
#define valType char

#define T 3

typedef struct{
	keyType key[KEY_SIZE];
	valType val[VAL_SIZE];
} KVPair;

typedef struct {
	bool isLeaf;
	int pos;
	int n;
	KVPair records[2*T-1];
	int children[2*T];
} BNode;


#endif

