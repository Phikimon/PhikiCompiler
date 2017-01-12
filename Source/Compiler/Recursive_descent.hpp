#ifndef RECURSIVE_DESCENT_HPP
#define RECURSIVE_DESCENT_HPP

#include "Compiler.hpp"
#include "Lexical_analyzer.hpp"
#include "Tree/Tree.hpp"

enum
{
    COMPILER_MAX_FUNC_ARG_QT = 6,
    COMPILER_MAX_FUNC_VAR_QT = 6,
    COMPILER_MAX_NAMES_QT    = COMPILER_MAX_FUNC_VAR_QT +
                               COMPILER_MAX_FUNC_ARG_QT,
    COMPILER_MAX_FUNC_QT     = 16,
    COMPILER_MAX_OP_QT       = 128,
};

class SyntaxAnalyzer
{
    public:
        SyntaxAnalyzer(TokenListNode* firstNode,
                       FILE*          sourceFile,
                       bool           verbose);
        ~SyntaxAnalyzer();
        TreeNode* getG0(); // Same as __getProgram
    private:
        void _assertTokenType (LangKeywordType type,
                               unsigned line,
                               const char* func);
        void _assertToken(LangKeywordType type,
                          double data,
                          unsigned line,
                          const char* func);

        void movePtr();
        void dump(TokenListNode* token = NULL);
        void putErr();

        TokenListNode* curTok_;
        charBuf_t      sourceBuf_;
        bool           verbose_;

        TreeNode* getN          (); // Get number
        TreeNode* getVAR        (); // Get variable
        TreeNode* getP          (); // Get expression in brackets
        TreeNode* getMATH_FUNCS (); // Get math function
        TreeNode* getUNMIN      (); // Get unary minus
        TreeNode* getNEG        (); // Get unary negation
        TreeNode* getD          (); // Get degree
        TreeNode* getT          (); // Get multiplication or division
        TreeNode* getS          (); // Get sum or substraction
        TreeNode* getEQ         (); // Get exact comparison '==' '!='
        TreeNode* getGL         (); // Get relative comparison '>' '<'
        TreeNode* getEXPR       (); // Get expression
        TreeNode* getRET        (); // Get 'return' operator
        TreeNode* getLOOP       (); // Get 'while'-loop operator
        TreeNode* getCOND       (); // Get 'if'-conditional operator
        TreeNode* getIO         (); // Get IO operator - print, scan
        TreeNode* getNEWVAR     (); // Get new variable declaration
        TreeNode* getVARASS     (); // Get variable assignment operator
        TreeNode* getOP         (); // Get some operator
        TreeNode* getOPBLOCK    (); // Get operator block
        TreeNode* getCALLARGS   (); // Get function arguments in function call
        TreeNode* getARGS       (); // Get function arguments
        TreeNode* getPROGRAM    (); // Get the whole program
};

#endif // RECURSIVE_DESCENT_HPP
