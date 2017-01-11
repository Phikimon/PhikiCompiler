#include "./Recursive_descent.hpp"

TokenListNode* TokenListThatIsToBeSyntaxAnalyzed_neverUseIt = NULL;
charBuf_t* SourceToBeCompiled = NULL;
bool COMP_globalVerbose = false;

void printLine(size_t line)
{
    printf("\n\033[1m\033[31mString #%d contains error:\n", line);
    putGlobalInLocal();
    if (src)
    {
        int i = 0;
        int k = 1;
        while ( (k < line) && (i < src->Size) )
        {
            if (src->buf[i] == '\n')
                k++;
            i++;
        }
        if (i == src->Size) return;
        printf("\033[1m\033[32m\033[4m\033[5m");
        while (src->buf[i] != '\n')
        {
            printf("%c", src->buf[i++]);
        }
        printf("\033[0m\n");
    }
}

void _assertTokenType(TokenListNode* token, treeNodeDataType type, unsigned line, const char* func)
{
    if (token->type != type)
    {
        printLine(token->line);
        printf("\033[31m\033[1mIn func %s in line %u token type(%d)(%s) is not equal to %d(%s). Token:\n", func, line, token->type, LangKeywordsTypesStrings[token->type], type, LangKeywordsTypesStrings[type]);
        tokenDump(token);
        printf("\033[0m\n");
        assert(!"Assertion failed");
    }
};

void _assertIdToken(TokenListNode* token, treeNodeDataType type, const char* id, unsigned line, const char* func)
{
    _assertTokenType(token, type, line, func);
    if (strcmp(token->idName, id))
    {
        printLine(token->line);
        printf("In func %s in line %u (FILE LINE %d)token id '%s' is not equal to %s. Token:\n", func, line, token->line, token->idName, id);
        tokenDump(token);
        assert(!"Assertion failed");
    }
};

void _assertToken(TokenListNode* token, treeNodeDataType type, double data, unsigned line, const char* func)
{
    _assertTokenType(token, type, line, func);
    if (!D_EQ(token->data, data))
    {
        printf("In func %s in line %u (FILE LINE %d)token data '%lg' is not equal to %lg. Token:\n", func, line, token->line, token->data, data);
        tokenDump(token);
        TokenListNode expectedToken = {};
        expectedToken.data = data;
        expectedToken.type = (LangKeywordType)(unsigned)type;
        expectedToken.line = token->line;
        printf("Expected token:\n");
        tokenDump(token);
        assert(!"Assertion failed");
    }
};

void movePtr()
{
    putGlobalInLocal();
    assert(curTok);
    printif("I am movePointer!\n");
    if (!curTok->next)
    {
        tokenDump(curTok);
        assert(!"Attempt to go out of array borders");
    };
    if (COMP_globalVerbose)
        tokenDump(curTok);
    assert(curTok->next);
    curTok = curTok->next;
}

void compSyntAnalCtor(TokenListNode* firstNode, charBuf_t* source)
{
    assert(firstNode);
    putGlobalInLocal();
    src = source;
    curTok = firstNode;
}

void compSyntAnalDtor()
{
    putGlobalInLocal();
    assert(curTok);
    
    tokenListDtor(curTok);
}
