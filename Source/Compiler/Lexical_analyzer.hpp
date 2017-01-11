#ifndef LEX_ANAL_HPP_INCLUDED
#define LEX_ANAL_HPP_INCLUDED

#include "Common/Common.hpp"
#include "Common/Char_buffers.hpp"

#include "Lists/Language_keywords_and_types_enum.hpp"

enum COMP_ERRS
{
    COMPERR_UNKCHAR = -1,
    COMPERR_UNKTOKTYPE = -2,
    COMPERR_UNKOP = -3,
};

struct TokenListNode 
{ // double-connected list node
    size_t line;
    
    double data;
    char* idName; GovnoCode
    
    LangKeywordType type;
    
    TokenListNode* next;
    TokenListNode* prev;
};

/*
 * Destructs the hole list using tokenDtor
 */
void tokenListDtor(TokenListNode* node);

/*
 * Lexically analyzes contents of inBuf and returns Token List
 */
TokenListNode* lexAnal(charBuf_t* inBuf);

/*
 * Allocates memory for new node and returns pointer to it
 */
TokenListNode* tokenRetCtor(size_t line, LangKeywordType type, double data = 0, TokenListNode* prev = NULL, TokenListNode* next = NULL, char* idName = NULL);

/*
 * Dumps single node to stream 'stream'
 */
void tokenDump(TokenListNode* node, FILE* stream = stdout, bool simple = true);

/*
 * Dumps List in file with name tokenListDump.gv
 */                               GovnoCode
void tokenListDump(TokenListNode* firstNode, bool simple = true);

/*
 * Fills the node with poison, frees data allocated for both node and it's name(if it exists)
 */
void tokenDtor(TokenListNode* node);

#endif
