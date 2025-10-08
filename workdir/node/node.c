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
    n->strVal = strdup(str);
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


Tnode* createArrIndexNode(Tnode* idNode, Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ARR_IND;

    if(idNode->gsTableEntry->dimension == 0) {
        printf("Error : indexing not allowed\n");
        exit(1);
    }
    if(exprNode->type != INTEGER_TYPE) {
        printf("Error : type mismatch in array index\n");
        exit(1);
    }

    n->type = idNode->gsTableEntry->dataType;
    n->left = idNode;
    n->right = exprNode;
    return n;
}

Tnode* createArrIndex2DNode(Tnode* idNode, Tnode* expr1Node, Tnode* expr2Node) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ARR_IND_2D;

    if(idNode->gsTableEntry->dimension != 2) {
        printf("Error : indexing not allowed\n");
        exit(1);
    }
    if(expr1Node->type != INTEGER_TYPE || expr2Node->type != INTEGER_TYPE) {
        printf("Error : type mismatch in array index\n");
        exit(1);
    }

    n->type = idNode->gsTableEntry->dataType;
    n->left = idNode;
    n->right = createConnectorNode(expr1Node, expr2Node);
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

Tnode* createAssignNode(Tnode* leftNode, Tnode* exprNode) {
    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ASSIGN;

    if(exprNode->type == BOOLEAN_TYPE || leftNode->type != exprNode->type) {
        printf("Error : type mismatch\n");
        exit(1);
    }

    n->left = leftNode;
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


Tnode* createAddrToNode(Tnode* idNode) {
    if(idNode->gsTableEntry->dimension != 0) {
        printf("Error : not pointer addressable\n");
        exit(1);
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_ADDR_TO;
    n->type = idNode->gsTableEntry->dataType;
    n->left = idNode;

    return n;
}

Tnode* createDerefNode(Tnode* idNode) {
    if(idNode->gsTableEntry->isPtr == 0) {
        printf("Error : not dereferencable\n");
        exit(1);
    }

    Tnode* n = createEmptyNode();
    n->tnodeType = NODE_DEREF;
    n->type = idNode->gsTableEntry->dataType;
    n->left = idNode;

    return n;
}


void inorder(Tnode* node) {
    if (node == NULL) return;

    inorder(node->left);

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
        default:               printf("UNKNOWN\n"); break;
    }

    inorder(node->right);
}
