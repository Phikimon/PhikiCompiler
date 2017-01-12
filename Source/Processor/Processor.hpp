#ifndef PROCESSOR_HPP_INCLUDED
#define PROCESSOR_HPP_INCLUDED

#include "Assembler/CommonAssembler.hpp"
#include "Common/Char_buffers.hpp"
#include "ProcessorOptions.hpp"
#include "Stack.hpp"
#include <array>

enum { CPU_REGISTERS_QT = 22 };
enum { CPU_STACK_SIZE   = 64 };

enum CPUERRORS_T
{
    CPUERR_OK = 0,   //< No error occured
    CPUERR_UNKCMD,   //< Unknown command in hex file
};

class Processor
{
    public:
        Processor(int argc, char* argv[]);
        ~Processor();
        int  ok();
        void dump();
        void execute();
    private:
        CPUStack stack_;
        std::array < double, CPU_REGISTERS_QT > registers_;
        charBuf_t                               progMem_;
        size_t                                  PC_;
        ProcessorOptions                        opts_;
        bool                                    verbose_;


#define DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE)          \
        void func_##CMD_NAME(size_t argQt,                 \
                             const double* args,           \
                             const ArgAddrMode* argTypes);
#include "Processor/ProcessorCmdList.hpp"
#undef DEF_CMD
        void subfunc_MOVEPC(unsigned argQt);
        void subfunc_JUMP  (unsigned newPC);
};

#endif
