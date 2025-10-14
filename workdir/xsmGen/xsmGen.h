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

void getImmediateValue(int storeReg, int val, FILE* targetFile);

void updateStackPointer(int addr, FILE* targetFile);

int getEffectiveAddr(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile);

void setMemValue(Tnode* idNode, int exprReg, int offsetReg, int rowReg, int colReg, FILE* targetFile);
int getMemValue(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile);

int pushAllRegisters(int pushedRegisters[], FILE* targetFile);
void popAllRegisters(int pushedRegisters[], int count, FILE* targetFile);

int pushAllArguments(Tnode* argListHead, FILE* targetFile);
int pushLocalVariables(LsTableEntry* lsTableHead, FILE* targetFile);

void functionEntryCodeGen(Tnode* node, FILE* targetFile);
void functionExitCodeGen(Tnode* node, FILE* targetFile);

#endif