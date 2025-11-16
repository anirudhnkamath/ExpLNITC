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

void xsmInit(FILE* targetFile) {
    int freeReg1 = getFreeRegister();
    fprintf(targetFile, "MOV R%d, \"Init\"\n", freeReg1);
    fprintf(targetFile, "PUSH R%d\n", freeReg1);
    releaseRegister(freeReg1);

    fprintf(targetFile, "ADD SP, 4\n");
    fprintf(targetFile, "CALL 0\n");
    fprintf(targetFile, "SUB SP, 5\n");
}

void initialiseMainFn(FILE* targetFile) {
    fprintf(targetFile, "MAIN:\n");

    // initialise
    xsmInit(targetFile);

    // setting up BP and SP
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

    // set v table entries
    initialiseVTable(targetFile);
}

void printToConsole(int regIndex, FILE* targetFile) {
    int freeReg = getFreeRegister();

    fprintf(
        targetFile,
        "MOV R%d, \"Write\"\nPUSH R%d\nMOV R%d, -2\nPUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\n",
        freeReg, freeReg, freeReg, freeReg, regIndex, freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    // dont waste lines
    fprintf(
        targetFile,
        "SUB SP, 5\n"
    );

    releaseRegister(freeReg);
}

void exitProgram(FILE* targetFile) {
    int freeReg = getFreeRegister();

    fprintf(
        targetFile,
        "MOV R%d, \"Exit\"\nPUSH R%d\nADD SP, 4\n",
        freeReg, freeReg
    );

    fprintf(targetFile, "CALL 0\n");

    // dont waste lines
    fprintf(
        targetFile,
        "SUB SP, 5\n"
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

        if(idNode->right->tnodeType == NODE_ID) {
            int offset;

            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);

            // class.id
            if(idNode->left->class)
                offset = (searchInClsFld(idNode->left->class->fieldList, idNode->right->varName))->index;

            // type.id
            else 
                offset = (searchInFieldList(idNode->left->type->fields, idNode->right->varName))->fieldIndex;

            fprintf(targetFile, "ADD R%d, %d\n", addrReg, offset);
        }

        // weird
        else if(idNode->right->tnodeType == NODE_FN_CALL) {
            int vtableReg = getFreeRegister();
            fprintf(targetFile, "MOV R%d, R%d\n", vtableReg, addrReg);
            fprintf(targetFile, "ADD R%d, 1\n", vtableReg);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);

            int freeReg = evaluateMethod(idNode->right, addrReg, vtableReg, targetFile);
            fprintf(targetFile, "MOV R%d, R%d\n", addrReg, freeReg);
            releaseRegister(freeReg);
            releaseRegister(vtableReg);
        }
        
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

    // dont waste lines
    fprintf(
        targetFile,
        "SUB SP, 5\n"
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
    fprintf(targetFile, "ADD SP, 4\n");

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R%d\n", retReg);
    fprintf(targetFile, "SUB SP, 4\n");
    
    releaseRegister(reg1);
    return retReg;
}

void xsmFree(Tnode* tupNode, FILE* targetFile) {

    int addrReg = getEffectiveAddr(tupNode, targetFile);
    int freeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, \"Free\"\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", addrReg);
    fprintf(targetFile, "ADD SP, 3\n");

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "SUB SP, 5\n");

    releaseRegister(freeReg);
    releaseRegister(addrReg);
}

void initialiseVTable(FILE* targetFile) {

    // set up v table entries

    ClassTable* class = classTableHead;
    while(class) {
        int base = STACK_START + (class->index * MAX_FIELDS);

        ClsMthdList* method = class->mthdList;
        while(method) {
            int addr = base + method->index;
            fprintf(targetFile, "MOV [%d], F%d\n", addr, method->fLabel);
            method = method->next;
        }

        class = class->next;
    }

    // set up v table pointers for each class variable

    GsTableEntry* var = gsTableHead;
    while(var) {
        // dont do for non-class variables
        if(!var->class) {
            var = var->next;
            continue;
        }

        int varAddr = var->binding + 1;
        int vTableAddr = STACK_START + (var->class->index * MAX_FIELDS);

        int reg1 = getFreeRegister();

        fprintf(targetFile, "MOV [%d], %d\n", varAddr, vTableAddr);

        releaseRegister(reg1);
        var = var->next;
    }
}
