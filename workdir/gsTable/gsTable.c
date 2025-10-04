#include "./gsTable.h"
#include "../node/node.h"
#include "../define/constants.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

GsTableEntry* gsTableHead = NULL;
int nextBinding = STATIC_ALLOC_START; 
int declarationOverFlag = 0;

int getNextBinding(int size) {

    if(size <= 0) {
        printf("Error : Invalid variable size\n");
        exit(1);
    }

    if(nextBinding > STATIC_ALLOC_END || nextBinding + size - 1 > STATIC_ALLOC_END) {
        printf("Error: Out of Static Memory\n");
        exit(1);
    }

    int ret = nextBinding;
    nextBinding += size;
    return ret;
}

GsTableEntry* findInGsTable(char* varName) {
    GsTableEntry* temp = gsTableHead;
    while(temp) {
        if(strcmp(temp->varName, varName) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}


GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols, int isPtr) {
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

    return n;
}

void insertToGsTable(char* varName, int dataType, int size, int dimension, int rows, int cols, int isPtr) {
    GsTableEntry* exists = findInGsTable(varName);

    if(exists) {
        printf("Error: Duplicate variable\n");
        exit(1);
    }

    int binding = getNextBinding(size);
    GsTableEntry* newEntry = createGsTableEntry(varName, dataType, size, binding, dimension, rows, cols, isPtr);

    // inserting in LL
    if(!gsTableHead) {
        gsTableHead = newEntry;
        return;
    }

    GsTableEntry* temp = gsTableHead;
    while(temp->next != NULL)
        temp = temp->next;

    temp->next = newEntry;

}


void insertIdToGsTable(char* varName, int dataType) {
    insertToGsTable(varName, dataType, 1, 0, -1, -1, 0);
}

void insertArrToGsTable(char* varName, int dataType, int size) {
    insertToGsTable(varName, dataType, size, 1, -1, -1, 0);
}

void insert2DArrToGsTable(char* varName, int dataType, int rows, int cols) {
    insertToGsTable(varName, dataType, rows*cols, 2, rows, cols, 0);
}

void insertPtrToGsTable(char* varName, int dataType) {
    insertToGsTable(varName, dataType, 1, 0, -1, -1, 1);
}


void printGsTable() {
    printf("\nName Type Size Binding\n");
    
    GsTableEntry* temp = gsTableHead;
    while (temp != NULL) {
        printf("%s %d %d %d %d %d %d\n", temp->varName, temp->dataType, temp->size, temp->binding, temp->dimension, temp->numRows, temp->numCols);
        temp = temp->next;
    }
}