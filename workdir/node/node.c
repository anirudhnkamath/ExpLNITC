#include "./node.h"
#include "../define/constants.h"
#include "../gsTable/gsTable.h"
#include "../typeTable/typeTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TypeTable* curFnType = NULL;

Tnode* createEmptyNode() {
    Tnode* n = (Tnode*)malloc(sizeof(Tnode));
    n->tnodeType = NODE_EMPTY;
    n->left = n->right = n->argList = NULL;
    n->gsTableEntry = NULL;
    n->lsTableEntry = NULL;
    n->strVal = NULL;
    n->val = _NA_;
    n->type = NULL;
    n->class = NULL;
    n->isPtr = _NA_;
    return n;
}

Tnode* createBrkpNode() {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_BRKP;
    return n;
}

Tnode* createNullNode() {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_NULL;
    n->type = searchInTypeTable(typeTableHead, "null");
    return n;
}

Tnode* createConnectorNode(Tnode* left, Tnode* right) {
    Tnode* n = (Tnode*)malloc(sizeof(Tnode));
    n->tnodeType = NODE_CONNECTOR;
    n->left = left;
    n->right = right;
    
    return n;
}


Tnode* createIntNode(int val) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_INT;
    n->val = val;
    n->type = searchInTypeTable(typeTableHead, "int");
    n->argList = NULL;
    
    return n;
}

Tnode* createStrLtrlNode(char* str) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_STR_LTRL;
    n->strVal = strdup(str);
    n->type = searchInTypeTable(typeTableHead, "str");
    return n;
}

Tnode* createIdNode(char varName[]) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ID;
    n->varName = varName;
    return n;
}


Tnode* createReadNode(Tnode* idNode) {
    // read id
    if(idNode->tnodeType == NODE_ID && idNode->arrOffset == NULL) {
        setIdNodeType(idNode);
        validateProperId(idNode);
    }

    // read id[][]...
    else if(idNode->tnodeType == NODE_ID && idNode->arrOffset != NULL) {
        setIdNodeType(idNode);
        validateArrOffset(idNode, idNode->arrOffset);
    }

    // read id.id
    else if(idNode->tnodeType == NODE_TUP_FIELD) {
        if(idNode->right->tnodeType == NODE_FN_CALL) {
            yyerror("Invalid expression in READ\n");
        }
    }

    else {
        yyerror("Invalid node in read\n");
    }

    if (!idNode->type || 
        (strcmp(idNode->type->name, "int") != 0 && strcmp(idNode->type->name, "str") != 0)
    ) {
        yyerror("Invalid read type\n");
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_READ;
    n->left = idNode;
    return n;
}

Tnode* createWriteNode(Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_WRITE;

    if( !exprNode->type || 
        (strcmp(exprNode->type->name, "int") != 0 && strcmp(exprNode->type->name, "str") != 0)
    ) {
        yyerror("Can't write anything other than STR or INT\n");
    }

    n->left = exprNode;
    return n;
}

Tnode* createAssignNode(Tnode* leftNode, Tnode* exprNode) {
    
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ASSIGN;
    n->left = leftNode;
    n->right = exprNode;

    if(exprNode->tnodeType == NODE_ALLOC) {
        if(!leftNode->type || leftNode->type->fields == NULL) {
            yyerror("heap memory can be allocated only for user-defined types\n");
        }
        else 
            return n;
    }

    if(exprNode->tnodeType == NODE_NULL) {
        if(!leftNode->type || !leftNode->type->fields) {
            yyerror("null value can be only assigned to user-defined types\n");
        }
        else 
            return n;
    }

    if(exprNode->tnodeType == NODE_NEW) {
        if( !leftNode->class || 
            (leftNode->class != exprNode->left->class && !isParentClass(exprNode->left->class, leftNode->class))
        ) {
            yyerror("type mismatch in assigning NEW\n");
        }

        return n;
    }

    if( (leftNode->type != exprNode->type) || 
        (leftNode->isPtr != exprNode->isPtr) ||
        (leftNode->class != exprNode->class && !isParentClass(exprNode->class, leftNode->class))
    ) {
        yyerror("type mismatch in assignment\n");
    }
    
    return n;
}


Tnode* createIfElseNode(Tnode* condNode, Tnode* ifNode, Tnode* elseNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_IF_ELSE;
    
    if(!condNode->type || strcmp(condNode->type->name, "bool") != 0) {
        yyerror("Mistmatch type\n");
    }

    n->left = condNode;
    n->right = createConnectorNode(ifNode, elseNode);

    return n;
}

Tnode* createIfNode(Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_IF;
    
    if(!condNode->type || strcmp(condNode->type->name, "bool") != 0) {
        yyerror("Mistmatch type\n");
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}

Tnode* createLoopNode(int tnodeType, Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if(!condNode->type || strcmp(condNode->type->name, "bool") != 0) {
        yyerror("Mistmatch type\n");
    }

    n->left = condNode;
    n->right = stmtNode;


    return n;
}

Tnode* createLoopJumpNode(int tnodeType) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;
    return n;
}


Tnode* createArithOpNode(int tnodeType, Tnode* left, Tnode* right) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if( !left->type || !right->type || 
        strcmp(left->type->name, "int") != 0 || strcmp(right->type->name, "int") != 0
    ) {
        yyerror("invalid type in arith OP\n");
    }

    n->left = left;
    n->right = right;
    n->type = searchInTypeTable(typeTableHead, "int");
    n->argList = NULL;
    return n;
}

Tnode* createRelOpNode(int tnodeType, Tnode* left, Tnode* right) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    n->left = left;
    n->right = right;
    n->type = searchInTypeTable(typeTableHead, "bool");
    n->argList = NULL;

    TypeTable* lt = left->type;
    TypeTable* rt = right->type;

    if(!lt || !rt) {
        yyerror("invalid operands in rel OP\n");
    }

    if(tnodeType == NODE_EQ || tnodeType == NODE_NEQ) {
        int leftNull = strcmp(lt->name, "null") == 0;
        int rightNull = strcmp(rt->name, "null") == 0;

        if(leftNull && rt->fields || rightNull && lt->fields || leftNull && rightNull) {
            return n;
        }

        if(strcmp(lt->name, rt->name) != 0) {
            yyerror("type mismatch in comparison\n");
        }

        if(lt->fields != NULL) {
            yyerror("user-defined types cannot be compared\n");
        }

        return n;
    }

    else {
        if(strcmp(lt->name, rt->name) != 0) {
            yyerror("type mismatch in comparison\n");
        }

        if(lt->fields != NULL) {
            yyerror("user-defined types cannot be compared\n");
        }
    }


    return n;
}

Tnode* createLogOpNode(int tNodeType, Tnode* left, Tnode* right) {
    if( !left->type || !right->type || 
        strcmp(left->type->name, "bool") != 0 || strcmp(right->type->name, "bool") != 0
    ) {
        yyerror("mistmatch type\n");
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = tNodeType;
    n->left = left;
    n->right = right;
    n->type = searchInTypeTable(typeTableHead, "bool");
    return n;
}


Tnode* createAddrToNode(Tnode* idNode) {
    setIdNodeType(idNode);
    validateProperId(idNode);

    if(!idNode->type || idNode->type->fields) {
        yyerror("pointers only allower with primitive types\n");
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ADDR_TO;
    n->left = idNode;
    n->type = idNode->type;
    n->isPtr = 1;
    return n;
}

Tnode* createDerefNode(Tnode* idNode) {

    setIdNodeType(idNode);

    TypeTable* curType; 
    if(idNode->isPtr != 1) {
        yyerror("invalid address access\n");
    }
    else curType = idNode->type;

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_DEREF;
    n->type = curType;
    n->left = idNode;

    return n;
}


Tnode* createFnCallNode(Tnode* idNode, Tnode* argListNode) {
    Tnode* n = createEmptyNode();

    setIdNodeType(idNode);

    if(idNode->gsTableEntry->fLabel == _NA_) {
        yyerror("ID doesnt accept aruments\n");
    }

    // checking datattypes with declarations
    ParamListEntry* declParams = idNode->gsTableEntry->paramList;
    ParamListEntry *temp1 = declParams;
    Tnode* temp2 = argListNode;
    while(temp1 && temp2) {
        if( temp1->type != temp2->type ||
            temp1->class != temp2->class ||
            temp1->isPtr != temp2->isPtr
        ) {
            yyerror("type mismatch in function call\n");
        }
        temp1 = temp1->next;
        temp2 = temp2->argList;
    }
    if(temp1 || temp2) {
        yyerror("invalid function call arguments\n");
    }

    n->tnodeType = NODE_FN_CALL;
    n->type = idNode->type;
    n->left = idNode;
    n->left->argList = argListNode;
    return n;
}

Tnode* addArgToArgList(Tnode* list, Tnode* expr) {
    Tnode* n = list;
    while(n->argList != NULL)
        n = n->argList;

    n->argList = expr;
    return list;
}

Tnode* createReturnNode(Tnode* exprNode) {
    if(!exprNode->type || strcmp(exprNode->type->name, curFnType->name) != 0) {
        yyerror("invalid return type for function\n");
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_RET;
    n->left = exprNode;
    return n;
}


Tnode* createTupEntryNode(Tnode* tupNode, Tnode* fieldNode) {

    // if class node
    if(tupNode->class) {

        // if accessing a field
        if(fieldNode->tnodeType == NODE_ID) {

            // if self is accessing
            if(tupNode->tnodeType == NODE_ID && strcmp(tupNode->varName, "self") == 0) {
                ClsFldList* foundField = NULL;
                if(tupNode->class->fieldList == NULL) {
                    yyerror("invalid class field being accessed\n");
                }

                else foundField = searchInClsFld(tupNode->class->fieldList, fieldNode->varName);
                if(!foundField) {
                    yyerror("invalid field in class\n");
                }

                Tnode* n = createEmptyNode();
                n->left = tupNode;
                n->right = fieldNode;
                n->tnodeType = NODE_TUP_FIELD;

                if(foundField->type) {
                    n->type = foundField->type;
                }
                else {
                    n->class = foundField->clsType;
                }
                
                return n;
            }

            // something else is accessing
            else {
                yyerror("private fields cannot be accesses without self pointer\n");
            }
        }

        // accessing a method
        else {
        
            if(tupNode->type || !tupNode->class->mthdList) {
                yyerror("method called on invalid type\n");
            }

            ClsMthdList* curMthd = findMthdByArgs(tupNode->class->mthdList, fieldNode->left->varName, fieldNode->left->argList);
            if(!curMthd) {
                yyerror("method called on invalid type\n");
            }

            ParamListEntry* temp1 = curMthd->paramlist;
            Tnode* temp2 = fieldNode->left->argList;

            while(temp1 && temp2) {
                if( temp1->type != temp2->type ||
                    temp1->class != temp2->class ||
                    temp1->isPtr != temp2->isPtr
                ) {
                    yyerror("conflicting arguments in method call\n");
                }

                temp1 = temp1->next;
                temp2 = temp2->argList;
            }

            if(temp1 || temp2) {
                yyerror("conflicting arguments in method call\n");
            }

            Tnode* n = createEmptyNode();
            n->tnodeType = NODE_TUP_FIELD;
            n->left = tupNode;
            n->right = fieldNode;
            n->right->left->class = tupNode->class;

            if(n->right->class) {
                yyerror("methods cannot return classes\n");
            }

            n->type = curMthd->type;
            return n;
        }
    }

    // if user type
    else {
        FieldList* foundField = NULL;
        if(tupNode->type->fields == NULL) {
            yyerror("invalid user-type being accessed\n");
        }

        else foundField = searchInFieldList(tupNode->type->fields, fieldNode->varName);
        if(!foundField) {
            yyerror("invalide field in user-type\n");
        }

        Tnode* n = createEmptyNode();
        n->left = tupNode;
        n->right = fieldNode;
        n->type = foundField->type;
        n->tnodeType = NODE_TUP_FIELD;
        
        return n;
    }
}


Tnode* createAllocNode() {
    Tnode* t = createEmptyNode();
    t->tnodeType = NODE_ALLOC;
    return t;
}

Tnode* createFreeNode(Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_FREE;

    if( !exprNode->type ||
        strcmp(exprNode->type->name, "int") == 0 || 
        strcmp(exprNode->type->name, "str") == 0
    ) {
        yyerror("primitive values cannot be freed\n");
    }

    n->left = exprNode;
    return n;
}

Tnode* createInitlzeNode() {
    Tnode* t = createEmptyNode();
    t->tnodeType = NODE_INITLZE;
    return t;
}


Tnode* createMthdCallNode(Tnode* idNode, Tnode* argList) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_FN_CALL;
    n->left = idNode;
    n->left->argList = argList;

    return n;
}

Tnode* createNewNode(Tnode* idNode) {
    ClassTable* class = searchInClassTable(classTableHead, idNode->varName);
    if(!class) {
        yyerror("NEW called on invalid class name\n");
    }

    Tnode* n = createEmptyNode();
    n->left = idNode;
    n->tnodeType = NODE_NEW;
    n->left->class = class;

    return n;
}

Tnode* createDeleteNode(Tnode* exprNode) {
    if(!exprNode->class) {
        yyerror("variables of non-class type cannot be deleted\n");
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_DELETE;
    n->left = exprNode;
    return n;
}


void setIdNodeType(Tnode* idNode) {
    LsTableEntry* lsEntry = findInLsTable(lsTableHead, idNode->varName);
    if(lsEntry) {
        idNode->lsTableEntry = lsEntry;
        idNode->isPtr = lsEntry->isPtr;
        idNode->type = lsEntry->type;
        idNode->class = lsEntry->class;
        return;
    }

    GsTableEntry* gsEntry = findInGsTable(gsTableHead, idNode->varName);
    if(gsEntry) {
        idNode->gsTableEntry = gsEntry;
        idNode->isPtr = gsEntry->isPtr;
        idNode->type = gsEntry->type;
        idNode->class = gsEntry->class;
        return;
    }

    yyerror("Undeclared variable being used\n");
}

void validateFunction(TypeTable* retType, Tnode* idNode, ParamListEntry* fnParams) {

    setIdNodeType(idNode);
    curFnType = idNode->type;

    if(!curFnType) {
        yyerror("invalid return type for function\n");
    }

    GsTableEntry* found = findInGsTable(gsTableHead, idNode->varName);
    if(!found) {
        yyerror("The defined function was never declared\n");
    }

    if(strcmp(retType->name, found->type->name) != 0) {
        yyerror("Return type of function definition and declaration conflicts\n");
    }

    if(found->fLabel == _NA_) {
        yyerror("Wrong function name in function definition\n");
    }

    ParamListEntry* declParams = found->paramList;
    ParamListEntry *temp1 = fnParams, *temp2 = declParams;
    while(temp1 && temp2) {
        if( temp1->class != temp2->class ||
            temp1->type != temp2->type ||
            temp1->isPtr != temp2->isPtr ||
            strcmp(temp1->varName, temp2->varName) != 0
        ) {
            yyerror("Parameters of function definition and declaration conflict\n");
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    if(temp1 || temp2) {
        yyerror("Parameters of function definition and declaration conflict\n");
    }

    return;
}

void validateProperId(Tnode* idNode) {
    if(idNode->isPtr == 1) {
        yyerror("Invalid use of proper ID\n");
    }
    
    // not fn and not array
    if( idNode->gsTableEntry && 
        idNode->gsTableEntry->fLabel == _NA_ && 
        idNode->gsTableEntry->dimensions == NULL &&
        idNode->gsTableEntry->isPtr == _NA_ &&
        (strcmp(idNode->gsTableEntry->type->name, "int") == 0 || strcmp(idNode->gsTableEntry->type->name, "str") == 0)
    ) return;

    if( idNode->lsTableEntry &&
        idNode->lsTableEntry->isPtr == _NA_ &&
        (strcmp(idNode->type->name, "int") == 0 || strcmp(idNode->type->name, "str") == 0)
    ) return;

    yyerror("Invalid use of proper ID\n");
}

void validateArrOffset(Tnode* idNode, Tnode* indexExprNode) {
    if(idNode->lsTableEntry) {
        yyerror("Invalid ID being indexed\n");
    }

    if(idNode->gsTableEntry && idNode->gsTableEntry->dimensions == NULL) {
        yyerror("Invalid ID being indexed\n");
    }

    Tnode* temp1 = idNode->gsTableEntry->dimensions;
    Tnode* temp2 = indexExprNode;

    while(temp1 && temp2) {
        temp1 = temp1->arrOffset;
        temp2 = temp2->arrOffset;
    }

    if(temp1 || temp2) {
        yyerror("invalid indexing\n");
    }
}

void validateIdForExpr(Tnode* idNode) {
    if(idNode->gsTableEntry) {
        if(idNode->gsTableEntry->fLabel != _NA_) {
            yyerror("invalid ID used in expression\n");
        }
        if(idNode->gsTableEntry->dimensions) {
            yyerror("invalid ID used in expression\n");
        }
        if(idNode->gsTableEntry->isPtr == 1) {
            yyerror("invalid ID used in expression\n");
        }
    }

    else { // in ls table
        if(idNode->isPtr == 1) {
            yyerror("invalid ID used in expression\n");
        }
    }
}

void freeTree(Tnode* root) {
    if (root == NULL) 
        return;

    freeTree(root->left);
    freeTree(root->right);
    freeTree(root->argList);
    root->argList = NULL;
    
    if(root->strVal) free(root->strVal);
    free(root);
}

void inorder(Tnode* node) {
    if (node == NULL) return;

    inorder(node->left);

    inorder(node->argList);
    switch (node->tnodeType) {
        case NODE_CONNECTOR:   printf("CONNECTOR\n"); break;
        case NODE_EMPTY:       printf("EMPTY\n"); break;
        case NODE_WRITE:       printf("WRITE\n"); break;
        case NODE_READ:        printf("READ\n"); break;
        case NODE_ASSIGN:      printf("ASSIGN\n"); break;
        case NODE_ADD:         printf("ADD\n"); break;
        case NODE_SUB:         printf("SUB\n"); break;
        case NODE_DIV:         printf("DIV\n"); break;
        case NODE_MULT:        printf("MULT\n"); break;
        case NODE_ID:          printf("ID(%s)\n", node->varName); break;
        case NODE_INT:         printf("INT(%d)\n", node->val); break;
        case NODE_STR_LTRL:    printf("STR_LTRL(%s)\n", node->strVal); break;
        case NODE_EQ:          printf("EQ\n"); break;
        case NODE_NEQ:         printf("NEQ\n"); break;
        case NODE_GTE:         printf("GTE\n"); break;
        case NODE_GT:          printf("GT\n"); break;
        case NODE_LTE:         printf("LTE\n"); break;
        case NODE_LT:          printf("LT\n"); break;
        case NODE_IF:          printf("IF\n"); break;
        case NODE_IF_ELSE:     printf("IF_ELSE\n"); break;
        case NODE_WHILE:       printf("WHILE\n"); break;
        case NODE_DOWHILE:     printf("DO_WHILE\n"); break;
        case NODE_REPEATUNTIL: printf("REPEAT_UNTIL\n"); break;
        case NODE_BREAK:       printf("BREAK\n"); break;
        case NODE_CONTINUE:    printf("CONTINUE\n"); break;
        case NODE_ARR_IND:     printf("NODE_ARR_INDEX\n"); break;
        case NODE_MOD:         printf("NODE_MOD\n"); break;
        case NODE_ARR_IND_2D:  printf("NODE_ARR_INDEX_2D\n"); break;
        case NODE_ADDR_TO:     printf("NODE_ADDR\n"); break;
        case NODE_DEREF:       printf("NODE_DEREF\n"); break;
        case NODE_FN_CALL:     printf("NODE_FN_CALL\n"); break;
        case NODE_RET:         printf("NODE_RET\n"); break;
        default:               printf("NOIDEA\n"); break;
    }

    inorder(node->right);
}

Tnode* insertToArrDimn(Tnode* root, Tnode* node) {
    Tnode* temp = root;

    if(!temp)
        return node;

    while(temp->arrOffset)
        temp = temp->arrOffset;

    temp->arrOffset = node;
    return root;
}
