
#ifndef KVPAIR_H
#define KVPAIR_H

#include <string.h>

#define KEY_SIZE 300
#define VAL_SIZE 512

#define key_type char
#define val_type char

#define GT(a,b) strcmp((a),(b)) > 0
#define EQ(a,b) strcmp((a),(b)) == 0
#define LT(a,b) strcmp((a),(b)) < 0

typedef struct{
	key_type key[KEY_SIZE];
	val_type val[VAL_SIZE];
} Kv_pair;

#endif