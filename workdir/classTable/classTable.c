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
            yyerror("invalid super class in class definition\n");
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
        yyerror("duplicate name classes detected\n");
    }

    TypeTable* ttemp = searchInTypeTable(typeTableHead, className);
    if(ttemp) {
        yyerror("class name is same as previously defined type\n");
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
    ClassTable* parent = clsEntry->parentPtr;

    // copy parent fields into final field list
    ClsFldList* finalFldList = NULL;
    if (parent) {
        ClsFldList* pcur = parent->fieldList;
        while (pcur) {

            // duplicate all entries
            ClsFldList* newFld = (ClsFldList*)malloc(sizeof(ClsFldList));
            newFld->name = strdup(pcur->name);
            newFld->type = pcur->type;
            newFld->clsType = pcur->clsType;
            newFld->index = pcur->index;
            newFld->next = NULL;

            finalFldList = concatClsFld(finalFldList, newFld);
            pcur = pcur->next;
        }
    }

    // add child fldlist to final fldlist
    finalFldList = concatClsFld(finalFldList, clsFldList);



    // add parent methodlist to final mtdhlist
    ClsMthdList* finalMthdList = NULL;
    if (parent) {

        ClsMthdList* pcur = parent->mthdList;

        while (pcur) {
            ClsMthdList* newMthd = (ClsMthdList*)malloc(sizeof(ClsMthdList));
            newMthd->name = strdup(pcur->name);
            newMthd->type = pcur->type;
            newMthd->paramlist = pcur->paramlist;
            newMthd->index = pcur->index;
            newMthd->fLabel = pcur->fLabel;
            newMthd->next = NULL;

            finalMthdList = concatMthdDecl(finalMthdList, newMthd);

            pcur = pcur->next;
        }
    }

    // add child mthdlist to final mthd list after overriding
    while (clsMthdList) {
        ClsMthdList* curMthd = clsMthdList;
        clsMthdList = clsMthdList->next;
        curMthd->next = NULL;

        // overriden fuction should have same signature
        ClsMthdList* override1 = searchInMthdDecl(finalMthdList, curMthd->name, curMthd->paramlist, 0);
        ClsMthdList* override2 = searchInMthdDecl(finalMthdList, curMthd->name, curMthd->paramlist, 1);

        if(override1 && !override2) {
            yyerror("invalid overriding function\n");
        }

        if(override1 && override1->type != curMthd->type) {
            yyerror("invalid return type for overriding function\n");
        }
        
        // copy content to parents node
        if(override1) {
            override1->type = curMthd->type;
            override1->paramlist = curMthd->paramlist;
            override1->fLabel = curMthd->fLabel;
        } 

        // else append
        else {
            if(!finalMthdList)
                finalMthdList = curMthd;
            else {
                ClsMthdList* temp = finalMthdList;
                while(temp->next) {
                    temp = temp->next;
                }
                temp->next = curMthd;
            }
        }

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
        yyerror("maximum fields in a class is limited to 8\n");
    }

    if(mIndex > MAX_FIELDS) {
        yyerror("maximum methods in a class is limited to 8\n");
    }

    // reserve space before global variables for vtable
    getNextBinding(MAX_FIELDS);
    
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
        yyerror("class-type fields not allowed\n");
    }

    printf("Error: undefined type for class field\n");
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
            yyerror("duplicate fields in class\n");
        }

        curr2 = curr2->next;
    }

    tail1->next = head2;
    return head1;
}



ClsMthdList* concatMthdDecl(ClsMthdList* head1, ClsMthdList* head2) {
    if (!head2) return head1;
    if (!head1) return head2;

    ClsMthdList* m2 = head2;
    while (m2) {
        ClsMthdList* m1 = head1;

        while (m1) {
            if (strcmp(m1->name, m2->name) == 0) {
                ParamListEntry *a = m1->paramlist, *b = m2->paramlist;
                
                int same = 1;
                while (a && b) {
                    if (a->type != b->type) {
                        same = 0;
                        break;
                    }

                    a = a->next;
                    b = b->next;
                }

                if (same && !a && !b) {
                    yyerror("multiple class methods with same signature\n");
                }
            }
            m1 = m1->next;
        }

        m2 = m2->next;
    }

    ClsMthdList* tail = head1;
    while (tail->next)
        tail = tail->next;
    tail->next = head2;

    return head1;
}

ClsMthdList* searchInMthdDecl(ClsMthdList* head, char* name, ParamListEntry* params, int nameMatters) {
    ClsMthdList* temp = head;

    while (temp) {

        if (strcmp(temp->name, name) == 0) {

            ParamListEntry* p = temp->paramlist;
            ParamListEntry* q = params;

            int same = 1;

            while (p && q) {
                if ((nameMatters && strcmp(p->varName, q->varName) != 0) || 
                    p->type != q->type
                ) {
                    same = 0;
                    break;
                }
                p = p->next;
                q = q->next;
            }

            if (same && !p && !q)
                return temp;
        }

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
}

ClsMthdList* validateMthd(TypeTable* returnType, Tnode* idNode, ParamListEntry* fnParams) {
    
    ClassTable* foundClass = classTableHead;
    while(foundClass->next)
        foundClass = foundClass->next;

    ClsMthdList* found = searchInMthdDecl(foundClass->mthdList, idNode->varName, fnParams, 1);
    if(!found) {
        yyerror("The method defined was not declared\n");
    }

    curFnType = found->type;

    if(strcmp(returnType->name, found->type->name) != 0) {
        yyerror("Mismatch in return type of method definition\n");
    }

    ParamListEntry* declParams = found->paramlist;
    ParamListEntry *temp1 = fnParams, *temp2 = declParams;
    while(temp1 && temp2) {
        if( temp1->type != temp2->type ||
            temp1->class != temp2->class ||
            strcmp(temp1->varName, temp2->varName) != 0 ||
            temp1->isPtr != temp2->isPtr
        ) {
            yyerror("Parameters of method definition and declaration conflict\n");
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    if(temp1 || temp2) {
        yyerror("Parameters of method definition and declaration conflict\n");
    }

    return found;
}

ClsMthdList* findMthdByArgs(ClsMthdList* head, char* name, Tnode* args) {
    ClsMthdList* temp = head;
    while(temp) {   

        if(strcmp(temp->name, name) == 0) {
            ParamListEntry* p = temp->paramlist;
            Tnode* q = args;

            int same = 1;
            while(p && q) {
                if(p->type != q->type) {
                    same = 0;
                    break;
                }

                p = p->next;
                q = q->argList;
            }

            if(same && !p && !q)
                return temp;
        }

        temp = temp->next;
    }

    return NULL;
}