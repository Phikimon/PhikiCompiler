#include "Processor.hpp"
#include <cmath>
#include <cassert>
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define VAL(argNum) ( (argTypes[argNum] == ARG_REG) ?           \
                       registers_.at((unsigned)args[argNum]) :  \
                       args[argNum] )

#define CPUBUGPRINT(...)     \
{                            \
    if (verbose_)            \
        printf(__VA_ARGS__); \
}                            \

#define DEBUG_PRINT_VAL(argNum, string_reg, string_imm)               \
if (argTypes[argNum] == ARG_REG)                                      \
{                                                                     \
    CPUBUGPRINT(string_reg, (unsigned)args[argNum],                   \
                            registers_.at((unsigned)args[argNum]));   \
} else                                                                \
{                                                                     \
    CPUBUGPRINT(string_imm, args[argNum]);                            \
};

#define PRINT_VAL(argNum, string_reg, string_imm)                \
if (argTypes[argNum] == ARG_REG)                                 \
{                                                                \
    printf(string_reg, (unsigned)args[argNum],                   \
                       registers_.at((unsigned)args[argNum]));   \
} else                                                           \
{                                                                \
    printf(string_imm, args[argNum]);                            \
};

//--------------------------------------BASIC-------------------------------------------
void Processor::func_NOP(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nNOP\n");
    subfunc_MOVEPC((unsigned)argQt);
};

void Processor::func_CPUID(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nCPUID: $0x%x\n", (unsigned)CPU_SIGNATURE);
    subfunc_MOVEPC((unsigned)argQt);
};


//MERGE
void Processor::func_INR(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    registers_.at((unsigned)args[0]) = args[1];
    CPUBUGPRINT("\nINR:   reg[%%%u] = $%lg\n", (unsigned)args[0],
                                                         args[1]);
    subfunc_MOVEPC((unsigned)argQt);
};

void Processor::func_MOV(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    registers_.at((unsigned)args[0]) = registers_.at((unsigned)args[1]);
    CPUBUGPRINT("\nMOV:   %%%u($%lg) -> %%%u\n", (unsigned)args[1],
                                                 registers_.at((unsigned)args[1]),
                                                 (unsigned)args[0]);
    subfunc_MOVEPC((unsigned)argQt);
};
//MERGE END
//--------------------------------------BASIC-END---------------------------------------



//--------------------------------------CALL-RET----------------------------------------
void Processor::func_CALL(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nCALL %x\n", (unsigned)args[0]);
    stack_.push((double)PC_);
    subfunc_JUMP((unsigned)args[0]);
    CPUBUGPRINT("\nPC_ = %d\n", (unsigned)PC_);
};

void Processor::func_RET(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nRET\n");
    PC_ = (size_t)stack_.top();
    stack_.pop();
    subfunc_MOVEPC((unsigned)getCmdArgQt(CPU_CALL));
};
//--------------------------------------CALL-RET END------------------------------------


//--------------------------------------I/O---------------------------------------------
void Processor::func_IN(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nIN:    reg[%%%u] = $", (unsigned)args[0]);
    scanf("%lg", &registers_.at((unsigned)args[0]));
    subfunc_MOVEPC((unsigned)argQt);
};

void Processor::func_OUT(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    DEBUG_PRINT_VAL(0, "\nOUT:   %%%u($%lg)\n", "\nOUT:   $%lg\n");
    if (!verbose_) printf("\n%lg\n", VAL(0));
    subfunc_MOVEPC((unsigned)argQt);
};

//--------------------------------------I/O-END-----------------------------------------
//--------------------------------------ARITHM-INSTRUCTIONS-----------------------------
#define ARITHM(name, operation, sign)                          \
void Processor::func_##name(size_t argQt,                      \
                            const double* args,                \
                            const ArgAddrMode* argTypes)       \
{                                                              \
    CPUBUGPRINT("\n" #name ":   %%%u <-", (unsigned)args[0]);  \
    DEBUG_PRINT_VAL(1, " ( %%%u($%lg) "#sign, " $%lg "#sign);  \
    DEBUG_PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");            \
    double a = VAL(1);                                         \
    double b = VAL(2);                                         \
    registers_.at((unsigned)args[0]) = operation;              \
    CPUBUGPRINT("($%lg)\n", registers_.at((unsigned)args[0])); \
    subfunc_MOVEPC((unsigned)argQt);                           \
};

ARITHM(ADD,  a + b, +)
ARITHM(SUB,  a - b, -)
ARITHM(MUL,  a * b, *)
ARITHM(DIV,  a / b, /)

ARITHM(POW,  pow(a, b), ^)

ARITHM(LG,   a >  b,      > )
ARITHM(LL,   a <  b,      < )
ARITHM(LGE,  a >= b,      >=)
ARITHM(LLE,  a <= b,      <=)
ARITHM(LE,    D_EQ(a, b), ==)
ARITHM(LNE,  !D_EQ(a, b), !=)

ARITHM(LAND, (double)((size_t)a & (size_t)b), &)
ARITHM(LOR,  (double)((size_t)a | (size_t)b), |)

#undef ARITHM
//--------------------------------------------------------------------------------------
#define BINARITHM(name, operation, sign)                       \
void Processor::func_##name##BIN(size_t argQt,                 \
                            const double* args,                \
                            const ArgAddrMode* argTypes)       \
{                                                              \
    CPUBUGPRINT("\n" #name "BIN:   %%%u <-", (unsigned)args[0]);  \
    DEBUG_PRINT_VAL(0, " ( %%%u($%lg) "#sign, " $%lg "#sign);  \
    DEBUG_PRINT_VAL(1, " %%%u($%lg) )", " $%lg )");            \
    double a = VAL(0);                                         \
    double b = VAL(1);                                         \
    registers_.at((unsigned)args[0]) = operation;              \
    CPUBUGPRINT("($%lg)\n", registers_.at((unsigned)args[0])); \
    subfunc_MOVEPC((unsigned)argQt);                           \
};

BINARITHM(ADD,  a + b, +)
BINARITHM(SUB,  a - b, -)
BINARITHM(MUL,  a * b, *)
BINARITHM(DIV,  a / b, /)

BINARITHM(POW,  pow(a, b), ^)

BINARITHM(LG,   a >  b,      > )
BINARITHM(LL,   a <  b,      < )
BINARITHM(LGE,  a >= b,      >=)
BINARITHM(LLE,  a <= b,      <=)
BINARITHM(LE,    D_EQ(a, b), ==)
BINARITHM(LNE,  !D_EQ(a, b), !=)

BINARITHM(LAND, (double)((size_t)a & (size_t)b), &)
BINARITHM(LOR,  (double)((size_t)a | (size_t)b), |)

#undef BINARITHM
//--------------------------------------ARITHM-INSTRUCTIONS-END-------------------------
void Processor::func_POP(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    registers_.at((unsigned)args[0]) = stack_.top();
    stack_.pop();
    CPUBUGPRINT("\nPOP:   ($%lg) -> %%%u\n", registers_.at((unsigned)args[0]),
                                             (unsigned)args[0]);
    subfunc_MOVEPC((unsigned)argQt);
};

void Processor::func_PUSH(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    stack_.push(VAL(0));
    DEBUG_PRINT_VAL(0, "\nPUSH:  stack <- %%%u($%lg)\n",
                       "\nPUSH:  stack <- $%lg\n");
    subfunc_MOVEPC((unsigned)argQt);
};

void Processor::func_MOVST(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    registers_.at((unsigned)args[0]) = stack_.getRandAccess(args[1]);
    CPUBUGPRINT("\nMOVST: ($%lg) -> %%%u\n", registers_.at((unsigned)args[0]),
                                             (unsigned)args[0]);
    subfunc_MOVEPC((unsigned)argQt);
};
//--------------------------------STACK-ARITHM-INSTRUCTIONS-----------------------------
#define STACKARITHM(name, operation, sign)                                       \
void Processor::func_##name(size_t argQt,                                        \
                            const double* args,                                  \
                            const ArgAddrMode* argTypes)                         \
{                                                                                \
    double b = stack_.top();                                                     \
    stack_.pop();                                                                \
    double a = stack_.top();                                                     \
    stack_.pop();                                                                \
    double res = operation;                                                      \
    stack_.push(res);                                                            \
    CPUBUGPRINT("\n" #name ":   $%lg <- $%lg " #sign " $%lg ", res, a, b);       \
    subfunc_MOVEPC((unsigned)argQt);                                             \
};

STACKARITHM ( MULST,  a * b,      *  )
STACKARITHM ( DIVST,  a / b,      /  )
STACKARITHM ( ADDST,  a + b,      +  )
STACKARITHM ( SUBST,  a - b,      -  )
STACKARITHM ( POWST,   pow(a, b), ^  )

STACKARITHM ( LGST,   a >  b,     >  )
STACKARITHM ( LGEST,  a >= b,     >= )
STACKARITHM ( LLST,   a <  b,     <  )
STACKARITHM ( LLEST,  a <= b,     <= )
STACKARITHM ( LNEST, !D_EQ(a, b), != )
STACKARITHM ( LEST,   D_EQ(a, b), == )

#undef FUNCST
//--------------------------------STACK-ARITHM-INSTRUCTIONS-END-------------------------
void Processor::subfunc_MOVEPC(unsigned argQt)
{
    //Skips addr type byte, cmd itself and argQt arguments
    PC_ += 2 * sizeof(char) + sizeof(double) * argQt;
};

void Processor::subfunc_JUMP(unsigned newPC)
{
    //  To skip CPUID
    //    V
    PC_ = 1;
    for (size_t i = 0; i < newPC; i++)
    {
        AsmCmd cmd = (AsmCmd)(progMem_.buf[PC_ + 1]);

        int argQt  = getCmdArgQt(cmd);
        assert(getCmdArgQt(cmd) >= 0);

        subfunc_MOVEPC((unsigned)argQt);
    };
};

void Processor::func_JMP(size_t argQt, const double* args, const ArgAddrMode* argTypes)
{
    CPUBUGPRINT("\nJUMP\n");
    subfunc_JUMP((unsigned)args[0]);
};
//---------------------------CONDITIONAL-JUMPS-INSTRUCTIONS-----------------------------
#define CJUMP(name, condition, sign)                                             \
void Processor::func_##name(size_t argQt,                                        \
                            const double* args,                                  \
                            const ArgAddrMode* argTypes)                         \
{                                                                                \
    double a = VAL(0);                                                           \
    double b = VAL(1);                                                           \
    bool cond = condition;                                                       \
    DEBUG_PRINT_VAL(0, "\n"#name":    ( %%%u($%lg) "#sign,                       \
                       "\n"#name":    ( $%lg "#sign      );                      \
                                                                                 \
    DEBUG_PRINT_VAL(1, " %%%u($%lg) ) == ",                                      \
                       " $%lg ) == ");                                           \
                                                                                 \
    CPUBUGPRINT((cond) ? "true => goto 'label'\n" :                              \
                         "false => programCounter++\n");                         \
    if (cond)                                                                    \
    {                                                                            \
        subfunc_JUMP((unsigned)args[2]);                                         \
    }                                                                            \
    else                                                                         \
    {                                                                            \
        subfunc_MOVEPC((unsigned)argQt);                                         \
    }                                                                            \
};

CJUMP(JE,   D_EQ(a, b), ==)
CJUMP(JNE, !D_EQ(a, b), !=)
CJUMP(JL,   a <  b    , < )
CJUMP(JLE,  a <= b    , <=)
CJUMP(JG,   a >  b    , > )
CJUMP(JGE,  a >= b    , >=)

#undef CJUMP
//---------------------------CONDITIONAL-JUMPS-INSTRUCTIONS-END-------------------------

#undef VAL
#undef CPUBUGPRINT
#undef DEBUG_PRINT_VAL
#undef PRINT_VAL
