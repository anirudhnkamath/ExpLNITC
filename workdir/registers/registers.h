#ifndef REGISTERS_H
#define REGISTERS_H

#include "../define/constants.h"

int registerFree[NUM_REGISTERS];

void resetRegisters();

int getFreeRegister();
int releaseRegister(reg_index_t index);

#endif