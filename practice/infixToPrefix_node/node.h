#ifndef NODE_H
#define NODE_H

struct Node {
	char* str;
	char op;
	struct Node* left;
	struct Node* right;
};

typedef struct Node Node;

Node* createLeafNode(char* strbuf);

Node* createInternalNode(char op, Node* left, Node* right);

void preorder(Node* node);	

#endif
