#ifndef ASM_HPP_INCLUDED
#define ASM_HPP_INCLUDED

#include "CommonAssembler.hpp"
#include "Common/Common.hpp"
#include "Common/Char_buffers.hpp"
#include "Labels.hpp"

class Assembler
{
    public:
        Assembler(FILE* srcFile);
        ~Assembler();
        void translate();
        void loadToFile(FILE* destFile);
        void dump();

    private:
        charBuf_t srcBuf_;
        charBuf_t resBuf_;
        AsmLabels labels_;
        bool isDestructed;

        void deleteComments();
        void fillLabels();
        int getArgsFromBuf(AsmCmd cmd,
                            char* str,
                            int line,
                            double args[MAX_ARG_QT],
                            char addrModes[MAX_ARG_QT]);
};

#endif
