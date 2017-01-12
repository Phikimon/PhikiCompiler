#include "CommonAssembler.hpp"

#include <cstring>
#include <cstdio>
#include <cassert>

bool isArgLabel(AsmCmd cmd, int argn)
{
    return ( cmd >  CPU_JMP && cmd <= CPU_JLE    && argn == 2 ||
           ( cmd == CPU_JMP || cmd == CPU_CALL ) && argn == 0 );
}

bool isJmpCmd(AsmCmd cmd)
{
    return (cmd == CPU_CALL || ((cmd >= CPU_JMP) && (cmd <= CPU_JGE)));
}

bool isCondJmpCmd(AsmCmd cmd)
{
    return ((cmd > CPU_JMP) && (cmd <= CPU_JGE));
}

ArgAddrMode char2Mode (char perstollar)
{
    switch (perstollar)
    {
        case '%': return ARG_REG;
        case '$': return ARG_IMM;
        default : return ARG_DEFAULT;
    }
}

char mode2Char (ArgAddrMode mode)
{
    switch (mode)
    {
        case ARG_REG: return '%';
        case ARG_IMM: return '$';
        default : return '\0';
    }
}

AsmCmd getCmdFromBuf(char* buf, int* toSkip, int line)
{
    assert(buf);
    char   word[MAX_LINE_LEN] = "";
    size_t wordlen = 0;
           *toSkip = -1;

    assert(MAX_LINE_LEN == 256);
    sscanf(buf, " %255s%n", word, toSkip);

    //If line consists of nothing
    if (*toSkip == -1) return CPU_DEFAULT;

    AsmCmd cmd = getCmdNum(word);

    if (cmd == CPU_DEFAULT)
    {
        prln();
        printf("\nUnknown command \"%s\" in line %i\n", word, (int)line + 1);
        myThrow(ASMERR_UNKCMD);
    }

    return cmd;
}

int checkAddrMode(AsmCmd cmd, int argn, char charMode)
{
    ArgAddrMode stdMode = getCmdArgMode (cmd, argn);
    ArgAddrMode mode = char2Mode(charMode);
    bool ok = (mode == stdMode) || (stdMode == ARG_ANY);

    if (!ok)
    {
        prln();
        printf("\nError: Wrong argument type\n");
        myThrow(ASMERR_WRGARGTYPE);
    };

    return ok;
}

AsmCmd getCmdNum(const char cmdName[MAX_CMD_LEN])
{
    #define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE)      \
    if (strncmp(#CMD_NAME, cmdName, MAX_CMD_LEN - 1) == 0) \
        return CPU_##CMD_NAME;

    #include "Processor/ProcessorCmdList.hpp"

    #undef DEF_CMD
    return CPU_DEFAULT;
}

const char* getCmdName(AsmCmd cmd)
{
    #define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE) \
    if (cmd == CPU_##CMD_NAME)                        \
        return #CMD_NAME;

    #include "Processor/ProcessorCmdList.hpp"

    #undef DEF_CMD
    return NULL;
}

ArgAddrMode getCmdArgMode(AsmCmd cmd, int argn)
{
    if ((argn < 0) || (argn > MAX_ARG_QT))
        return ARG_DEFAULT;
#define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE) \
case CPU_##CMD_NAME: \
    return (ArgAddrMode)((ARGS_ADDR_MODE >> (2 * argn)) & (0b11));

    switch (cmd)
    {
    #include "Processor/ProcessorCmdList.hpp"
    default:
        return ARG_DEFAULT;
    }

    #undef DEF_CMD
}

int getCmdArgQt(AsmCmd cmd)
{
    #define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE) \
    case CPU_##CMD_NAME: return ARG_QT;

    switch (cmd)
    {
    #include "Processor/ProcessorCmdList.hpp"
    default:
        return -1;
    }

    #undef DEF_CMD
}

const char* asmMathInstr(LangMathOperators oper)
{
    switch (oper)
    {
        case LANGMATH_mult:             return "MUL";

        case LANGMATH_div:              return "DIV";

        case LANGMATH_plus:             return "ADD";

        case LANGMATH_minus:            return "SUB";

        case LANGMATH_not_equal:        return "LNE";

        case LANGMATH_equal:            return "LE";

        case LANGMATH_negate:           return "NEG";

        case LANGMATH_degree:           return "POW";

        case LANGMATH_greater:          return "LG";

        case LANGMATH_less:             return "LL";

        case LANGMATH_greater_or_equal: return "LGE";

        case LANGMATH_less_or_equal:    return "LLE";

        default: {
                    prln();
                    DEBUGPRINTF("Unknown math operator is passed to %s",
                                __func__);
                    myThrow(ASMERR_WRONGOPER);
                 }; break;
    }
}
