#include <stdlib.h>
#include "node.h"

Node* makeLeafNode(int n) {
    Node* temp = (Node*)malloc(sizeof(Node));

    temp->op = 0;
    temp->val = n;

    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

Node* makeOperatorNode(char c, Node* l, Node* r) {
    Node* temp = (Node*)malloc(sizeof(Node));

    temp->op = c;
    temp->left = l;
    temp->right = r;

    return temp;
}

int evaluateTree(Node* t) {
    if (t->left == NULL && t->right == NULL) {
        return t->val;
    }
	else {
        switch (t->op) {
            case '+': return evaluateTree(t->left) + evaluateTree(t->right);
            case '-': return evaluateTree(t->left) - evaluateTree(t->right);
            case '*': return evaluateTree(t->left) * evaluateTree(t->right);
            case '/': return evaluateTree(t->left) / evaluateTree(t->right);
            default: return 0;
        }
    }
}
