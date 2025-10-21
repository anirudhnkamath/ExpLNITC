#ifndef XSMGEN_H
#define XSMGEN_H

#include <stdio.h>

#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../codeGen/codeGen.h"
#include <stdlib.h>
#include <stdio.h>

void setHeader(FILE* targetFile);
void initialiseMainFn(FILE* targetFile);

void readFromConsole(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile);
void printToConsole(int regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);

int getEffectiveAddr(Tnode* idNode, FILE* targetFile);

void setMemValue(Tnode* idNode, int exprReg, int offsetReg, int rowReg, int colReg, FILE* targetFile);
int getMemValue(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile);

void functionEntryCodeGen(Tnode* node, FILE* targetFile);
void functionExitCodeGen(Tnode* node, FILE* targetFile);

#endif