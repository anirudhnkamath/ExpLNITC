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

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols) {
    GsTableEntry* n = (GsTableEntry*)malloc(sizeof(GsTableEntry));
    n->varName = strdup(varName);
    n->dataType = dataType;
    n->size = size;
    n->binding = binding;
    n->dimension = dimension;
    n->numRows = rows;
    n->numCols = cols;

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

GsTableEntry* insertToGsTable(GsTableEntry* head, char* varName, int dataType, int size, int dimension, int rows, int cols) {
    GsTableEntry* exists = findInGsTable(head, varName);

    if(exists) {
        printf("Error: Duplicate variable\n");
        exit(1);
    }

    int binding = getNextBinding(size);
    GsTableEntry* newEntry = createGsTableEntry(varName, dataType, size, binding, dimension, rows, cols);

    if(!head)
        return newEntry;

    GsTableEntry* temp = head;
    while(temp->next != NULL)
        temp = temp->next;

    temp->next = newEntry;
    return head;
}

void printGsTable(GsTableEntry* head) {
    printf("\nName Type Size Binding\n");
    
    GsTableEntry* temp = head;
    while (temp != NULL) {
        printf("%s %d %d %d %d\n", temp->varName, temp->dataType, temp->size, temp->binding, temp->dimension);
        temp = temp->next;
    }
}