#ifndef CONSTANTS_H
#define CONSTANTS_H

#define reg_index_t int

#define NUM_REGISTERS 20

#define OPERATOR_NODE 1
#define INTEGER_NODE 2

enum errorCodes {
    SUCCESS = 0,
    E_OUTOFBOUNDS = -1000,

    E_REGFULL,
    E_INVALIDNODE
};

#endif