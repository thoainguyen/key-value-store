#ifndef UTILS_H
#define UTILS_H

#include "btree.h"
#include "bnode.h"

void write_to_file(B_tree *tree, B_node *node, int pos)
{
    if (tree->fp == NULL){
        printf("file is closed\n");
        return;
    }
    if (pos == -1) pos = tree->next_pos++;
    fseek(tree->fp, pos * sizeof(B_node), SEEK_SET);
    fwrite(node, sizeof(B_node), 1, tree->fp);
}

void read_from_file(B_tree *tree, B_node *node, int pos)
{
    if (tree->fp == NULL){
        printf("rd file is closed\n");
        exit(1);
    }
    fseek(tree->fp, pos * sizeof(B_node), SEEK_SET);
    fread(node, sizeof(B_node), 1, tree->fp);
    
}

void save_btree_to_file(B_tree *tree, char *fname)
{
    FILE *fin = fopen(fname, "w");
    if(fin == NULL){
        fin = fopen(fname, "w+");
    }
    fseek(fin, 0, SEEK_SET);
    fwrite(tree, sizeof(B_tree), 1, fin);
    fclose(fin);
}

void load_btree_from_file(B_tree *tree, char *fname)
{
    FILE *fin = fopen(fname, "r");
    if (fin == NULL)
    {
        printf("config file not found");
        return;
    }
    fseek(fin, 0, SEEK_SET);
    fread(tree, sizeof(B_tree), 1, fin);
    // co van de khi noi dung meta.dat la rong
    tree->fp = fopen(tree->file_name, "r+");
    if(tree->fp == NULL){
        tree->fp = fopen(tree->file_name,"w+");
    }
    fclose(fin);
}

#endif