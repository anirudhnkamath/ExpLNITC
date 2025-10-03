#ifndef NODE_H
#define NODE_H

#include "../gsTable/gsTable.h"

typedef struct Tnode {
    int tnodeType;
    int type;
    int val;
    char* strVal;
    char* varName;
    GsTableEntry* gsTableEntry;
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

void inorder(Tnode* root);

#endif
