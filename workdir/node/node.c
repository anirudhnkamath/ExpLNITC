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
    n->type = searchInTypeTable("int");
    n->argList = NULL;
    
    return n;
}

Tnode* createStrLtrlNode(char* str) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_STR_LTRL;
    n->strVal = strdup(str);
    n->type = searchInTypeTable("str");
    return n;
}

Tnode* createIdNode(char varName[]) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ID;
    n->varName = varName;
    return n;
}


Tnode* createReadNode(Tnode* idNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_READ;
    n->left = idNode;
    return n;
}

Tnode* createWriteNode(Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_WRITE;

    if(strcmp(exprNode->type->name, "bool") == 0) {
        printf("Error: Mismatched type\n");
        exit(1);
    }

    n->left = exprNode;
    return n;
}

Tnode* createAssignNode(Tnode* leftNode, Tnode* exprNode) {
    if(strcmp(leftNode->type->name, exprNode->type->name) != 0) {
        printf("Error : type mismatch\n");
        exit(1);
    }

    // lhs is id or ptr or arr offset
    if(leftNode->tnodeType == NODE_ID) {

        if(leftNode->arrOffset) {
            validateArrOffset(leftNode, leftNode->arrOffset);
        }

        else if(strcmp(leftNode->type->name, "intPtr") == 0 || strcmp(leftNode->type->name, "strPtr") == 0) {
            // do nothing
        }

        else {
            validateProperId(leftNode); 
        }
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ASSIGN;
    n->left = leftNode;
    n->right = exprNode;
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
    n->type = searchInTypeTable("int");
    n->argList = NULL;
    return n;
}

Tnode* createRelOpNode(int tnodeType, Tnode* left, Tnode* right) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if(strcmp(left->type->name, "int") != 0 || strcmp(right->type->name, "int") != 0) {
        printf("Error: mistmatch type\n");
        exit(1);
    }

    n->left = left;
    n->right = right;
    n->type = searchInTypeTable("bool");
    n->argList = NULL;
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
    n->type = searchInTypeTable("bool");
    return n;
}


Tnode* createAddrToNode(Tnode* idNode) {
    validateProperId(idNode);

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ADDR_TO;
    n->left = idNode;

    if(strcmp(idNode->type->name, "int") == 0)
        n->type = searchInTypeTable("intPtr");
    else
        n->type = searchInTypeTable("strPtr");
        
    return n;
}

Tnode* createDerefNode(Tnode* idNode) {

    TypeTable* curType; 
    if(idNode->lsTableEntry) {
        if(strcmp(idNode->lsTableEntry->type->name, "intPtr") != 0 && strcmp(idNode->lsTableEntry->type->name, "strPtr") != 0) {
            printf("Error : invalid address access\n");
            exit(1);
        }
        else curType = idNode->lsTableEntry->type;
    }
    else if(idNode->gsTableEntry) {
        if(strcmp(idNode->gsTableEntry->type->name, "intPtr") != 0 && strcmp(idNode->gsTableEntry->type->name, "strPtr") != 0) {
            printf("Error : invalid address access\n");
            exit(1);
        }
        else curType = idNode->gsTableEntry->type;
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_DEREF;

    if(strcmp(curType->name, "intPtr") == 0)
        n->type = searchInTypeTable("int");
    else
        n->type = searchInTypeTable("str");

    n->left = idNode;

    return n;
}


Tnode* createFnCallNode(Tnode* idNode, Tnode* argListNode) {
    Tnode* n = createEmptyNode();

    if(idNode->gsTableEntry->fLabel == _NA_) {
        printf("Error : ID doesnt accept aruments\n");
        exit(1);
    }

    // checking datattypes with declarations
    ParamListEntry* declParams = idNode->gsTableEntry->paramList;
    ParamListEntry *temp1 = declParams;
    Tnode* temp2 = argListNode;
    while(temp1 && temp2) {
        if(strcmp(temp1->type->name, temp2->type->name) != 0) {
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



void setIdNodeType(Tnode* idNode) {
    LsTableEntry* lsEntry = findInLsTable(lsTableHead, idNode->varName);
    if(lsEntry) {
        idNode->lsTableEntry = lsEntry;
        idNode->type = lsEntry->type;
        return;
    }

    GsTableEntry* gsEntry = findInGsTable(gsTableHead, idNode->varName);
    if(gsEntry) {
        idNode->gsTableEntry = gsEntry;
        idNode->type = gsEntry->type;
        return;
    }

    printf("Error : undeclared variable\n");
    exit(1);
    return;
}

void validateFunction(int retType, char* fnName, ParamListEntry* fnParams) {
    GsTableEntry* found = findInGsTable(gsTableHead, fnName);
    if(!found) {
        printf("Error : fn not exists\n");
        exit(1);
    }

    if(retType == INTEGER_TYPE && strcmp(found->type->name, "int") != 0 ||
       retType == STRING_TYPE && strcmp(found->type->name, "str") != 0
    ) {
        printf("Error : invalid ret type in fn definition\n");
        exit(1);
    }

    if(found->fLabel == _NA_) {
        printf("Error : invalid fn\n");
        exit(1);
    }

    ParamListEntry* declParams = found->paramList;
    ParamListEntry *temp1 = fnParams, *temp2 = declParams;
    while(temp1 && temp2) {
        if(strcmp(temp1->type->name, temp2->type->name) != 0 || strcmp(temp1->varName, temp2->varName) != 0) {
            printf("Error : definition doesnt match declaration\n");
            exit(1);
        }
        temp1 = temp1->next;
        temp2 = temp2->next;
    }

    if(temp1 || temp2) {
        printf("Error : definition doesnt match declaration\n");
        exit(1);
    }

    return;
}

void validatePointer(Tnode* idNode) {
    if(idNode->lsTableEntry && (strcmp(idNode->lsTableEntry->type->name, "intPtr") == 0 || strcmp(idNode->lsTableEntry->type->name, "strPtr") == 0))
        return;

    if(idNode->gsTableEntry && (strcmp(idNode->gsTableEntry->type->name, "intPtr") == 0 || strcmp(idNode->gsTableEntry->type->name, "strPtr") == 0))
        return;

    printf("Error : invalid pointer\n");
    exit(1);
}

void validateProperId(Tnode* idNode) {
    if( idNode->lsTableEntry && 
        (strcmp(idNode->lsTableEntry->type->name, "int") == 0 || strcmp(idNode->lsTableEntry->type->name, "str") == 0)
    ) return;
    
    // not fn and not array
    if( idNode->gsTableEntry && 
        idNode->gsTableEntry->fLabel == _NA_ && 
        idNode->gsTableEntry->dimensions == NULL &&
        (strcmp(idNode->gsTableEntry->type->name, "int") == 0 || strcmp(idNode->gsTableEntry->type->name, "str") == 0)
    ) return;

    printf("Error : invalid use of ID\n");
    exit(1);
}

void validateArrOffset(Tnode* idNode, Tnode* indexExprNode) {
    if(idNode->lsTableEntry) {
        printf("Error : non-array can't be indexed\n");
        exit(1);
    }

    if(idNode->gsTableEntry && idNode->gsTableEntry->fLabel != _NA_) {
        printf("Error : functions can't be indexed\n");
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
