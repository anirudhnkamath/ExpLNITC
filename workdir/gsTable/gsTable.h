#ifndef GSTABLE_H
#define GSTABLE_H

typedef struct GsTableEntry {
    char* varName;
    int dataType;
    int size;
    int binding;
    struct GsTableEntry * next;
} GsTableEntry;

int getNextBinding();

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding);
GsTableEntry* insertToGsTable(GsTableEntry* head, char* varName, int dataType, int size);

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName);

void printGsTable(GsTableEntry* head);

#endif