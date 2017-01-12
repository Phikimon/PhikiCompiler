//lib
#include "Compiler.hpp"

#include "Assembler/Assembler.hpp"
#include "Assembler/Disassembler.hpp"
#include "Processor/Processor.hpp"
#include <cstring>
#include <cctype>

//Current project
#include "Compiler/Lexical_analyzer.hpp"
#include "Compiler/Recursive_descent.hpp"
#include "Compiler/Semantical_analyzer.hpp"
#include "Compiler.hpp"

#define COMPILER_VERSION "v1.0beta"

int compilerOptionsCtor(CompilerOptions* opts, int argc, char** argv)
{
    assert(opts); assert(argv); assert(argc >= 0);

    opts->aflag = opts->sflag = opts->rflag = opts->tflag = false;
    opts->ovalue = opts->ivalue = NULL;

    int c = 0;
    opterr = 0;

    #define caseopt(opt, optchar)        \
    case optchar:                        \
    {                                    \
       opts->opt##flag = true;           \
    }; break;

    #define caseargopt(opt, optchar)     \
    case optchar:                        \
    {                                    \
        opts->opt##value = optarg;       \
    }; break;

    while ((c = getopt (argc, argv, "asrti:o:") ) != -1)
    {
        switch (c)
        {
            caseopt(a, 'a');
            caseopt(s, 's');
            caseopt(r, 'r');
            caseopt(t, 't');
            caseargopt(i, 'i');
            caseargopt(o, 'o');

            case '?':
            {
                if ( (optopt == 'i') || (optopt == 'o') )
                    printf ("Option -%c requires an argument.\n",  optopt);
                else if (isprint (optopt))
                    printf ("Unknown option `-%c'.\n",             optopt);
                else
                    printf ("Unknown option character `\\x%x'.\n", optopt);
                return 1;
            }
            default:
                return 1;
        }
    }

//#define PRINT_SET_OPTIONS
#ifdef PRINT_SET_OPTIONS
    printf ("aflag = %i, sflag = %i, rflag = %i, tflag = %i\n"
            "ivalue = %s, ovalue = %s",
             opts->aflag, opts->sflag, opts->rflag, opts->tflag, opts->ivalue, opts->ovalue);
#endif

    for (unsigned index = optind; index < argc; index++)
        printf ("Ignored argument %s\n", argv[index]);

    return 0;
}


int compile(int argc, char** argv)
{
#define ifFlagSetDo(opt, value, code)       \
    {                                       \
        if (opts->opt##flag == value)       \
        {                                   \
            code;                           \
        }                                   \
    }

    CompilerOptions* opts = NULL;
    ALLOC( opts = (CompilerOptions*)calloc(1, sizeof(*opts)) );
    if (compilerOptionsCtor(opts, argc, argv))
    return -1;

    printf("\n  +-----------------------------+\n"                 );
    printf(  "  | Phila's compiler %s   |\n"         , COMPILER_VERSION);
    printf(  "  +-----------------------------+\n"                 );

    //GET TOKEN SEQUENCE

    FILE* sourceFile = NULL;
    FOPEN(sourceFile = fopen(opts->ivalue, "r"));

    TokenList* lst = new TokenList(sourceFile);

    //GET TREE

    SyntaxAnalyzer san(lst->analyze(), sourceFile, opts->rflag);

    ifFlagSetDo(t, true, lst->dump(false));

    fclose(sourceFile);

    TreeNode* syntaxTree = san.getG0();

    ifFlagSetDo(s, true, Tree(syntaxTree).dotDump());

    //GET ASM CODE

    SyntaxTreeTranslator trans = SyntaxTreeTranslator(syntaxTree);

    trans.translate();

    char asmFileName[MAX_LINE_LEN] = "";
    strncpy(asmFileName, trans.getAsmFileName(), MAX_LINE_LEN);

    delete syntaxTree;

    //GET HEX CODE

    FILE* asmFile = NULL;
    FOPEN(asmFile = fopen(asmFileName, "r"));

    Assembler assm(asmFile);

    fclose(asmFile);

    ifFlagSetDo(a, false, printfTo(system, "rm %s", asmFileName));

    assm.translate();

    char hexFileName[MAX_LINE_LEN] = "";
    sprintf(hexFileName, "%s.hex", (opts->ovalue ? opts->ovalue : asmFileName) );

    FILE* hexFile = NULL;
    FOPEN(hexFile = fopen(hexFileName, "w"));

    assm.loadToFile(hexFile);
    FCLOSE(hexFile);
#ifdef DISASSEMBLER
    FOPEN(hexFile = fopen(hexFileName, "r"));
    Disassembler disasm(hexFile);
    fclose(hexFile);

    disasm.disassemble();
    disasm.dump();
#endif
#undef ifFlagSetDo
    printf("\nCompilation is done successfully!\n");

    return 0;

}

#undef COMPILER_VERSION
