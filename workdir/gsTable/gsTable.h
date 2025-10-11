#ifndef GSTABLE_H
#define GSTABLE_H

/* BINDS */

extern int nextBinding;
int getNextBinding(int size);


/* PARAMLIST */

typedef struct ParamListEntry {
    char* varName;
    int dataType;
    struct ParamListEntry* next;
} ParamListEntry;

ParamListEntry* createParamListEntry(char* varName, int dataType);
ParamListEntry* findInParamList(ParamListEntry* head, char* varName);
ParamListEntry* concatParamList(ParamListEntry* head1, ParamListEntry* head2);

void printParamList(ParamListEntry* paramListHead);


/* GLOBAL SYMBOL TABLE */

int nextFLabel;
int getNextFLabel();

typedef struct GsTableEntry {
    char* varName;
    int dataType;
    int size;
    int binding;
    int fLabel;
    struct ParamListEntry* paramList;
    struct GsTableEntry * next;

    int dimension;
    int numRows;
    int numCols;
    int isPtr;
    
} GsTableEntry;

extern GsTableEntry* gsTableHead;

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName);
GsTableEntry* concatGsTable(GsTableEntry* head1, GsTableEntry* head2);

GsTableEntry* createGsTableEntry(char* varName, int dataType, int size, int binding, int dimension, int rows, int cols, int isPtr, ParamListEntry* paramListHead);
GsTableEntry* createIdEntryInGsTable(char* varName);
GsTableEntry* createArrEntryInGsTable(char* varName, int size);
GsTableEntry* createFnEntryInGsTable(char* varName, ParamListEntry* paramListHead);

GsTableEntry* setGsTableType(GsTableEntry* head, int type);

void printGsTable();


/* LOCAL SYMBOL TABLE*/

typedef struct LsTableEntry {
    char* varName;
    int dataType;
    int binding;
    struct LsTableEntry* next;
} LsTableEntry;

extern LsTableEntry* lsTableHead;

LsTableEntry* findInLsTable(LsTableEntry* head, char* varName);

LsTableEntry* createLsTableEntry(char* varName, int dataType, int binding);
LsTableEntry* createIdEntryInLsTable(char* varName);

LsTableEntry* concatLsTable(LsTableEntry* head1, LsTableEntry* head2);
LsTableEntry* addParamsToLsTable(LsTableEntry* head, ParamListEntry* paramListHead);

LsTableEntry* setLsTableType(LsTableEntry* head, int type);

void freeLsTable();
void printLsTable();


#endif