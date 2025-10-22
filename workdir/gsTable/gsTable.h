#ifndef GSTABLE_H
#define GSTABLE_H

#include "../typeTable/typeTable.h"

typedef struct Tnode Tnode;
typedef struct ParamListEntry ParamListEntry;
typedef struct LsTableEntry LsTableEntry;


/* BINDS */

extern int nextBinding;
int getNextBinding(int size);


/* PARAMLIST */

typedef struct ParamListEntry {
    char* varName;
    TypeTable* type;
    struct ParamListEntry* next;
    int isPtr;
} ParamListEntry;

ParamListEntry* createParamListEntry(char* varName, TypeTable* dataType, int isPtr);
ParamListEntry* findInParamList(ParamListEntry* head, char* varName);
ParamListEntry* concatParamList(ParamListEntry* head1, ParamListEntry* head2);

void printParamList(ParamListEntry* paramListHead);


/* GLOBAL SYMBOL TABLE */

int nextFLabel;
int getNextFLabel();

typedef struct GsTableEntry {
    char* varName;
    TypeTable* type;
    int size;
    int binding;
    int fLabel;
    struct ParamListEntry* paramList;
    struct GsTableEntry * next;
    struct Tnode* dimensions;
    int isPtr;
} GsTableEntry;

extern GsTableEntry* gsTableHead;

GsTableEntry* findInGsTable(GsTableEntry* head, char* varName);
GsTableEntry* concatGsTable(GsTableEntry* head1, GsTableEntry* head2);

GsTableEntry* createEmptyGsTableEntry();
GsTableEntry* createIdEntryInGsTable(char* varName);
GsTableEntry* createArrEntryInGsTable(char* varName, Tnode* arrOffsetNode);
GsTableEntry* createFnEntryInGsTable(char* varName, ParamListEntry* paramListHead);
GsTableEntry* createPtrEntryInGsTable(char* varName);

GsTableEntry* setGsTableType(GsTableEntry* head, TypeTable* type);

void printGsTable();


/* LOCAL SYMBOL TABLE*/

typedef struct LsTableEntry {
    char* varName;
    TypeTable* type;
    int binding;
    struct LsTableEntry* next;
    int isPtr;
} LsTableEntry;

extern LsTableEntry* lsTableHead;

LsTableEntry* findInLsTable(LsTableEntry* head, char* varName);

LsTableEntry* createLsTableEntry();
LsTableEntry* createIdEntryInLsTable(char* varName);
LsTableEntry* createPtrEntryInLsTable(char* varName);

LsTableEntry* concatLsTable(LsTableEntry* head1, LsTableEntry* head2);
LsTableEntry* addParamsToLsTable(LsTableEntry* head, ParamListEntry* paramListHead);

LsTableEntry* setLsTableType(LsTableEntry* head, TypeTable* type);

void setLDeclBinding(LsTableEntry* head);

void freeLsTable();
void printLsTable();


#endif