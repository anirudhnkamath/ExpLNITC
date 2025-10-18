#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./typeTable.h"

TypeTable *typeTableHead = NULL;

void createTypeTable() {
    typeTableHead = NULL;
    insertToTypeTable("int", 1, NULL);
    insertToTypeTable("str", 1, NULL);
    insertToTypeTable("bool", 1, NULL);
    insertToTypeTable("null", 1, NULL);
    insertToTypeTable("void", 0, NULL);
    insertToTypeTable("intPtr", 1, NULL);
    insertToTypeTable("strPtr", 1, NULL);
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

TypeTable* insertToTypeTable(char *name, int size, FieldList *fields) {
    if(searchInTypeTable(name) != NULL)
        return NULL;

    TypeTable *newType = (TypeTable*)malloc(sizeof(TypeTable));
    newType->name = strdup(name);
    newType->size = size;
    newType->fields = fields;
    newType->next = NULL;

    if(!typeTableHead) {
        typeTableHead = newType;
    } 
    else {
        TypeTable *temp = typeTableHead;
        while (temp->next) temp = temp->next;
        temp->next = newType;
    }

    return newType;
}

int getSizeOfType(TypeTable *type) {
    if(!type) return 0;
    return type->size;
}

void printTypeTable() {
    TypeTable *t = typeTableHead;
    while (t) {
        printf("Type: %s, Size: %d\n", t->name, t->size);
        FieldList *f = t->fields;
        while (f) {
            printf("   Field: %s (Index %d) Type: %s\n", f->name, f->fieldIndex, f->type->name);
            f = f->next;
        }
        t = t->next;
    }
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

FieldList* searchField(TypeTable *type, char *name) {
    if(!type || !type->fields) return NULL;
    FieldList *temp = type->fields;
    while (temp) {
        if(strcmp(temp->name, name) == 0) return temp;
        temp = temp->next;
    }
    return NULL;
}
