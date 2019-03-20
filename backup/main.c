#include "btree.h"
#include <stdio.h>

void INSERT(BTree *tree, const char *key, const char *val)
{
    KVPair kv;
    strcpy(kv.key, key);
    strcpy(kv.val, val);
    insert(tree, &kv);
}

valType *SEARCH(BTree *tree, keyType *key)
{
    KVPair *kv = search(tree, key);
    if (kv != NULL)
        return kv->val;
    else
    {
        printf("Khong tim thay");
    }
    
}

void TRAVERSE(BTree *tree, int root)
{
    traverse(tree, root);
}

int main()
{

    BTree *tree = BTreeInit("data.dat", true);

    // INSERT(tree, "thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "anhnq6", "Nguyen Quoc Anh");

    // INSERT(tree, "baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "nghiatdh", "Ta Dang Hieu Nghia");

    // INSERT(tree, "1thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "1anhnq6", "Nguyen Quoc Anh");
    // INSERT(tree, "1baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "1nghiatdh", "Ta Dang Hieu Nghia");

    // INSERT(tree, "2thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "2anhnq6", "Nguyen Quoc Anh");
    // INSERT(tree, "2baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "2nghiatdh", "Ta Dang Hieu Nghia");

    // INSERT(tree, "3thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "3anhnq6", "Nguyen Quoc Anh");
    // INSERT(tree, "3baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "3nghiatdh", "Ta Dang Hieu Nghia");

    // INSERT(tree, "4thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "4anhnq6", "Nguyen Quoc Anh");
    // INSERT(tree, "4baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "4nghiatdh", "Ta Dang Hieu Nghia4");

    // INSERT(tree, "5thoainh", "Nguyen Huynh Thoai");
    // INSERT(tree, "5anhnq6", "Nguyen Quoc Anh");
    // INSERT(tree, "5baonlc", "Nguyen Le Chi Bao");
    // INSERT(tree, "5nghiatdh", "Ta Dang Hieu Nghia");

    // removeFromTree(tree, "4anhnq6");

    TRAVERSE(tree, tree->root);

    BTreeDestroy(tree);

    return 0;
}