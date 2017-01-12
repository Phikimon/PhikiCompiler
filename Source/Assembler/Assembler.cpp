#include "CommonAssembler.hpp"
#include "Assembler.hpp"
#include <cstring>
#include <cassert>

Assembler::Assembler(FILE* srcFile):
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
    if (strDiv(&srcBuf_))
    {
        prln();
        printf("\nError: Empty program is in\n");
        myThrow(ASMERR_EMPPROG);
    };
}

Assembler::~Assembler()
{
    if (isDestructed) return;
    bufdtor(&srcBuf_);
    bufdtor(&resBuf_);
    isDestructed = true;
}

void Assembler::deleteComments()
{
    assert(srcBuf_.strQuant);
    assert(srcBuf_.strValid);
    assert(srcBuf_.strings);

    for (unsigned line = 0; line < srcBuf_.strQuant; line++)
    {
        myString* str = &srcBuf_.strings[line];
        char* semicolonPtr = strchr(str->str, ';');
        if (semicolonPtr)
        {
            memset(semicolonPtr, '\0',
                   str->str + str->len - semicolonPtr);
            str->len -= semicolonPtr - str->str;
        }
    }
}

void Assembler::fillLabels()
{
    assert(srcBuf_.strQuant);
    assert(srcBuf_.strValid);
    assert(srcBuf_.strings);
    int cmdWrittenQt = 0;

    for (unsigned line = 0; line < srcBuf_.strQuant; line++)
    {
        char word[MAX_LINE_LEN + 1] = "";
        int wordIndicator = -1;
        size_t wordlen = 0;

        assert(MAX_LINE_LEN == 256);
        sscanf(srcBuf_.strings[line].str, " %256[a-zA-Z0-9_:]%n",
               word, &wordIndicator);
        wordlen = strlen(word);

        if (wordIndicator == -1)
        {
            srcBuf_.strings[line].len = 0;
            continue;
        }

        //Process if label
        if (word[wordlen - 1] == ':')
        {
            DEBUGPRINTF("\nword = %s; labelNum_ == %d\n", word,
                           labels_.getNum());
            if (wordlen == 1) //< only ':'
            {
                prln();
                printf("\nNull length label in line %i\n", (int)line + 1);
                myThrow(ASMERR_NULLLENLBL);
            }
            if (wordlen >= MAX_LBL_LEN)
            {
                prln();
                printf("\nToo long label \"%s\" in line %i."
                       " Max label length is %i\n",
                       word, (int)line + 1, MAX_LBL_LEN - 1);
                myThrow(ASMERR_TOOLNGLBL);
            }
            if (labels_.name2Addr(word) != -1)
            {
                prln();
                printf("\nLabel with similar name \"%s\" is "
                        "already declared in line %i\n",
                        word, line + 1);
                myThrow(ASMERR_UNKCMD);
            }

            //Clear line
            memset(srcBuf_.strings[line].str, '\0',
                    srcBuf_.strings[line].len);
            srcBuf_.strings[line].len = 0;
            //Fill label
            word[wordlen - 1] = '\0'; //erase ':'
            labels_.appendLabel(word, cmdWrittenQt);
            continue;
        };

        cmdWrittenQt++;

    }
}

int Assembler::getArgsFromBuf(AsmCmd cmd,
                    char* str,
                    int line,
                    double args[MAX_ARG_QT],
                    char addrModes[MAX_ARG_QT])
{
    int toSkipSum = 0;
    DEBUGPRINTF("\ncmd = %d(%s)\n", cmd, getCmdName(cmd));
    for (size_t argn = 0; argn < getCmdArgQt(cmd); argn++)
    {
        int toSkip = -1;
        sscanf(str + toSkipSum, " %c%n", addrModes + argn, &toSkip);
        DEBUGPRINTF("addrMode[%d] = %c\n", argn, addrModes[argn]);
        toSkipSum += toSkip;

        if (isArgLabel(cmd, argn))
        {
            char labelName[MAX_LBL_LEN + 1] = {};
            assert(MAX_LBL_LEN == 64);
            sscanf(str + toSkipSum, "%64s %n", labelName, &toSkip);
            if (labelName[64] != '\0')
            {
                prln();
                printf("\nToo long argument in line %i\n", (int)line + 1);
                myThrow(ASMERR_TOOLNGLBL);
            }
            args[argn] = labels_.name2Addr(labelName);
            DEBUGPRINTF("label = args[%d] = %s, %lg\n", argn, labelName, args[argn]);

            if (D_EQ(args[argn], -1))
            {
                prln();
                printf("\nUnknown label in line %i\n", (int)line + 1);
                myThrow(ASMERR_UNDECLLBL);
            };
        } else
        {
            sscanf(str + toSkipSum, "%lg %n", &args[argn], &toSkip);
            DEBUGPRINTF("args[%d] = %lg\n", argn, args[argn]);
        }
        toSkipSum += toSkip;
        if (toSkip == -1)
        {
            prln();
            printf("\nToo few arguments in line %i\n", (int)line + 1);
            myThrow(ASMERR_TOOFEWARG);
        }
    }
    return toSkipSum;
}

void Assembler::translate()
{
    assert(srcBuf_.strQuant);
    assert(srcBuf_.strValid);
    //Fill 'labels' array
    deleteComments();
    fillLabels();
    //
    resBuf_.sizeFilled = 0;
    //Put cpu sinature in hex file
    putcBuf(&resBuf_, CPU_SIGNATURE);

    char* str = srcBuf_.strings[0].str;
    for (int line = 0; line < srcBuf_.strQuant; line++)
    {
        str = srcBuf_.strings[line].str;
        assert(str);
        assert(srcBuf_.strings);
        //Get cmd
        int toSkipSum = 0;
        AsmCmd cmd = getCmdFromBuf(str, &toSkipSum, line);
        if (cmd == CPU_DEFAULT) continue;

        double args      [MAX_ARG_QT] = {};
        char   addrModes [MAX_ARG_QT] = {};
        //Get args' addr modes and args' values
        toSkipSum += getArgsFromBuf(cmd,
                                    str + toSkipSum,
                                    line,
                                    args,
                                    addrModes);
        //Set arguments' addressing modes
        char cmdArgMode = 0;
        for (int argn = 0; argn < getCmdArgQt(cmd); argn++)
        {
            DEBUGPRINTF("getCmdArgQt = %d\n", getCmdArgQt(cmd));
            bool ok = checkAddrMode(cmd, argn, addrModes[argn]);
            ArgAddrMode mode = char2Mode(addrModes[argn]);
            cmdArgMode |= mode << (argn * 2);
        }
        putcBuf(&resBuf_, cmdArgMode);
        putcBuf(&resBuf_, cmd);
        for (int argn = 0; argn < getCmdArgQt(cmd); argn++)
        {
            putdBuf(&resBuf_, args[argn]);
        }
    }
    DEBUGPRINTF("sizeFilled = %d\n", resBuf_.sizeFilled);
}

void Assembler::dump()
{
    printf("\n  +----------------------------------+\n");
    printf(  "  | Phila's assembler '%d'        |\n", CPU_SIGNATURE);
    printf(  "  +----------------------------------+\n");
    printf("    {\n");
    labels_.dump();

    printf("\t\n Buffers' Dumps:\n\n");
    bufOk(&srcBuf_, true);
    bufOk(&resBuf_, true);
    printf("    }\n\n");
}

void Assembler::loadToFile(FILE* destFile)
{
    assert(destFile);
    bufToStream(&resBuf_, destFile);
}

#undef DEBUGPRINTF
