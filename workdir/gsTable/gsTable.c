#include "./gsTable.h"
#include "../node/node.h"
#include "../classTable/classTable.h"
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



GsTableEntry* createEmptyGsTableEntry() {
    GsTableEntry* n = (GsTableEntry*)malloc(sizeof(GsTableEntry));
    n->varName = NULL;
    n->type = NULL;
    n->class = NULL;
    n->size = _NA_;
    n->binding = _NA_;
    n->next = NULL;
    n->paramList = NULL;
    n->fLabel = _NA_;
    n->dimensions = NULL; 
    n->isPtr = _NA_;
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
    GsTableEntry* n = createEmptyGsTableEntry();
    n->varName = strdup(varName);
    n->size = 1;
    return n;
}

GsTableEntry* createArrEntryInGsTable(char* varName, Tnode* arrOffsetNode) {
    GsTableEntry* n = createEmptyGsTableEntry();
    n->varName = strdup(varName);

    int size = 1;
    Tnode* temp = arrOffsetNode;
    while(temp) {
        size *= temp->val;
        temp = temp->arrOffset;
    }

    n->size = size;
    n->dimensions = arrOffsetNode;

    return n;
}

GsTableEntry* createFnEntryInGsTable(char* varName, ParamListEntry* paramListHead) {
    GsTableEntry* n = createEmptyGsTableEntry();
    n->varName = strdup(varName);
    n->paramList = paramListHead;
    n->fLabel = getNextFLabel();

    return n;
}

GsTableEntry* createPtrEntryInGsTable(char* varName) {
    GsTableEntry* n = createEmptyGsTableEntry();
    n->varName = strdup(varName);
    n->size = 1;
    n->isPtr = 1;
    return n;
}

GsTableEntry* setGsTableType(GsTableEntry* head, char* typeName) {

    TypeTable* type = searchInTypeTable(typeTableHead, typeName);
    ClassTable* class = searchInClassTable(classTableHead, typeName);

    if(!type && !class || type && class) {
        printf("Error : invalid type in global declarations\n");
        exit(1);
    }

    GsTableEntry* temp = head;
    while(temp) {

        if(
            (temp->isPtr == 1 && class) ||
            (temp->isPtr == 1 && strcmp(type->name, "int") != 0 && strcmp(type->name, "str") != 0)
        ) {
            printf("Error : user defined or class pointers not allowed\n");
            exit(1);
        }

        if(temp->fLabel == _NA_ && temp->isPtr == _NA_) {
            temp->binding = getNextBinding(temp->size);
        }

        else if(temp->isPtr == 1) {
            temp->binding = getNextBinding(1);
        }

        temp->class = class;
        temp->type = type;
        temp = temp->next;
    }

    return head;
}

void printGsTable() {
    GsTableEntry* temp = gsTableHead;

    printf("\n--- Global Symbol Table ---\n\n");

    if(temp == NULL) {
        printf("(Empty)\n");
        printf("--- End of Table ---\n\n");
        return;
    }

    while(temp) {
        printf("Name       : %s\n", temp->varName);
        printf("Type       : %s\n", temp->type ? temp->type->name : temp->class->name);
        printf("Size       : %d\n", temp->size);
        printf("Binding    : %d\n", temp->binding);
        printf("FLabel     : %d\n", temp->fLabel);
        printf("IsPtr      : %d\n", temp->isPtr);

        if(temp->dimensions) {
            printf("Dimensions : [");
            Tnode* dim = temp->dimensions;
            while(dim) {
                printf("%d", dim->val);
                if(dim->arrOffset) printf("][");
                dim = dim->arrOffset;
            }
            printf("]\n");
        }

        if(temp->paramList) {
            printParamList(temp->paramList);
        }

        printf("\n");
        temp = temp->next;
    }

    printf("--- End of Table ---\n\n");
}


ParamListEntry* createParamListEntry(char* varName, TypeTable* dataType, int isPtr) {
    ParamListEntry* n = (ParamListEntry*)malloc(sizeof(ParamListEntry));
    n->varName = strdup(varName);
    n->type = dataType;
    n->next = NULL;
    n->isPtr = isPtr;
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
    if(paramListHead == NULL) {
        printf("Parameters : (none)\n");
        return;
    }

    printf("Parameters : ");
    ParamListEntry* temp = paramListHead;
    
    while(temp) {
        printf("%s(%s, isPtr(%d))", temp->varName, temp->type->name, temp->isPtr);
        if(temp->next) printf(", ");
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

LsTableEntry* createLsTableEntry() {
    LsTableEntry* n = (LsTableEntry*)malloc(sizeof(LsTableEntry));
    n->varName = NULL;
    n->binding = _NA_;
    n->type = NULL;
    n->class = NULL;
    n->next = NULL;
    n->isPtr = _NA_;
    return n;
}

LsTableEntry* createIdEntryInLsTable(char* varName) {
    LsTableEntry* n =  createLsTableEntry();
    n->varName = strdup(varName);
    return n;
}

LsTableEntry* createPtrEntryInLsTable(char* varName) {
    LsTableEntry* n =  createLsTableEntry();
    n->varName = strdup(varName);
    n->isPtr = 1;
    return n;
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
    int paramCount = 0;
    while(temp) {
        paramCount += 1;
        temp = temp->next;
    }

    temp = paramListHead;
    int bind = PARAMS_BINDING_START - paramCount + 1;
    while(temp) {
        LsTableEntry* n = createLsTableEntry();
        n->varName = strdup(temp->varName);
        n->type = temp->type;
        n->binding = bind;
        n->isPtr = temp->isPtr;

        head = concatLsTable(head, n);
        temp = temp->next;
        bind += 1;
    }

    return head;
}

LsTableEntry* setLsTableType(LsTableEntry* head, TypeTable* type) {
    if(!type) {
        printf("Error : unknown type used in local declaration\n");
        exit(1);
    }
    LsTableEntry* temp = head;
    while(temp) {
        temp->type = type;
        temp = temp->next;
    }
    return head;
}

LsTableEntry* insertSelfToLsTable(LsTableEntry* head) {
    LsTableEntry* n = createIdEntryInLsTable("self");

    ClassTable* curClass = classTableHead;
    while(curClass->next)
        curClass = curClass->next;

    n->class = curClass;
    
    return concatLsTable(head, n);
}

void setLDeclBinding(LsTableEntry* head) {
    int bind = LDECL_BINDING_START;

    LsTableEntry* temp = head;
    while(temp) {
        temp->binding = bind;
        bind += 1;
        temp = temp->next;
    }
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
    
    printf("\n--- Local Symbol Table ---\n\n");

    if(temp == NULL) {
        printf("(Empty)\n");
        printf("--- End of Table ---\n\n");
        return;
    }

    while(temp) {
        printf("Name    : %s\n", temp->varName);
        printf("Type    : %s\n", temp->type != NULL ? temp->type->name : temp->class->name);
        printf("Binding : %d\n", temp->binding);
        printf("isPtr   : %d\n", temp->isPtr);
        printf("\n");
        temp = temp->next;
    }

    printf("--- End of Table ---\n\n");
}