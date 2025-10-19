#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../gsTable/gsTable.h"
#include "./typeTable.h"

TypeTable *typeTableHead = NULL;

void createTypeTable() {
    typeTableHead = NULL;
    insertToTypeTable("int", NULL);
    insertToTypeTable("str", NULL);
    insertToTypeTable("bool", NULL);
    insertToTypeTable("null", NULL);
    insertToTypeTable("void", NULL);
    insertToTypeTable("intPtr", NULL);
    insertToTypeTable("strPtr", NULL);
}

TypeTable* searchInTypeTable(char *name) {
    TypeTable *temp = typeTableHead;
    while(temp) {
        if(strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
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

TypeTable* insertToTypeTable(char *name, ParamListEntry* paramListHead) {
    if(searchInTypeTable(name) != NULL) {
        printf("Error : duplicate type exists\n");
        exit(1);
    }


    ParamListEntry* temp = paramListHead;
    FieldList* fhead = NULL;
    int index = 0;
    while(temp) {
        FieldList* f = createField(temp->varName, temp->type, index);

        if(searchInFieldList(fhead, f->name) != NULL) {
            printf("Error : duplicate field\n");
            exit(1);
        }

        index += 1;
        fhead = insertField(fhead, f);
        temp = temp->next;
    }

    TypeTable* t = (TypeTable*)malloc(sizeof(TypeTable));
    t->fields = fhead;
    t->name = strdup(name);
    t->next = NULL;
    t->size = index > 1 ? index : 1;

    TypeTable* tail = typeTableHead;
    if(!tail) {
        typeTableHead = t;
        return t;
    }

    while(tail->next) {
        tail = tail->next;
    }
    tail->next = t;

    return typeTableHead;
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
                printf("  - %s (Index: %d, Type: %s)\n", 
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

FieldList* createField(char *name, TypeTable *type, int index) {
    FieldList *f = (FieldList*)malloc(sizeof(FieldList));
    f->name = strdup(name);
    f->type = type;
    f->fieldIndex = index;
    f->next = NULL;
    return f;
}

FieldList* insertField(FieldList *head, FieldList *newField) {
    if(!head) return newField;
    FieldList *temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newField;
    return head;
}

