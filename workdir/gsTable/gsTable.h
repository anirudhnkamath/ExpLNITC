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
} GsTableEntry;

extern int declarationOverFlag;
extern int nextBinding;
extern GsTableEntry* gsTableHead;

int getNextBinding(int size);

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols);
GsTableEntry* insertToGsTable(GsTableEntry* head, char* varName, int dataType, int size, int dimension, int rows, int cols);

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName);

void printGsTable(GsTableEntry* head);

#endif