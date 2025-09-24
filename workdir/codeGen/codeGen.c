#include "./codeGen.h"
#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include <stdlib.h>
#include <stdio.h>

int curLoopStartLabel = -1, curLoopEndLabel = -1, newLabel = 0;;

int getNewLabel() {
    int ret = newLabel;
    newLabel += 1;
    return ret;
}

int getStaticAddress(char* varName) {
    GsTableEntry* found = findInGsTable(gsTableHead, varName);
    if(!found) {
        printf("Error: undeclared variable\n");
        exit(1);
    }

    return found->binding;
}

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

int codeGen(Tnode* node, FILE* targetFile) {
    if(node == NULL)
        return E_INVALIDNODE;

    switch(node->tnodeType) {
        case NODE_CONNECTOR :
            codeGen(node->left, targetFile);
            codeGen(node->right, targetFile);
            break;

        case NODE_WRITE : {
            int freeReg = evaluateExpression(node->left, targetFile);
            printToConsole(freeReg, targetFile);
            break;
        }

        case NODE_READ : {
            readFromConsole(node->left->varName, targetFile);
            break;
        }

        case NODE_ASSIGN : {
            int freeReg = evaluateExpression(node->right, targetFile);
            setVariableValue(node->left->varName, freeReg, targetFile);
            break;
        }

        case NODE_WHILE : {
            generateWhileLoop(node, targetFile);
            break;
        }

        case NODE_IF : {
            generateIfThen(node, targetFile);
            break;
        }

        case NODE_IF_ELSE : {
            generateIfElse(node, targetFile);
            break;
        }

        case NODE_DOWHILE : {
            generateDoWhile(node, targetFile);
            break;
        }

        case NODE_REPEATUNTIL : {
            generateRepeatUntil(node, targetFile);
            break;
        }

        case NODE_BREAK : {
            fprintf(targetFile, "JMP L%d\n", curLoopEndLabel);
            break;
        }

        case NODE_CONTINUE : {
            fprintf(targetFile, "JMP L%d\n", curLoopStartLabel);
            break;
        }

        default:
            break;
    }

    return -1;
}

void generateWhileLoop(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    curLoopStartLabel = label1;
    curLoopEndLabel = label2;

    fprintf(targetFile, "L%d:\n", label1);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label2);

    codeGen(node->right, targetFile);
    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    curLoopStartLabel = -1;
    curLoopEndLabel = -1;
}

void generateIfThen(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label1);

    codeGen(node->right, targetFile);
    fprintf(targetFile, "L%d:\n", label1);

    releaseRegister(flagRegIndex);
}

void generateIfElse(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    int flagRegIndex = evaluateExpression(node->left, targetFile);

    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label1);

    codeGen(node->right->left, targetFile);
    fprintf(targetFile, "JMP L%d\n", label2);

    fprintf(targetFile, "L%d:\n", label1);
    codeGen(node->right->right, targetFile);

    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);
}

void generateDoWhile(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    curLoopStartLabel = label1;
    curLoopEndLabel = label2;

    fprintf(targetFile, "L%d:\n", label1);
    codeGen(node->right, targetFile);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label2);

    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    curLoopStartLabel = -1;
    curLoopEndLabel = -1;
}

void generateRepeatUntil(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    curLoopStartLabel = label1;
    curLoopEndLabel = label2;

    fprintf(targetFile, "L%d:\n", label1);
    codeGen(node->right, targetFile);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JNZ R%d, L%d\n", flagRegIndex, label2);

    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    curLoopStartLabel = -1;
    curLoopEndLabel = -1;
}

int evaluateExpression(Tnode* node, FILE* targetFile) {

    if(node == NULL)
        return E_INVALIDNODE;

    if(node->tnodeType == NODE_ID) {
        int freeReg = getFreeRegister();
        getVariableValue(freeReg, node->varName, targetFile);
        return freeReg;
    }

    if(node->tnodeType == NODE_INT) {
        int freeReg = getFreeRegister();
        getImmediateValue(freeReg, node->val, targetFile);
        return freeReg;
    }

    int leftReg = evaluateExpression(node->left, targetFile);
    int rightReg = evaluateExpression(node->right, targetFile);

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

        case NODE_EQ :
            fprintf(targetFile, "EQ R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_NEQ :
            fprintf(targetFile, "NE R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_GT :
            fprintf(targetFile, "GT R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_GTE :
            fprintf(targetFile, "GE R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_LT :
            fprintf(targetFile, "LT R%d, R%d\n", leftReg, rightReg);
            break;

        case NODE_LTE :
            fprintf(targetFile, "LE R%d, R%d\n", leftReg, rightReg);
            break;

        default :
            break;
    }

    releaseRegister(rightReg);
    return leftReg;
}

