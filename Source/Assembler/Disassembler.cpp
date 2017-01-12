#include "Disassembler.hpp"
#include <cassert>
#include <cstring>

#define DEBUGPRINTF(...) printf(__VA_ARGS__)
//#define DEBUGPRINTF(...)

Disassembler::Disassembler(FILE* srcFile):
    srcBuf_      ({}),
    resBuf_      ({}),
    labels_      (),
    isDestructed (false)
{
    assert(CPU_DEFAULT <= 255); //There is only one-byte instructions
    assert(srcFile);

    //Initialize source and res files' buffers
    streamToBuf(&srcBuf_, srcFile);
    bufctor(&resBuf_);
}

Disassembler::~Disassembler()
{
    if (isDestructed) return;
    bufdtor(&srcBuf_);
    bufdtor(&resBuf_);
    isDestructed = true;
}

void Disassembler::fillLabels()
{
    DEBUGPRINTF("\nI am %s!\n", __func__);

    if ((unsigned char)srcBuf_.buf[0] != CPU_SIGNATURE)
    {
        prln();
        printf("\nWrong CPU Signature '%x'", (unsigned char)srcBuf_.buf[0]);
        myThrow(ASMERR_WRSIGN);
    }

    AsmCmd cmd = CPU_DEFAULT; 
    // skip CPU signature
    //           V  
    for (int i = 1; i < srcBuf_.sizeFilled; 
             i += sizeof(double) * getCmdArgQt(cmd))
    {
        DEBUGPRINTF("\ncounter = %d, char = 0x%x, l = %d\n", i, srcBuf_.buf[i], __LINE__);
        unsigned char addrByte = srcBuf_.buf[i++];
        DEBUGPRINTF("\ncounter = %d, char = 0x%x, l = %d\n", i, srcBuf_.buf[i], __LINE__);
                   cmd = (AsmCmd)srcBuf_.buf[i++];
        DEBUGPRINTF("\ncounter = %d, char = 0x%x, l = %d\n", i, srcBuf_.buf[i], __LINE__);

        DEBUGPRINTF("\ncmd = %x(%s)\n"
                     "addrByte = %x\n"
                     "argQt = %d\n", (unsigned char)cmd,
                                        getCmdName(cmd),
                                        (unsigned char)addrByte,
                                        getCmdArgQt(cmd));
        //Recognize the command
        if ((cmd < 0) || (cmd >= CPU_DEFAULT))
        {
            prln();
            printf("\nUnknown command '%x' in character %i", cmd, i);
            myThrow(ASMERR_UNKCMD);
        };

        if (isJmpCmd(cmd))
        {
            int labelArgNum = isCondJmpCmd(cmd);
            if (isCondJmpCmd(cmd))
            {
                labelArgNum = 2; // Conditional jump cmds' has label as a third arg
            } else
            {
                labelArgNum = 0; // But JMP and CALL has label as first arg
            }
            double* labelPtr = (double*)(srcBuf_.buf + i) + labelArgNum;

            //Append label
            char tempString[MAX_LBL_LEN] = {};
            assert(strlen(labels_.stdLabel()) < MAX_LBL_LEN);
            sprintf(tempString, "%s%i", labels_.stdLabel(), labels_.getNum() + 1);
            labels_.appendLabel(tempString, (int)*labelPtr); 
        }
    };
    DEBUGPRINTF("\nBye from %s!\n", __func__);
};

void Disassembler::disassemble()
{
    DEBUGPRINTF("\nI am %s!\n", __func__);
    int cmdWrittenQt = 0;
    fillLabels();

    //         skip CPU signature
    //                 V
    for (int counter = 1; counter < srcBuf_.sizeFilled;)
    {
#define putcB(charac) putcBuf(&resBuf_, charac)
        //Put label
        for (int i = 0; i < labels_.getNum(); i++)
            if (labels_.num2Addr(i) == cmdWrittenQt)
            {
                strcatBuf(&resBuf_, labels_.num2Name(i));
                putcB(':');
                putcB('\n');
            }
        //Put cmd
        unsigned char addrByte =        srcBuf_.buf[counter++];
            DEBUGPRINTF("\ncounter = %d, l = %d\n", counter, __LINE__);
        AsmCmd cmd = (AsmCmd) srcBuf_.buf[counter++];
            DEBUGPRINTF("\ncounter = %d, l = %d\n", counter, __LINE__);
        DEBUGPRINTF("\ncmd = %x(%s)\n"
                     "addrByte = %x\n", (unsigned char)cmd,
                                        getCmdName(cmd),
                                        (unsigned char)addrByte);

        if ((cmd < 0) || (cmd >= CPU_DEFAULT))
        {
            prln();
            printf("\nUnknown command '%x' in character %i", cmd,
                    srcBuf_.buf[counter]);
            myThrow(ASMERR_UNKCMD);
        };

        cmdWrittenQt++;
        strcatBuf(&resBuf_, getCmdName(cmd));
        putcB(' ');
        //Put arguments
        for (int argn = 0; argn < getCmdArgQt(cmd); argn++)
        {
            //Put argument addr mode
            ArgAddrMode addrMode = (ArgAddrMode)((addrByte >> (argn * 2)) & 0b11);
            unsigned char addrChar = mode2Char(addrMode);
            checkAddrMode(cmd, argn, addrChar);
            putcB(addrChar);
            //Put argument
            double arg = *(double*)(srcBuf_.buf + counter);
            if (isArgLabel(cmd, argn))
            {
                prdel();
                prdel();
                printf("\n!!!%d!!!\n", labels_.getNum());
                dump();
                int labelNum = -1;
                for (int i = 0; i < labels_.getNum(); i++)
                    if (labels_.num2Addr(i) == (int)arg)
                    {
                        labelNum = i;
                        break;
                    }
                if (labelNum == -1)
                {
                    prln();
                    printf("\nUnknown label with addr %i in cmd '%s',"
                           " argn =  %i\n", (int)arg, getCmdName(cmd), argn);
                    myThrow(ASMERR_UNDECLLBL);
                }
                strcatBuf(&resBuf_, labels_.num2Name(labelNum)); 
            } else
            {
                //Convert argument value to string
                char argst[MAX_LINE_LEN] = "";
                assert(MAX_LINE_LEN == 256);
                sprintf(argst, "%lg", arg);
                strcatBuf(&resBuf_, argst, strlen(argst) );
            }
            putcB(' ');
            counter += sizeof(double);
            DEBUGPRINTF("\ncounter = %d, l = %d\n", counter, __LINE__);
        }
        putcB('\n' );
#undef putcB
    };
    DEBUGPRINTF("\nBye from %s!\n", __func__);
}

void Disassembler::dump()
{
    bool ok = _bufOk(&srcBuf_) && _bufOk(&resBuf_);
    printf("\n  +----------------------------------%s+\n", (ok) ? "----" : "");
    printf(  "  | Phila's disassembler '%d' %s |\n", CPU_SIGNATURE, 
                !ok ? "is okay" : "is NOT okay");
    printf(  "  +----------------------------------%s+\n", (ok) ? "----" : "");
    printf("    {\n");
    labels_.dump();

    printf("\t\n Buffers' Dumps:\n\n");
    bufOk(&srcBuf_, true);
    bufOk(&resBuf_, true);
    printf("    }\n\n");
}

void Disassembler::loadToFile(FILE* destFile) 
{
    assert(destFile);
    bufToStream(&resBuf_, destFile);
}

#undef DEBUGPRINTF
