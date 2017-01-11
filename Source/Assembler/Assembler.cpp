#include "Assembler.hpp"

Govnocode //Rewrite completely

const char* utResFileName = "ut_temp_res_file.o";
const char* utDisFileName = "ut_temp_dis_file.o";
const char *asmVersion = "v1.0";  //current assembler version

const char* myAsmCmdArg[] =
{
    #define DEF_CMD(CMD_NAME, CMD_ARG_STRING, CMD_CODE) \
    #CMD_ARG_STRING ,
    
    #include "Lists/Processor_cmd_list.hpp"
    
    #undef DEF_CMD
};

const char* myAsmCmd[] =
{
    #define DEF_CMD(CMD_NAME, CMD_ARG_STRING, CMD_CODE) \
    #CMD_NAME , 
    
    #include "Lists/Processor_cmd_list.hpp"
    
    #undef DEF_CMD
};

unsigned _checkSum(asm_t* assm, bool verbose, const char asmName[MAX_LINE_LEN]) 
{
    assert(assm);
    
    unsigned checkSum = 0;
    if (verbose) prdel();
   /* 
    myLabel* labels[MAX_LBL_QT];
    */
    
                                   if (verbose) printf("\n\tCalculating CheckSum\n");
    checkSum ^= assm->leftCanary;  if (verbose) printf("\t stack->leftCanary = %x\n", assm->leftCanary);
    checkSum ^= assm->rightCanary; if (verbose) printf("\t stack->rightCanary = %x\n", assm->rightCanary);
    
    checkSum ^= assm->cmdWrittenQt;
    checkSum ^= assm->labelPtr;
    
    checkSum ^= (long unsigned)assm->srcBuf;
  //checkSum ^= checkSum(srcBuf);
  
    checkSum ^= (long unsigned)assm->resBuf;
  //checkSum ^= checkSum(resBuf);
  
    for (int i = 0; i < MAX_LBL_QT; i++)
    {
        checkSum ^= (long unsigned)assm->labels[i];
    }
    

    for (int i = 0; i < 2; i++)
    {
        checkSum ^= assm->seed[i]; if (verbose) printf("\t stack->seed[%i] = %x\n", i, assm->seed[i]);
    }

    if (verbose) printf("\n\t CheckSum = %u\n     CheckSumPrev = %u\n", checkSum, assm->checkSum);
    if (verbose) printf("}\n");
    if (verbose) prdel();
    
    return checkSum;
}

/**
 constructor of 'assembler' structure
 */
int asmctor(asm_t *assm, FILE* srcFile, asmType T, int verbose)
{
    assert(ASMCMDQT <= 32);
    //Check input params
    assert(assm);
    assert(srcFile);

    //Generate random seeds
    for (int i = 0; i < 2; i++)
    {
        assm->seed[i] = genSeed();
    };
    //Set canaries
    assm->leftCanary = myRandom(assm->seed[0]);
    assm->rightCanary= myRandom(assm->seed[1]);
    //Initialize all variables
    assm->cmdWrittenQt = 0;
    assm->srcBuf = (charBuf_t*)calloc(1, sizeof(*assm->srcBuf));
    assm->resBuf = (charBuf_t*)calloc(1, sizeof(*assm->resBuf));
    //assm->labels stays uninitialized, 'cause it has to be initialized when it is first used
    assm->labelPtr = 0;
    labelsctor(assm->labels);

    //Initialize source and res files buffers
    streamToBuf(assm->srcBuf, srcFile);
    if (T == asm_Translate)
    {
        if (strDiv(assm->srcBuf))
        {
            prln();
            printf("\nError: Empty program is in\n");
            throw ASMERR_EMPPROG;
        };
    }
    //Initialize empty string of length START_FILE_SIZE
    bufctor(assm->resBuf);

    //Invitation
    asmOk(assm, verbose);

    assm->checkSum = _checkSum(assm, verbose, "assm");

    return 0;
}

/**
 destructor for 'assembler' structure
 */
void asmdtor(asm_t *assm)
{
    assert(assm);
    assert(assm->srcBuf);
    assert(assm->resBuf);

    bufdtor(assm->srcBuf);
    bufdtor(assm->resBuf);
    
    labelsdtor(assm->labels);
    for (int i = 0; i < MAX_LBL_QT; i++)
    {
         assm->labels[i] = NULL;
    };
    
    free(assm);
}

/**
  Look for labels in the source file. Fill the 'labels' array.
 */
void fillLabelsTranslate(asm_t *assm)
{
    assert(assm);
    assert(assm->srcBuf->strQuant);
    assert(assm->srcBuf->strValid);
    
    for (unsigned line = 0; line < assm->srcBuf->strQuant; line++)
    {
        assert(assm->srcBuf->strings);
        unsigned char c = '\0';
        unsigned int k = 0, i = 0;
        
        #define skipdelims() while ( isspace(c = assm->srcBuf->strings[line].str[i]) ) {i++;}
        #define skipspaces() while ( ( (c = assm->srcBuf->strings[line].str[i]) == ' ') || (c == '\t') ) {i++;}
        #define skipnotspaces() while ( !isspace(c = assm->srcBuf->strings[line].str[i]) && (c)) {i++;}

        //Skip delimers
        skipdelims();
        if ((c == ';') || (c == '\0'))
        {
            continue;
        };
        
        char* labelName = (char*)calloc( MAX_CMD_LBL_LEN, sizeof(char));  //assembler command string
        
        //Read command
        if (isalnum(c = assm->srcBuf->strings[line].str[i]))
        {
            while ( isalnum(c = assm->srcBuf->strings[line].str[i++]) || (c == '_') )
            {
                labelName[k++] = toupper(c);
                if (k >= MAX_CMD_LBL_LEN)
                {
                    prln();
                    printf("\nToo long label \"%s\" in line %i. Max label length is %i\n", labelName, (int)line + 1, MAX_CMD_LBL_LEN);
                    throw ASMERR_TOOLNGLBL;
                }
            }
        }
        
        //If this is a label, put it to array and skip this line
        if (c == ':')
        {
            if (strlen(labelName) == 0)
            {
                prln();
                printf("\nNull length label in line %i\n", (int)line + 1);
                throw ASMERR_NULLLENLBL;
            }
            for (int p = 0; p < assm->labelPtr; p++)
            {
                //If there is already declared label with the similar name, then throw
                if (strcmp(assm->labels[p]->name, labelName) == 0)
                {
                    prln();
                    printf("\nLabel with similar name \"%s\" is already declared in line %i\n", labelName, assm->labels[p]->ptr + 1);
                    throw ASMERR_UNKCMD;
                }
            };
            strcpy(assm->labels[assm->labelPtr]->name, labelName);

            assm->labels[assm->labelPtr]->ptr = assm->cmdWrittenQt;

            assm->labelPtr++;
            
            free(labelName);
            continue;
        } else
        {
        for (myAsmCmdNum i = (myAsmCmdNum)0; i < ASMCMDQT; i = (myAsmCmdNum)(i+1)) //For each known assembler command
            {
                if (!strcmp(labelName, myAsmCmd[i]))       //Compare it with current command. If they are similar, go on
                {
                    assm->cmdWrittenQt++;
                    break;
                }
            }
        }
        
        free(labelName);
    }
}

/**
get command and translate it into
*/
int translate(asm_t *assm)
{
    assert(assm);
    assert(assm->srcBuf->strQuant);
    assert(assm->srcBuf->strValid);
    
    fillLabelsTranslate(assm);
    
    char cpuSign = cpuSignature;
    char* cpuSignatureString = &cpuSign;
    strcatBuf(assm->resBuf, cpuSignatureString, 1);
    
    //Complete assembling
    for (unsigned line = 0; line < assm->srcBuf->strQuant; line++)
    {
        char* asmCmdHex = (char*)calloc( 1  +   sizeof(double)*MAX_ARG_QT                       +   1, sizeof(char)); //Assembler command line in hex
        /*                               ^                        ^                                 ^
                                         |                        |                                 |
                                1 byte for cmd opcode |sizeof(double) bytes for each argument|1 byte for '\0'
        1(%) - register; 0($) - immediate.

        asmCmdHex:
            0 byte                        1 byte            2 byte
        +------------------------------+------------------+------------------+------------------+------------------+
        | 1 | 0 | somecommand(6 bits)  | 0b0..00101       |   0b0..00100     |   3rd arg        |    4th arg       |
        +------------------------------+------------------+------------------+------------------+------------------+
          ^   ^
          |   |
          %5, $4
        */
        char* asmCmd = (char*)calloc( MAX_CMD_LBL_LEN, sizeof(char));  //assembler command string
        assert(assm->srcBuf->strings);
        unsigned char c = '\0';
        unsigned int k = 0, i = 0;
        //Skip delimers
        skipdelims();
        if ((c == ';') || (c == '\0'))
        {
            free(asmCmd);
            free(asmCmdHex);
            continue;
        };
        //Read command
        if (isalnum(c = assm->srcBuf->strings[line].str[i]))
        {
            while ( isalnum(c = assm->srcBuf->strings[line].str[i++]) || (c == '_') )
            {
                asmCmd[k++] = toupper(c);
                //if command is too long it is treated as a label
                if (k >= MAX_CMD_LBL_LEN)
                {
                    prln();
                    printf("\nToo long label \"%s\" in line %i. Max label length is %i\n", asmCmd, (int)line + 1, MAX_CMD_LBL_LEN);
                    throw ASMERR_TOOLNGLBL;
                }
            };
        }
        //If this is a label
        if (c == ':')
        {
            free(asmCmd);
            free(asmCmdHex);
            continue;
        };
        myAsmCmdNum asmCmdNum = ASMCMDQT; //< initialize 'number of command that was read' with default value
        //Recognize command
        {
            assert(asmCmd);
            //Compare with known commands
            for (myAsmCmdNum i = (myAsmCmdNum)0; i < ASMCMDQT; i = (myAsmCmdNum)(i+1)) //For each known assembler command
            {
                //printf("\n!!!%s[%d] == %s!!!\n", myAsmCmd[i], i , asmCmd);
                if (!strcmp(asmCmd, myAsmCmd[i]))       //Compare it with current command. If they are similar,
                {
                    //printf("\n!!!OK!!!\n");
                    asmCmdNum = i;
                    break;
                }
            }
            //Check
            if (asmCmdNum == ASMCMDQT)  //< if it still stores default value then throw
            {
                prln();
                printf("\nUnknown command \"%s\" in line %i\n", asmCmd, (int)line + 1);
                throw ASMERR_UNKCMD;
            }; 
            free(asmCmd);
            //Write command to resBuf
            asmCmdHex[0] = asmCmdNum;
        }

        //Read arguments
        for (int argn = 1; argn <= myAsmCmdArg[asmCmdNum][0] - '0'; argn++)
        {
            //Read argument type(register/immediate)
            {
                skipdelims();
                assert(assm->srcBuf->strings);
                if (( myAsmCmdArg[asmCmdNum][argn] == '*') || ((c = assm->srcBuf->strings[line].str[i]) == myAsmCmdArg[asmCmdNum][argn]) )
                {
                    if (c == '%')
                    {
                        asmCmdHex[0] |= (1<<(8-argn));
                    }
                    else if (c == '$')
                    {
                        asmCmdHex[0] &= ~(1<<(8-argn));
                    }
                    else
                    {
                        prln();
                        printf("\nUnknown argument type \"%c\" in line %i(may be there is a space between label and colon?)\n", c, (int)line + 1);
                        throw ASMERR_UNKARGTYPE;
                    };
                }
                else
                {
                    prln();
                    printf("\nError: Wrong argument type in line %i\n", line + 1);
                    throw ASMERR_WRGARGTYPE;
                };
                i++;
            }
            
            //Read argument value
            {
                double arg;
                // If it is a label
                if ( ((argn == 3) && (asmCmdNum > CPU_JMP) && (asmCmdNum <= CPU_JLE)) || (asmCmdNum == CPU_JMP) || (asmCmdNum == CPU_CALL) )
                {
                    assert(assm->labelPtr);
                    char *label = (char*) calloc(MAX_CMD_LBL_LEN, sizeof(char));
                    int j = 0;
                    //Read label
                    if (isalnum(c = assm->srcBuf->strings[line].str[i]))
                    {
                        while ( isalnum(c = assm->srcBuf->strings[line].str[i]) || (c == '_') )
                        {
                                    i++;
                                    label[j++] = c;
                        }
                    }
                    //Throw if no label is in
                    if ( j == 0 )
                    {
                        prln();
                        printf("\nThere is no label in line %i\n", (int)line + 1);
                        throw ASMERR_NULLLENLBL;
                    };
                    
                    //Throw if it is too long
                    if ( isalnum(c) )
                    {
                        prln();
                        printf("\nToo long label \"%s\" in line %i. Max label length is %i(char c = '%c'(0x%x)\n", asmCmd, (int)line + 1, MAX_CMD_LBL_LEN, c, c);
                        throw ASMERR_TOOLNGLBL;
                    }
                    //Find where this label is used
                    unsigned p = 0;
                    for (p = 0; p < (unsigned)assm->labelPtr; p++)
                    {
                      if (strcasecmp(assm->labels[p]->name, label) == 0)
                        {
                            arg = assm->labels[p]->ptr;
                            break;
                        };
                    };
                    if ( p == (unsigned)assm->labelPtr )
                        {
                            prln();
                            printf("\nLabel '%s'(in line %i: '%s') is not declared or declared in last line.\n", label, int(line)+1, 
                                                                                                                 assm->srcBuf->strings[line].str);
                            printf("If you want to declare label in the end of program, put NOP in last line\n"); 
                            for (unsigned m = 0; m < (unsigned)assm->labelPtr; m++)
                            {
                                printf("\nLabel[%i]->name = %s\n", m, assm->labels[m]->name);
                                printf("strcasecmp(%s, %s) = %i\n", assm->labels[m]->name, label, strcasecmp(assm->labels[m]->name, label));
                            };
                            throw ASMERR_UNDECLLBL;
                        }
                    free(label);
                } else
                {
                    assert(assm->srcBuf->strings);
                    if (sscanf(assm->srcBuf->strings[line].str+i, "%lg", &arg) != 1) //< if there is not enough arguments then
                    {
                        prln();
                        printf("\nToo few arguments(or not 'double' value is entered) in line %i\n", (int)line + 1);
                        throw ASMERR_TOOFEWARG;
                    }
                    //skip argument(not spaces)
                    while ( !isspace(c = assm->srcBuf->strings[line].str[i]) && (c) && (c != ';'))
                    {
                        i++;
                    }
                }
                
                //Write value to asmCmdHex
                for (unsigned int m = 0; m < sizeof(double); m++)
                {
                    *(asmCmdHex + (argn - 1)*8 + m + 1) = ((char*)&arg)[m];
                }

                if (c != ';') skipspaces();
                if ( (argn == myAsmCmdArg[asmCmdNum][0] - '0') && ( (c != ';')  && (c != 0) ) )
                {
                    prln();
                    printf("\nError: Two commands in one line or extra argument in line %i. Char c = '%c'(num(0d) = %i)\n", (int)line + 1, c, c);
                    printf("Line = %s", assm->srcBuf->buf);
                    throw ASMERR_TWOCMD;
                }
                //SKIP COMMENT
                if (c == ';')
                {
                    c = assm->srcBuf->strings[line].str[i+1];
                    skipspaces();
                    skipnotspaces();
                    skipspaces();
                }
                skipdelims();
            }
            
        }
        //Concatenate asmCmdHex to resBuf
        strcatBuf(assm->resBuf, asmCmdHex, 1 + sizeof(double)*(myAsmCmdArg[asmCmdNum][0] - '0'));
        free(asmCmdHex);
#undef skipspaces
#undef skipdelims
#undef skipnotspaces
    }
    return 0;
}

/**
  Look for labels in the hex file. Fill the 'labels' array.
 */
void fillLabelsUntranslate(asm_t *assm)
{
    assert(assm);
    
    char* slider = assm->srcBuf->buf;
    unsigned char c = '\0';

    if (*(slider++) != cpuSignature)
    {
        prln();
        printf("\nWrong CPU Signature");
        throw ASMERR_WRSIGN;
    }
    
    while (slider < assm->srcBuf->sizeFilled + assm->srcBuf->buf)
    {
        unsigned char asmCmd = *(slider++);

        //Recognize the command
        myAsmCmdNum asmCmdNum = (myAsmCmdNum) (asmCmd & 0b00011111); // Cut bits that sets addressing mode and recognize the command
        if ((asmCmdNum < 0) || (asmCmdNum >= ASMCMDQT))
        {
            prln();
            printf("\nUnknown command \"%x\" -> \"%x\" in character %i", c, asmCmdNum, (unsigned int)(slider - assm->srcBuf->buf));
            throw ASMERR_UNKCMD;
        };
        
        //If it is a jump
        if ( (asmCmdNum >= CPU_JMP) && (asmCmdNum <= CPU_CALL) )
        {
                if ( (asmCmdNum != CPU_JMP) && (asmCmdNum != CPU_CALL) )
                {
                    slider += 2 * sizeof(double);//< Go to JMP->ptr
                }

                
            //Set pointer
            assm->labels[assm->labelPtr]->ptr = (int)(*((double*)slider));
        
            //Set name
            {
                char* labelName = (char*)calloc(MAX_CMD_LBL_LEN, sizeof(*labelName));
                sprintf(labelName, "%s%i", standardLabel, assm->labelPtr);
                
                strcpy(assm->labels[assm->labelPtr]->name, labelName);
                free(labelName);
            }
        
            //Increase label counter
            assm->labelPtr++;
            //Move slider
            slider = slider + sizeof(double);
        } else
        {
            //Move slider
            slider = slider + sizeof(double)*( myAsmCmdArg[asmCmdNum][0] - '0' );
        };
    }; 
};

/**
 Translates assembler code to known commands
 */
int unTranslate(asm_t *assm)
{
    assert(assm);
    char* slider = assm->srcBuf->buf + 1;
    unsigned char c = '\0';
    //Fill 'labels' array
    fillLabelsUntranslate(assm);
    
    while (slider < assm->srcBuf->sizeFilled + assm->srcBuf->buf)
    {
#define putcB(charac) putcBuf(assm->resBuf, charac)
        //Put label
        for (int i = 0; i < assm->labelPtr; i++)
        {
            
            if (assm->labels[i]->ptr == assm->cmdWrittenQt)
            {
                
                strcatBuf( assm->resBuf, assm->labels[i]->name, strlen(assm->labels[i]->name) );
                
                putcB(':'); 
                
                putcB('\n');
            }
            
        }

        unsigned char asmCmd = *(slider++);
        
        //Analyze the first byte of the command
        myAsmCmdNum asmCmdNum = (myAsmCmdNum) (asmCmd & 0b00011111); // Cut bits that sets addressing mode and recognize the command
        if ((asmCmdNum < 0) || (asmCmdNum >= ASMCMDQT))
        {
            prln();
            printf("\nUnknown command \"%x\" -> \"%x\" in character %i", c, asmCmdNum, (unsigned int)(slider - assm->srcBuf->buf));
            throw ASMERR_UNKCMD;
        };
        
        assm->cmdWrittenQt++;
        strcatBuf( assm->resBuf, myAsmCmd[asmCmdNum], strlen(myAsmCmd[asmCmdNum]) );
        putcB(' ');

        //Recognize arguments
        for (int arg = 1; arg <= myAsmCmdArg[asmCmdNum][0] - '0'; arg++)
        {
            //Put addressing mode token
            if (asmCmd & (1 << (8-arg)))
                putcB('%');
            else putcB('$');
            
            //Convert argument value to string
            
            char argst[MAX_LINE_LEN] = "";
            
            sprintf(argst, "%lg", *((double*)slider));
            
            //Put string to resBuf
            if ( ((arg == 3) && (asmCmdNum > CPU_JMP) && (asmCmdNum <= CPU_JLE)) || (asmCmdNum == CPU_JMP) || (asmCmdNum == CPU_CALL) ) //<If it is a label
            { 
                
                //Put label as an argument
                int labelNum = -1;
                for (int i = 0; i < assm->labelPtr; i++)
                {
                    if (assm->labels[i]->ptr == (int)*((double*)slider))
                    {
                        labelNum = i;
                        break;
                    }
                }
                assert(labelNum != -1);
                
                strcatBuf( assm->resBuf, assm->labels[labelNum]->name, strlen(assm->labels[labelNum]->name) );
                
            } else
            {
                strcatBuf( assm->resBuf, argst, strlen(argst) );
                //Put space
                putcB(' ');
            }
            //Move slider
            slider = slider + sizeof(double);
        }
        //Go to next command
        putcB('\n' );
#undef putcB
    };
    return 0;
}

/**
 validates the asm structure and prints dump if needed
 */
int asmOk(asm_t *assm, int verbose)
{
    assert(assm);
    
    //Validate
    bool labelsOk = true;
    bool labelHasToBeOk = true;
    int firstBadPtrName = -1;
    int firstGoodLabelThatHasToBeNull = -1;
    for (int i = 0; i <= MAX_CMD_LBL_LEN; i++)
    {
        if (!assm->labels[i]) // if label is not ok
        {
            labelHasToBeOk = false;
        } else // else if label is ok
        {
            if (labelHasToBeOk) 
            {
                if (labelOk(assm->labels[i], false)) // if it is not truly ok
                {
                    labelsOk = false;
                    firstBadPtrName = i;
                    break;
                }
                
            } else // if it has to be NULL and it is ok
            {
                labelsOk = false;
                firstGoodLabelThatHasToBeNull = i;
                break;
            };
        };
    };
    
    bool ok = (assm->srcBuf) && (assm->resBuf) && (labelsOk);
    //Shout if neccesary
    if (verbose)
    {
        printf("\n  +----------------------------------%s+\n", (!ok) ? "----" : "");
        printf(  "  | Phila's assembler '%s' %s |\n", asmVersion, ok ? "is okay" : "is NOT okay");
        printf(  "  +----------------------------------%s+\n", (!ok) ? "----" : "");
        printf("    {\n");
        printf("\t Are labels Okay? - %s;\n", labelsOk ? "Yes" : "No");
        if (firstGoodLabelThatHasToBeNull != -1)
        {
            printf("\t First good label struct that has to be null:");
            labelOk(assm->labels[firstGoodLabelThatHasToBeNull], true);
        };
        if (firstBadPtrName != -1)
        {
            printf("\t First label with nonzero pointer that has null name pointer or uninitizalized program pointer:");
            labelOk(assm->labels[firstBadPtrName], true);
        };
        if (verbose > 1)
        {
            printf("\t\n Buffers' Dumps:\n\n");
            bufOk(assm->srcBuf, verbose);
            bufOk(assm->resBuf, verbose);
        }

        printf("    }\n\n");
    };
    return !ok;
}
Govnocode //Replace with static
bool ASM_ERROR_HAPPENED = false;
/**
 Unit tests for assembler
 */
void ut_asm_disasm(bool verbose, unsigned int mask)
{
///    cmdlnOptions_T *cmdlnOptions = (cmdlnOptions_T*)calloc(1, sizeof(*cmdlnOptions));
    /* +--------------------+
       |  CORRECT  INPUT    |
       +--------------------+  */
    /*
    What every unit test consists of?
       +--------------------+ translating +-------------+ disassembling +-------------------+ translating +-------------+
       |  Source file       | ----------> |  Hex file   | ------------> | Disassembly file  | ----------> | Result file |
       +--------------------+             +-------------+               +-------------------+             +-------------+
    I do not compare Source and Disassembly file, because Disassembly has ideal indentations, style, etc., but source file does not.
    I do compare Hex file and Result file. If they are similar, then assembler and disassembler both work correctly.
    */
    const char* testName = NULL;
    const char* inpFileName = NULL;
    //Perform all tests
    for (unsigned int test = 1; test <= sizeof(mask)*8; test++)
    {
        if (mask & (1 << (test - 1)) )
        {
            bool flag = false; // does such test exist?
            //Choose file with source
            switch (test)
            {

#define DEF_UT_ASM(testNum0, testName0, inputFileName)   \
                                                                \
            case testNum0:                                      \
            {                                                   \
             testName = #testName0;                               \
             inpFileName = inputFileName;                       \
             flag = true;                                       \
            }; break;
            
#include "Lists/ut_asm.hpp"

#undef DEF_UT_ASM

            default:
                flag = false;
                break; // such test does not exist
            }

            if (flag) // if it does exist then enter it
            {
                try
                {
                        prdel();
                        printf("testNum = %i\ntestName = %s\ninputFileName = %s\n", test, testName, inpFileName);

                    //Translate. inputFileName->outputFileName
                    {
                        asm_t* assm = (asm_t*)calloc(1, sizeof(*assm));
                        //Set cmd line input
                        const int argc = 5;
                        const char* argv[] = { "myProg", inpFileName, "-v0", "-v1", "-v2" };
                        //Try to get cmdLineOptions
                        try
                        {
///                            getCmdLnOptions(cmdlnOptions, argc, argv);
                        }
                        catch (ERRORS_T err)
                        {
                            if (err != ERR_TWICE)
                            {
                                printf(ERR_ST(err));
                                throw(err);
                            };
                        };
                        //Set default output file name if it's not entered
///                        assert(cmdlnOptions);
///                        if (!cmdlnOptions->optionsChosen[cmdlnOption_o])
///                       {
///                           cmdlnOptions->outputFileName = (char*)calloc(sizeof("output.o"), sizeof(char));
///                           assert(cmdlnOptions->outputFileName);
///                           strcpy(cmdlnOptions->outputFileName, "output.o");
///                       }

                        FILE *srcFile = NULL;
                        FILE *resFile = NULL;

                        //Open source file for reading and hex file for writing
///                        assert(cmdlnOptions->inputFileName);
///                       assert(cmdlnOptions->outputFileName);

///                        if (!(srcFile = fopen(cmdlnOptions->inputFileName, "r")))
///                            throw(ERR_OPFILER);
///                        if (!(resFile = fopen(cmdlnOptions->outputFileName, "w")))
///                            throw(ERR_OPFILEW);

                        asmctor(assm, srcFile, asm_Translate, false);
                        assert(srcFile);

                        fclose(srcFile);

                        translate(assm);

                        if (verbose)
                        {
                            prdel();
                            _bufOk(assm->srcBuf, true, "source file");
                            prdel();
                            _bufOk(assm->resBuf, true, "hex file");
                            prdel();
                        }
                        bufToStream(assm->resBuf, resFile);

                        assert(resFile);
                        fclose(resFile);

                        asmdtor(assm);
                    }
                    //Disassemble. outputFileName->utDisFileName
                    {
                        asm_t* assm = (asm_t*)calloc(1, sizeof(*assm));

                        //Open hex file
                        FILE *hexFile = NULL;
///                        if (!(hexFile = fopen(cmdlnOptions->outputFileName, "r")))
///                        {
///                            printf("Failed to open output file for reading?!");
///                            throw ERR_EMPTY;
///                        };
                        //Open file for disassembly
                        FILE *disFile;

                        if (!(disFile = fopen(utDisFileName, "w")))
                            throw(ERR_OPFILEW);

                        asmctor(assm, hexFile, asm_Untranslate);

                       // fclose(hexFile);

                        unTranslate(assm);
                        
                        if (verbose)
                        {
                            //_bufOk(assm->srcBuf, true, "hex file again");
                            //prdel();
                            _bufOk(assm->resBuf, true, "disassembly file");
                            prdel();
                        }
                        bufToStream(assm->resBuf, disFile);

                        assert(disFile);
                        fclose(disFile);

                        asmdtor(assm);
                    }
                    //Translate. utDisFileName->utResFileName
                    {
                        asm_t* assm = (asm_t*)calloc(1, sizeof(*assm));

                        FILE *srcFile;
                        FILE *resFile;

                        //Open source file for reading and hex file for writing
                        assert(utDisFileName);
                        assert(utResFileName);

                        if (!(srcFile = fopen(utDisFileName, "r")))
                            throw(ERR_OPFILER);
                        if (!(resFile = fopen(utResFileName, "w")))
                            throw(ERR_OPFILEW);

                        asmctor(assm, srcFile, asm_Translate, false);

                        assert(srcFile);
                        fclose(srcFile);

                        translate(assm);
                        if (verbose)
                        {
                            //_bufOk(assm->srcBuf, true, "disassembly file again");
                            //prdel();
                            _bufOk(assm->resBuf, true, "result file");
                        }
                        bufToStream(assm->resBuf, resFile);

                        assert(resFile);
                        fclose(resFile);

                        asmdtor(assm);
                    }
                    //Compare outputFileName == utResFileName
                    {
                        FILE *hexFile;
                        FILE *resFile;

                        //Open source file for reading and hex file for writing
///                        assert(cmdlnOptions->outputFileName);
                        assert(utResFileName);

///                        if (!(hexFile = fopen(cmdlnOptions->outputFileName, "r")))
///                            throw(ERR_OPFILER);
                        if (!(resFile = fopen(utResFileName, "r")))
                            throw(ERR_OPFILEW);

                        charBuf_t* hexBuf = (charBuf_t*)malloc(sizeof(*hexBuf));
                        streamToBuf(hexBuf, hexFile);
                        charBuf_t* resBuf = (charBuf_t*)malloc(sizeof(*resBuf));
                        streamToBuf(resBuf, resFile);
#define CALLDUMPS() \
                if (!ASM_ERROR_HAPPENED) \
                { \
                    ASM_ERROR_HAPPENED = true; \
                    printf("\n\n'%s' test is NOT okay. There are some dumps for you:\n", testName); \
                    prdel(); \
                    ut_asm_disasm(true, (unsigned)(1<<(test - 1)) ); \
                }

                        if (bufcmp(hexBuf, resBuf))
                        {
                            printf("\n\n'%s' test is okay", testName);
                            prdel();
                        }
                        else
                        {
                            CALLDUMPS();
                        }
                    }
                }
                catch (ERRORS_T errCode)
                {
                    printf("\n%s\n", ERR_ST(errCode));
                    CALLDUMPS();
                    throw(ERR_EMPTY);
                }
                catch (ASMERRORS_T errCode)
                {
                    printf("\nError ASMERRORS_T #(%i)\n", errCode);
                    CALLDUMPS();

                    throw(ERR_EMPTY);
                };
#undef CALLDUMPS
            }
        }
    }
/// free(cmdlnOptions);
    /* +--------------------+
       |  INCORRECT  INPUT  |
       +--------------------+  */
}
