#include "./classTable.h"
#include "../define/constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ClassTable* classTableHead = NULL;

ClassTable* searchInClassTable(ClassTable* head, char* name) {
    ClassTable* temp = head;
    while(temp) {
        if(strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

ClassTable* concatClassTable(ClassTable* head1, ClassTable* head2) {
    if (!head2) return head1;
    if (!head1) return head2;

    ClassTable* tail1 = head1;

    while (tail1->next)
        tail1 = tail1->next;

    tail1->next = head2;
    return head1;
}

ClassTable* createEmptyClass(char* className) {
    ClassTable* newCls = (ClassTable*)malloc(sizeof(ClassTable));
    newCls->name = strdup(className);
    newCls->fieldList = NULL;
    newCls->mthdList = NULL;
    newCls->parentPtr = NULL;
    newCls->index = _NA_;
    newCls->fieldCount = _NA_;
    newCls->methodCount = _NA_;
    newCls->next = NULL;

    ClassTable* temp = searchInClassTable(classTableHead, className);
    if (temp) {
        printf("Error : duplicate name classes detected\n");
        exit(1);
    }

    TypeTable* ttemp = searchInTypeTable(typeTableHead, className);
    if(ttemp) {
        printf("Error : class name is same as previously defined type\n");
        exit(1);
    }

    return newCls;
}

void updateClassTableEntry(char* name, ClsFldList* clsFldList, ClsMthdList* clsMthdList) {
    ClassTable* clsEntry = searchInClassTable(classTableHead, name);
    if (!clsEntry) {
        printf("Error: class doesnt exist while searching\n");
        exit(1);
    }

    clsEntry->fieldList = clsFldList;
    int fIndex = 0;
    ClsFldList* ftemp = clsFldList;
    while (ftemp) {
        ftemp->index = fIndex++;
        ftemp = ftemp->next;
    }
    clsEntry->fieldCount = fIndex;

    clsEntry->mthdList = clsMthdList;
    int mIndex = 0;
    ClsMthdList* mtemp = clsMthdList;
    while (mtemp) {
        mtemp->index = mIndex++;
        mtemp = mtemp->next;
    }
    clsEntry->methodCount = mIndex;

    return;
}

ClassTable* setClassTableIndices(ClassTable* head) {
    if (!head) return NULL;

    int index = 0;
    ClassTable* temp = head;

    while (temp) {
        temp->index = index++;
        temp = temp->next;
    }

    return head;
}

void printClassTable() {
    ClassTable *cls = classTableHead;
    if (!cls) {
        printf("Class Table is empty.\n");
        return;
    }

    while (cls) {
        printf("Class: %s", cls->name);
        if (cls->parentPtr)
            printf(" (Parent: %s)", cls->parentPtr->name);
        printf("\n");

        printf("  Fields:\n");
        ClsFldList *fld = cls->fieldList;
        if (!fld) printf("    None\n");
        while (fld) {
            printf("    %s : %s", fld->name, fld->type ? fld->type->name : "class");
            if (fld->clsType) printf(" (%s)", fld->clsType->name);
            printf(" [Index: %d]\n", fld->index);
            fld = fld->next;
        }

        printf("  Methods:\n");
        ClsMthdList *mthd = cls->mthdList;
        if (!mthd) printf("    None\n");
        while (mthd) {
            printf("    %s : %s [Index: %d, Label: %d]\n",
                   mthd->name,
                   mthd->type ? mthd->type->name : "void",
                   mthd->index,
                   mthd->fLabel);
            mthd = mthd->next;
        }

        printf("\n");
        cls = cls->next;
    }
}



ClsFldList* createClsFld(char* type, char* name) {
    ClsFldList* newFld = (ClsFldList*)malloc(sizeof(ClsFldList));
    newFld->name = strdup(name);
    newFld->index = _NA_;
    newFld->next = NULL;
    newFld->type = NULL;
    newFld->clsType = NULL;

    TypeTable* ttype = searchInTypeTable(typeTableHead, type);
    if (ttype) {
        newFld->type = ttype;
        return newFld;
    }

    ClassTable* ctype = searchInClassTable(classTableHead, type);
    if (ctype) {
        newFld->clsType = ctype;
        return newFld;
    }

    printf("Error: undefined type for class field\n");
    exit(1);
}

ClsFldList* searchInClsFld(ClsFldList* head, char* name) {
    ClsFldList* temp = head;
    while (temp) {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

ClsFldList* concatClsFld(ClsFldList* head1, ClsFldList* head2) {
    if (!head2) return head1;
    if (!head1) return head2;

    ClsFldList* curr2 = head2;
    ClsFldList* tail1 = head1;

    while (tail1->next)
        tail1 = tail1->next;

    while (curr2) {
        if(searchInClsFld(head1, curr2->name) != NULL) {
            printf("Error : duplicate fields in class\n");
            exit(1);
        }

        curr2 = curr2->next;
    }

    tail1->next = head2;
    return head1;
}



ClsMthdList* concatMthdDecl(ClsMthdList* head1, ClsMthdList* head2) {
    if (!head2) return head1;
    if (!head1) return head2;

    ClsMthdList* curr2 = head2;

    while (curr2) {
        if(searchInMthdDecl(head1, curr2->name) != NULL) {
            printf("Error : multiple class methods with same name\n");
            exit(1);
        }
        curr2 = curr2->next;
    }

    ClsMthdList* tail1 = head1;
    while (tail1->next)
        tail1 = tail1->next;

    tail1->next = head2;

    return head1;
}

ClsMthdList* searchInMthdDecl(ClsMthdList* head, char* name) {
    ClsMthdList* temp = head;
    while (temp) {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

ClsMthdList* createMthdDecl(char* type, char* name, ParamListEntry* paramList) {
    ClsMthdList* newMthd = (ClsMthdList*)malloc(sizeof(ClsMthdList));
    newMthd->name = strdup(name);
    newMthd->paramlist = paramList;
    newMthd->index = _NA_;
    newMthd->fLabel = getNextFLabel();
    newMthd->next = NULL;
    newMthd->type = NULL;

    TypeTable* ttype = searchInTypeTable(typeTableHead, type);
    if (ttype) {
        newMthd->type = ttype;
        return newMthd;
    }

    printf("Error: Undefined return type for class method\n");
    exit(1);
}
