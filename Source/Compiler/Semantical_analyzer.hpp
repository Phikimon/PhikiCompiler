#ifndef SEMANTICAL_ANALYZER_HPP_INCLUDED
#define SEMANTICAL_ANALYZER_HPP_INCLUDED

#include "Compiler.hpp"
#include "Assembler/CommonAssembler.hpp"
#include "Recursive_descent.hpp"

#include "Tree/Tree.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"

enum SyntaxTreeTranslatorErrors
{
    STTERR_OK = 0,      //< No error occured
    STTERR_UNKTYPE,     //< Unknown data type passed to the
                        //< assertSynt
    STTERR_SIMNAMES,    //< Two arguments or variables of similar
                        //< names found
    STTERR_UNKNAME,     //< Undeclared variable
    STTERR_UNDECLFUNC,  //< Undeclared function
    STTERR_TOOFEWARGS,  //< Not enough arguments are passed
                        //< to the function
    STTERR_TOOMANYARGS, //< Too many arguments are passed
                        //< to the function
};

class SyntaxTreeTranslator
{
    public:
        explicit SyntaxTreeTranslator(TreeNode* root);
        ~SyntaxTreeTranslator();

        void translate();
        const char* getAsmFileName();
    private:
        FILE* asmFile_;
        char asmFileName_[MAX_LINE_LEN];

        struct FunctionContext
        {
            char  functionName[MAX_LINE_LEN];
            char* names[COMPILER_MAX_NAMES_QT];
            int   argsQt;
            int   varsQt;
            int namesQt()
            {
                return argsQt + varsQt;
            }
        };
        FunctionContext functions_[COMPILER_MAX_FUNC_QT];
        int functionCounter_;

        TreeNode* root_;

        void _assertSyntType(TreeNode* node,
                            LangKeywordType type,
                            unsigned line, const char* func);
        template <typename T>
        void _assertSynt(TreeNode* node,
                         LangKeywordType type,
                         T data,
                         unsigned line, const char* func);

        void checkNamesCollision();
        int  findName(TreeNode* nodeWithName);

        int  getVariables(TreeNode* variables,
                         char* names[COMPILER_MAX_FUNC_VAR_QT]);

        int  getArguments(TreeNode* arguments,
                         char* names[COMPILER_MAX_FUNC_ARG_QT]);


        void compileProgram              (TreeNode* node);
        void compileFunction             (TreeNode* node);
        void compileOperatorsBlock       (TreeNode* currentDot);

        void compileConditionalOperator  (TreeNode* node);
        void compileReturnOperator       (TreeNode* node);
        void compileLoopOperator         (TreeNode* node);
        void compileInputOrOutputOperator(TreeNode* node);
        void compileExpression           (TreeNode* node);
        void compileAssignmentOperator   (TreeNode* node);

        void compileFunctionCall         (TreeNode* node);
        void compileFunctionCallEpilogue (TreeNode* node,
                                         int funcNumber);
        void compileFunctionCallPrologue (TreeNode* node,
                                         int funcNumber);

        void compileRValue               (TreeNode* node);
};

template <typename T>
void SyntaxTreeTranslator::_assertSynt(TreeNode* node,
                                       LangKeywordType type,
                                       T data,
                                       unsigned line, const char* func)
{
    _assertSyntType(node, type, line, func);
    bool equal = false;
    switch (getDataType(type))
    {
        case TYPE_CHAR_PTR:
        {
            equal = ((const char*)data ==
                     (const char*)node->data_.idName);
        }; break;
        case TYPE_DOUBLE:
        {
            equal = ((double)data ==
                     (double)node->data_.number);
        }; break;
        case TYPE_SIZE_T:
        {
            equal = ((size_t)data ==
                     (size_t)node->data_.kwdNum);
        }; break;
        case TYPE_NIL:
        {
            equal = false;
        }; break;
        default:
        {
            prln();
            printf("\nError: Unknown type of node == '%i'"
                   " is passed to the 'SyntAnal::assertSynt' method\n", type);
            myThrow(STTERR_UNKTYPE);
        }; break;
    }
    if (!equal)
    {
        printf("\033[0mIn func '%s' in line #%u", func, line);
        printf(" syntax-tree node isn't equal to expected."
               " Node:\n"
               "\033[31m\033[1m");
        node->dump();
        TreeNode* expected = new TreeNode(type, data);
        printf("\033[0m"
               "\nExpected node:\n"
               "\033[31m\033[1m");
        expected->dump();
        printf("\033[0m\n");
        assert(!"Assertion failed");
    }
};

#endif
