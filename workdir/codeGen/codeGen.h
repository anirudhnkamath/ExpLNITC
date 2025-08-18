#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "../define/constants.h"
#include "../node/node.h"

int getStaticAddress(char* varName);

reg_index_t codeGen(Tnode* node, FILE* targetFile);
reg_index_t evaluateExpression(Tnode* node, FILE* targetFile);

void setHeader(FILE* targetFile);
void readFromConsole(char* varName, FILE* targetFile);
void printToConsole(reg_index_t regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);
void updateStackPointer(int addr, FILE* targetFile);

void setVariableValue(char* varName, reg_index_t storeReg, FILE* targetFile);
void getVariableValue(reg_index_t storeReg, char* varName, FILE* targetFile);
void getImmediateValue(reg_index_t storeReg, int val, FILE* targetFile);

#endif