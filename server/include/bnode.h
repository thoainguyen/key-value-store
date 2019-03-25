#ifndef BNODE_H
#define BNODE_H

#include <stdio.h>
#include <stdbool.h>

#include "kvpair.h"

#define T 3

typedef struct {
	bool is_leaf;
	int  position;
	int  num_records;
	Kv_pair records[2*T-1];
	int  children[2*T];
} B_node;

void print_bnode(B_node * node);

#endif

