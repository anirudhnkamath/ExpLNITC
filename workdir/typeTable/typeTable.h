#ifndef TYPETABLE_H
#define TYPETABLE_H

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
TypeTable* searchInTypeTable(char *name);
TypeTable* insertToTypeTable(char *name, int size, FieldList *fields);
int getSizeOfType(TypeTable *type);
void printTypeTable();

FieldList* createField(char *name, TypeTable *type, int index);
FieldList* insertField(FieldList *head, FieldList *newField);
FieldList* searchField(TypeTable *type, char *name);

#endif
