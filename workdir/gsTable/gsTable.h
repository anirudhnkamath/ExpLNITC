#ifndef GSTABLE_H
#define GSTABLE_H

typedef struct GsTableEntry {
    char* varName;
    int dataType;
    int size;
    int binding;
    int dimension;
    struct GsTableEntry * next;
    int numRows;
    int numCols;
    int isPtr;
} GsTableEntry;

extern int declarationOverFlag;
extern int nextBinding;
extern GsTableEntry* gsTableHead;

int getNextBinding(int size);

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols, int isPtr);
void insertToGsTable(char* varName, int dataType, int size, int dimension, int rows, int cols, int isPtr);

GsTableEntry* findInGsTable(char* varName);

void insertIdToGsTable(char* varName, int dataType);
void insertArrToGsTable(char* varName, int dataType, int size);
void insert2DArrToGsTable(char* varName, int dataType, int rows, int cols);
void insertPtrToGsTable(char* varName, int dataType);

void printGsTable();

#endif