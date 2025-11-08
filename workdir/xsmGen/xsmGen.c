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
    
    // loop for initialising heap

    int label1 = getNewLabel();
    int label2 = getNewLabel();
    int label3 = getNewLabel();

    fprintf(targetFile, "MOV R0, %d\n", HEAP_START);

    fprintf(targetFile, "L%d:\n", label1);

    fprintf(targetFile, "MOV R1, R0\n");
    fprintf(targetFile, "ADD R1, 8\n");
    fprintf(targetFile, "MOV R2, %d\n", HEAP_END);
    fprintf(targetFile, "LT R2, R1\n");
    fprintf(targetFile, "JNZ R2, L%d\n", label2);

    fprintf(targetFile, "MOV [R0], R1\n");
    fprintf(targetFile, "JMP L%d\n", label3);

    fprintf(targetFile, "L%d:\n", label2);
    fprintf(targetFile, "MOV [R0], -1\n");

    fprintf(targetFile, "L%d:\n", label3);
    fprintf(targetFile, "ADD R0, 8\n");
    fprintf(targetFile, "MOV R3, %d\n", HEAP_END);
    fprintf(targetFile, "MOV R1, R0\n");
    fprintf(targetFile, "LE R1, R3\n");
    fprintf(targetFile, "JNZ R1, L%d\n", label1);

    
    // setting up BP and SP

    fprintf(targetFile, "MOV BP, %d\n", nextBinding+2);
    fprintf(targetFile, "MOV SP, %d\n", nextBinding+2);

    // push space for local variables
    LsTableEntry* temp = lsTableHead;
    int freeReg = getFreeRegister();
    while(temp) {
        for(int i=0; i<temp->type->size; i++) 
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

    // for proper id and for pointers
    if(idNode->tnodeType == NODE_ID) {
        if(idNode->lsTableEntry) {
            fprintf(targetFile, "MOV R%d, BP\n", addrReg);
            fprintf(targetFile, "ADD R%d, %d\n", addrReg, idNode->lsTableEntry->binding);
        }
        else {
            fprintf(targetFile, "MOV R%d, %d\n", addrReg, idNode->gsTableEntry->binding);
        }
    }

    // for tuples 
    else if(idNode->tnodeType == NODE_TUP_FIELD) {
        releaseRegister(addrReg);

        int addrReg = getEffectiveAddr(idNode->left, targetFile);
        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);

        int offset = (searchInFieldList(idNode->left->type->fields, idNode->right->varName))->fieldIndex;
        fprintf(targetFile, "ADD R%d, %d\n", addrReg, offset);

        return addrReg;
    }

    // for dereference
    else if(idNode->tnodeType == NODE_DEREF) {
        if(idNode->left->lsTableEntry) {
            fprintf(targetFile, "MOV R%d, BP\n", addrReg);
            fprintf(targetFile, "ADD R%d, %d\n", addrReg, idNode->left->lsTableEntry->binding);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        }
        else {
            fprintf(targetFile, "MOV R%d, %d\n", addrReg, idNode->left->gsTableEntry->binding);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        }
    }



    // this condition only for arrays
    if(idNode->gsTableEntry && idNode->gsTableEntry->dimensions) {
        Tnode* dimNode = idNode->gsTableEntry->dimensions;
        Tnode* indexNode = idNode->arrOffset;
        int offsetReg = getFreeRegister();

        fprintf(targetFile, "MOV R%d, 0\n", offsetReg);

        while (dimNode && indexNode) {
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
    }
    
    return addrReg;
}

void readFromConsole(Tnode* idNode, FILE* targetFile) {
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

void functionEntryCodeGen(int label, FILE* targetFile) {

    // generate label for function
    fprintf(targetFile, "F%d:\n", label);

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

void functionExitCodeGen(FILE* targetFile) {
    fprintf(targetFile, "MOV SP, BP\n");
    fprintf(targetFile, "POP BP\n");
    fprintf(targetFile, "RET\n");
}

int xsmAlloc(FILE* targetFile) {

    int retReg = getFreeRegister();
    int reg1 = getFreeRegister();
    fprintf(targetFile, "MOV R%d, \"Alloc\"\n", reg1);
    fprintf(targetFile, "PUSH R%d\n", reg1);
    fprintf(targetFile, "PUSH R%d\n", reg1);
    fprintf(targetFile, "PUSH R%d\n", reg1);
    fprintf(targetFile, "PUSH R%d\n", reg1);
    fprintf(targetFile, "PUSH R%d\n", reg1);

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R%d\n", retReg);
    fprintf(targetFile, "POP R%d\n", reg1);
    fprintf(targetFile, "POP R%d\n", reg1);
    fprintf(targetFile, "POP R%d\n", reg1);
    fprintf(targetFile, "POP R%d\n", reg1);
    
    releaseRegister(reg1);
    return retReg;
}

void xsmFree(Tnode* tupNode, FILE* targetFile) {

    int addrReg = getEffectiveAddr(tupNode, targetFile);
    int freeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, \"Free\"\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", addrReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);

    releaseRegister(freeReg);
    releaseRegister(addrReg);
}

