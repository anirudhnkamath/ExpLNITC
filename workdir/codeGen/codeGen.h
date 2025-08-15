#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "../define/constants.h"

typedef struct Tnode {
    int tnodeType;
    char op;
    int val;
    struct Tnode* left;
    struct Tnode* right;
} Tnode;

Tnode* createLeafNode(int val);
Tnode* createInternalNode(char op, Tnode* left, Tnode* right);

void setHeader(FILE* targetFile);
reg_index_t codeGen(Tnode* node, FILE* targetFile);
void printToConsole(reg_index_t regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);

#endif