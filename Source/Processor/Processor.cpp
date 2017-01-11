#include "Processor.hpp"

char* procMathCmd(unsigned mathOp)
{
    switch (mathOp)
    {
        case '*': return "MUL";
                 
        case '/': return "DIV";
                 
        case '+': return "ADD";
                 
        case '-': return "SUB";
                 
        case 'n': return "LNE";
                 
        case 'e': return "LE";
                 
        case 'f': return "NEG";
                 
        case '^': return "POW";
                 
        case '>': return "LG";
                 
        case '<': return "LL";
                 
        case 'G': return "LGE";
                 
        case 'L': return "LLE";
                 
        default: {
                    prln();
                    printf("Unknown math operator is passed to the procMathCmd");
                    throw(CPUERR_UNKMATH);
                 }; break;
    }
}

int procctor(processor* proc, FILE* programHex)
{
    assert(proc);
    ALLOC(proc->registers = (double*)calloc(CPU_REGISTERS_QT, sizeof(*proc->registers)));
    ALLOC(proc->dataStack = (stack_t*)calloc(1, sizeof(*proc->dataStack))); stackctor(proc->dataStack, CPU_STACK_SIZE);
    ALLOC(proc->callStack = (stack_t*)calloc(1, sizeof(*proc->callStack))); stackctor(proc->callStack, CPU_STACK_SIZE);
    
    ALLOC(proc->programMemory = (charBuf_t*)calloc(1, sizeof(*proc->programMemory)));
    
    streamToBuf(proc->programMemory, programHex); 

    proc->programCounter = 0;
    
    for (int i = 0; i < CPU_REGISTERS_QT; i++)
    {
      proc->registers[i] = 0;  
    };
    
    return 0;
};

void procdtor(processor* proc)
{
    stackdtor(proc->dataStack);
    stackdtor(proc->callStack);
    bufdtor(proc->programMemory);

    free(proc->registers);
    free(proc);
};

int _procOk(processor* proc, bool verbose, const char procName[MAX_LINE_LEN])
{
    if ( (!proc) || (!proc->dataStack) || (!proc->callStack) || (!proc->programMemory) )
    {
       return -1; 
    };
    bool ok = ( (!_stackOk(proc->dataStack, false)) && (!_stackOk(proc->callStack, false)) && 
                (proc->programCounter <= proc->programMemory->sizeFilled) && (!_bufOk(proc->programMemory, false)) );
    
    if (verbose)
    { 
        printf("\t Processor '%s' is %s [%p] \n", procName, (ok ? "okay" : "NOT OKAY"), proc);
        printf("\t{\n");
        printf("\t programCounter = %i\n", proc->programCounter);
        printf("\t registers:\n");
        for (int i = 0; i <= CPU_REGISTERS_QT; i++)
        {
            printf("\t   register[%i] = %u\n", i, proc->registers[i]);
        };
        bufOk(proc->programMemory, true);
        stackOk(proc->dataStack, true);
        stackOk(proc->callStack, true);
        printf("\n\t}\n");
    }
    return !ok;
};

void moveProgramCounter(processor* proc, unsigned argQt)
{
    assert(proc);
    
    proc->programCounter += sizeof(char) + sizeof(double) * argQt;
};

void CPU_CMD_JUMP(processor* proc, unsigned addr)
{
    proc->programCounter = 0;
    for (unsigned cmdCounter = 0; cmdCounter <= addr; cmdCounter++)
    {
        unsigned cmdNum = proc->programMemory->buf[proc->programCounter] & 0b11111;
        
        unsigned argQt = myAsmCmdArg[ cmdNum ][0] - '0';
        
        moveProgramCounter(proc, argQt);
    };
};

void procExecute(processor* proc, bool verbose)
{
    assert(proc);
    assert(proc->programMemory);

    while (proc->programCounter < proc->programMemory->sizeFilled)
    {
        switch (proc->programMemory->buf[proc->programCounter] & 0b11111/*leave bits that defines addresing mode*/)
        {
            #define DEF_CMD(CMD_NAME, CMD_ARG_STRING, CMD_CODE) \
            case CPU_##CMD_NAME: \
            { \
                unsigned argQt = #CMD_ARG_STRING [0] - '0'; \
                double* args = (double*)calloc(argQt, sizeof(*args)); \
                double* argType = (double*)calloc(argQt, sizeof(*argType)); \
                for (int arg = 0; arg < argQt; arg++) \
                { /*Argument position in program memory buffer */ \
                   double* argPosInBuf = (double*) ( proc->programMemory->buf + proc->programCounter + 1 + arg*sizeof(double) ); \
                   args[arg] = *argPosInBuf; \
                   argType[arg] = (double)( (proc->programMemory->buf[proc->programCounter] & (1 << (7 - arg))) >> (7 - arg) ); \
                };   \
                CMD_CODE; \
                free(args); free(argType); \
            }; \
                break;  \

            #include "Lists/Processor_cmd_list.hpp"
            
            #undef DEF_CMD
            
            default: 
            {
                prln();
                printf("Processor error: Unknown command '%x'\n", proc->programMemory->buf[proc->programCounter]&0b11111);
                throw (CPUERR_UNKCMD);
            }; break;
        };
    };
    //printf("proc->programCounter == %i; proc->programMemory->sizeFilled == %i\n",proc->programCounter , proc->programMemory->sizeFilled); 
};

void ut_proc(unsigned int mask, unsigned verbose)
{
    Govnocode
    ///cmdlnOptions_T *cmdlnOptions = (cmdlnOptions_T*)calloc(1, sizeof(*cmdlnOptions));
    const char* testName = NULL;
    const char* inpFileName = NULL;
    //Perform all tests
    for (unsigned test = 1; test <= sizeof(mask)*8; test++)
    {
        if (mask & (1 << (test - 1)) )
        {
            bool flag = false; // does such test exist?
            //Choose file with source
            switch (test)
            {

#define DEF_UT_PROC(testNum0, testName0, inputFileName)   \
                                                                \
            case testNum0:                                      \
            {                                                   \
             testName = #testName0;                             \
             inpFileName = inputFileName;                       \
             flag = true;                                       \
            }; break;

#include "Lists/ut_proc.hpp"

#undef DEF_UT_PROC

            default:
                flag = false;
                break; // such test does not exist
            }

            if (flag) // if it does exist then enter it
            {
                try
                {
                        prdel();
                        printf("testNum = %u\ntestName = %s\ninputFileName = %s", test, testName, inpFileName);
                    //Translate
                        asm_t* assm = (asm_t*)calloc(1, sizeof(*assm));
                        //Set cmd line input
                        const int argc = 5;
                        const char* argv[] = { "myProg", inpFileName, "-v0", "-v1", "-v2" };
                        //Try to get cmdLineOptions
                        try
                        {
Govnocode ///                            getCmdLnOptions(cmdlnOptions, argc, argv);
                        }
                        catch (ERRORS_T err)
                        {
                            
                            if (err != ERR_TWICE)
                            {
                                printf("%s", ERR_ST(err));
                                throw(err);
                            };
                        };
                        //Set default output file name if it's not entered
                      ///  assert(cmdlnOptions);
                        
                       /// if (!cmdlnOptions->optionsChosen[cmdlnOption_o])
                       /// {
                       ///     assert(cmdlnOptions->outputFileName);
                       ///    strcpy(cmdlnOptions->outputFileName, "output.o");
                        ///}

                        FILE *srcFile = NULL;
                        FILE *resFile = NULL;

                        //Open source file for reading and hex file for writing
                        ///assert(cmdlnOptions->inputFileName);
                        ///assert(cmdlnOptions->outputFileName);

                        /*if (!(srcFile = fopen(cmdlnOptions->inputFileName, "r")))
                            throw(ERR_OPFILER);
                        if (!(resFile = fopen(cmdlnOptions->outputFileName, "w")))
                            throw(ERR_OPFILEW);*/

                        asmctor(assm, srcFile, asm_Translate, false);
                        assert(srcFile);

                        fclose(srcFile);

                        translate(assm);
                        
                        if (verbose == 3)
                        {
                            prdel();
                            _bufOk(assm->srcBuf, true, "source file");
                            prdel();
                            _bufOk(assm->resBuf, true, "hex file");
                        } else
                        if (verbose == 2)
                        {
                            prdel();
                            _bufOk(assm->resBuf, true, "result file");
                        };
                        prdel();
                        
                        bufToStream(assm->resBuf, resFile);
                        
                        assert(resFile);
                        fclose(resFile);

                        asmdtor(assm); 
                //Execute
                        //Create processor instance
                        processor* proc = NULL;
                        ALLOC(proc = (processor*)calloc(1, sizeof(*proc)));
                        
                     ///   if (!(resFile = fopen(cmdlnOptions->outputFileName, "r")))
                     ///       throw(ERR_OPFILEW);
                        charBuf_t* buf = (charBuf_t*)calloc(1, sizeof(*buf));
                        streamToBuf(buf, resFile);

                        procctor(proc, resFile);
                        //Execution itself
                        procOk(proc, false);
                        procExecute(proc, verbose);
                        
                        bufdtor(buf);
                        procdtor(proc); 
                        
                } 
                catch (ERRORS_T errCode) { printf("\n%s\n", ERR_ST(errCode));  throw(ERR_EMPTY);} \
                catch (ASMERRORS_T errCode) { printf("\nError ASMERRORS_T #(%i)\n", errCode); throw(ERR_EMPTY); } \
                catch (STACKERRORS_T errCode) { printf("\nError STACKERRORS_T #(%i)\n", errCode); throw(ERR_EMPTY); }; 
            }
        }
    }
    ///cmdlnOptionsdtor(cmdlnOptions);
};
