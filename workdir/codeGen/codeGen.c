#include "./codeGen.h"
#include "../registers/registers.h"
#include "../gsTable/gsTable.h"
#include "../label/label.h"
#include "../xsmGen/xsmGen.h"
#include <string.h>
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

        case NODE_FREE : {
            xsmFree(node->left, targetFile);
            break;
        }

        case NODE_DELETE : {
            xsmFree(node->left, targetFile);
            break;
        }

        case NODE_BRKP : {
            fprintf(targetFile, "BRKP\n");
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
    readFromConsole(node, targetFile);
}

void generateAssignCode(Tnode* lhs, Tnode* rhs, FILE* targetFile) {

    if(rhs->tnodeType == NODE_NEW) {
        int heapReg = xsmAlloc(targetFile);
        int storeReg = getEffectiveAddr(lhs, targetFile);
        fprintf(targetFile, "MOV [R%d], R%d\n", storeReg, heapReg);

        // vtable
        int vTableAddr = STACK_START + (rhs->left->class->index * MAX_FIELDS);
        fprintf(targetFile, "ADD R%d, 1\n", storeReg);
        fprintf(targetFile, "MOV [R%d], %d\n", storeReg, vTableAddr);

        releaseRegister(heapReg);
        releaseRegister(storeReg);

        return;
    }

    if(rhs->tnodeType == NODE_ALLOC) {
        int heapReg = xsmAlloc(targetFile);
        int storeReg = getEffectiveAddr(lhs, targetFile);
        fprintf(targetFile, "MOV [R%d], R%d\n", storeReg, heapReg);
        releaseRegister(heapReg);
        releaseRegister(storeReg);

        return;
    }

    int lhsAddr = getEffectiveAddr(lhs, targetFile);
    int exprReg = evaluateExpression(rhs, targetFile);

    fprintf(targetFile, "MOV [R%d], R%d\n", lhsAddr, exprReg);

    releaseRegister(lhsAddr);
    releaseRegister(exprReg);

    // virtual table set up
    if(lhs->class && rhs->class) {

        int lhsAddr = getEffectiveAddr(lhs, targetFile);
        int exprReg = getEffectiveAddr(rhs, targetFile);

        fprintf(targetFile, "ADD R%d, 1\n", lhsAddr);
        fprintf(targetFile, "ADD R%d, 1\n", exprReg);
        fprintf(targetFile, "MOV [R%d], [R%d]\n", lhsAddr, exprReg);

        releaseRegister(lhsAddr);
        releaseRegister(exprReg);
    }
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

    if(node->tnodeType == NODE_NULL) {
        int freeReg = getFreeRegister();
        fprintf(targetFile, "MOV R%d, %d\n", freeReg, BIG_INT);
        return freeReg;
    }

    if(node->tnodeType == NODE_DEREF) {
        int addrReg = getEffectiveAddr(node, targetFile);
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
        int addrReg = getEffectiveAddr(node, targetFile);
        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        return addrReg;
    }

    if(node->tnodeType == NODE_STR_LTRL) {
        int freeReg = getFreeRegister();
        fprintf(targetFile, "MOV R%d, %s\n", freeReg, node->strVal);
        return freeReg;
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
        int addrReg = getEffectiveAddr(node, targetFile);

        if( node->right->tnodeType == NODE_FN_CALL)
            return addrReg;

        fprintf(targetFile, "MOV R%d, [R%d]\n", addrReg, addrReg);
        return addrReg;
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

    // get free registers for later use
    int freeReg1 = getFreeRegister();
    int freeReg2 = getFreeRegister();
    registerFree[freeReg1] = registerFree[freeReg2] = 1;


    // save current state
    int pushedRegisters[20];
    int pushedRegCount = 0;
    for(int i=0; i<NUM_REGISTERS; i++) {
        if(!registerFree[i]) {
            fprintf(targetFile, "PUSH R%d\n", i);
            pushedRegisters[pushedRegCount] = i;
            pushedRegCount += 1;
        }
    }
    resetRegisters();
    registerFree[freeReg1] = registerFree[freeReg2] = 0;

    // push all arguments
    int numArgs = 0;
    Tnode* temp = fnNode->left->argList;
    while(temp) {
        int argReg = evaluateExpression(temp, targetFile);
        fprintf(targetFile, "PUSH R%d\n", argReg);
        releaseRegister(argReg);
        numArgs += 1;
        temp = temp->argList;
    }


    // push space for retval and call function
    resetRegisters();
    fprintf(targetFile, "PUSH R0\n");
    fprintf(targetFile, "CALL F%d\n", fnNode->left->gsTableEntry->fLabel);
    resetRegisters();

    
    // retrieve return val in freeReg1
    registerFree[freeReg1] = 0;
    fprintf(targetFile, "POP R%d\n", freeReg1);


    // temporary method to pop all args
    for(int i=0; i<numArgs; i++)
        fprintf(targetFile, "POP R%d\n", freeReg2);


    // recover the saved state
    for(int i=pushedRegCount-1; i>=0; i--) {
        int r = pushedRegisters[i];
        fprintf(targetFile, "POP R%d\n", r);
        registerFree[r] = 0;
    }

    // return the register storing the return value
    return freeReg1;
}

int evaluateMethod(Tnode* fnNode, int selfReg, int vtableReg, FILE* targetFile) {

    // get free registers for later use
    int freeReg1 = getFreeRegister();
    int freeReg2 = getFreeRegister();

    // save current state
    registerFree[freeReg1] = registerFree[freeReg2] = 1;
    int pushedRegisters[20];
    int pushedRegCount = 0;
    for(int i=0; i<NUM_REGISTERS; i++) {
        if(!registerFree[i]) {
            fprintf(targetFile, "PUSH R%d\n", i);
            pushedRegisters[pushedRegCount] = i;
            pushedRegCount += 1;
        }
    }
    resetRegisters();
    registerFree[freeReg1] = registerFree[freeReg2] = 0;
    registerFree[selfReg] = registerFree[vtableReg] = 0;


    // push all arguments
    int numArgs = 0;
    Tnode* temp = fnNode->left->argList;
    while(temp) {
        int argReg = evaluateExpression(temp, targetFile);
        fprintf(targetFile, "PUSH R%d\n", argReg);
        releaseRegister(argReg);
        numArgs += 1;
        temp = temp->argList;
    }

    // push self value
    if(fnNode->left->class) {
        fprintf(targetFile, "PUSH R%d\n", selfReg);
        fprintf(targetFile, "MOV R%d, [R%d]\n", freeReg1, vtableReg);
        fprintf(targetFile, "PUSH R%d skldjflsdkjf\n", freeReg1);
    }


    // push space for retval and call function
    ClsMthdList* foundMthd = findMthdByArgs(fnNode->left->class->mthdList, fnNode->left->varName, fnNode->left->argList);
    int offset = foundMthd->index;

    fprintf(targetFile, "MOV R%d, [R%d]\n", freeReg1, vtableReg);
    fprintf(targetFile, "ADD R%d, %d\n", freeReg1, offset);

    resetRegisters();
    fprintf(targetFile, "PUSH R0\n");
    fprintf(targetFile, "MOV R0, [R%d]\n", freeReg1);
    fprintf(targetFile, "CALL R0\n");
    resetRegisters();


    // retrieve return val in freeReg1
    registerFree[freeReg1] = 0;
    fprintf(targetFile, "POP R%d\n", freeReg1);

    // temporary method to pop all args
    for(int i=0; i<numArgs + 2; i++)
        fprintf(targetFile, "POP R%d\n", freeReg2);



    // recover the saved state
    for(int i=pushedRegCount-1; i>=0; i--) {
        int r = pushedRegisters[i];
        fprintf(targetFile, "POP R%d\n", r);
        registerFree[r] = 0;
    }

    // return the register storing the return value
    return freeReg1;
}
