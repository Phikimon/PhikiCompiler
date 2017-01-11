#ifndef RECURSIVE_DESCENT_H
#define RECURSIVE_DESCENT_H

#include "Compiler/Lexical_analyzer.hpp"
#include "Common/Trees.hpp"

extern TokenListNode* TokenListThatIsToBeSyntaxAnalyzed_neverUseIt;
extern charBuf_t* SourceToBeCompiled;
extern bool COMP_globalVerbose;

Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode Govnocode 

void printLine(size_t line);

void _assertTokenType(TokenListNode* token, treeNodeDataType type, unsigned line, const char* func);

void _assertIdToken(TokenListNode* token, treeNodeDataType type, const char* id, unsigned line, const char* func);

void _assertToken(TokenListNode* token, treeNodeDataType type, double data, unsigned line, const char* func);

#define putGlobalInLocal() TokenListNode* & curTok = TokenListThatIsToBeSyntaxAnalyzed_neverUseIt; \
                           charBuf_t* & src = SourceToBeCompiled;

#define assertToken(token, type, data) _assertToken(token, type, data, __LINE__, __func__)
#define assertIdToken(token, type, id) _assertIdToken(token, type, id, __LINE__, __func__)
#define assertTokenType(token, type) _assertTokenType(token, type, __LINE__, __func__)

//Essential functions to be used in declarations.h

#define printif(...) { if (COMP_globalVerbose) { printf(__VA_ARGS__); } }

void movePtr();

//Result of analyzing
#define GR(GR_NAME) \
    get##GR_NAME()

//Put result of analyzing in result var
#define GR2RES(GR_NAME) \
    result = get##GR_NAME();


void compSyntAnalCtor(TokenListNode* firstNode, charBuf_t* source = NULL);

void compSyntAnalDtor();

//Implements function that processes read of some expression
#define GR_ASSIGN(GR_NAME, GR_CONFIG) \
treeNode_t* get##GR_NAME () \
{ \
    unsigned dot = '.'; \
    \
    putGlobalInLocal(); \
    printif("\n\033[1m\033[36mI am get%s()! token:\033[0m\n", #GR_NAME); \
    if (COMP_globalVerbose) \
        tokenDump(curTok); \
    treeNode_t* result = NULL; \
    \
    { \
        GR_CONFIG; \
    }; \
    \
    printif("\033[1m\033[31mThat was get%s()! Bye-Bye!\033[0m\n\n", #GR_NAME); \
    return result; \
}

enum 
{
    COMP_MAX_FUNC_ARG_QT = 24,
    COMP_MAX_FUNC_QT = 16,
    COMP_MAX_OP_QT = 128,
};

typedef treeNode_t* rec_type; Govnocode //what is rec_type?

Govnocode // macro for functions' pre-definitions

treeNode_t* getN();
treeNode_t* getVAR();
treeNode_t* getP();
treeNode_t* getMATH_FUNCS();
treeNode_t* getUNMIN();
treeNode_t* getNEG();
treeNode_t* getD();
treeNode_t* getT();
treeNode_t* getS();
treeNode_t* getEQ();
treeNode_t* getGL();
treeNode_t* getEXPR();
treeNode_t* getRET();
treeNode_t* getLOOP();
treeNode_t* getCOND();
treeNode_t* getIO();
treeNode_t* getNEWVAR();
treeNode_t* getVARASS();
treeNode_t* getOP();
treeNode_t* getOPBLOCK();
treeNode_t* getARGS();
treeNode_t* getPROGRAM();
treeNode_t* getG0();

#endif
