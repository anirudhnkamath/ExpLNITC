#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../codeGen/codeGen.h"
#include "../typeTable/typeTable.h"
#include "xsmGen.h"
#include <stdlib.h>
#include <stdio.h>

void setHeader(FILE* targetFile) {
    fprintf(targetFile, "0\nMAIN\n0\n0\n0\n0\n0\n0\n");
}

void initialiseMainFn(FILE* targetFile) {
    fprintf(targetFile, "MAIN:\n");
    fprintf(targetFile, "MOV BP, %d\n", nextBinding+2);
    fprintf(targetFile, "MOV SP, %d\n", nextBinding+2);

    // push space for local variables
    LsTableEntry* temp = lsTableHead;
    int freeReg = getFreeRegister();
    while(temp) {
        fprintf(targetFile, "PUSH R%d\n", freeReg);
        temp = temp->next;
    }
    releaseRegister(freeReg);
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

int getEffectiveAddr(Tnode* idNode, FILE* targetFile) {

    int addrReg = getFreeRegister();

    if(idNode->tnodeType == NODE_TUP_FIELD) {
        fprintf(targetFile, "MOV R%d, %d\n", addrReg, idNode->left->gsTableEntry->binding);
        fprintf(targetFile, "ADD R%d, %d\n", addrReg, searchInFieldList(idNode->left->gsTableEntry->type->fields, idNode->right->varName)->fieldIndex);
        return addrReg;
    }

    if (idNode->lsTableEntry) {
        fprintf(targetFile, "MOV R%d, BP\n", addrReg);
        fprintf(targetFile, "ADD R%d, %d\n", addrReg, idNode->lsTableEntry->binding);
        return addrReg;
    }

    fprintf(targetFile, "MOV R%d, %d\n", addrReg, idNode->gsTableEntry->binding);

    if (!idNode->gsTableEntry->dimensions) {
        return addrReg;
    }

    Tnode* dimNode = idNode->gsTableEntry->dimensions;
    Tnode* indexNode = idNode->arrOffset;
    int offsetReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, 0\n", offsetReg);

    while (dimNode && indexNode) {
        // got index
        int indexValReg = evaluateExpression(indexNode, targetFile);

        int mult = 1;
        Tnode* temp = dimNode->arrOffset;
        while(temp) {
            mult *= dimNode->val;
            temp = temp->arrOffset;
        }

        fprintf(targetFile, "MUL R%d, %d\n", indexValReg, mult);
        fprintf(targetFile, "ADD R%d, R%d\n", offsetReg, indexValReg);

        releaseRegister(indexValReg);
        dimNode = dimNode->arrOffset;
        indexNode = indexNode->arrOffset;
    }

    fprintf(targetFile, "ADD R%d, R%d\n", addrReg, offsetReg);

    releaseRegister(offsetReg);

    return addrReg;
}

void setMemValue(Tnode* idNode, int exprReg, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, targetFile);
    
    fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, exprReg);
    releaseRegister(addrReg);
}

int getMemValue(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, targetFile);
    int storeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, [R%d]\n", storeReg, addrReg);
    releaseRegister(addrReg);
    return storeReg;
}

void readFromConsole(Tnode* idNode, int offsetReg, int rowReg, int colReg, FILE* targetFile) {
    int addrReg = getEffectiveAddr(idNode, targetFile);
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

void functionEntryCodeGen(Tnode* node, FILE* targetFile) {

    // generate label for function
    fprintf(targetFile, "F%d:\n", node->gsTableEntry->fLabel);

    // save old bp and move bp to sp
    fprintf(targetFile, "PUSH BP\n");
    fprintf(targetFile, "MOV BP, SP\n");

    // push space for local variables
    LsTableEntry* temp = lsTableHead;
    int freeReg = getFreeRegister();
    while(temp) {
        fprintf(targetFile, "PUSH R%d\n", freeReg);
        temp = temp->next;
    }
    releaseRegister(freeReg);
}

void functionExitCodeGen(Tnode* node, FILE* targetFile) {
    fprintf(targetFile, "MOV SP, BP\n");
    fprintf(targetFile, "POP BP\n");
    fprintf(targetFile, "RET\n");
}

