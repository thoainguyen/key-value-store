#include "btree.h"
#include <stdio.h>

#define STORAGE_FILE   "data.dat"
#define CONFIG_FILE    "meta.dat"

int main0(){
    
    //B_tree *tree = create_loading_btree(CONFIG_FILE);
    B_tree *tree = create_empty_btree(STORAGE_FILE);
    insert0(tree, "x", "x");
    // insert0(tree, "thoainh", "Nguyen Huynh Thoai");
    // insert0(tree, "anhnq6", "Nguyen Quoc Anh");
    // insert0(tree, "nghiatdh","Ta Dang Hieu Nghia");
    // insert0(tree, "nghianv2","Nguyen Van Nghia");
    // insert0(tree, "phucnnh", "Ngo Nguyen Hong Phuc");
    // insert0(tree, "baonlc", "Nguyen le Chi Bao");
    // insert0(tree, "kiettt4","Trinh Tuan Kiet");
    // insert0(tree, "namvh3","Vo Huy Nam");
    // insert0(tree, "tannh","Huynh Minh Tan");

    // traverse(tree, tree->root_pos);

    // printf("%s",(Kv_pair*)search(tree,"baonlc")->val);
    
    destroy_btree(tree, CONFIG_FILE);
    return 0;
}