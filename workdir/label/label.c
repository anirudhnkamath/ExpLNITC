#include "./label.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Label* labelHead = NULL;
int newLabel = 0;

Label* createLabel(char labelName[], int addr) {
    Label* newNode = (Label*)malloc(sizeof(Label));
    strcpy(newNode->labelName, labelName);
    newNode->addr = addr;
    newNode->next = NULL;

    return newNode;
}

void insertNewLabel(char labelName[], int addr) {
    Label* newNode = createLabel(labelName, addr);

    if(labelHead == NULL) {
        labelHead = newNode;
        return;
    }

    Label* temp = labelHead;
    while(temp->next)
        temp = temp->next;

    temp->next = newNode;
}

int findLabelAddr(char labelName[]) {
    Label* temp = labelHead;
    while(temp) {
        if (strcmp(temp->labelName, labelName) == 0)
            return temp->addr;
        temp = temp->next;
    }
    
    printf("Label not found\n");
    exit(1);

    return 0;
}

int getNewLabel() {
    int ret = newLabel;
    newLabel += 1;
    return ret;
}



// run time stack for using break and continue

LabelStackNode* labelStackHead = NULL;

void addToLabelStack(int startLabel, int endLabel) {
    LabelStackNode* n = (LabelStackNode*)malloc(sizeof(LabelStackNode));
    n->startLabel = startLabel;
    n->endLabel = endLabel;
    n->next = NULL;

    n->next = labelStackHead;
    labelStackHead = n;
}

void popLabelStack() {
    if(labelStackHead == NULL) {
        printf("Error: label stack empty\n");
        exit(1);
    }

    LabelStackNode* top = labelStackHead;
    labelStackHead = labelStackHead->next;

    free(top);
}

LabelStackNode* peekLabelStack() {
    return labelStackHead;
}


