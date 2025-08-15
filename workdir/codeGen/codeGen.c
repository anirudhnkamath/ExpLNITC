#include "./codeGen.h"
#include "../registers/registers.h"
#include <stdlib.h>
#include <stdio.h>

Tnode* createLeafNode(int val) {
    Tnode* node = (Tnode*)malloc(sizeof(Tnode));

    node->tnodeType = INTEGER_NODE;
    node->val = val;
    node->left = node->right = NULL;

    return node;
}

Tnode* createInternalNode(char op, Tnode* left, Tnode* right) {
    Tnode* node = (Tnode*)malloc(sizeof(Tnode));

    node->tnodeType = OPERATOR_NODE;
    node->op = op;
    node->left = left;
    node->right = right;

    return node;
}

void setHeader(FILE* targetFile) {
    fprintf(targetFile, "0\n2056\n0\n0\n0\n0\n0\n0\n");
}

reg_index_t codeGen(Tnode* node, FILE* targetFile) {

    if(node->tnodeType == INTEGER_NODE) {
        reg_index_t freeReg = getFreeRegister();
        if(freeReg == E_REGFULL) {
            printf("Registers full\n");
            exit(1);
        }

        fprintf(targetFile, "MOV R%d, %d\n", freeReg, node->val);
        return freeReg;
    }

    reg_index_t leftReg = codeGen(node->left, targetFile);
    reg_index_t rightReg = codeGen(node->right, targetFile);

    if(leftReg == E_REGFULL || rightReg == E_REGFULL) {
        printf("Registers full\n");
        exit(1);
    }

    switch(node->op) {
        case '+' :
            fprintf(targetFile, "ADD R%d, R%d\n", leftReg, rightReg);
            break;

        case '-' :
            fprintf(targetFile, "SUB R%d, R%d\n", leftReg, rightReg);
            break;

        case '*' :
            fprintf(targetFile, "MUL R%d, R%d\n", leftReg, rightReg);
            break;

        case '/' :
            fprintf(targetFile, "DIV R%d, R%d\n", leftReg, rightReg);
            break;

        default :
            break;
    }

    releaseRegister(rightReg);
    return leftReg;
}

void printToConsole(reg_index_t regIndex, FILE* targetFile) {
    reg_index_t freeReg = getFreeRegister();
    if(freeReg == E_REGFULL) {
        printf("Registers full\n");
        exit(1);
    }

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

void exitProgram(FILE* targetFile) {
    reg_index_t freeReg = getFreeRegister();
    if(freeReg == E_REGFULL) {
        printf("Registers full\n");
        exit(1);
    }

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