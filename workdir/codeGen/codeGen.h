#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "../define/constants.h"
#include "../node/node.h"

int getStaticAddress(char* varName);

int codeGen(Tnode* node, FILE* targetFile);
int evaluateExpression(Tnode* node, FILE* targetFile);

void generateWhileLoop(Tnode* node, FILE* targetFile);
void generateIfThen(Tnode* node, FILE* targetFile);
void generateIfElse(Tnode* node, FILE* targetFile);
void generateRepeatUntil(Tnode* node, FILE* targetFile);
void generateDoWhile(Tnode* node, FILE* targetFile);

void generateWriteCode(Tnode* node, FILE* targetFile);
void generateReadCode(Tnode* node, FILE* targetFile);
void generateAssignCode(Tnode* lhs, Tnode* rhs, FILE* targetFile);


#endif