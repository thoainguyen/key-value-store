#include "btree.h"
#include "utils.h"
#include "bnode.h"

B_tree* create_empty_btree(char *storage) { 
    B_tree *tree = (B_tree*)malloc(sizeof(B_tree));
    strcpy(tree->file_name, storage);
    tree->fp = fopen(storage, "r+");
    tree->root_pos = 0;
    tree->next_pos = 0;
    return tree;
}


B_tree *create_loading_btree(char *config){
    B_tree *tree = (B_tree*)malloc(sizeof(B_tree));
    load_btree_from_file(tree, config);
    return tree;
}

void destroy_btree(B_tree* tree, char *config) {
    save_btree_to_file(tree, config);
    free(tree);
}


int find_key(B_node* node, key_type* k) {
    int idx=0;
    while (idx < node->num_records && LT(node->records[idx].key,k))
        ++idx;
    return idx;
}

void split_child(B_tree* tree, B_node* x, int i, B_node* y) {

    B_node* z = (B_node*)malloc(sizeof(B_node)); 
    create_bnode(z, y->is_leaf, tree);
    z->num_records = T - 1;

    int j;
    for(j = 0; j < T-1; j++) {
        z->records[j] = y->records[j+T];
    }

    if(!y->is_leaf) {
        for(j = 0; j < T; j++) {
            z->children[j] = y->children[j+T];
        }
    }

    y->num_records = T - 1;
    for(j = x->num_records; j >= i+1; j--) {
        x->children[j+1] = x->children[j];
    }

    x->children[i+1] = z->position;
    for(j = x->num_records -1 ; j >= i; j--) {
        x->records[j+1] = x->records[j];
    }
    
    x->records[i] = y->records[T - 1];
    
    x->num_records ++;
    
    write_to_file(tree, x, x->position);
    write_to_file(tree, y, y->position);
    write_to_file(tree, z, z->position);
    free(z);
    
}


void insert_non_full(B_tree* tree, B_node *node, Kv_pair *record) {
    
    int i = node->num_records - 1;
    if(node->is_leaf) {
        while(i >=0 && GT(node->records[i].key,record->key)) {
            node->records[i+1] = node->records[i];
            i--;
        }
        node->records[i+1] = *record;
        node->num_records++;
        write_to_file(tree, node, node->position);
    }
    
    else {
        while( i >= 0 && GT(node->records[i].key,record->key)) {
            i--;
        }
        B_node *child = (B_node*)malloc(sizeof(B_node));
        read_from_file(tree, child, node->children[i+1]);
        if(child->num_records == (2*T-1)) {
            split_child(tree, node, i+1, child);
            if(LT(node->records[i+1].key, record->key)) {
                i++;
            }
        }
        read_from_file(tree, child, node->children[i+1]);
        insert_non_full(tree, child, record);
        free(child);
    }
}

void insert(B_tree* tree, Kv_pair *record) {
    
    if(!tree->next_pos) {
        tree->root_pos = tree->next_pos;
        B_node *root_node = malloc(sizeof(B_node)); 
        create_bnode(root_node, true, tree);
        root_node->records[0] = *record;
        root_node->num_records++;
        write_to_file(tree, root_node, root_node->position);
        free(root_node);
    }
    
    else {
        B_node *root = malloc(sizeof(B_node));
        read_from_file(tree, root, tree->root_pos);
        if(root->num_records == (2*T-1)) {

            B_node *new_root = malloc(sizeof(B_node)); 
            create_bnode(new_root, false, tree);
            new_root->children[0] = tree->root_pos;
            split_child(tree, new_root, 0, root);
            int i = 0;

            if(LT(new_root->records[0].key, record->key)) {
                i++;
            }
            
            B_node *child = (B_node*)malloc(sizeof(B_node));
            read_from_file(tree, child, new_root->children[i]);
            insert_non_full(tree, child, record);
    
            tree->root_pos = new_root->position;        
            write_to_file(tree, root, root->position);
            free(new_root);
        }
        else {
            insert_non_full(tree, root, record);
        }
        
        free(root);
    }
}

void insert0(B_tree *btree, key_type *key, val_type *val){
    Kv_pair pair;
    strcpy(pair.key, key);
    strcpy(pair.val, val);
    insert(btree, &pair);
}

void traverse(B_tree* tree, int root) {
    
    if(root == -1) {    
        return;
    }
    B_node *node = (B_node*)malloc(sizeof(B_node));
    read_from_file(tree, node, root);
    print_bnode(node);
    
    for(int i = 0; i < 2*T; i++) {
        traverse(tree, node->children[i]);
    }
    free(node);   
}

Kv_pair* search(B_tree* tree, key_type* key) {
    
    B_node* root = (B_node*)malloc(sizeof(B_node));
    read_from_file(tree, root, tree->root_pos);
    return recursive_search(tree, key, root);
}


Kv_pair* recursive_search(B_tree* tree, key_type* key, B_node* root) {

    int i = 0;
    while(i < root->num_records && GT(key, root->records[i].key))
        i++;
    if(i < root->num_records && EQ(key, root->records[i].key)){
        return &root->records[i];
    }
    else if(root->is_leaf) {
        return NULL;
    }
    else {
        B_node* child = (B_node*)malloc(sizeof(B_node));
        read_from_file(tree, child, root->children[i]);
        return recursive_search(tree, key, child);
    }
}

bool remove0(B_tree* tree, key_type* key) {
    B_node *root = (B_node*)malloc(sizeof(B_node));
    read_from_file(tree, root, tree->root_pos);
    return remove_bnode(tree, root, key);
}

 
void merge(B_tree* tree, B_node *node, int idx) {

    B_node *children = (B_node*)malloc(sizeof(B_node));
    B_node *sibling = (B_node*)malloc(sizeof(B_node));
    
    read_from_file(tree, children, node->children[idx]);
    read_from_file(tree, sibling, node->children[idx+1]);

    children->records[T-1] = node->records[idx];

    for (int i=0; i < sibling->num_records; ++i)
        children->records[i+T] = sibling->records[i];

    if (!children->is_leaf) {
        for(int i=0; i <= sibling->num_records; ++i)
            children->children[i+T] = sibling->children[i];
    }
 
    for (int i = idx+1; i < node->num_records; ++i)
        node->records[i-1] = node->records[i];

    for (int i = idx+2; i <= node->num_records; ++i)
        node->children[i-1] = node->children[i];
 

    children->num_records += sibling->num_records+1;
    node->num_records--;
 
    write_to_file(tree, node, node->position);
    write_to_file(tree, children, children->position);
    write_to_file(tree, sibling, sibling->position);
}


void borrow_from_next(B_tree* tree, B_node *node, int idx) {
 
    B_node *children = (B_node*)malloc(sizeof(B_node));
    B_node *sibling = (B_node*)malloc(sizeof(B_node));
    
    read_from_file(tree, children, node->children[idx]);
    read_from_file(tree, sibling, node->children[idx+1]);

    children->records[children->num_records] = node->records[idx];
 
    if (! children->is_leaf)
        children->children[children->num_records+1] = sibling->children[0];

    node->records[idx] = sibling->records[0];

    for (int i=1; i < sibling->num_records; ++i)
        sibling->records[i-1] = sibling->records[i];

    if (!sibling->is_leaf) {
        for(int i = 1; i <= sibling->num_records; ++i)
            sibling->children[i-1] = sibling->children[i];
    }

    children->num_records += 1;
    sibling->num_records  -= 1;
    
    write_to_file(tree, node, node->position);
    write_to_file(tree, children, children->position);
    write_to_file(tree, sibling, sibling->position);
}


void borrow_from_prev(B_tree* tree, B_node *node, int idx) {
    B_node *children = (B_node*)malloc(sizeof(B_node));
    B_node *sibling = (B_node*)malloc(sizeof(B_node));
    
    read_from_file(tree, children, node->children[idx]);
    read_from_file(tree, sibling, node->children[idx-1]);
    
    for (int i = children->num_records-1; i >= 0; --i)
        children->records[i+1] = children->records[i];

    if (!children->is_leaf) {
        for(int i = children->num_records; i >= 0; --i)
            children->children[i+1] = children->children[i];
    }
 
    children->records[0] = node->records[idx-1];

    if (!node->is_leaf)
        children->children[0] = sibling->children[sibling->num_records];

    node->records[idx-1] = sibling->records[sibling->num_records-1];
 
    children->num_records += 1;
    sibling->num_records -= 1;
    
    write_to_file(tree, node, node->position);
    write_to_file(tree, children, children->position);
    write_to_file(tree, sibling, sibling->position);
}

void fill(B_tree* tree, B_node *node, int idx) {
 
    B_node *prev = (B_node*)malloc(sizeof(B_node));
    B_node *succ = (B_node*)malloc(sizeof(B_node));
    read_from_file(tree, prev, node->children[idx-1]);
    read_from_file(tree, succ, node->children[idx+1]);

    if (idx != 0 && prev->num_records >= T)
        borrow_from_prev(tree, node, idx);

    else if (idx != node->num_records && succ->num_records >= T)
        borrow_from_next(tree, node, idx);
    else {
        if (idx != node->num_records)
            merge(tree, node, idx);
        else
            merge(tree, node, idx-1);
    }
}

Kv_pair *get_successor(B_tree *tree, B_node *node, int idx) {
 
    B_node *current = (B_node*)malloc(sizeof(B_node));
    read_from_file(tree, current, node->children[idx]);
    while (!current->is_leaf)
        read_from_file(tree, current, current->children[0]);
    return &(current->records[0]);
}

Kv_pair *get_predecessor(B_tree *tree, B_node *node, int idx) {

    B_node *current = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, current, node->children[idx]);
    while (!current->is_leaf)
        read_from_file(tree, current, current->children[current->num_records]);
    return &(current->records[current->num_records-1]);
}


bool remove_from_non_leaf(B_tree* tree, B_node *node, int idx) {
 
    key_type* k = node->records[idx].key;
    bool res;
    B_node *children = (B_node*)malloc(sizeof(B_node));
    B_node *sibling  = (B_node*)malloc(sizeof(B_node));
    
    read_from_file(tree, children, node->children[idx]);
    read_from_file(tree, sibling, node->children[idx-1]);
 
    if (children->num_records >= T) {
        Kv_pair *pred = get_predecessor(tree, node, idx);
        node->records[idx] = *pred;
        res = remove_bnode(tree, children, pred->key);
    }

    else if  (sibling->num_records >= T)
    {
        Kv_pair *succ = get_successor(tree, node, idx);
        node->records[idx] = *succ;
        res = remove_bnode(tree, sibling, succ->key);
    }
    else {
        merge(tree, node, idx);
        res = remove_bnode(tree, children, k);
    }
    
    write_to_file(tree, children, children->position);
    write_to_file(tree, sibling, sibling->position);
    return res;
}


bool remove_from_leaf (B_tree* tree, B_node *node, int idx) {
    for (int i = idx+1; i < node->num_records; ++i)
        node->records[i-1] = node->records[i];
    node->num_records--;
    return true;
}
 


bool remove_bnode(B_tree *tree, B_node *node, key_type *k) {
    int idx = find_key(node, k);
    bool res = false;
    if (idx < node->num_records && EQ(node->records[idx].key,k)) {
        if (node->is_leaf)
            res = remove_from_leaf(tree, node, idx);
        else
            res = remove_from_non_leaf(tree, node, idx);
        write_to_file(tree, node, node->position);
    }
    else {
        if (node->is_leaf) {
            return false;
        }
        bool flag = (idx == node->num_records);
 
        B_node *child = (B_node*)malloc(sizeof(B_node));
        read_from_file(tree, child, node->children[idx]);
        
        if (child->num_records < T)
            fill(tree, node, idx);
 
        if (flag && idx > node->num_records) {
            B_node *sibling = (B_node*)malloc(sizeof(B_node));
            read_from_file(tree, sibling, node->children[idx-1]);
            remove_bnode(tree, sibling, k);
            write_to_file(tree, sibling, sibling->position);
        }
        else{
            remove_bnode(tree, child, k);
        }
        write_to_file(tree, child, child->position);
        res = true;
    }
    return res;
}
 


void create_bnode(B_node *node, bool is_leaf, B_tree *tree) {
    node->is_leaf = is_leaf;
    node->num_records = 0;
    node->position = tree->next_pos;
    tree->next_pos++;
    for(int i = 0; i < 2*T; i++) {
        node->children[i] = -1;
    }
}