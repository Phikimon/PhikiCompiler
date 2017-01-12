#include "Lexical_analyzer.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"
#include "Compiler.hpp"
#include <cassert>
#include <cctype>
#include <cstring>

TokenList::TokenList(FILE* sourceFile):
    curChar_  (0),
    curLine_  (0),
    firstNode_(NULL),
    sourceBuf_({})
{
   streamToBuf(&sourceBuf_, sourceFile);
}

TokenList::~TokenList()
{
    if (!firstNode_) return;
    curNode_   = firstNode_;
    firstNode_ = NULL;

    while (curNode_->next_)
    {
        curNode_ = curNode_->next_;
        delete curNode_;
    }
    delete curNode_;

    bufdtor(&sourceBuf_);
}

void TokenList::deleteComments()
{
    //Multi-line comments
    char* commentBegin = strstr(sourceBuf_.buf, "/*");
    while (commentBegin)
    {
        char* commentEnd = strstr(commentBegin + sizeof("/*"), "*/"); if (!commentEnd) {
            prln();
            printf("\nUnmatched /* in char #%d\n",
                    commentBegin - sourceBuf_.buf);
            myThrow(COMPERR_UNTERMCOMMENT);
        }

        memset(commentBegin, ' ', commentEnd - commentBegin);
        commentBegin = strstr(commentEnd + sizeof("*/"), "/*");
    }
    //Single-line comments
    commentBegin = strstr(sourceBuf_.buf, "//");
    while (commentBegin)
    {
        char* commentEnd = strchr(commentBegin + sizeof("//"), '\n');
        if (commentEnd)
        {
            memset(commentBegin, ' ', commentEnd - commentBegin);
            commentBegin = strstr(commentEnd + sizeof('\n'), "//");
        } else
        {
            memset(commentBegin, ' ', strlen(commentBegin));
            commentBegin = NULL;
        };
    }
}

#define       kwdNode(type, data)                     \
        TokenListNode((LangKeywordType)type, (size_t)data, curLine_, curNode_)
#define        idNode(type, data)                     \
        TokenListNode((LangKeywordType)type, (char*)data, curLine_, curNode_)
#define       numNode(type, data)                     \
        TokenListNode((LangKeywordType)type, (double)data, curLine_, curNode_)
TokenListNode* TokenList::getKeywordOrIdentifier()
{
    char identifier[MAX_LINE_LEN] = "";

    int idLen = -1;
    assert(MAX_LINE_LEN == 256);
    sscanf(&sourceBuf_.buf[curChar_], "%255[a-zA-Z_]%n", identifier, &idLen);
    curChar_ += (unsigned)idLen;

    for (unsigned i = 0; i < LANGKWD_KWDQT; i++)
    {
        if (!strcmp(langKeywordsStrings((LangKeyword)i), identifier))
        {
            return new kwdNode(getKeywordType((LangKeyword)i), (LangKeyword)i);
        }
    }

    return new idNode(LANGKWT_id, identifier);
}

TokenListNode* TokenList::getGreaterLowerSign()
{
    char oper = '\0';

    if (!strncmp(&sourceBuf_.buf[curChar_], ">=", sizeof(">=") - sizeof('\0')))
    {
        oper = LANGMATH_greater_or_equal;
        curChar_ += sizeof(">=");
    } else
    if (!strncmp(&sourceBuf_.buf[curChar_], "<=", sizeof("<=") - sizeof('\0')))
    {
        oper = LANGMATH_less_or_equal;
        curChar_ += sizeof("<=");
    } else
    if (sourceBuf_.buf[curChar_] == '<')
    {
        oper = LANGMATH_less;
        curChar_ += sizeof("<");
    } else
    if (sourceBuf_.buf[curChar_] == '>')
    {
        oper = LANGMATH_greater;
        curChar_ += sizeof(">");
    };

    return new kwdNode(LANGKWT_mathOperator, oper);
}

TokenListNode* TokenList::getNotEqualNegation()
{
    char oper = '\0';

    if (!strncmp(&sourceBuf_.buf[curChar_], "!=", sizeof("!=") - sizeof('\0')))
    {
        oper = LANGMATH_not_equal;
        curChar_ += sizeof("!=");
    } else
    {
        oper = LANGMATH_negate;
        curChar_ += sizeof("!");
    }

    return new kwdNode(LANGKWT_mathOperator, oper);
}

TokenListNode* TokenList::getEqualAssignment()
{
    char oper = '\0';

    if (!strncmp(&sourceBuf_.buf[curChar_], "==", sizeof("==") - sizeof('\0')))
    {
        oper = LANGMATH_equal;
        curChar_ += sizeof("==");
    } else
    {
        oper = LANGKWD_assign;
        curChar_ += sizeof("=");
    }

    return new kwdNode(LANGKWT_mathOperator, oper);
}

TokenListNode* TokenList::getNumber()
{
    double number    =  0;
    int    numberLen = -1;
    sscanf(&sourceBuf_.buf[curChar_], "%lg%n", &number, &numberLen);
    assert(numberLen != -1);
    //Because I have '.' instead of ';',
    //  return 10.
    //is "return->10->." not "return->(double)10."
    if (sourceBuf_.buf[curChar_ + numberLen - 1] == '.')
        numberLen -= 1;
    curChar_ += numberLen;

    return new numNode(LANGKWT_double, number);
}

TokenListNode* TokenList::analyze()
{
    deleteComments();
    firstNode_ = new TokenListNode(LANGKWT_nil,
                                   LANGKWD_KWDQT);
    curNode_  = firstNode_;

    while (curChar_ < sourceBuf_.Size)
    {
        TokenListNode* newToken  = NULL;
        if ( isspace(sourceBuf_.buf[curChar_]) )
        {
            if (sourceBuf_.buf[curChar_] == '\n')
                curLine_++;
            curChar_++;
            continue;
        }

        switch (sourceBuf_.buf[curChar_])
        {
            case '(':
            case ')':
            {
                LangSecondary data = LANGSEC_SECQT;
                switch(sourceBuf_.buf[curChar_++])
                {
                    case '(': data = LANGSEC_left_bracket; break;
                    case ')': data = LANGSEC_rght_bracket; break;
                }
                assert(data != LANGSEC_SECQT);
                newToken = new kwdNode(LANGKWT_num_bracket,
                                       data);
            }; break;

            case '-':
            case '/':
            case '*':
            case '+':
            case '~':
            {
                LangMathOperators data = LANGMATH_MATHQT;
                switch(sourceBuf_.buf[curChar_++])
                {
                    case '-': data = LANGMATH_minus; break;
                    case '/': data = LANGMATH_div;   break;
                    case '*': data = LANGMATH_mult;  break;
                    case '+': data = LANGMATH_plus;  break;
                    case '~': data = LANGMATH_sqrt;  break;
                }
                assert(data != LANGMATH_MATHQT);
                newToken = new kwdNode(LANGKWT_mathOperator,
                                       data);
            }; break;

            case ',':
            case '.':
            {
                LangSecondary data = LANGSEC_SECQT;
                switch(sourceBuf_.buf[curChar_++])
                {
                    case ',': data = LANGSEC_comma; break;
                    case '.': data = LANGSEC_dot;   break;
                }
                assert(data != LANGSEC_SECQT);
                newToken = new kwdNode(LANGKWT_delim,
                                       data);
            }; break;

            case '_':
            case 'a'...'z':
            case 'A'...'Z':
            {
                newToken = getKeywordOrIdentifier();
            }; break;

            case '=':
            {
                newToken = getEqualAssignment();
            }; break;

            case '!':
            {
                newToken = getNotEqualNegation();
            }; break;

            case '>':
            case '<':
            {
                newToken = getGreaterLowerSign();
            }; break;

            case '0'...'9':
            {
                newToken = getNumber();
            }; break;

            default:
            {
                if ( ispunct(sourceBuf_.buf[curChar_]) )
                {
                    printf("punct %c\n", sourceBuf_.buf[curChar_]);
                    curChar_++;
                    break;
                }
                prln();
                printf("Unrecognized char %c in lexical analyzer",
                        sourceBuf_.buf[curChar_]);
                myThrow(COMPERR_UNKCHAR);
            }; break;
        };
        if (newToken)
        {
            curNode_->next_ = newToken;
            curNode_ = curNode_->next_;
        }
    }

    if (firstNode_->next_)
    {
        firstNode_ = firstNode_->next_;
        delete firstNode_->prev_;
        firstNode_->prev_ = NULL;
        return firstNode_;
    } else
        return NULL;
}
#undef node

void TokenList::dump(bool shortDump)
{
    if (!firstNode_)
        return;

    FILE* dumpFile = NULL;
    FOPEN(dumpFile = fopen("./temp/tokenListDump.gv", "w"));

    fprintf(dumpFile, "digraph G {\n"
                      "graph [ dpi = 300 ]");

    TokenListNode* currentNode = firstNode_;
    while (currentNode)
    {
        fprintf(dumpFile, "listNode_%p [label=\"", currentNode);

        if (shortDump)
            currentNode->demoDump(dumpFile);
        else
            currentNode->fullDump(dumpFile);

        fprintf(dumpFile, "\"]\n");

        if (currentNode->next_)
        {
            fprintf(dumpFile, "listNode_%p -> listNode_%p\n",
                              currentNode, currentNode->next_);
        }
        currentNode = currentNode->next_;
    }

    fprintf(dumpFile, "\n}");

    fclose(dumpFile);

    system("dot ./temp/tokenListDump.gv -Tpng -o ./temp/tokenListDump.png");
    system("xdot ./temp/tokenListDump.gv");
};
