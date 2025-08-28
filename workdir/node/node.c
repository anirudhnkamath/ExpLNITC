#include "./node.h"
#include "../define/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tnode* createTnode(int tnodeType, int type, int val, char* varName, Tnode* left, Tnode* right) {
    Tnode* node = (Tnode*)malloc(sizeof(Tnode));
    
    node->tnodeType = tnodeType;
    node->val = val;
    node->varName = (varName == NULL ? NULL : strdup(varName));
    node->left = left;
    node->right = right;

    int curType = validateExpressionType(node, left, right);
    node->type = curType;

    return node;
}

int validateExpressionType(Tnode* root, Tnode* left, Tnode* right) {

    if(root->tnodeType == NODE_ID_INT || root->tnodeType == NODE_INT)
        return INTEGER_TYPE;

    if(
        root->tnodeType == NODE_ADD ||
        root->tnodeType == NODE_SUB ||
        root->tnodeType == NODE_MULT ||
        root->tnodeType == NODE_DIV
    ) {
        if(left->type != INTEGER_TYPE || right->type != INTEGER_TYPE) {
            printf("Error: Type mismatch\n");
            exit(1);
        }

        return INTEGER_TYPE;
    }

    else if (
        root->tnodeType == NODE_EQ ||
        root->tnodeType == NODE_NEQ ||
        root->tnodeType == NODE_GT ||
        root->tnodeType == NODE_GTE ||
        root->tnodeType == NODE_LT ||
        root->tnodeType == NODE_LTE
    ) {
        if (left->type != INTEGER_TYPE || right->type != INTEGER_TYPE) {
            printf("Error: Type mismatch\n");
            exit(1);
        }
        return BOOLEAN_TYPE;
    }

    int valid = 1;

    if (
        (root->tnodeType == NODE_WRITE && left->type == BOOLEAN_TYPE) ||
        (root->tnodeType == NODE_ASSIGN && right->type == BOOLEAN_TYPE)
    ) {
        valid = 0;
    }

    else if (
        root->tnodeType == NODE_IF || 
        root->tnodeType == NODE_IF_ELSE || 
        root->tnodeType == NODE_WHILE
    ) {
        if (left->type != BOOLEAN_TYPE)
            valid = 0;
    }

    if(valid == 0) {
        printf("Error: Type mismatch\n");
        exit(1);
    }

    return NO_TYPE;
}