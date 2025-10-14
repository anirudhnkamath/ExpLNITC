#ifndef NODE_H
#define NODE_H

#include "../gsTable/gsTable.h"

extern int curFnType;

typedef struct Tnode {
    int tnodeType;
    int type;
    int val;
    char* strVal;
    char* varName;
    GsTableEntry* gsTableEntry;
    LsTableEntry* lsTableEntry;
    struct Tnode* argList;
    struct Tnode* left;
    struct Tnode* right;
} Tnode;

Tnode* createConnectorNode(Tnode* left, Tnode* right);

Tnode* createIntNode(int val);
Tnode* createStrLtrlNode(char* str);
Tnode* createIdNode(char varName[]);

Tnode* createArrIndexNode(Tnode* idNode, Tnode* exprNode);
Tnode* createArrIndex2DNode(Tnode* idNode, Tnode* expr1Node, Tnode* expr2Node);

Tnode* createReadNode(Tnode* idNode);
Tnode* createWriteNode(Tnode* exprNode);
Tnode* createAssignNode(Tnode* idNode, Tnode* exprNode);

Tnode* createIfElseNode(Tnode* condNode, Tnode* ifNode, Tnode* elseNode);
Tnode* createIfNode(Tnode* condNode, Tnode* stmtNode);

Tnode* createWhileNode(Tnode* condNode, Tnode* stmtNode);
Tnode* createDoWhileNode(Tnode* condNode, Tnode* stmtNode);
Tnode* createRepeatUntilNode(Tnode* condNode, Tnode* stmtNode);

Tnode* createBreakNode();
Tnode* createContinueNode();

Tnode* createArithOpNode(int tNodeType, Tnode* left, Tnode* right);
Tnode* createRelOpNode(int tNodeType, Tnode* left, Tnode* right);

Tnode* createAddrToNode(Tnode* idNode);
Tnode* createDerefNode(Tnode* idNode);

Tnode* addArgToArgList(Tnode* list, Tnode* expr);
Tnode* createFnCallNode(Tnode* idNode, Tnode* argListNode);
Tnode* createReturnNode(Tnode* exprNode);

void functionValidate(int retType, char* fnName, ParamListEntry* fnParams);
void validateIdForExpr(Tnode* idNode);

void setIdNodeType(Tnode* idNode);

void freeTree(Tnode* root);
void inorder(Tnode* root);

#endif
