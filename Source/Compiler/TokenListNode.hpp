#ifndef TOKEN_LIST_NODE_HPP_INCLUDED
#define TOKEN_LIST_NODE_HPP_INCLUDED

#include "Compiler.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"

union TokenData
{
    double data;
    char*  idName;
};

class TokenListNode
{
    public:
        TokenListNode(LangKeywordType type = LANGKWT_nil,
                      double          data = LANGKWD_KWDQT,
                      size_t          line = 0,
                      TokenListNode*  prev = NULL,
                      TokenListNode*  next = NULL);

        TokenListNode(LangKeywordType type   = LANGKWT_nil,
                      char*           idName = NULL,
                      size_t          line   = 0,
                      TokenListNode*  prev   = NULL,
                      TokenListNode*  next   = NULL);

        bool friend operator == (const TokenListNode& left, const TokenListNode& rght);
        bool friend operator != (const TokenListNode& left, const TokenListNode& rght);

        ~TokenListNode();

        void demoDump(FILE* stream);
        void fullDump(FILE* stream);

        LangKeywordType type_;
        bool            isID;
        TokenData       val_;
        size_t          line_;

        TokenListNode*  next_;
        TokenListNode*  prev_;
};

#endif
