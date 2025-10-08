#ifndef GSTABLE_H
#define GSTABLE_H


typedef struct ParamListEntry {
    char* varName;
    int dataType;
    struct ParamListEntry* next;
} ParamListEntry;

ParamListEntry* createParamListEntry(char* varName, int dataType);
ParamListEntry* findInParamList(ParamListEntry* head, char* varName);
ParamListEntry* concatParamList(ParamListEntry* head1, ParamListEntry* head2);

void printParamList(ParamListEntry* paramListHead);




typedef struct GsTableEntry {
    char* varName;
    int dataType;
    int size;
    int binding;
    struct ParamListEntry* paramList;
    struct GsTableEntry * next;

    int dimension;
    int numRows;
    int numCols;
    int isPtr;
    
} GsTableEntry;

extern int declarationOverFlag;
extern int nextBinding;
extern GsTableEntry* gsTableHead;

int getNextBinding(int size);

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName);
GsTableEntry* concatGsTable(GsTableEntry* head1, GsTableEntry* head2);

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols, int isPtr, ParamListEntry* paramListHead);
GsTableEntry* createIdEntryInGsTable(char* varName);
GsTableEntry* createArrEntryInGsTable(char* varName, int size);
GsTableEntry* createFnEntryInGsTable(char* varName, ParamListEntry* paramListHead);

GsTableEntry* setGsTableType(GsTableEntry* head, int type);

void printGsTable();


#endif