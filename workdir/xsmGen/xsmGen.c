#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../codeGen/codeGen.h"
#include "xsmGen.h"
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

void getImmediateValue(int storeReg, int val, FILE* targetFile) {
    fprintf(targetFile, "MOV R%d, %d\n", storeReg, val);
}

void updateStackPointer(int addr, FILE* targetFile) {
    fprintf(targetFile, "MOV SP, %d\n", addr);
}

int movStrLtrlToReg(Tnode* node, FILE* targetFile) {
    int freeReg = getFreeRegister();

    if(node->tnodeType == NODE_DEREF) {
        int addrReg = getFreeRegister();
        fprintf(targetFile, "MOV R%d, [%d]\n", addrReg, node->left->gsTableEntry->binding);
        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        return addrReg;
    }
    
    if(node->tnodeType == NODE_ID) {
        freeReg = getMemValue(node, -1, -1, -1, targetFile);
    }
    else {
        fprintf(targetFile, "MOV R%d, %s\n", freeReg, node->strVal);
    }
    
    return freeReg;
}

int getEffectiveAddr(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile) {

    // offset is only used for 1d and rowreg and colreg are used for 2d

    int addrReg = getFreeRegister();
    int baseAddr = idNode->gsTableEntry->binding;
    int dimension = idNode->gsTableEntry->dimension;

    fprintf(targetFile, "MOV R%d, %d\n", addrReg, baseAddr);

    if(dimension == 0) {
        return addrReg;
    }
    else if(dimension == 1) {
        fprintf(targetFile, "ADD R%d, R%d\n", addrReg, rowReg);
        return addrReg;
    }
    else if(dimension == 2) {
        int temp = getFreeRegister();
        int numCols = idNode->gsTableEntry->numCols;
        fprintf(targetFile, "MOV R%d, %d\n", temp, numCols);
        fprintf(targetFile, "MUL R%d, R%d\n", rowReg, temp);
        fprintf(targetFile, "ADD R%d, R%d\n", rowReg, colReg);
        fprintf(targetFile, "ADD R%d, R%d\n", addrReg, rowReg);

        releaseRegister(temp);
        return addrReg;
    }
    else {
        printf("Error : wrong dimension\n");
        exit(1);
    }

    return -1;
}

void setMemValue(Tnode* idNode, int exprReg, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, offsetReg, rowReg, colReg, targetFile);
    
    fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, exprReg);
    releaseRegister(addrReg);
}

int getMemValue(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, offsetReg, rowReg, colReg, targetFile);
    int storeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, [R%d]\n", storeReg, addrReg);
    releaseRegister(addrReg);
    return storeReg;
}

void readFromConsole(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, offsetReg, rowReg, colReg, targetFile);
    int freeReg = getFreeRegister();

    fprintf(
        targetFile,
        "MOV R%d, \"Read\"\nPUSH R%d\nMOV R%d, -1\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\n",
        freeReg, freeReg, freeReg, freeReg, addrReg, freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    fprintf(
        targetFile,
        "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n",
        freeReg, freeReg, freeReg, freeReg, freeReg
    );

    releaseRegister(freeReg);
    releaseRegister(addrReg);
}



