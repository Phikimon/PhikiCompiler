#include "Processor.hpp"
#include "getopt.h"
#include <cassert>
#include <cstring>
#include <cstdio>

Processor::Processor(int argc, char* argv[]):
    stack_        (),
    registers_    (),
    progMem_      (),
    PC_           (0),
    opts_         (argc, argv),
    verbose_      (false)
{
    verbose_ = opts_.vflag;
    FILE* hexFile = NULL;
    FOPEN( hexFile = fopen(opts_.ivalue, "r") );
    streamToBuf(&progMem_, hexFile);
    fclose(hexFile);

    if (opts_.pflag)
        dump();
};

Processor::~Processor()
{
    bufdtor(&progMem_);
};

void Processor::dump()
{
    printf("\t Processor is %s[%p] \n", ok() ? "ok" : "NOT OK", this);
    printf("\t{\n");
    printf("\t programCounter = %zu\n", PC_);
    printf("\t registers:\n");
    for (unsigned i = 0; i < CPU_REGISTERS_QT; i++)
    {
        printf("\t   register[%-2u] = %lg\n", i, registers_.at(i));
    };
    bufOk(&progMem_, true);
    stack_.dump();
    printf("\n\t}\n");
}

int Processor::ok()
{
    return _bufOk(&progMem_, false);
};

void Processor::execute()
{
    verbose_ = opts_.vflag;
    PC_ = 1; //To skip CPUID put in the beginning of the binfile

    while (PC_ < progMem_.sizeFilled)
    {
        char* cmdPtr = progMem_.buf + PC_;

        char     argAddrTypes =         *(cmdPtr++);
        AsmCmd   cmd          = (AsmCmd)*(cmdPtr++);
        int      argQt        = getCmdArgQt(cmd);
        assert(argQt >= 0);
        double*  args = (double*)(cmdPtr);
        ArgAddrMode argTypes[MAX_ARG_QT] = {};

        for (int argNum = 0; argNum < argQt; argNum++)
        {
            argTypes[argNum] = (ArgAddrMode)((argAddrTypes >> (argNum * 2)) & 0b11);
        };

        DEBUGPRINTF("PC_ == %i;\n cmd = %s\n argQt = %d\n",
                     PC_ , getCmdName(cmd), argQt);

        switch (cmd)
        {
#define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE)                 \
            case CPU_##CMD_NAME:                                  \
            {                                                     \
                func_##CMD_NAME((unsigned)argQt, args, argTypes); \
            }; break;
#include "Processor/ProcessorCmdList.hpp"

#undef DEF_CMD

            case CPU_DEFAULT:
            default:
            {
                prln();
                printf("Processor error: Unknown command '%x'\n", (unsigned)cmd);
                myThrow(CPUERR_UNKCMD);
            }; break;
        };
        if (opts_.tflag)
        {
            scanf("%*c");
        }
    };
};
