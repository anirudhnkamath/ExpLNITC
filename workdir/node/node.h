#ifndef NODE_H
#define NODE_H

typedef struct Tnode {
    int tnodeType;
    int val;
    char* varName;
    struct Tnode* left;
    struct Tnode* right;
} Tnode;

Tnode* createTnode(int tnodeType, int val, char* varName, Tnode* left, Tnode* right);
 
#endif
