#ifndef REGISTERS_H
#define REGISTERS_H

#include "../define/constants.h"

extern int registerFree[NUM_REGISTERS];

void resetRegisters();

int getFreeRegister();
int releaseRegister(int index);

#endif