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
    }

    else {
        printf("Error : Invalid node in read\n");
        exit(1);
    }

    if(strcmp(idNode->type->name, "int") != 0 && strcmp(idNode->type->name, "str") != 0) {
        printf("Error : Invalid read type\n");
        exit(1);
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_READ;
    n->left = idNode;
    return n;
}

Tnode* createWriteNode(Tnode* exprNode) {
    
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_WRITE;

    if(strcmp(exprNode->type->name, "int") != 0 && strcmp(exprNode->type->name, "str") != 0) {
        printf("Error: Can't write anything other than STR or INT\n");
        exit(1);
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
        if(leftNode->type->fields == NULL) {
            printf("Error : heap memory can be allocated only for user-defined types\n");
            exit(1);
        }
        else 
            return n;
    }

    if(exprNode->tnodeType == NODE_NULL) {
        if(leftNode->type->fields)
            return n;
        else {
            printf("Error : null value can be only assigned to user-defined types\n");
            exit(1);
        }
    }

    else if(strcmp(leftNode->type->name, exprNode->type->name) != 0 || leftNode->isPtr != exprNode->isPtr) {
        printf("Error : type mismatch in assignment\n");
        exit(1);
    }
    
    return n;
}


Tnode* createIfElseNode(Tnode* condNode, Tnode* ifNode, Tnode* elseNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_IF_ELSE;
    
    if(strcmp(condNode->type->name, "bool") != 0) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = createConnectorNode(ifNode, elseNode);

    return n;
}

Tnode* createIfNode(Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_IF;
    
    if(strcmp(condNode->type->name, "bool") != 0) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}

Tnode* createLoopNode(int tnodeType, Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if(strcmp(condNode->type->name, "bool") != 0) {
        printf("Error: Mistmatch type\n");
        exit(1);
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

    if(strcmp(left->type->name, "int") != 0 || strcmp(right->type->name, "int") != 0) {
        printf("Error: mistmatch type\n");
        exit(1);
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

    if(tnodeType == NODE_EQ || tnodeType == NODE_NEQ) {
        int leftNull = strcmp(lt->name, "null") == 0;
        int rightNull = strcmp(rt->name, "null") == 0;

        if(leftNull && rt->fields || rightNull && lt->fields || leftNull && rightNull) {
            return n;
        }

        if(strcmp(lt->name, rt->name) != 0) {
            printf("Error : type mismatch in comparison\n");
            exit(1);
        }

        if(lt->fields != NULL) {
            printf("Error : user-defined types cannot be compared\n");
            exit(1);
        }

        return n;
    }

    else {
        if(strcmp(lt->name, rt->name) != 0) {
            printf("Error : type mismatch in comparison\n");
            exit(1);
        }

        if(lt->fields != NULL) {
            printf("Error : user-defined types cannot be compared\n");
            exit(1);
        }
    }


    return n;
}

Tnode* createLogOpNode(int tNodeType, Tnode* left, Tnode* right) {
    if(strcmp(left->type->name, "bool") != 0 || strcmp(right->type->name, "bool") != 0) {
        printf("Error: mistmatch type\n");
        exit(1);
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
    if(idNode->isPtr != 1 && idNode->isPtr != 1) {
        printf("Error : invalid address access\n");
        exit(1);
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
        printf("Error : ID doesnt accept aruments\n");
        exit(1);
    }

    // checking datattypes with declarations
    ParamListEntry* declParams = idNode->gsTableEntry->paramList;
    ParamListEntry *temp1 = declParams;
    Tnode* temp2 = argListNode;
    while(temp1 && temp2) {
        if(strcmp(temp1->type->name, temp2->type->name) != 0 || temp1->isPtr != temp2->isPtr) {
            printf("Error : type mismatch in function call\n");
            exit(1);
        }
        temp1 = temp1->next;
        temp2 = temp2->argList;
    }
    if(temp1 || temp2) {
        printf("Error : invalid function call arguments\n");
        exit(1);
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
    if(strcmp(exprNode->type->name, curFnType->name) != 0) {
        printf("Error : invalid return type for function\n");
        exit(1);
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_RET;
    n->left = exprNode;
    return n;
}


Tnode* createTupEntryNode(Tnode* tupNode, Tnode* fieldNode) {

    if(tupNode->tnodeType == NODE_TUP_FIELD) {
        // do nothing, always right
    }
    // else if(tupNode->tnodeType == NODE_ID && tupNode->arrOffset == NULL) {
    //     setIdNodeType(tupNode);
    // }
    else if(tupNode->tnodeType == NODE_ID) {
        setIdNodeType(tupNode);
    }
    else {
        printf("Error : invalid tuple access\n");
        exit(1);
    }

    FieldList* foundField = NULL;
    if(tupNode->type->fields == NULL) {
        printf("Error : invalid tuple\n");
        exit(1);
    }
    else foundField = searchInFieldList(tupNode->type->fields, fieldNode->varName);

    if(!foundField) {
        printf("Error : invalide field in tuple\n");
        exit(1);
    }

    Tnode* n = createEmptyNode();
    n->left = tupNode;
    n->right = fieldNode;
    n->type = foundField->type;
    n->tnodeType = NODE_TUP_FIELD;

    return n;
}


Tnode* createAllocNode() {
    Tnode* t = createEmptyNode();
    t->tnodeType = NODE_ALLOC;
    return t;
}

Tnode* createFreeNode(Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_FREE;

    if(strcmp(exprNode->type->name, "int") == 0 || strcmp(exprNode->type->name, "str") == 0) {
        printf("Error : primitive values cannot be freed\n");
        exit(1);
    }

    n->left = exprNode;
    return n;
}

Tnode* createInitlzeNode() {
    Tnode* t = createEmptyNode();
    t->tnodeType = NODE_INITLZE;
    return t;
}



void setIdNodeType(Tnode* idNode) {
    LsTableEntry* lsEntry = findInLsTable(lsTableHead, idNode->varName);
    if(lsEntry) {
        idNode->lsTableEntry = lsEntry;
        idNode->isPtr = lsEntry->isPtr;
        idNode->type = lsEntry->type;
        return;
    }

    GsTableEntry* gsEntry = findInGsTable(gsTableHead, idNode->varName);
    if(gsEntry) {
        idNode->gsTableEntry = gsEntry;
        idNode->isPtr = gsEntry->isPtr;
        idNode->type = gsEntry->type;
        return;
    }

    printf("Error : Undeclared variable being used\n");
    exit(1);
}

void validateFunction(TypeTable* retType, Tnode* idNode, ParamListEntry* fnParams) {

    setIdNodeType(idNode);
    curFnType = idNode->type;

    GsTableEntry* found = findInGsTable(gsTableHead, idNode->varName);
    if(!found) {
        printf("Error : The defined function was never declared\n");
        exit(1);
    }

    if(strcmp(retType->name, found->type->name) != 0) {
        printf("Error : Return type of function definition and declaration conflicts\n");
        exit(1);
    }

    if(found->fLabel == _NA_) {
        printf("Error : Wrong function name in function definition\n");
        exit(1);
    }

    ParamListEntry* declParams = found->paramList;
    ParamListEntry *temp1 = fnParams, *temp2 = declParams;
    while(temp1 && temp2) {
        if(strcmp(temp1->type->name, temp2->type->name) != 0 || strcmp(temp1->varName, temp2->varName) != 0) {
            printf("Error : Parameters of function definition and declaration conflict\n");
            exit(1);
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    if(temp1 || temp2) {
        printf("Error : Parameters of function definition and declaration conflict\n");
        exit(1);
    }

    return;
}

void validateProperId(Tnode* idNode) {
    if(idNode->isPtr == 1) {
        printf("Error : Invalid use of proper ID\n");
        exit(1);
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

    printf("Error : Invalid use of proper ID\n");
    exit(1);
}

void validateArrOffset(Tnode* idNode, Tnode* indexExprNode) {
    if(idNode->lsTableEntry) {
        printf("Error : Invalid ID being indexed\n");
        exit(1);
    }

    if(idNode->gsTableEntry && idNode->gsTableEntry->dimensions == NULL) {
        printf("Error : Invalid ID being indexed\n");
        exit(1);
    }

    Tnode* temp1 = idNode->gsTableEntry->dimensions;
    Tnode* temp2 = indexExprNode;

    while(temp1 && temp2) {
        temp1 = temp1->arrOffset;
        temp2 = temp2->arrOffset;
    }

    if(temp1 || temp2) {
        printf("Error : invalid indexing\n");
        exit(1);
    }
}

void validateIdForExpr(Tnode* idNode) {
    if(idNode->gsTableEntry) {
        if(idNode->gsTableEntry->fLabel != _NA_) {
            printf("Error : invalid ID used in expression\n");
            exit(1);
        }
        if(idNode->gsTableEntry->dimensions) {
            printf("Error : invalid ID used in expression\n");
            exit(1);
        }
        if(idNode->gsTableEntry->isPtr == 1) {
            printf("Error : invalid ID used in expression\n");
            exit(1);
        }
    }

    else { // in ls table
        if(idNode->isPtr == 1) {
            printf("Error : invalid ID used in expression\n");
            exit(1);
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
