#ifndef CLASSTABLE_H
#define CLASSTABLE_H

#include "../gsTable/gsTable.h"
#include "../node/node.h"
#include "../typeTable/typeTable.h"

int yyerror(const char* msg);

typedef struct ClassTable ClassTable;
typedef struct ClsFldList ClsFldList;
typedef struct ClsMthdList ClsMthdList;
typedef struct ParamListEntry ParamListEntry;
typedef struct Tnode Tnode;

typedef struct ClassTable {
    char *name;
    struct ClsFldList *fieldList;
    struct ClsMthdList *mthdList;
    struct ClassTable *parentPtr;
    int index;
    int fieldCount;
    int methodCount;
    struct ClassTable *next;
} ClassTable;

typedef struct ClsFldList {
    char *name;
    int index;
    struct TypeTable *type;
    struct ClassTable *clsType;
    struct ClsFldList *next;
} ClsFldList;

typedef struct ClsMthdList {
    char *name;
    struct TypeTable *type;
    struct ParamListEntry *paramlist;
    int index;
    int fLabel;
    struct ClsMthdList *next;
} ClsMthdList;

extern ClassTable* classTableHead;

ClassTable* searchInClassTable(ClassTable* head, char* name);
ClassTable* concatClassTable(ClassTable* head1, ClassTable* head2);
ClassTable* createEmptyClass(char* className, char* parentName);
void updateClassTableEntry(char* name, ClsFldList* clsFldList, ClsMthdList* clsMthdList);
ClassTable* setClassTableIndices(ClassTable* head);
int isParentClass(ClassTable* class, ClassTable* parent);
void printClassTable();

ClsFldList* createClsFld(char* type, char* name);
ClsFldList* searchInClsFld(ClsFldList* head, char* name);
ClsFldList* concatClsFld(ClsFldList* head1, ClsFldList* head2);

ClsMthdList* createMthdDecl(char* type, char* name, struct ParamListEntry* paramList);
ClsMthdList* searchInMthdDecl(ClsMthdList* head, char* name, ParamListEntry* params, int nameMatters);
ClsMthdList* concatMthdDecl(ClsMthdList* head1, ClsMthdList* head2);
ClsMthdList* createMthdDecl(char* type, char* name, ParamListEntry* paramList);
ClsMthdList* validateMthd(TypeTable* returnType, Tnode* idNode, ParamListEntry* fnParams);
ClsMthdList* findMthdByArgs(ClsMthdList* head, char* name, Tnode* args);

#endif
