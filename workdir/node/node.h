#ifndef NODE_H
#define NODE_H

#include "../gsTable/gsTable.h"
#include "../typeTable/typeTable.h"

extern TypeTable* curFnType;

typedef struct Tnode {
    int tnodeType;
    TypeTable* type;
    int val;
    char* strVal;
    char* varName;
    GsTableEntry* gsTableEntry;
    LsTableEntry* lsTableEntry;
    struct Tnode* argList;
    struct Tnode* left;
    struct Tnode* right;
    struct Tnode* arrOffset;
    int isPtr;
} Tnode;

Tnode* createConnectorNode(Tnode* left, Tnode* right);

Tnode* createIntNode(int val);
Tnode* createStrLtrlNode(char* str);
Tnode* createIdNode(char varName[]);

Tnode* createReadNode(Tnode* idNode);
Tnode* createWriteNode(Tnode* exprNode);
Tnode* createAssignNode(Tnode* idNode, Tnode* exprNode);

Tnode* createIfElseNode(Tnode* condNode, Tnode* ifNode, Tnode* elseNode);
Tnode* createIfNode(Tnode* condNode, Tnode* stmtNode);
Tnode* createLoopNode(int tnodeType, Tnode* condNode, Tnode* stmtNode);
Tnode* createLoopJumpNode(int tnodeType);

Tnode* createArithOpNode(int tNodeType, Tnode* left, Tnode* right);
Tnode* createRelOpNode(int tNodeType, Tnode* left, Tnode* right);
Tnode* createLogOpNode(int tNodeType, Tnode* left, Tnode* right); 

Tnode* createAddrToNode(Tnode* idNode);
Tnode* createDerefNode(Tnode* idNode);

Tnode* createTupEntryNode(Tnode* tupNode, Tnode* fieldNode);

Tnode* addArgToArgList(Tnode* list, Tnode* expr);
Tnode* createFnCallNode(Tnode* idNode, Tnode* argListNode);
Tnode* createReturnNode(Tnode* exprNode);

void validateFunction(TypeTable* retType, Tnode* idNode, ParamListEntry* fnParams);
void validateProperId(Tnode* idNode);
void validateIdForExpr(Tnode* idNode);

void setIdNodeType(Tnode* idNode);

void freeTree(Tnode* root);
void inorder(Tnode* root);

Tnode* insertToArrDimn(Tnode* root, Tnode* node);
void validateArrOffset(Tnode* idNode, Tnode* indexExprNode);

#endif
