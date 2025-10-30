#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../gsTable/gsTable.h"
#include "./typeTable.h"

TypeTable *typeTableHead = NULL;

void createTypeTable() {
    typeTableHead = NULL;
    typeTableHead = concatTypeTable(typeTableHead, createNewType("int", NULL));
    typeTableHead = concatTypeTable(typeTableHead, createNewType("str", NULL));
    typeTableHead = concatTypeTable(typeTableHead, createNewType("bool", NULL));
    typeTableHead = concatTypeTable(typeTableHead, createNewType("null", NULL));
}

TypeTable* createNewType(char* varName, FieldList* fields) {
    TypeTable* t = (TypeTable*)malloc(sizeof(TypeTable));
    t->name = strdup(varName);
    t->next = NULL;
    t->fields = fields;

    FieldList* temp = fields;
    int size = 0;
    while(temp) {
        size += 1;
        temp = temp->next;
    }

    t->size = size > 0 ? size : 1;

    return t;
}

TypeTable* updateType(char* varName, FieldList* fields) {
    TypeTable* t = searchInTypeTable(typeTableHead, varName);
    t->fields = fields;

    FieldList* temp = fields;
    int size = 0;
    while(temp) {
        size += 1;
        temp = temp->next;
    }

    t->size = size > 0 ? size : 1;

    return t;
}

TypeTable* searchInTypeTable(TypeTable* head, char *name) {
    TypeTable *temp = head;
    while(temp) {
        if(strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

TypeTable* concatTypeTable(TypeTable* head1, TypeTable* head2) {
    if(!head1) return head2;

    TypeTable* temp2 = head2;
    while(temp2) {
        if(searchInTypeTable(head1, temp2->name)) {
            printf("Error : duplicate type detected\n");
            exit(1);
        }

        temp2 = temp2->next;
    }

    TypeTable* temp1 = head1;
    while(temp1->next) temp1 = temp1->next;
    temp1->next = head2;

    return head1;
}

TypeTable* validateUserType(TypeTable* head, char* typeName) {
    TypeTable* t = searchInTypeTable(head, typeName);
    if(!t) {
        printf("Error : user type not defined\n");
        exit(1);
    }

    return t;
}

void printTypeTable() {
    TypeTable *t = typeTableHead;
    
    printf("\n--- Type Table ---\n\n");
    
    if(!t) {
        printf("(Empty)\n");
        printf("--- End of Table ---\n\n");
        return;
    }
    
    while(t) {
        printf("Type Name  : %s\n", t->name);
        printf("Size       : %d\n", t->size);
        
        if(t->fields) {
            printf("Fields     :\n");
            FieldList *f = t->fields;
            while(f) {
                printf("\t%s (Index: %d, Type: %s)\n", 
                       f->name, 
                       f->fieldIndex, 
                       f->type->name);
                f = f->next;
            }
        } else {
            printf("Fields     : (none)\n");
        }
        
        printf("\n");
        t = t->next;
    }
    
    printf("--- End of Table ---\n\n");
}



FieldList* searchInFieldList(FieldList* head, char* name) {
    FieldList *temp = head;
    while(temp) {
        if(strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

FieldList* createField(char *name, TypeTable *type) {

    if(!type) {
        printf("%s\n", name);
        printf("Error : Type not declared\n");
        exit(1);
    }

    FieldList *f = (FieldList*)malloc(sizeof(FieldList));
    f->name = strdup(name);
    f->type = type;
    f->next = NULL;
    return f;
}

FieldList* concatFieldList(FieldList* head1, FieldList* head2) {
    if(!head1) return head2;
    
    FieldList* temp2 = head2;
    while(temp2) {
        if(searchInFieldList(head1, temp2->name)) {
            printf("Error : duplicate fields detected\n");
            exit(1);
        }

        temp2 = temp2->next;
    }

    FieldList* temp1 = head1;
    while(temp1->next) temp1 = temp1->next;
    temp1->next = head2;

    return head1;
}

FieldList* setFieldIndex(FieldList* head) {
    int index = 0;
    FieldList* temp = head;
    while(temp) {
        temp->fieldIndex = index;
        index += 1;
        temp = temp->next;
    }

    return head;
}
