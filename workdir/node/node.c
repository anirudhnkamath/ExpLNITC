#include "./node.h"
#include "../define/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inorder(Tnode* root) {
    if(root == NULL)
        return;

    inorder(root->left);
    printf("%d\n", root->tnodeType);
    inorder(root->right);
}

Tnode* createTnode(int tnodeType, int val, char* varName, Tnode* left, Tnode* right) {
    Tnode* node = (Tnode*)malloc(sizeof(Tnode));
    
    node->tnodeType = tnodeType;
    node->val = val;
    node->varName = (varName == NULL ? NULL : strdup(varName));
    node->left = left;
    node->right = right;

    return node;
}
