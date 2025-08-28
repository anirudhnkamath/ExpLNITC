#ifndef LABEL_H
#define LABEL_H

struct Label {
    char labelName[10];
    int addr;
    struct Label* next;
};
typedef struct Label Label;

extern Label* labelHead;

Label* createLabel(char labelName[], int addr);
void insertNewLabel(char labelName[], int addr);
int findLabelAddr(char labelName[]);

#endif