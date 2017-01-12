#ifndef TOKEN_LIST_HPP_INCLUDED
#define TOKEN_LIST_HPP_INCLUDED

#include "Compiler.hpp"
#include "TokenListNode.hpp"
#include "Common/Char_buffers.hpp"

class TokenList
{
    public:
        TokenList (FILE* sourceFile);
        ~TokenList();

        operator TokenListNode*() const
        {
            return firstNode_;
        };
        TokenListNode* analyze   ();
        void           dump      (bool shortDump = true);
    private:
        size_t         curChar_;
        size_t         curLine_;
        TokenListNode* curNode_;
        TokenListNode* firstNode_;
        charBuf_t      sourceBuf_;

        void           deleteComments();

        TokenListNode* getKeywordOrIdentifier();
        TokenListNode* getGreaterLowerSign();
        TokenListNode* getNotEqualNegation();
        TokenListNode* getEqualAssignment();
        TokenListNode* getNumber();
};

#endif
