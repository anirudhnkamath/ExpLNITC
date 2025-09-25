#ifndef LABEL_H
#define LABEL_H

struct Label {
    char labelName[10];
    int addr;
    struct Label* next;
};
typedef struct Label Label;

extern Label* labelHead;
extern int newLabel;

Label* createLabel(char labelName[], int addr);
void insertNewLabel(char labelName[], int addr);
int findLabelAddr(char labelName[]);
int getNewLabel();



// runtime stack for break and continue

typedef struct LabelStackNode {
    int startLabel;
    int endLabel;
    struct LabelStackNode* next;
} LabelStackNode;

extern LabelStackNode* labelStackHead;

void addToLabelStack(int startLabel, int endLabel);
void popLabelStack();
LabelStackNode* peekLabelStack();

#endif