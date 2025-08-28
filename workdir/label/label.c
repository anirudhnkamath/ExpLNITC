#include "./label.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Label* labelHead = NULL;

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
