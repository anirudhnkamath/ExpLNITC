#include "node.h"
#include <stdlib.h>
#include <stdio.h>

Node* createLeafNode(char* strbuf) {
	Node* n = (Node*)malloc(sizeof(Node));

	n->op = 0;
	n->str = strbuf;
	n->left = n->right = NULL;
	return n;
}

Node* createInternalNode(char op, Node* left, Node* right) {
	Node* n = (Node*)malloc(sizeof(Node));

	n->op = op;
	n->str = NULL;
	n->left = left;
	n->right = right;
	return n;
}

void preorder(Node* node) {
	if(node == NULL)
		return;

	if(node->left == NULL && node->right == NULL)
		printf("%s ", node->str);
	else
		printf("%c ", node->op);

	preorder(node->left);
	preorder(node->right);
}
