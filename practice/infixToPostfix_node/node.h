#ifndef NODE_H
#define NODE_H

struct Node {
	int val;
	char op;
	struct Node* left;
	struct Node* right;
};

typedef struct Node Node;

Node* makeLeafNode(int n);

Node* makeOperatorNode(char op, Node* l, Node* r);

int evaluateTree(Node* t);

#endif
