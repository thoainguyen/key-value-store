#include "btree.h"

// BNODE IMPLEMENTATION

void BNodeInit(BNode *node, bool isLeaf, BTree *tree) {
    
    node->isLeaf = isLeaf;
    node->n = 0;
    node->pos = tree->nextPos;
    tree->nextPos++;
    
    for(int i = 0; i < 2*T; i++) {
        node->children[i] = -1;
    }
    
}

void writeFile(BTree* ptr_tree, BNode* p, int pos) {
    if(pos == -1) {
        pos = ptr_tree->nextPos++;
    }
    fseek(ptr_tree->fp, pos * sizeof(BNode), SEEK_SET);
    fwrite(p, sizeof(BNode), 1, ptr_tree->fp);
    printf("%ld\n",sizeof(BNode));
    // fsync(ptr_tree->fp);
}

void readFile(BTree* ptr_tree, BNode* p, int pos) {
    fseek(ptr_tree->fp, pos * sizeof(BNode), SEEK_SET);
    fread(p, sizeof(BNode), 1, ptr_tree->fp);
}

void dispNode(BNode * node){

    printf("KEYS: ");

    for(int i = 0; i < node->n; i++) {
        printf(" KVPair(%s,%s)", node->records[i].key, node->records[i].val);
    }

    printf("\nCHILDS: ");
    for(int i = 0; i <= node->n; i++) {
        printf("%d ", node->children[i]);
    }
    printf("\n");
}

// BTREE IMPLEMENTATION

BTree* BTreeInit(char *fname, bool mode) { 
    
    //false: Create new
    BTree *tree = (BTree*)malloc(sizeof(BTree));
    
    if(!mode) {
        strcpy(tree->fname, fname);
        tree->fp = fopen(fname, "r+");
        tree->root = 0;
        tree->nextPos = 0;
        return tree;
    }

    //true: Use existing
    FILE *fin = fopen("meta.dat", "r");
    fread(tree, sizeof(BTree), 1, fin);
    fclose(fin);
    tree->fp = fopen(fname, "r+");
    return tree;
}


void BTreeDestroy(BTree* tree) {
    FILE *fin = fopen("meta.dat", "w");
    fseek(fin,0,SEEK_SET);
    fwrite(tree, sizeof(BTree), 1, fin);
    fclose(fin);
    fclose(tree->fp);
    free(tree);
}


void splitChild(BTree* tree, BNode* x, int i, BNode* y) {

    BNode* z = malloc(sizeof(BNode)); 
    BNodeInit(z, y->isLeaf, tree);
    z->n = T - 1;
    
    int j;
    for(j = 0; j < T-1; j++) {
        z->records[j] = y->records[j+T];
    }
    
    if(!y->isLeaf) {
        for(j = 0; j < T; j++) {
            z->children[j] = y->children[j+T];
        }
    }
    
    y->n = T - 1;
    
    for(j = x->n; j >= i+1; j--) {
        x->children[j+1] = x->children[j];
    }
    
    x->children[i+1] = z->pos;
    
    for(j = x->n-1; j >= i; j--) {
        x->records[j+1] = x->records[j];
    }
    
    x->records[i] = y->records[T - 1];
    
    x->n++;
    
    
    writeFile(tree, x, x->pos);
    writeFile(tree, y, y->pos);
    writeFile(tree, z, z->pos);
    free(z);
    
}


void insertNonFull(BTree* tree, BNode *node, KVPair *record) {
    
    int i = (node->n)-1;
    
    if(node->isLeaf) {
        
        while(i >=0 && GT(node->records[i].key,record->key)) {
            node->records[i+1] = node->records[i];
            i--;
        }
        node->records[i+1] = *record;
        node->n++;
        writeFile(tree, node, node->pos);
        
    }
    
    else {
        while( i >= 0 && GT(node->records[i].key,record->key)) {
            i--;
        }
        
        BNode *c = malloc(sizeof(BNode));
        
        readFile(tree, c, node->children[i+1]);
        
        if(c->n == (2*T-1)) {
            splitChild(tree, node, i+1, c);
            
            if(node->records[i+1].key < record->key) {
                i++;
            }
        }
        readFile(tree, c, node->children[i+1]);
        insertNonFull(tree, c, record);
        free(c);
    }
    
}

void insert(BTree* tree, KVPair *record) {
    
    if(!(tree->nextPos)) {
        tree->root = tree->nextPos;
        BNode *root_node = malloc(sizeof(BNode)); 
        BNodeInit(root_node, true, tree);
        root_node->records[0] = *record;
        root_node->n++;
        writeFile(tree, root_node, root_node->pos);
        free(root_node);
    }
    
    else {
        BNode *root = malloc(sizeof(BNode));
        readFile(tree, root, tree->root);
        if(root->n == (2*T-1)) {
            
            BNode *new_root = malloc(sizeof(BNode)); 
            BNodeInit(new_root, false, tree);
            new_root->children[0] = tree->root;
            
            splitChild(tree, new_root, 0, root);
            
            int i = 0;
            if(new_root->records[0].key < record->key) {
                i++;
            }
            
            BNode *c = malloc(sizeof(BNode));
            readFile(tree, c, new_root->children[i]);
            insertNonFull(tree, c, record);
            
            tree->root = new_root->pos;
            
            writeFile(tree, root, root->pos);
            
            free(new_root);
        }
        
        else {
            
            insertNonFull(tree, root, record);
            
        }
        
        free(root);
    }
}

void traverse(BTree* tree, int root) {
    
    if(root == -1) {    
        return;
    }
    
    BNode *to_print = malloc(sizeof(BNode));
    
    readFile(tree, to_print, root);
    
    dispNode(to_print);
    
    for(int i = 0; i < 2*T; i++) {
        traverse(tree, to_print->children[i]);
    }
    
    free(to_print);
    
}

KVPair* search(BTree* tree, keyType* key) {
    
    BNode* root = malloc(sizeof(BNode));
    readFile(tree, root, tree->root);
    return searchRecursive(tree, key, root);
}


KVPair* searchRecursive(BTree* tree, keyType* key, BNode* root) {

    if (root != NULL) {
        printf("Children: [ ");
        for (int i = 0; i <= root->n    ; i ++){
            printf("%d ",root->children[i]);
        }
        printf("]\n");
    }

    int i = 0;
    
    while(i < root->n && GT(key,root->records[i].key))
        i++;
    
    
    if(i < root->n && EQ(key,root->records[i].key)){
        return &root->records[i];
    }
        
    
    
    else if(root->isLeaf) {
        return NULL;
    }

    else {
        BNode* c = malloc(sizeof(BNode));
        readFile(tree, c, root->children[i]);
        return searchRecursive(tree, key, c);
    }
}

bool removeFromTree(BTree* tree, keyType* key) {
    
    BNode *root = malloc(sizeof(BNode));
    readFile(tree, root, tree->root);
    return removeNode(tree, root, key);
    
}

 
// A function to merge C[idx] with C[idx+1]
// C[idx+1] is freed after merging
void merge(BTree* tree, BNode *node, int idx) {
    
    BNode *child = malloc(sizeof(BNode));
    BNode *sibling = malloc(sizeof(BNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx+1]);
 
    // Pulling a key from the current node and inserting it into (t-1)th
    // position of C[idx]
    child->records[T-1] = node->records[idx];
 
    // Copying the keys from C[idx+1] to C[idx] at the end
    for (int i=0; i<sibling->n; ++i)
        child->records[i+T] = sibling->records[i];
 
    // Copying the child pointers from C[idx+1] to C[idx]
    if (!child->isLeaf) {
        for(int i=0; i<=sibling->n; ++i)
            child->children[i+T] = sibling->children[i];
    }
 
    // Moving all keys after idx in the current node one step before -
    // to fill the gap created by moving keys[idx] to C[idx]
    for (int i=idx+1; i<node->n; ++i)
        node->records[i-1] = node->records[i];
 
    // Moving the child pointers after (idx+1) in the current node one
    // step before
    for (int i=idx+2; i<=node->n; ++i)
        node->children[i-1] = node->children[i];
 
    // Updating the key count of child and the current node
    child->n += sibling->n+1;
    node->n--;
 
    // Freeing the memory occupied by sibling
    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);
    return;
}


// A function to borrow a key from the C[idx+1] and place
// it in C[idx]
void borrowFromNext(BTree* tree, BNode *node, int idx) {
 
    BNode *child = malloc(sizeof(BNode));
    BNode *sibling = malloc(sizeof(BNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx+1]);
 
    // keys[idx] is inserted as the last key in C[idx]
    child->records[(child->n)] = node->records[idx];
 
    // Sibling's first child is inserted as the last child
    // into C[idx]
    if (!(child->isLeaf))
        child->children[(child->n)+1] = sibling->children[0];
 
    //The first key from sibling is inserted into keys[idx]
    node->records[idx] = sibling->records[0];
 
    // Moving all keys in sibling one step behind
    for (int i=1; i<sibling->n; ++i)
        sibling->records[i-1] = sibling->records[i];
 
    // Moving the child pointers one step behind
    if (!sibling->isLeaf) {
        for(int i=1; i<=sibling->n; ++i)
            sibling->children[i-1] = sibling->children[i];
    }
 
    // Increasing and decreasing the key count of C[idx] and C[idx+1]
    // respectively
    child->n += 1;
    sibling->n -= 1;
    
    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);
    
    return;
}


 
// A function to borrow a key from C[idx-1] and insert it
// into C[idx]
void borrowFromPrev(BTree* tree, BNode *node, int idx) {
    BNode *child = malloc(sizeof(BNode));
    BNode *sibling = malloc(sizeof(BNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx-1]);
    
    // The last key from C[idx-1] goes up to the parent and key[idx-1]
    // from parent is inserted as the first key in C[idx]. Thus, the  loses
    // sibling one key and child gains one key
 
    // Moving all key in C[idx] one step ahead
    for (int i=child->n-1; i>=0; --i)
        child->records[i+1] = child->records[i];
 
    // If C[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->isLeaf) {
        for(int i=child->n; i>=0; --i)
            child->children[i+1] = child->children[i];
    }
 
    // Setting child's first key equal to keys[idx-1] from the current node
    child->records[0] = node->records[idx-1];
 
    // Moving sibling's last child as C[idx]'s first child
    if (!node->isLeaf)
        child->children[0] = sibling->children[sibling->n];
 
    // Moving the key from the sibling to the parent
    // This reduces the number of keys in the sibling
    node->records[idx-1] = sibling->records[sibling->n-1];
 
    child->n += 1;
    sibling->n -= 1;
    
    writeFile(tree, node, node->pos);
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);
    
    return;
}


// A function to fill child C[idx] which has less than t-1 keys
void fill(BTree* tree, BNode *node, int idx) {
 
    BNode *c_prev = malloc(sizeof(BNode));
    BNode *c_succ = malloc(sizeof(BNode));
    readFile(tree, c_prev, node->children[idx-1]);
    readFile(tree, c_succ, node->children[idx+1]);
    // If the previous child(C[idx-1]) has more than t-1 keys, borrow a key
    // from that child
    if (idx!=0 && c_prev->n>=T)
        borrowFromPrev(tree, node, idx);
 
    // If the next child(C[idx+1]) has more than t-1 keys, borrow a key
    // from that child
    else if (idx!=node->n && c_succ->n>=T)
        borrowFromNext(tree, node, idx);
 
    // Merge C[idx] with its sibling
    // If C[idx] is the last child, merge it with with its previous sibling
    // Otherwise merge it with its next sibling
    else {
        if (idx != node->n)
            merge(tree, node, idx);
        else
            merge(tree, node, idx-1);
    }
    return;
}

KVPair *getSucc(BTree* tree, BNode *node, int idx) {
 
    // Keep moving the left most node starting from C[idx+1] until we reach a leaf
    BNode *curr = malloc(sizeof(BNode));
    readFile(tree, curr, node->children[idx]);
    while (!curr->isLeaf)
        readFile(tree, curr, curr->children[0]);
 
    // Return the first key of the leaf
    return &(curr->records[0]);
}

// A function to get predecessor of keys[idx]
KVPair *getPred(BTree* tree, BNode *node, int idx) {
    // Keep moving to the right most node until we reach a leaf
    
    BNode *curr = malloc(sizeof(BNode));
    readFile(tree, curr, node->children[idx]);
    while (!curr->isLeaf)
        readFile(tree, curr, curr->children[curr->n]);
        
    // Return the last key of the leaf
    return &(curr->records[curr->n-1]);
}

// A function to remove the idx-th key from this node - which is a non-leaf node
bool removeFromNonLeaf(BTree* tree, BNode *node, int idx) {
 
    keyType* k = node->records[idx].key;
    bool res;
    
    BNode *child = malloc(sizeof(BNode));
    BNode *sibling = malloc(sizeof(BNode));
    
    readFile(tree, child, node->children[idx]);
    readFile(tree, sibling, node->children[idx-1]);
 
    // If the child that precedes k (C[idx]) has atleast t keys,
    // find the predecessor 'pred' of k in the subtree rooted at
    // C[idx]. Replace k by pred. Recursively delete pred
    // in C[idx]
    if (child->n >= T) {
        KVPair *pred = getPred(tree, node, idx);
        node->records[idx] = *pred;
        res = removeNode(tree, child, pred->key);
    }
 
    // If the child C[idx] has less that t keys, examine C[idx+1].
    // If C[idx+1] has atleast t keys, find the successor 'succ' of k in
    // the subtree rooted at C[idx+1]
    // Replace k by succ
    // Recursively delete succ in C[idx+1]
    else if  (sibling->n >= T)
    {
        KVPair *succ = getSucc(tree, node, idx);
        node->records[idx] = *succ;
        res = removeNode(tree, sibling, succ->key);
    }
 
    // If both C[idx] and C[idx+1] has less that t keys,merge k and all of C[idx+1]
    // into C[idx]
    // Now C[idx] contains 2t-1 keys
    // Free C[idx+1] and recursively delete k from C[idx]
    else {
        merge(tree, node, idx);
        res = removeNode(tree, child, k);
    }
    
    writeFile(tree, child, child->pos);
    writeFile(tree, sibling, sibling->pos);
    
    return res;
}

// A function to remove the idx-th key from this node - which is a leaf node
bool removeFromLeaf (BTree* tree, BNode *node, int idx) {
 
    // Move all the keys after the idx-th pos one place backward
    for (int i=idx+1; i<node->n; ++i)
        node->records[i-1] = node->records[i];
 
    // Reduce the count of keys
    node->n--;
 
    return true;
}
 


bool removeNode(BTree* tree, BNode* node, keyType* k) {
    int idx = findKey(node, k);
    bool res;
    // The key to be removed is present in this node
    if (idx < node->n && EQ(node->records[idx].key,k)) {
 
        // If the node is a leaf node - removeFromLeaf is called
        // Otherwise, removeFromNonLeaf function is called
        if (node->isLeaf)
            res = removeFromLeaf(tree, node, idx);
        else
            res = removeFromNonLeaf(tree, node, idx);
        
        writeFile(tree, node, node->pos);
    }
    else {
 
        // If this node is a leaf node, then the key is not present in tree
        if (node->isLeaf) {
            printf("Not Present!\n");
            return false;
        }
 
        // The key to be removed is present in the sub-tree rooted with this node
        // The flag indicates whether the key is present in the sub-tree rooted
        // with the last child of this node
        bool flag = ( (idx == node->n ) ? true : false );
 
        // If the child where the key is supposed to exist has less that t keys,
        // we fill that child
        
        BNode *c = malloc(sizeof(BNode));
        readFile(tree, c, node->children[idx]);
        
        if (c->n < T)
            fill(tree, node, idx);
 
        // If the last child has been merged, it must have merged with the previous
        // child and so we recurse on the (idx-1)th child. Else, we recurse on the
        // (idx)th child which now has atleast t keys
        if (flag && idx > node->n) {
            BNode *sibling = malloc(sizeof(BNode));
            readFile(tree, sibling, node->children[idx-1]);
            removeNode(tree, sibling, k);
            writeFile(tree, sibling, sibling->pos);
        }
        else
            removeNode(tree, c, k);
        
        writeFile(tree, c, c->pos);
        res = true;
    }
    return res;
}
 
int findKey(BNode* node, keyType* k) {
    int idx=0;
    while (idx < node->n && LT(node->records[idx].key,k))
        ++idx;
    return idx;
}

 


 

