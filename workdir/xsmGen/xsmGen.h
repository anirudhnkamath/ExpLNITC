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

void readFromConsole(Tnode* idNode, FILE* targetFile);
void printToConsole(int regIndex, FILE* targetFile);
void exitProgram(FILE* targetFile);

int getEffectiveAddr(Tnode* idNode, FILE* targetFile);

void functionEntryCodeGen(int label, FILE* targetFile);
void functionExitCodeGen(FILE* targetFile);

int xsmAlloc(FILE* targetFile);
void xsmFree(Tnode* node, FILE* targetFile);
void xsmInit(FILE* targetFile);

void initialiseVTable(FILE* targetFile);

#endif