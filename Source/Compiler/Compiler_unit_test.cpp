#include "Compiler_unit_test.hpp"
Govnocode //See TODO
void ut_comp(size_t verbose, unsigned mask)
{
    const char* testName = NULL;
    const char* fileName = NULL;
    
    //Perform all tests
    for (unsigned test = 1; test <= sizeof(mask)*8; test++)
    {
        if (mask & (1 << (test - 1)) )
        {
            bool flag = false; // does such test exist?
            //Choose file with source
            switch (test)
            {

#define DEF_UT_COMP(testNum0, testName0, inputFileName)         \
                                                                \
            case testNum0:                                      \
            {                                                   \
             testName = #testName0;                             \
             fileName = inputFileName;                          \
             flag = true;                                       \
            }; break;

#include "Lists/ut_compiler.hpp"

#undef DEF_UT_COMP

            default:
                flag = false;
                break; // such test does not exist
            }

            if (flag) // if it does exist then enter it
            {
                try
                {
                    prdel();
                    printf("testNum = %u\ntestName = %s\ninputFileName = %s\n\n", test, testName, fileName);
                    
                    system("rm ./temp -r");  printf("\n");
                    system("mkdir ./temp/");
                    
                    //GET CHAR BUFFER
                    char transFilePath[MAX_LINE_LEN] = "";
                    sprintf(transFilePath, "./temp/%s", fileName);
                    
                    char command[MAX_LINE_LEN] = "";
                    sprintf(command, "python /home/philipp/Documents/Trees_git/Compiler/Trans/Python_Trans /home/philipp/Documents/Trees_git/Compiler/UT/%s > %s", fileName, transFilePath);
                    system(command);
                    
                    FILE* holy_square = NULL;
                    FOPEN(holy_square = fopen(transFilePath, "r"));
                    
                    charBuf_t* buf = (charBuf_t*)calloc(1, sizeof(*buf));
                    streamToBuf(buf, holy_square);
                    
                    fclose(holy_square);
                    
                    sprintf(command, "gedit %s", transFilePath);
                    
                    if (verbose & 0b1) system(command);
                    
                    if (verbose & 0b10) bufOk(buf, true);
                    
                    //GET TOKEN SEQUENCE
    
                    TokenListNode* tokenList = lexAnal(buf);
                    
                    if (verbose & 0b100) tokenListDump(tokenList, true);
                    
                    //GET TREE
                    
                    compSyntAnalCtor(tokenList, buf);
                    
                    COMP_globalVerbose = (verbose & 0b1000);
                    
                    treeNode_t* syntaxTree = getG0();
                    bufdtor(buf);
                    
                    if (verbose & 0b10000) treeDump(syntaxTree, syntDump); //nodeDump
                    
                    compSyntAnalDtor();
                    
                    //GET ASM CODE
                    
                    syntaxTreeTranslator_t* trans = (syntaxTreeTranslator_t*)calloc(1, sizeof(*trans));
                    
                    transCtor(trans, syntaxTree);
                    assert(trans->root);
                    translateTree(trans, trans->root);
                    
                    char asmFileName[MAX_LINE_LEN] = "";
                    strcpy(asmFileName, trans->asmFileName);
                    
                    transDtor(trans); free(trans);
                        
                    if (verbose & 0b100000)
                    {
                        sprintf(command, "gedit %s", asmFileName);
                        
                        system(command);
                    }
                    
                    treedtor(syntaxTree);
                    
                    //GET HEX CODE
                    
                    FILE* asmFile = NULL;
                    FOPEN(asmFile = fopen(asmFileName, "r"));
                    
                    asm_t assm = {};
                    
                    asmctor(&assm, asmFile, asm_Translate, false);
                    
                    fclose(asmFile);
                    
                    translate(&assm);
                    
                    if (verbose & 0b1000000)
                    {
                        prdel();
                        _bufOk(assm.resBuf, true, "hex file");
                        prdel();
                    }
                    
                    char hexFileName[MAX_LINE_LEN] = "";
                    sprintf(hexFileName, "%s.hex", asmFileName);
                    
                    FILE* hexFile = NULL;
                    FOPEN(hexFile = fopen(hexFileName, "w"));
                    
                    bufToStream(assm.resBuf, hexFile);
                    
                    fclose(hexFile);
                    
                    //EXECUTE
                    
                    //Create processor instance
                    processor* proc = (processor*)calloc(1, sizeof(*proc));
                    
                    FOPEN(hexFile = fopen(hexFileName, "r"));
                    
                    charBuf_t* hexBuf = (charBuf_t*)calloc(1, sizeof(*hexBuf));
                    streamToBuf(hexBuf, hexFile);

                    procctor(proc, hexFile);
                    //Execution itself
                    procOk(proc, false);
                    procExecute(proc, verbose & 0b10000000);
                    
                    bufdtor(hexBuf);
                    procdtor(proc); 
                    
                    printf("\n");
                
                } 
                catch (ERRORS_T errCode) { printf("\n%s\n", ERR_ST(errCode));  throw(ERR_EMPTY);} 
            }
        }
    }
};
