#ifndef DISASM_HPP_INCLUDED
#define DISASM_HPP_INCLUDED

#include "CommonAssembler.hpp"
#include "Common/Common.hpp"
#include "Common/Char_buffers.hpp"
#include "Labels.hpp"

class Disassembler
{
    public:
        Disassembler(FILE* srcFile);
        ~Disassembler();
        void disassemble();
        void loadToFile(FILE* destFile);
        void dump();

    private:
        charBuf_t srcBuf_;
        charBuf_t resBuf_;
        AsmLabels labels_;
        bool isDestructed;

        void fillLabels();
};

#endif
