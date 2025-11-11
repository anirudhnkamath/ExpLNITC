#include "./classTable.h"
#include "../define/constants.h"
#include "../node/node.h"
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

ClassTable* createEmptyClass(char* className, char* parentName) {

    ClassTable* parent;
    if(parentName) {
        parent = searchInClassTable(classTableHead, parentName);
        if(!parent) {
            printf("Error : invalid super class in class definition\n");
            exit(1);
        }
    }
    else
        parent = NULL;

    ClassTable* newCls = (ClassTable*)malloc(sizeof(ClassTable));
    newCls->name = strdup(className);
    newCls->fieldList = NULL;
    newCls->mthdList = NULL;
    newCls->parentPtr = parent;
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

int isParentClass(ClassTable* class, ClassTable* parent) {
    ClassTable* temp = class;
    while(temp != NULL) {
        if(temp == parent)
            return 1;
        temp = temp->parentPtr;
    }
    return 0;
}

void updateClassTableEntry(char* name, ClsFldList* clsFldList, ClsMthdList* clsMthdList) {
    ClassTable* clsEntry = searchInClassTable(classTableHead, name);
    if (!clsEntry) {
        printf("Error: Class '%s' not found while updating class table.\n", name);
        exit(1);
    }
    ClassTable* parent = clsEntry->parentPtr;

    if(parent && parent->parentPtr) {
        printf("Error : only one level of inheritance allowed\n");
        exit(1);
    }

    ClsFldList* finalFldList = NULL;
    ClsMthdList* finalMthdList = NULL;

    // add parent fldlist
    if (parent) {

        ClsFldList* pcur = parent->fieldList;
        ClsFldList* prev = NULL;

        while (pcur) {
            ClsFldList* newFld = (ClsFldList*)malloc(sizeof(ClsFldList));
            newFld->name = strdup(pcur->name);
            newFld->type = pcur->type;
            newFld->clsType = pcur->clsType;
            newFld->index = pcur->index;
            newFld->next = NULL;

            if (!finalFldList)
                finalFldList = newFld;
            else
                prev->next = newFld;
            prev = newFld;

            pcur = pcur->next;
        }
    }

    // add child fldlist
    finalFldList = concatClsFld(finalFldList, clsFldList);


    // add parent methodlist
    if (parent) {

        ClsMthdList* pcur = parent->mthdList;
        ClsMthdList* prev = NULL;

        while (pcur) {
            ClsMthdList* newMthd = (ClsMthdList*)malloc(sizeof(ClsMthdList));
            newMthd->name = strdup(pcur->name);
            newMthd->type = pcur->type;
            newMthd->paramlist = pcur->paramlist;
            newMthd->index = pcur->index;
            newMthd->fLabel = pcur->fLabel;
            newMthd->next = NULL;

            if (!finalMthdList)
                finalMthdList = newMthd;
            else
                prev->next = newMthd;
            prev = newMthd;

            pcur = pcur->next;
        }
    }

    // if methods are re-declared, change the label
    ClsMthdList* childM = clsMthdList;
    while (childM) {
        ClsMthdList* inherited = searchInMthdDecl(finalMthdList, childM->name);

        if (inherited) {
            inherited->type = childM->type;
            inherited->paramlist = childM->paramlist;
            inherited->fLabel = childM->fLabel;
        } else {
            finalMthdList = concatMthdDecl(finalMthdList, createMthdDecl(childM->type->name, childM->name, childM->paramlist));
        }
        childM = childM->next;
    }



    clsEntry->fieldList = finalFldList;
    clsEntry->mthdList = finalMthdList;

    // set index and count

    int fIndex = 0;
    for (ClsFldList* f = finalFldList; f; f = f->next)
        f->index = fIndex++;
    clsEntry->fieldCount = fIndex;

    int mIndex = 0;
    for (ClsMthdList* m = finalMthdList; m; m = m->next)
        m->index = mIndex++;
    clsEntry->methodCount = mIndex;

    if(fIndex > MAX_FIELDS) {
        printf("Error : maximum fields in a class is limited to 8\n");
        exit(1);
    }

    if(mIndex > MAX_FIELDS) {
        printf("Error : maximum methods in a class is limited to 8\n");
        exit(1);
    }

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

ClsMthdList* validateMthd(TypeTable* returnType, Tnode* idNode, ParamListEntry* fnParams) {
    
    ClassTable* foundClass = classTableHead;
    while(foundClass->next)
        foundClass = foundClass->next;

    ClsMthdList* found = searchInMthdDecl(foundClass->mthdList, idNode->varName);
    if(!found) {
        printf("Error : The method defined was not declared\n");
        exit(1);
    }

    curFnType = found->type;

    if(strcmp(returnType->name, found->type->name) != 0) {
        printf("Error : Mismatch in return type of method definition\n");
        exit(1);
    }

    ParamListEntry* declParams = found->paramlist;
    ParamListEntry *temp1 = fnParams, *temp2 = declParams;
    while(temp1 && temp2) {
        if( temp1->type != temp2->type ||
            temp1->class != temp2->class ||
            strcmp(temp1->varName, temp2->varName) != 0 ||
            temp1->isPtr != temp2->isPtr
        ) {
            printf("Error : Parameters of method definition and declaration conflict\n");
            exit(1);
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    if(temp1 || temp2) {
        printf("Error : Parameters of method definition and declaration conflict\n");
        exit(1);
    }

    return found;
}
