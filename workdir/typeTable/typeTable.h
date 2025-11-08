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
TypeTable* createNewType(char* varName, FieldList* fields);
TypeTable* updateType(char* varName, FieldList* fields);
TypeTable* searchInTypeTable(TypeTable* head, char *name);
TypeTable* concatTypeTable(TypeTable* head1, TypeTable* head2);
TypeTable* validateType(TypeTable* head, char* typeName);
void printTypeTable();

FieldList* searchInFieldList(FieldList* head, char* name);
FieldList* createField(char *name, TypeTable *type);
FieldList* concatFieldList(FieldList* head1, FieldList* head2);
FieldList* setFieldIndex(FieldList* head);

#endif
