#ifndef TYPETABLE_H
#define TYPETABLE_H

#include "../gsTable/gsTable.h"

typedef struct ParamListEntry ParamListEntry;
typedef struct LsTableEntry LsTableEntry;


typedef struct FieldList {
    char *name;
    int fieldIndex;
    struct TypeTable *type;
    struct FieldList *next;
} FieldList;

typedef struct TypeTable {
    char *name;
    int size;
    struct FieldList *fields;
    struct TypeTable *next;
} TypeTable;

extern TypeTable *typeTableHead;

void createTypeTable();
TypeTable* searchInTypeTable(char *name) ;
TypeTable* insertToTypeTable(char *name, ParamListEntry* paramListHead);
void printTypeTable();

FieldList* searchInFieldList(FieldList* head, char* name);
FieldList* createField(char *name, TypeTable *type, int index);
FieldList* insertField(FieldList *head, FieldList *newField);


#endif
