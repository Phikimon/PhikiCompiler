#pragma GCC diagnostic ignored "-Wunused-variable"
#include "Recursive_descent.hpp"
#include <cstring>

SyntaxAnalyzer::SyntaxAnalyzer(TokenListNode* firstNode,
                               FILE*          sourceFile,
                               bool           verbose   ):
    curTok_   (firstNode),
    sourceBuf_({}),
    verbose_  (verbose)
{
    assert(firstNode);
    streamToBuf(&sourceBuf_, sourceFile);
}

SyntaxAnalyzer::~SyntaxAnalyzer()
{
    bufdtor(&sourceBuf_);
}

void SyntaxAnalyzer::dump(TokenListNode* token)
{
    //NULL is default value
    if (token == NULL) token = curTok_;
    token->fullDump(stdout);
}

void SyntaxAnalyzer::putErr()
{
    size_t line = curTok_->line_;
    printf("\n\033[1m\033[31m"
           "String #%d contains error:\n",
                   line); int i = 0;
    int k = 1;
    while ( (k < line) && (i < sourceBuf_.Size) )
    {
        if (sourceBuf_.buf[i++] == '\n')
            k++;
    }
    if (i == sourceBuf_.Size) return;
    printf("\033[1m\033[32m\033[4m\033[5m");
    while (sourceBuf_.buf[i] != '\n')
    {
        printf("%c", sourceBuf_.buf[i++]);
    }
    printf("\033[0m\n");
}

void SyntaxAnalyzer::_assertTokenType (LangKeywordType type,
                                      unsigned line, const char* func)
{
    if (curTok_->type_ != type)
    {
        putErr();
        printf("\033[31m\033[1m");
        printf("In func %s in line %u curTok_ type", func, line);
        printf("%d(%s) is not equal to %d(%s).\n",
                curTok_->type_,
                langKeywordsTypesStrings((LangKeywordType)curTok_->type_),
                type          ,
                langKeywordsTypesStrings((LangKeywordType)type) );

        printf("Token: "); dump();

        printf("\033[0m\n");
        myThrow(COMPERR_WRNGTYPE);
    }
};

void SyntaxAnalyzer::_assertToken(LangKeywordType type,
                                 double data,
                                 unsigned line, const char* func)
{
    _assertTokenType(type, line, func);
    if (!D_EQ(curTok_->val_.data, data))
    {
        putErr();

        printf("\033[31m\033[1m");
        printf("In func %s in line %u (FILE LINE %d)", func, line, curTok_->line_);
        printf("curTok_ data '%lg' is not equal to %lg.\n", curTok_->val_.data, data);

        printf("Token: "); dump();

        TokenListNode* expectedToken = new TokenListNode(type, curTok_->line_, data);

        printf("Expected token:\n"); dump(expectedToken);

        printf("\033[0m\n");
        myThrow(COMPERR_WRNGDATA);
    }
};

void SyntaxAnalyzer::movePtr()
{
    if (verbose_)
    {
        printf("I am %s!\n", __func__);
        dump();
    }

    assert(curTok_);
    if (!curTok_->next_)
    {
        dump();
        printf("\nError: Attempt to call non-existent list node\n");
        myThrow(COMPERR_NULLTOK);
    };

    curTok_ = curTok_->next_;
}
