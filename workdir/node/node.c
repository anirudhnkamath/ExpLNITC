#include "./node.h"
#include "../define/constants.h"
#include "../gsTable/gsTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tnode* createEmptyNode() {
    Tnode* n = (Tnode*)malloc(sizeof(Tnode));
    n->tnodeType = NODE_EMPTY;
    n->left = n->right = NULL;
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
    n->type = INTEGER_TYPE;
    
    return n;
}


Tnode* createStrLtrlNode(char* str) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_STR_LTRL;
    n->strVal = str;
    n->type = STRING_TYPE;
    
    return n;
}


Tnode* createIdNode(char varName[]) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ID;
    n->varName = varName;

    if(declarationOverFlag == 0)
        return n;

    GsTableEntry* found = findInGsTable(gsTableHead, varName);
    if(!found) {
        printf("Error: undeclared variable\n");
        exit(1);
    }

    n->type = found->dataType;
    n->gsTableEntry = found;
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

    if(exprNode->type == BOOLEAN_TYPE) {
        printf("Error: Mismatched type\n");
        exit(1);
    }

    n->left = exprNode;
    return n;
}

Tnode* createAssignNode(Tnode* idNode, Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ASSIGN;

    if(exprNode->type == BOOLEAN_TYPE) {
        printf("Error : type mismatch\n");
        exit(1);
    }

    if(idNode->type != exprNode->type) {
        printf("Error : type mismatch\n");
        exit(1);
    }

    n->left = idNode;
    n->right = exprNode;
    return n;
}


Tnode* createIfElseNode(Tnode* condNode, Tnode* ifNode, Tnode* elseNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_IF_ELSE;
    
    if(condNode->type != BOOLEAN_TYPE) {
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
    
    if(condNode->type != BOOLEAN_TYPE) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}


Tnode* createWhileNode(Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_WHILE;

    if(condNode->type != BOOLEAN_TYPE) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}

Tnode* createDoWhileNode(Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_DOWHILE;

    if(condNode->type != BOOLEAN_TYPE) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}

Tnode* createRepeatUntilNode(Tnode* condNode, Tnode* stmtNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_REPEATUNTIL;

    if(condNode->type != BOOLEAN_TYPE) {
        printf("Error: Mistmatch type\n");
        exit(1);
    }

    n->left = condNode;
    n->right = stmtNode;

    return n;
}


Tnode* createBreakNode() {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_BREAK;
    return n;
}

Tnode* createContinueNode() {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_CONTINUE;
    return n;
}


Tnode* createArithOpNode(int tnodeType, Tnode* left, Tnode* right) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if(left->type != INTEGER_TYPE || right->type != INTEGER_TYPE) {
        printf("Error: mistmatch type\n");
        exit(1);
    }

    n->left = left;
    n->right = right;
    n->type = INTEGER_TYPE;
    return n;
}

Tnode* createRelOpNode(int tnodeType, Tnode* left, Tnode* right) {
    Tnode* n = createEmptyNode();
    n->tnodeType = tnodeType;

    if(left->type != INTEGER_TYPE || right->type != INTEGER_TYPE) {
        printf("Error: mistmatch type\n");
        exit(1);
    }

    n->left = left;
    n->right = right;
    n->type = BOOLEAN_TYPE;
    return n;
}