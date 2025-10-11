#include "./gsTable.h"
#include "../node/node.h"
#include "../define/constants.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

GsTableEntry* gsTableHead = NULL;
LsTableEntry* lsTableHead = NULL;

int nextFLabel = 0;
int getNextFLabel() {
    return nextFLabel++;
}

int nextBinding = STACK_START;
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
    n->fLabel = -1;

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

    if(!tail)
        return head2;
        
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
    GsTableEntry * n = createGsTableEntry(varName, -1, -1, -1, -1, -1, -1, -1, paramListHead);
    n->fLabel = getNextFLabel();
    return n;
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

    printf("\n--- Global Symbol Table ---\n\n");

    while (temp) {
        const char* dtype = (temp->dataType == INTEGER_TYPE) ? "INT" : "STR";

        printf("Name       : %s\n", temp->varName);
        printf("Type       : %s\n", dtype);
        printf("Size       : %d\n", temp->size);
        printf("Binding    : %d\n", temp->binding);
        printf("fLabel     : %d\n", temp->fLabel);
        printf("Dimension  : %d\n", temp->dimension);
        printf("Rows       : %d\n", temp->numRows);
        printf("Cols       : %d\n", temp->numCols);
        printf("isPtr      : %d\n", temp->isPtr);

        if (temp->paramList) {
            printf("Parameters :\n");
            printParamList(temp->paramList);
        } else {
            printf("Parameters : None\n");
        }

        printf("\n----------------------------\n");
        temp = temp->next;
    }

    printf("\n--- End of Table ---\n");
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
    if(!tail)
        return head2;

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


LsTableEntry* findInLsTable(LsTableEntry* head, char* varName) {
    LsTableEntry* temp = head;
    while(temp) {
        if(strcmp(temp->varName, varName) == 0) 
            return temp;
        temp = temp->next;
    }
    return NULL;
}

LsTableEntry* createLsTableEntry(char* varName, int dataType, int binding) {
    LsTableEntry* n = (LsTableEntry*)malloc(sizeof(LsTableEntry));
    n->varName = strdup(varName);
    n->dataType = dataType;
    n->binding = binding;
    n->next = NULL;
    return n;
}

LsTableEntry* createIdEntryInLsTable(char* varName) {
    return createLsTableEntry(varName, -1, -1);
}

LsTableEntry* concatLsTable(LsTableEntry* head1, LsTableEntry* head2) {
    LsTableEntry* temp = head2;
    while(temp) {
        LsTableEntry* found = findInLsTable(head1, temp->varName);
        if(found) {
            printf("Error : duplicat local varaible\n");
            exit(1);
        }
        temp = temp->next;
    }

    LsTableEntry* tail = head1;
    if(!tail)
        return head2;

    while(tail->next)
        tail = tail->next;

    tail->next = head2;
    return head1;
}

LsTableEntry* addParamsToLsTable(LsTableEntry* head, ParamListEntry* paramListHead) {
    ParamListEntry* temp = paramListHead;
    while(temp) {
        LsTableEntry* n = createLsTableEntry(temp->varName, temp->dataType, -1);
        head = concatLsTable(head, n);
        temp = temp->next;
    }

    return head;
}

LsTableEntry* setLsTableType(LsTableEntry* head, int type) {
    LsTableEntry* temp = head;
    while(temp) {
        temp->dataType = type;
        temp = temp->next;
    }
    return head;
}

void freeLsTable() {
    LsTableEntry* temp = lsTableHead;
    while(temp) {
        free(temp->varName);
        LsTableEntry* temp2 = temp->next;
        free(temp);
        temp = temp2;
    }
}

void printLsTable() {
    LsTableEntry* temp = lsTableHead;
    printf("\nLocal symbol table : \n\n");
    while(temp) {
        printf("Name : %s\nType : %s\nBind : %d\n\n", temp->varName, temp->dataType == INTEGER_TYPE ? "INT" : "STR", temp->binding);
        temp = temp->next;
    }
}
