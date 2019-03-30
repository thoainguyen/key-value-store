#include "btree.h"
#include "utils.h"
#include "bnode.h"

B_tree *create_empty_btree(char *storage)
{

    B_tree *tree = (B_tree *)malloc(sizeof(B_tree));
    strcpy(tree->file_name, storage);
    tree->fp = fopen(storage, "r+");
    if (tree->fp == NULL){
        tree->fp = fopen(storage, "w+");
    }
    
    tree->root_pos = 0;
    tree->next_pos = 0;
    B_node *node = (B_node *)malloc(sizeof(B_node));
    create_bnode(node, true, tree);
    write_to_file(tree, node, 0);
    free(node);
    return tree;
}

B_tree *create_loading_btree(char *config)
{

    B_tree *tree = (B_tree *)malloc(sizeof(B_tree));
    load_btree_from_file(tree, config);
    return tree;
}

void destroy_btree(B_tree *tree, char *config)
{

    save_btree_to_file(tree, config);
    if(tree->fp != NULL)
        fclose(tree->fp);
    free(tree);
}

int find_key(B_node *node, key_type *k)
{
    int idx = 0;
    while (idx < node->num_records && LT(node->records[idx].key, k))
        ++idx;
    return idx;
}

B_node *split_child(B_tree *tree, B_node *x, int i, B_node *y)
{

    B_node *z = (B_node *)malloc(sizeof(B_node));
    create_bnode(z, y->is_leaf, tree);
    z->num_records = T - 1;

    int j;
    for (j = 0; j < T - 1; j++)
    {
        z->records[j] = y->records[j + T];
    }

    if (!y->is_leaf)
    {
        for (j = 0; j < T; j++)
        {
            z->child[j] = y->child[j + T];
        }
    }

    y->num_records = T - 1;
    for (j = x->num_records; j >= i + 1; j--)
    {
        x->child[j + 1] = x->child[j];
    }

    x->child[i + 1] = z->position;
    for (j = x->num_records - 1; j >= i; j--)
    {
        x->records[j + 1] = x->records[j];
    }

    x->records[i] = y->records[T - 1];

    x->num_records++;

    write_to_file(tree, x, x->position);
    write_to_file(tree, y, y->position);
    write_to_file(tree, z, z->position);
    return z;
}

void insert_non_full(B_tree *tree, B_node *node, Kv_pair *record)
{

    int i = node->num_records - 1;
    int k = find_key(node, record->key);

    if (k < node->num_records && EQ(node->records[k].key, record->key))
    {
        node->records[k] = *record;
        write_to_file(tree, node, node->position);
        return;
    }
    else if (node->is_leaf)
    {
        while (i >= 0 && GT(node->records[i].key, record->key))
        {
            node->records[i + 1] = node->records[i];
            i--;
        }

        node->records[i + 1] = *record;
        node->num_records++;
        write_to_file(tree, node, node->position);
    }
    else
    {
        while (i >= 0 && GT(node->records[i].key, record->key))
        {
            i--;
        }

        B_node *child = (B_node *)malloc(sizeof(B_node));
        read_from_file(tree, child, node->child[i + 1]);

        if (child->num_records == (2 * T - 1))
        {

            B_node *z = split_child(tree, node, i + 1, child);
            if (LT(node->records[i + 1].key, record->key))
            {
                B_node *temp = child;
                child = z;
                free(temp);
            }
            else
            {
                free(z);
            }
        }

        insert_non_full(tree, child, record);
        free(child);
    }
}

void insert(B_tree *tree, Kv_pair *record)
{

    B_node *root = malloc(sizeof(B_node));
    read_from_file(tree, root, tree->root_pos);

    int r = tree->root_pos;

    if (root->num_records == (2 * T - 1))
    {

        B_node *s = malloc(sizeof(B_node));
        create_bnode(s, false, tree);

        tree->root_pos = s->position;

        s->child[0] = r;

        B_node *z = split_child(tree, s, 0, root);

        if (!GT(z->records[0].key, record->key))
        {
            B_node *temp = s;
            s = z;
            free(temp);
        }
        else
        {
            free(z);
        }

        insert_non_full(tree, s, record);

        free(s);
    }
    else
    {
        insert_non_full(tree, root, record);
    }

    free(root);
}

void insert0(B_tree *btree, key_type *key, val_type *val)
{
    Kv_pair pair;
    strcpy(pair.key, key);
    strcpy(pair.val, val);
    insert(btree, &pair);
}

void traverse(B_tree *tree, int root)
{

    if (root == -1)
    {
        return;
    }
    B_node *node = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, node, root);
    print_bnode(node);

    for (int i = 0; i < node->num_records + 1; i++)
    {
        traverse(tree, node->child[i]);
    }
    free(node);
}

Kv_pair *search(B_tree *tree, key_type *key)
{
    B_node *root = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, root, tree->root_pos);
    Kv_pair *ret = recursive_search(tree, key, root);
    free(root);
    return ret;
}

Kv_pair *recursive_search(B_tree *tree, key_type *key, B_node *root)
{
    int i = 0;
    while (i < root->num_records && GT(key, root->records[i].key))
        i++;
    if (i < root->num_records && EQ(key, root->records[i].key))
    {
        return &root->records[i];
    }
    else if (root->is_leaf)
    {
        return NULL;
    }
    else
    {
        B_node *child = (B_node *)malloc(sizeof(B_node));
        read_from_file(tree, child, root->child[i]);
        Kv_pair *ret = recursive_search(tree, key, child);
        free(child);
        return ret;
    }
}

bool remove0(B_tree *tree, key_type *key)
{
    B_node *root = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, root, tree->root_pos);
    return remove_bnode(tree, root, key);
}

void merge(B_tree *tree, B_node *node, int idx)
{
    B_node *child = (B_node *)malloc(sizeof(B_node));
    B_node *sibling = (B_node *)malloc(sizeof(B_node));

    read_from_file(tree, child, node->child[idx]);
    read_from_file(tree, sibling, node->child[idx + 1]);

    child->records[T - 1] = node->records[idx];

    for (int i = 0; i < sibling->num_records; ++i)
        child->records[i + T] = sibling->records[i];

    if (!child->is_leaf)
    {
        for (int i = 0; i <= sibling->num_records; ++i)
            child->child[i + T] = sibling->child[i];
    }

    for (int i = idx + 1; i < node->num_records; ++i)
        node->records[i - 1] = node->records[i];

    for (int i = idx + 2; i <= node->num_records; ++i)
        node->child[i - 1] = node->child[i];

    child->num_records += (sibling->num_records + 1);
    node->num_records--;


    write_to_file(tree, node, node->position);
    write_to_file(tree, child, child->position);
    write_to_file(tree, sibling, sibling->position);
    // delete sibling
    free(child);
    free(sibling);
}

void borrow_from_next(B_tree *tree, B_node *node, int idx)
{

    B_node *child = (B_node *)malloc(sizeof(B_node));
    B_node *sibling = (B_node *)malloc(sizeof(B_node));

    read_from_file(tree, child, node->child[idx]);
    read_from_file(tree, sibling, node->child[idx + 1]);

    child->records[child->num_records] = node->records[idx];

    if (!child->is_leaf)
        child->child[child->num_records + 1] = sibling->child[0];

    node->records[idx] = sibling->records[0];

    for (int i = 1; i < sibling->num_records; ++i)
        sibling->records[i - 1] = sibling->records[i];

    if (!sibling->is_leaf)
    {
        for (int i = 1; i <= sibling->num_records; ++i)
            sibling->child[i - 1] = sibling->child[i];
    }

    child->num_records += 1;
    sibling->num_records -= 1;

    write_to_file(tree, node, node->position);
    write_to_file(tree, child, child->position);
    write_to_file(tree, sibling, sibling->position);

    free(child);
    free(sibling);
}

void borrow_from_prev(B_tree *tree, B_node *node, int idx)
{
    B_node *child = (B_node *)malloc(sizeof(B_node));
    B_node *sibling = (B_node *)malloc(sizeof(B_node));

    read_from_file(tree, child, node->child[idx]);
    read_from_file(tree, sibling, node->child[idx - 1]);

    for (int i = child->num_records - 1; i >= 0; --i)
        child->records[i + 1] = child->records[i];

    if (!child->is_leaf)
    {
        for (int i = child->num_records; i >= 0; --i)
            child->child[i + 1] = child->child[i];
    }

    child->records[0] = node->records[idx - 1];

    if (!node->is_leaf)
        child->child[0] = sibling->child[sibling->num_records];

    node->records[idx - 1] = sibling->records[sibling->num_records - 1];

    child->num_records += 1;
    sibling->num_records -= 1;

    write_to_file(tree, node, node->position);
    write_to_file(tree, child, child->position);
    write_to_file(tree, sibling, sibling->position);

    free(child);
    free(sibling);
}

void fill(B_tree *tree, B_node *node, int idx)
{

    B_node *prev = (B_node *)malloc(sizeof(B_node));
    B_node *succ = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, prev, node->child[idx - 1]);
    read_from_file(tree, succ, node->child[idx + 1]);

    int nprev = prev->num_records;
    int nsucc = succ->num_records;

    free(prev);
    free(succ);

    if (idx != 0 && nprev >= T)
        borrow_from_prev(tree, node, idx);
    else if (idx != node->num_records && nsucc >= T)
        borrow_from_next(tree, node, idx);
    else
    {
        if (idx != node->num_records)
            merge(tree, node, idx);
        else
            merge(tree, node, idx - 1);
    }
}

Kv_pair get_successor(B_tree *tree, B_node *node, int idx)
{

    B_node *current = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, current, node->child[idx]);
    while (!current->is_leaf)
        read_from_file(tree, current, current->child[0]);
    Kv_pair succ = current->records[0];
    free(current);
    return succ;
}

Kv_pair get_predecessor(B_tree *tree, B_node *node, int idx)
{

    B_node *current = (B_node *)malloc(sizeof(B_node));
    read_from_file(tree, current, node->child[idx]);
    while (!current->is_leaf)
        read_from_file(tree, current, current->child[current->num_records]);
    Kv_pair pred = current->records[current->num_records - 1];
    free(current);
    return pred;
}

bool remove_from_non_leaf(B_tree *tree, B_node *node, int idx)
{

    key_type *k = node->records[idx].key;

    B_node *child = (B_node *)malloc(sizeof(B_node));
    B_node *sibling = (B_node *)malloc(sizeof(B_node));

    read_from_file(tree, child, node->child[idx]);
    read_from_file(tree, sibling, node->child[idx+1]);

    if (child->num_records >= T)
    {
        Kv_pair pred = get_predecessor(tree, node, idx);
        node->records[idx] = pred;
        write_to_file(tree, node, node->position);
        free(sibling);
        return remove_bnode(tree, child, pred.key);
    }

    else if (sibling->num_records >= T)
    {
        Kv_pair succ = get_successor(tree, node, idx);
        node->records[idx] = succ;
        write_to_file(tree, node, node->position);
        free(child);
        return remove_bnode(tree, sibling, succ.key);
    }
    else
    {
        merge(tree, node, idx);
        free(sibling);
        return remove_bnode(tree, child, k);
    }

}

bool remove_from_leaf(B_tree *tree, B_node *node, int idx)
{
    for (int i = idx + 1; i < node->num_records; ++i)
        node->records[i - 1] = node->records[i];
    node->num_records--;
    write_to_file(tree, node, node->position);
    free(node);
    return true;
}

bool remove_bnode(B_tree *tree, B_node *node, key_type *k)
{
    if(node->num_records == -1){
        tree->root_pos = 0;
        tree->next_pos = 0;
        B_node *node = (B_node *)malloc(sizeof(B_node));
        create_bnode(node, true, tree);
        write_to_file(tree, node, 0);
        return true;
    }

    int idx = find_key(node, k);
    if (idx < node->num_records && EQ(node->records[idx].key, k))
    {
        if (node->is_leaf)
            return remove_from_leaf(tree, node, idx);
        else
            return remove_from_non_leaf(tree, node, idx);
    }
    else
    {
        if (node->is_leaf)
        {
            free(node);
            return false;
        }

        bool flag = (idx == node->num_records);

        B_node *child = (B_node *)malloc(sizeof(B_node));
        read_from_file(tree, child, node->child[idx]);
        int nrecord = child->num_records;
        free(child);

        if (nrecord < T){
            fill(tree, node, idx);
        }
        
        if (flag && idx > node->num_records)
        {
            B_node *c_i = (B_node *)malloc(sizeof(B_node));
            read_from_file(tree, c_i, node->child[idx - 1]);
            free(node);
            return remove_bnode(tree, c_i, k);
        }
        else
        {
            B_node *c_j = (B_node *)malloc(sizeof(B_node));
            read_from_file(tree, c_j, node->child[idx]);
            free(node);
            return remove_bnode(tree, c_j, k);
        }

    }
}

void create_bnode(B_node *node, bool is_leaf, B_tree *tree)
{
    node->is_leaf = is_leaf;
    node->num_records = 0;
    node->position = tree->next_pos;
    tree->next_pos++;
    for (int i = 0; i < 2 * T; i++)
    {
        node->child[i] = -1;
    }
}