#include "./codeGen.h"
#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../xsmGen/xsmGen.h"
#include <stdlib.h>
#include <stdio.h>

int codeGen(Tnode* node, FILE* targetFile) {
    if(node == NULL)
        return E_INVALIDNODE;

    switch(node->tnodeType) {
        case NODE_CONNECTOR :
            codeGen(node->left, targetFile);
            codeGen(node->right, targetFile);
            break;

        case NODE_WRITE : {
            generateWriteCode(node->left, targetFile);
            break;
        }

        case NODE_READ : {
            generateReadCode(node->left, targetFile);
            break;
        }

        case NODE_ASSIGN : {
            generateAssignCode(node->left, node->right, targetFile);
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
            if(peekLabelStack()) {
                fprintf(targetFile, "JMP L%d\n", peekLabelStack()->endLabel);
            }
            break;
        }

        case NODE_CONTINUE : {
            if(peekLabelStack()) {
                fprintf(targetFile, "JMP L%d\n", peekLabelStack()->startLabel);
            }
            break;
        }

        case NODE_RET : {
            int freeReg = evaluateExpression(node->left, targetFile);

            int addrReg = getFreeRegister();
            fprintf(targetFile, "MOV R%d, BP\n", addrReg);
            fprintf(targetFile, "ADD R%d, -2\n", addrReg);
            fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, freeReg);

            releaseRegister(freeReg);
            releaseRegister(addrReg);
            break;
        }

        default:
            break;
    }

    return -1;
}

void generateWriteCode(Tnode* node, FILE* targetFile) {

    int exprReg = evaluateExpression(node, targetFile);

    printToConsole(exprReg, targetFile);
    releaseRegister(exprReg);
}

void generateReadCode(Tnode* node, FILE* targetFile) {
    readFromConsole(node, _NA_, _NA_, _NA_, targetFile);
}

void generateAssignCode(Tnode* lhs, Tnode* rhs, FILE* targetFile) {

    // p = &x;
    if(rhs->tnodeType == NODE_ADDR_TO) {
        int exprReg = getFreeRegister();

        if(rhs->left->lsTableEntry) {
            fprintf(targetFile, "MOV R%d,  BP\n", exprReg);
            fprintf(targetFile, "ADD R%d, %d\n", exprReg, rhs->left->lsTableEntry->binding);
        }
        else {
            fprintf(targetFile, "MOV R%d, %d\n", exprReg, rhs->left->gsTableEntry->binding);
        }

        setMemValue(lhs, exprReg, _NA_, _NA_, _NA_, targetFile);
        releaseRegister(exprReg);

        return;
    }
    

    // calculate rhs
    int exprReg = evaluateExpression(rhs, targetFile);

    

    // assign to lhs
    if(lhs->tnodeType == NODE_ID) {
        setMemValue(lhs, exprReg, _NA_, _NA_, _NA_, targetFile);
    }
    else if(lhs->tnodeType == NODE_DEREF) {
        int addrReg = getFreeRegister();

        if(lhs->left->lsTableEntry) {
            fprintf(targetFile, "MOV R%d, BP\n", addrReg);
            fprintf(targetFile, "ADD R%d, %d\n", addrReg, lhs->left->lsTableEntry->binding);
            fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        }
        else {
            fprintf(targetFile, "MOV R%d, [%d]\n", addrReg, lhs->left->gsTableEntry->binding);
        }

        fprintf(targetFile, "MOV [R%d], R%d\n", addrReg, exprReg);
        releaseRegister(addrReg);
    }
    else if(lhs->tnodeType == NODE_TUP_FIELD) {
        setMemValue(lhs, exprReg, _NA_, _NA_, _NA_, targetFile);
    }

    releaseRegister(exprReg);
}

void generateWhileLoop(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    addToLabelStack(label1, label2);

    fprintf(targetFile, "L%d:\n", label1);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label2);

    codeGen(node->right, targetFile);
    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    popLabelStack();
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

    addToLabelStack(label1, label2);

    fprintf(targetFile, "L%d:\n", label1);
    codeGen(node->right, targetFile);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JZ R%d, L%d\n", flagRegIndex, label2);

    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    popLabelStack();
}

void generateRepeatUntil(Tnode* node, FILE* targetFile) {
    int label1 = getNewLabel();
    int label2 = getNewLabel();

    addToLabelStack(label1, label2);

    fprintf(targetFile, "L%d:\n", label1);
    codeGen(node->right, targetFile);

    int flagRegIndex = evaluateExpression(node->left, targetFile);
    fprintf(targetFile, "JNZ R%d, L%d\n", flagRegIndex, label2);

    fprintf(targetFile, "JMP L%d\n", label1);
    fprintf(targetFile, "L%d:\n", label2);

    releaseRegister(flagRegIndex);

    popLabelStack();
}

int evaluateExpression(Tnode* node, FILE* targetFile) {

    if(node == NULL)
        return E_INVALIDNODE;

    if(node->tnodeType == NODE_DEREF) {
        int addrReg = getFreeRegister();

        if(node->left->lsTableEntry) {
            fprintf(targetFile, "MOV R%d, BP\n", addrReg);
            fprintf(targetFile, "ADD R%d, %d\n", addrReg, node->left->lsTableEntry->binding);
        }
        else {
            fprintf(targetFile, "MOV R%d, [%d]\n", addrReg, node->left->gsTableEntry->binding);
        }

        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        return addrReg;
    }

    if(node->tnodeType == NODE_ADDR_TO) {
        int freeReg = getFreeRegister();

        if(node->left->lsTableEntry) {
            fprintf(targetFile, "MOV R%d,  BP\n", freeReg);
            fprintf(targetFile, "ADD R%d, %d\n", freeReg, node->left->lsTableEntry->binding);
        }
        else {
            fprintf(targetFile, "MOV R%d, %d\n", freeReg, node->left->gsTableEntry->binding);
        }

        return freeReg;
    }

    if(node->tnodeType == NODE_ID) {
        return getMemValue(node, _NA_, _NA_, _NA_, targetFile);
    }

    if(node->tnodeType == NODE_STR_LTRL) {
        int freeReg = getFreeRegister();
        fprintf(targetFile, "MOV R%d, %s\n", freeReg, node->strVal);
        releaseRegister(freeReg);
    }

    if(node->tnodeType == NODE_INT) {
        int freeReg = getFreeRegister();
        fprintf(targetFile, "MOV R%d, %d\n", freeReg, node->val);
        return freeReg;
    }

    if(node->tnodeType == NODE_FN_CALL) {
        return evaluateFunction(node, targetFile);
    }

    if(node->tnodeType == NODE_TUP_FIELD) {
        return getMemValue(node, _NA_, _NA_, _NA_, targetFile);
    }

    if (node->tnodeType == NODE_OR) {
        int leftReg = evaluateExpression(node->left, targetFile);

        int label1 = getNewLabel();
        int label2 = getNewLabel();

        fprintf(targetFile, "JZ R%d, L%d\n", leftReg, label1);

        fprintf(targetFile, "MOV R%d, 1\n", leftReg);
        fprintf(targetFile, "JMP L%d\n", label2);

        fprintf(targetFile, "L%d:\n", label1);

        int rightReg = evaluateExpression(node->right, targetFile);
        fprintf(targetFile, "MOV R%d, R%d\n", leftReg, rightReg);
        releaseRegister(rightReg);

        fprintf(targetFile, "L%d:\n", label2);

        return leftReg;
    }

    if (node->tnodeType == NODE_AND) {
        int leftReg = evaluateExpression(node->left, targetFile);

        int label1 = getNewLabel();
        int label2 = getNewLabel();

        fprintf(targetFile, "JNZ R%d, L%d\n", leftReg, label1);

        fprintf(targetFile, "MOV R%d, 0\n", leftReg);
        fprintf(targetFile, "JMP L%d\n", label2);

        fprintf(targetFile, "L%d:\n", label1);

        int rightReg = evaluateExpression(node->right, targetFile);
        fprintf(targetFile, "MOV R%d, R%d\n", leftReg, rightReg);
        releaseRegister(rightReg);

        fprintf(targetFile, "L%d:\n", label2);
        
        return leftReg;
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

        case NODE_MOD :
            fprintf(targetFile, "MOD R%d, R%d\n", leftReg, rightReg);
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

int evaluateFunction(Tnode* fnNode, FILE* targetFile) {
    int pushedRegisters[20];
    int pushedRegCount = pushAllRegisters(pushedRegisters, targetFile);

    int numArgs = pushAllArguments(fnNode->left->argList, targetFile);
    
    fprintf(targetFile, "PUSH R0\n");
    fprintf(targetFile, "CALL F%d\n", fnNode->left->gsTableEntry->fLabel);

    // temporary method to retreive return val
    int freeReg = pushedRegCount == 0 ? 0 : pushedRegisters[pushedRegCount-1] + 1;
    if(freeReg >= NUM_REGISTERS) {
        printf("Out of registers\n");
        exit(1);
    }
    fprintf(targetFile, "POP R%d\n", freeReg);

    // temporary method to pop all args
    int freeReg2 = freeReg + 1;
    for(int i=0; i<numArgs; i++)
        fprintf(targetFile, "POP R%d\n", freeReg2);

    popAllRegisters(pushedRegisters, pushedRegCount, targetFile);
    registerFree[freeReg] = 0;
    return freeReg;
}
