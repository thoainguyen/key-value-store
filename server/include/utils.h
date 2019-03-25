#ifndef UTILS_H
#define UTILS_H

#include "btree.h"
#include "bnode.h"



void write_to_file(B_tree* tree, B_node* node, int pos){
    if(pos == -1) pos = tree->next_pos++;
    fseek(tree->fp, pos * sizeof(B_node), SEEK_SET);
    fwrite(node, sizeof(B_node), 1, tree->fp);
}

void read_from_file(B_tree* tree, B_node* node, int pos){
    fseek(tree->fp, pos * sizeof(B_node), SEEK_SET);
    fread(node, sizeof(B_node), 1, tree->fp);
}

void save_btree_to_file(B_tree* tree, char* fname){
    FILE *fin = fopen(fname, "w");
    fseek(fin,0,SEEK_SET);
    fwrite(tree, sizeof(B_tree), 1, fin);
    fclose(tree->fp);
    fclose(fin);
}

void load_btree_from_file(B_tree* tree, char* fname){
    FILE *fin = fopen(fname, "r");
    if(fin == NULL) {
        printf("config file does not exist");
    }
    fseek(fin,0,SEEK_SET);
    fread(tree, sizeof(B_tree), 1, fin);
    tree->fp = fopen(tree->file_name, "r+");
    fclose(fin);
}

#endif