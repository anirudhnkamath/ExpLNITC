#include "./codeGen.h"
#include "../registers/registers.h"
#include <stdlib.h>
#include <stdio.h>

int getStaticAddress(char* varName) {
    return STATIC_ALLOC_START + varName[0] - 'a';
}

void setHeader(FILE* targetFile) {
    fprintf(targetFile, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}

void printToConsole(reg_index_t regIndex, FILE* targetFile) {
    reg_index_t freeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, \"Write\"\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "MOV R%d, -2\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", regIndex);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);

    releaseRegister(freeReg);
}

void readFromConsole(char* varName, FILE* targetFile) {
    int addr = getStaticAddress(varName);

    reg_index_t freeReg = getFreeRegister();

    reg_index_t storeReg = getFreeRegister();
    fprintf(targetFile, "MOV R%d, %d\n", storeReg, addr);

    fprintf(targetFile, "MOV R%d, \"Read\"\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "MOV R%d, -1\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", storeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);

    fprintf(targetFile, "CALL 0\n");

    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);
    fprintf(targetFile, "POP R%d\n", freeReg);

    releaseRegister(freeReg);
    releaseRegister(storeReg);
}

void exitProgram(FILE* targetFile) {
    reg_index_t freeReg = getFreeRegister();

    fprintf(targetFile, "MOV R%d, \"Exit\"\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
    fprintf(targetFile, "PUSH R%d\n", freeReg);
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
}

void setVariableValue(char* varName, reg_index_t storeReg, FILE* targetFile) {
    int addr = STATIC_ALLOC_START + varName[0] - 'a';
    fprintf(targetFile, "MOV [%d], R%d\n", addr, storeReg);
    releaseRegister(storeReg);
}

void getVariableValue(reg_index_t storeReg, char* varName, FILE* targetFile) {
    int addr = STATIC_ALLOC_START + varName[0] - 'a';
    fprintf(targetFile, "MOV R%d, [%d]\n", storeReg, addr);
}

void getImmediateValue(reg_index_t storeReg, int val, FILE* targetFile) {
    fprintf(targetFile, "MOV R%d, %d\n", storeReg, val);
}

void updateStackPointer(int addr, FILE* targetFile) {
    fprintf(targetFile, "MOV SP, %d\n", addr);
}

reg_index_t codeGen(Tnode* node, FILE* targetFile) {
    if(node == NULL)
        return E_INVALIDNODE;

    switch(node->tnodeType) {
        case NODE_CONNECTOR :
            codeGen(node->left, targetFile);
            codeGen(node->right, targetFile);
            break;

        case NODE_WRITE : {
            reg_index_t freeReg = evaluateExpression(node->left, targetFile);
            printToConsole(freeReg, targetFile);
            break;
        }

        case NODE_READ : {
            readFromConsole(node->left->varName, targetFile);
            break;
        }

        case NODE_ASSIGN : {
            reg_index_t freeReg = evaluateExpression(node->right, targetFile);
            setVariableValue(node->left->varName, freeReg, targetFile);
            break;
        }

        default:
            break;
    }

    return -1;
}

reg_index_t evaluateExpression(Tnode* node, FILE* targetFile) {

    if(node == NULL)
        return E_INVALIDNODE;

    if(node->tnodeType == NODE_ID_INT) {
        reg_index_t freeReg = getFreeRegister();
        getVariableValue(freeReg, node->varName, targetFile);
        return freeReg;
    }

    if(node->tnodeType == NODE_INT) {
        reg_index_t freeReg = getFreeRegister();
        getImmediateValue(freeReg, node->val, targetFile);
        return freeReg;
    }

    reg_index_t leftReg = evaluateExpression(node->left, targetFile);
    reg_index_t rightReg = evaluateExpression(node->right, targetFile);

    switch(node->tnodeType) {
        case NODE_ADD :
            fprintf(targetFile, "ADD R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_SUB :
            fprintf(targetFile, "SUB R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_DIV :
            fprintf(targetFile, "DIV R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_MULT :
            fprintf(targetFile, "MUL R%d, R%d\n", leftReg, rightReg);
            break;

        default :
            break;
    }

    releaseRegister(rightReg);
    return leftReg;
}

