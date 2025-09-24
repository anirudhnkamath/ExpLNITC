#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "../define/constants.h"
#include "../node/node.h"

extern int newLabel;
extern int curLoopStartLabel;
extern int curLoopEndLabel;
int getNewLabel();

int getStaticAddress(char* varName);

int codeGen(Tnode* node, FILE* targetFile);
int evaluateExpression(Tnode* node, FILE* targetFile);

void setHeader(FILE* targetFile);
void readFromConsole(char* varName, FILE* targetFile);
void printToConsole(int regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);
void updateStackPointer(int addr, FILE* targetFile);

void setVariableValue(char* varName, int storeReg, FILE* targetFile);
void getVariableValue(int storeReg, char* varName, FILE* targetFile);
void getImmediateValue(int storeReg, int val, FILE* targetFile);

void generateWhileLoop(Tnode* node, FILE* targetFile);
void generateIfThen(Tnode* node, FILE* targetFile);
void generateIfElse(Tnode* node, FILE* targetFile);
void generateRepeatUntil(Tnode* node, FILE* targetFile);
void generateDoWhile(Tnode* node, FILE* targetFile);

#endif