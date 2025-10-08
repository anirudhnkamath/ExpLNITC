#include "./gsTable.h"
#include "../node/node.h"
#include "../define/constants.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

GsTableEntry* gsTableHead = NULL;
int nextBinding = STACK_START; 
int declarationOverFlag = 0;



int getNextBinding(int size) {

    if(size <= 0) {
        printf("Error : Invalid variable size\n");
        exit(1);
    }

    if(nextBinding > STACK_END || nextBinding + size - 1 > STACK_END) {
        printf("Error: Out of Static Memory\n");
        exit(1);
    }

    int ret = nextBinding;
    nextBinding += size;
    return ret;
}

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols, int isPtr, ParamListEntry* paramListHead) {
    GsTableEntry* n = (GsTableEntry*)malloc(sizeof(GsTableEntry));
    n->varName = strdup(varName);
    n->dataType = dataType;
    n->size = size;
    n->binding = binding;
    n->dimension = dimension;
    n->numRows = rows;
    n->numCols = cols;
    n->next = NULL;
    n->isPtr = isPtr;
    n->paramList = paramListHead;

    return n;
}

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName) {
    GsTableEntry* temp = head;
    while(temp) {
        if(strcmp(temp->varName, varName) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

GsTableEntry* concatGsTable(GsTableEntry* head1, GsTableEntry* head2) {
    GsTableEntry* temp = head2;
    while(temp) {
        GsTableEntry* found = findInGsTable(head1, temp->varName);
        if(found) {
            printf("Error : Duplicate variable found\n");
            exit(1);
        }
        temp = temp->next;
    }

    GsTableEntry* tail = head1;
    while(tail->next)
        tail = tail->next;

    tail->next = head2;
    return head1;
}

GsTableEntry* createIdEntryInGsTable(char* varName) {
    return createGsTableEntry(varName, -1, 1, getNextBinding(1), 0, -1, -1, 0, NULL);
}

GsTableEntry* createArrEntryInGsTable(char* varName, int size) {
    return createGsTableEntry(varName, -1, size, getNextBinding(size), 1, -1, -1, 0, NULL);
}

GsTableEntry* createFnEntryInGsTable(char* varName, ParamListEntry* paramListHead) {
    return createGsTableEntry(varName, -1, 1, -1, 0, -1, -1, 0, paramListHead);
}

GsTableEntry* setGsTableType(GsTableEntry* head, int type) {
    GsTableEntry* temp = head;
    while(temp) {
        temp->dataType = type;
        temp = temp->next;
    }
    return head;
}

void printGsTable() {
    GsTableEntry* temp = gsTableHead;
    while(temp) {
        printf("Name  : %s\nType  : %s\nSize  : %d\nBind  : %d\n", temp->varName, temp->dataType == INTEGER_TYPE ? "INT" : "STR", temp->size, temp->binding);
        if(temp->paramList) printParamList(temp->paramList);
        else printf("No parameters\n");
        printf("\n");
        temp = temp->next;
    }
}


ParamListEntry* createParamListEntry(char* varName, int dataType) {
    ParamListEntry* n = (ParamListEntry*)malloc(sizeof(ParamListEntry));
    n->varName = strdup(varName);
    n->dataType = dataType;
    n->next = NULL;
    return n;
}

ParamListEntry* findInParamList(ParamListEntry* head, char* varName) {
    ParamListEntry* temp = head;
    while(temp) {
        if(strcmp(temp->varName, varName) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

ParamListEntry* concatParamList(ParamListEntry* head1, ParamListEntry* head2) {
    ParamListEntry* temp = head2;
    while(temp) {
        if(findInParamList(head1, temp->varName)) {
            printf("Error : Duplicate parameters in function declaration\n");
            exit(1);
        }
        temp = temp->next;
    }

    ParamListEntry* tail = head1;
    while(tail->next)
        tail = tail->next;

    tail->next = head2;
    return head1;
}

void printParamList(ParamListEntry* paramListHead) {
    ParamListEntry* temp = paramListHead;
    printf("Params: ");
    while(temp) {
        printf("%s(%s) ", temp->varName, temp->dataType == INTEGER_TYPE ? "INT" : "STR");
        temp = temp->next;
    }
    printf("\n");
}