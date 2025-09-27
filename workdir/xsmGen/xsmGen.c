#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../codeGen/codeGen.h"
#include <stdlib.h>
#include <stdio.h>

void setHeader(FILE* targetFile) {
    fprintf(targetFile, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}

void printToConsole(int regIndex, FILE* targetFile) {
    int freeReg = getFreeRegister();

    fprintf(
        targetFile,
        "MOV R%d, \"Write\"\nPUSH R%d\nMOV R%d, -2\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\n",
        freeReg, freeReg, freeReg, freeReg, regIndex, freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    fprintf(
        targetFile,
        "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n",
        freeReg, freeReg, freeReg, freeReg, freeReg
    );

    releaseRegister(freeReg);
}

void readFromConsole(char* varName, FILE* targetFile) {
    int addr = getStaticAddress(varName);

    int freeReg = getFreeRegister();
    int storeReg = getFreeRegister();
    fprintf(targetFile, "MOV R%d, %d\n", storeReg, addr);

    fprintf(
        targetFile,
        "MOV R%d, \"Read\"\nPUSH R%d\nMOV R%d, -1\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\n",
        freeReg, freeReg, freeReg, freeReg, storeReg, freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    fprintf(
        targetFile,
        "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n",
        freeReg, freeReg, freeReg, freeReg, freeReg
    );


    releaseRegister(freeReg);
    releaseRegister(storeReg);
}

void exitProgram(FILE* targetFile) {
    int freeReg = getFreeRegister();

    fprintf(
        targetFile,
        "MOV R%d, \"Exit\"\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\n",
        freeReg, freeReg, freeReg, freeReg, freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    fprintf(
        targetFile,
        "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n",
        freeReg, freeReg, freeReg, freeReg, freeReg
    );

    releaseRegister(freeReg);
}

void setVariableValue(char* varName, int storeReg, FILE* targetFile) {
    int addr = getStaticAddress(varName);
    fprintf(targetFile, "MOV [%d], R%d\n", addr, storeReg);
    releaseRegister(storeReg);
}

void getVariableValue(int storeReg, char* varName, FILE* targetFile) {
    int addr = getStaticAddress(varName);
    fprintf(targetFile, "MOV R%d, [%d]\n", storeReg, addr);
}

void getImmediateValue(int storeReg, int val, FILE* targetFile) {
    fprintf(targetFile, "MOV R%d, %d\n", storeReg, val);
}

void updateStackPointer(int addr, FILE* targetFile) {
    fprintf(targetFile, "MOV SP, %d\n", addr);
}

int movStrLtrlToReg(Tnode* node, FILE* targetFile) {
    int freeReg = getFreeRegister();
    fprintf(targetFile, "MOV R%d, %s\n", freeReg, node->strVal);
    return freeReg;
}

