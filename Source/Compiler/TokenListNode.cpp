#include <cctype>
#include <cstring>
#include <cassert>

#include "Compiler.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"
#include "Lexical_analyzer.hpp"

TokenListNode::TokenListNode(LangKeywordType type, double data, size_t line,
                             TokenListNode* prev, TokenListNode* next):
    type_(type),
    isID (false),
    val_ (),
    line_(line),
    next_(next),
    prev_(prev)
{
    val_.data = data;
}

TokenListNode::TokenListNode(LangKeywordType type, char* idName, size_t line,
                             TokenListNode* prev, TokenListNode* next):
    type_(type),
    isID (true),
    val_ (),
    line_(line),
    next_(next),
    prev_(prev)
{
    size_t nameLen = strnlen(idName, MAX_LINE_LEN);
    val_.idName = (char*)calloc(nameLen + 1, sizeof(char));
    strncpy(val_.idName, idName, nameLen);
}

TokenListNode::~TokenListNode()
{
    if (isID) free(val_.idName);
}

void TokenListNode::demoDump(FILE* stream)
{
    assert(stream);

    switch (getDataType(type_))
    {
        case TYPE_DOUBLE:
        {
            fprintf(stream, "%lg",
                    (double)val_.data);
        }; break;

        case TYPE_CHAR_PTR:
        {
            assert(val_.idName);
            fprintf(stream, "'%s'",
                    val_.idName);
        }; break;

        case TYPE_SIZE_T:
        {
            char stringToPrint[MAX_LINE_LEN] = {};
            switch (type_)
            {
                case LANGKWT_mathOperator:
                {
                    assert(val_.data >= 0           &&
                           val_.data < LANGMATH_MATHQT);
                    fprintf(stream, "'%s'",
                            langMathOperatorsStrings((LangMathOperators)val_.data));
                }; break;
                case LANGKWT_num_bracket:
                case LANGKWT_delim:
                {
                    assert(val_.data >= 0           &&
                           val_.data < LANGSEC_SECQT);
                    fprintf(stream, "'%s'",
                            langSecondaryStrings((LangSecondary)val_.data));
                }; break;
                default:
                {
                    assert(val_.data >= 0           &&
                           val_.data < LANGKWD_KWDQT);
                    fprintf(stream, "'%s'",
                            langKeywordsStrings((LangKeyword)val_.data));

                }; break;
            };

        }; break;

        case TYPE_NIL:
        {
            fprintf(stream, "(nil)");
            return;
        }; break;

        default:
        {
            prln();
            printf("Unknown node type in TokenListNode::dump");
            myThrow(COMPERR_WRNGTYPE);
        }
    }
}

void TokenListNode::fullDump(FILE* stream)
{
    assert(stream);

    fprintf(stream, "ListNode[0x%p]\n"
                    "{\n", this);

    fprintf(stream, "\tvalue = ");

    demoDump(stream);

    fprintf(stream, "\n\ttype = %i (%s)\n"
                    "}\n",
                    type_,
                    langKeywordsTypesStrings(type_));

}

bool operator == (const TokenListNode& left, const TokenListNode& rght)
{
    return (left.type_       == rght.type_)   &&
           (left.val_.idName == rght.val_.idName);
}

bool operator != (const TokenListNode& left, const TokenListNode& rght)
{
    return (!(left == rght));
}
