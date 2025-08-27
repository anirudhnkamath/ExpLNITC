#ifndef CONSTANTS_H
#define CONSTANTS_H

#define reg_index_t int

#define NUM_REGISTERS 20

#define STATIC_ALLOC_START 4096


enum nodeTypes {
    NODE_CONNECTOR = 1,
    NODE_WRITE,
    NODE_READ,
    NODE_ASSIGN,
    NODE_ADD,
    NODE_SUB,
    NODE_DIV,
    NODE_MULT,
    NODE_ID_INT,
    NODE_INT,
    NODE_EQ,
    NODE_NEQ,
    NODE_GTE,
    NODE_GT,
    NODE_LTE,
    NODE_LT,
    NODE_IF,
    NODE_IF_ELSE,
    NODE_WHILE
};

enum errorCodes {
    SUCCESS = 0,
    E_OUTOFBOUNDS = -1000,

    E_REGFULL,
    E_INVALIDNODE
};

#endif