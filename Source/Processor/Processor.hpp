#ifndef PROCESSOR_HPP_INCLUDED
#define PROCESSOR_HPP_INCLUDED

#include "Assembler/Labels.hpp"
#include "Assembler/Assembler.hpp"
#include "Stack.hpp"

enum { CPU_REGISTERS_QT = 256   };
enum { CPU_STACK_SIZE   = 64    };
enum { CPUID_VAL        = 0xFAD };

enum CPUERRORS_T
{
    CPUERR_EMPTY    =  0, //< No error occured
    CPUERR_UNKCMD   = -1, //< Unknown command in hex file
    CPUERR_UNKMATH  = -2, //< Unknown math operator
    CPU_TOOMANYVARS = -3, //< Not enough registers for all variables
};

struct processor
{
    stack_t* dataStack;
    stack_t* callStack;
    double* registers;
    
    charBuf_t* programMemory;   
    unsigned programCounter;
};

/**
 Returns assembler command that corresponds with the mathematical operation
 */
char* procMathCmd(unsigned mathOp); Govnocode//This funct has to be in asm.hpp

/**
 Constructor for Processor struct
 */
int procctor(processor* proc, FILE* programHex);

/**
 Destructor for Processor struct
 */
void procdtor(processor* proc);

/**
 Processor struct verificator
 */
int _procOk(processor* proc, bool verbose, const char procName[MAX_LINE_LEN]);
#define procOk(proc, verb) { assert(strlen(#proc) <= MAX_LINE_LEN); _procOk(proc, verb, #proc); }

/**
 Move processor's program counter
 */
void moveProgramCounter(processor* proc, unsigned argQt);

/**
 Perform jump by moving program counter to exact 'addr'
 */
void CPU_CMD_JUMP(processor* proc, unsigned addr);

/**
 Execute loaded is the progamMemory program
 */
void procExecute(processor* proc, bool verbose = false);

/**
 Unit tests for processor 
 */
void ut_proc(unsigned int mask, unsigned verbose);

#endif
