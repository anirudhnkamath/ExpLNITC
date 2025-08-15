#include "./registers.h"

void resetRegisters() {
    for(int i=0; i<NUM_REGISTERS; i++)
        registerFree[i] = 1;
}

int getFreeRegister() {
    for(int i=0; i<NUM_REGISTERS; i++) {
        if(registerFree[i] == 1) {
            registerFree[i] = 0;
            return i;
        }
    }

    return E_REGFULL;
}

int releaseRegister(int index) {
    if(index < 0 || index > NUM_REGISTERS)
        return E_OUTOFBOUNDS;

    registerFree[index] = 1;
}