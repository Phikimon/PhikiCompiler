#ifndef COMMON_ASM_HPP_INCLUDED
#define COMMON_ASM_HPP_INCLUDED

#include "Labels.hpp"
#include "Common/Common.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"

enum { MAX_CMD_LEN = 64 };
enum { MAX_ARG_QT      = 3 };

enum { CPU_SIGNATURE   = 0xFA };

enum ArgAddrMode
{
    ARG_NOARGS = 0,
    ARG_REG = 0b10,
    ARG_IMM = 0b01,
    ARG_ANY = 0b11,
    ARG_DEFAULT,
};

//Commands for myAsm
enum AsmCmd
{
    #define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE) \
    CPU_##CMD_NAME,

    #include "Processor/ProcessorCmdList.hpp"

    #undef DEF_CMD
    CPU_DEFAULT // my assembler command quantity
};

enum AsmErrors
{
    ASMERR_EMPTY = 0,      //< No error occured
    ASMERR_UNKCMD,         //< Error: Unknown assembler command
    ASMERR_TOOFEWARG,      //< Error: Too few arguments
    ASMERR_UNKARGTYPE,     //< Error: Unknown argument type
    ASMERR_TWOCMD,         //< Error: Two commands in one line or extra argument
    ASMERR_EMPPROG,        //< Error: Empty program is in
    ASMERR_WRGARGTYPE,     //< Error: Wrong argument type
    ASMERR_TOOLNGLBL,      //< Error: Too long label
    ASMERR_NULLLENLBL,     //< Error: Label with null length
    ASMERR_UNDECLLBL,      //< Error: Undeclared label
    ASMERR_WRSIGN,
    ASMERR_WRONGOPER,      //< Error: unknown math operator
                           //< is passed to asmMathExpr
};

/*
 * Returns if the argument has to be label
 */
bool isArgLabel(AsmCmd cmd, int argn);

/*
 * Returns if the command is JMP or CALL or one of CJMP-s
 */
bool isJmpCmd(AsmCmd cmd);

/*
 * Returns if the command is one of cond. JMP-s
 */
bool isCondJmpCmd(AsmCmd cmd);

/*
 * Returns if the command is one of cond. JMP-s
 */
ArgAddrMode char2Mode (char perstollar);

/*
 * Returns if the command is one of cond. JMP-s
 */
char mode2Char (ArgAddrMode mode);

/*
 * Reads and returns instruction read from the buf
 */
AsmCmd getCmdFromBuf(char* buf, int* toSkip, int line);

/*
 * Checks if charMode suits its' command
 */
int checkAddrMode(AsmCmd cmd, int argn, char charMode);

/*
 * Returns number of command, passed as a char*
 */
AsmCmd getCmdNum(const char cmdName[MAX_CMD_LEN]);

/*
 * Returns cmd name as a const char* by cmd num
 */
const char* getCmdName(AsmCmd cmd);

/*
 * Get argument addressing mode
 */
ArgAddrMode getCmdArgMode(AsmCmd cmd, int argn);

/*
 * Get command arguments' quantity
 */
int getCmdArgQt(AsmCmd cmd);

/**
 Returns assembler command that corresponds with the mathematical operation
 */
const char* asmMathInstr(LangMathOperators mathOp);

#endif
