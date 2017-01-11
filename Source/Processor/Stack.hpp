#ifndef STACK_HPP_INCLUDED
#define STACK_HPP_INCLUDED

#include "Common/Common.hpp"

#define INIT_STACK_SIZE 32

enum STACKERRORS_T
{
    STACKERR_EMPTY = 0,         //< No error occured
    STACKERR_NULL_SIZE = -1,    //< Stack size can't be equal to zero
    STACKERR_NOPOP = -2,        //< Nothing to pop
};

typedef double stackData_t;

struct stack_t
{
    unsigned checkSum;
    unsigned leftCanary;

    int      count;
    int      size;
    unsigned seed[2];
    stackData_t* data;
    
    unsigned rightCanary;
};

/**
 Constructor for 'stack' structure
 */
int stackctor(stack_t* stack, int Size);

/**
 Destructor for 'stack' structure
 */
int stackdtor(stack_t* stack);

/**
 Calculates the checksum
 */
unsigned _checkSum(stack_t* stack, bool verbose = false, const char stackName[MAX_LINE_LEN] = "name");

/**
 Realloc memory for stack data
 */
int reallocStack(stack_t* stack);

/**
 Stack verificator and dump
 */
bool _stackOk(stack_t* stack, bool verbose = false, const char stackName[MAX_LINE_LEN] = "name");
#define stackOk(stk, verb) { assert(strlen(#stk) <= MAX_LINE_LEN); _stackOk(stk, verb, #stk); }

/**
 Push value in stack
 */
int stackPush(stack_t* stack, stackData_t value, bool verbose = false);

/**
 Pop value from stack
 */
stackData_t stackPop(stack_t* stack, bool verbose = false);

/**
 Unit tests for stack
 */
void ut_stack(bool verbose = false);

#endif
