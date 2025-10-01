#ifndef XSMGEN_H
#define XSMGEN_H

#include <stdio.h>

void setHeader(FILE* targetFile);

void readFromConsole(char* varName, FILE* targetFile);
void printToConsole(int regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);

void updateStackPointer(int addr, FILE* targetFile);

void setVariableValue(char* varName, int storeReg, FILE* targetFile);
void getVariableValue(int storeReg, char* varName, FILE* targetFile);
void getImmediateValue(int storeReg, int val, FILE* targetFile);

void setArrIndexValue(char* varName, int indexReg, int exprReg, FILE* targetFile);
void readArrIndex(char* varName, int indexReg, FILE* targetFile);

int movStrLtrlToReg(Tnode* node, FILE* targetFile);



#endif