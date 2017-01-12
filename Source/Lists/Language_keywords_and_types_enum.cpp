#include "Language_keywords_and_types_enum.hpp"
#include "Common/Char_buffers.hpp"
#include <cassert>
#include <cstring>

static void fillLangKeywordsStrings(char keywordsStrings[LANGKWD_KWDQT][MAX_LINE_LEN]);

DataType getDataType(LangKeywordType type)
{
    switch (type)
    {
        case LANGKWT_id:
        case LANGKWT_string:
        case LANGKWT_function:
                                return TYPE_CHAR_PTR;
        case LANGKWT_double:
                                return TYPE_DOUBLE;
        case LANGKWT_num_bracket:
        case LANGKWT_delim:
        case LANGKWT_operator_bracket:
        case LANGKWT_conditional:
        case LANGKWT_io:
        case LANGKWT_new_program:
        case LANGKWT_new_function:
        case LANGKWT_new_variable:
        case LANGKWT_return:
        case LANGKWT_assign:
        case LANGKWT_loop:
        case LANGKWT_mathOperator:
                                return TYPE_SIZE_T;
        case LANGKWT_nil:
                                return TYPE_NIL;
        default:
        {
            prln();
            printf("\nError: Unknown type of node == '%i'"
                   " is passed to the 'getDataType' method\n", type);
            myThrow(LANGERR_UNKTYPE);
        }; break;
    }
}

LangKeywordType getKeywordType(LangKeyword kwd)
{
#define LANG_KEYWORDS
#define LANG_KEYWORD(keyword, keyword_type) \
{                                           \
    case LANGKWD_##keyword: return LANGKWT_##keyword_type;      \
}
    switch (kwd)
    {
       #include "Lists/Language_keywords.hpp"
       default: myThrow(ERR_STD);
    }
#undef LANG_KEYWORD
#undef LANG_KEYWORDS
};

LangKeywordType getKeywordType(LangSecondary name)
{
#define LANG_SECONDARIES
#define LANG_SECONDARY(sec, type)              \
{                                              \
    case LANGSEC_##sec: return LANGKWT_##type; \
}
    switch (name)
    {
       #include "Lists/Language_keywords.hpp"
       default: myThrow(ERR_STD);
    }
#undef LANG_SECONDARY
#undef LANG_SECONDARIES
};

const char* langKeywordsStrings(LangKeyword kwd)
{
    static char keywordsStrings[LANGKWD_KWDQT][MAX_LINE_LEN] = {};
    static bool isCalled = false;
    if (!isCalled)
    {
        fillLangKeywordsStrings(keywordsStrings);
#ifdef PRINT_KEYWORDS_DEBUG
        for (unsigned i = 0; i < LANGKWD_KWDQT; i++)
        {
           printf("%s\n", keywordsStrings[i]);
        }
#endif //PRINT_KEYWORDS_DEBUG
        isCalled = true;
    }
    assert(kwd <  LANGKWD_KWDQT);
    assert(kwd >= 0            );
    return keywordsStrings[kwd];
}

const char* langMathOperatorsStrings(LangMathOperators op)
{
    static const char* mathOperatorsStrings[] =
    {
    #define LANG_MATH_OPERATORS

    #define LANG_MATH_OP(name, priority) #name,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_MATH_OP

    #undef LANG_MATH_OPERATORS
    };
    assert(op <  LANGMATH_MATHQT);
    assert(op >= 0              );
    return mathOperatorsStrings[op];
}

const char* langSecondaryStrings(LangSecondary sec)
{
    static const char* secondaryStrings[] =
    {
    #define LANG_SECONDARIES

    #define LANG_SECONDARY(name, type) #name,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_SECONDARY

    #undef LANG_SECONDARIES
    };
    assert(sec <  LANGSEC_SECQT);
    assert(sec >= 0            );
    return secondaryStrings[sec];
}

const char* langKeywordsTypesStrings(LangKeywordType type)
{
    static const char* keywordsTypesStrings[] =
    {
        #define LANG_TYPES

        #define LANG_TYPE(type) #type,

        #include "Lists/Language_keywords.hpp"

        #undef LANG_TYPE

        #undef LANG_TYPES
    };
    assert(type <  LANGKWT_KWTQT);
    assert(type >= 0            );
    return keywordsTypesStrings[type];
}

static void fillLangKeywordsStrings(char keywordsStrings[LANGKWD_KWDQT][MAX_LINE_LEN])
{
    //Open config file
    FILE*  configFile = NULL;
    FOPEN( configFile = fopen("mylangconfig.cfg", "r") );
    //Allocate memory for char buffer
    charBuf_t txt = {};
    //Put text from file to buffer
    streamToBuf(&txt, configFile);
    //Divide buffer to lines
    if (strDiv(&txt))
    {
        printf("\nError dividing buffer to lines\n");
        myThrow(ERR_STD);
    }
#ifdef CHECK_NUMBER_OF_STRINGS
    assert(txt.strValid);
    //Check number of keyword's synonyms set
    if ( txt.strQuant != LANGKWD_KWDQT )
    {
        printf("\nError: wrong number of keywords(%u) is set(%u)\n",
                LANGKWD_KWDQT, txt.strQuant);
        myThrow(ERR_STD);
    }
#endif
    int lineCounter = -1;
    int keywordCounter = 0;

#define LANG_KEYWORD( keyword, keyword_type )                        \
{                                                                    \
    while (!(txt.strings[++lineCounter].len));                       \
    char kwdName[MAX_LINE_LEN] = {};                                 \
    assert(MAX_LINE_LEN == 256);                                     \
    int strLen = -1;                                                 \
    sscanf(txt.strings[lineCounter].str,                             \
           "%255[a-zA-Z]%*255[ =]%255[a-zA-Z_]%n",                   \
           kwdName,                                                  \
           keywordsStrings[keywordCounter],                          \
           &strLen);                                                 \
                                                                     \
    if ((strLen == -1) || (strLen != txt.strings[lineCounter].len))  \
    {                                                                \
        printf("\nError: wrong keyword synonym(not an identifier)"   \
               " or extra chars"                                     \
               "(even spaces after synonym are prohibited)\n");      \
        prStringLen(txt.strings[lineCounter].str);                   \
        prStringLen(kwdName);                                        \
        prStringLen(keywordsStrings[keywordCounter]);                \
        myThrow(ERR_STD);                                            \
    }                                                                \
                                                                     \
    if (strcasecmp(kwdName, #keyword))                               \
    {                                                                \
        printf("\nError: wrong keyword name or keywords' order\n");  \
        myThrow(ERR_STD);                                            \
    };                                                               \
    keywordCounter++;                                                \
}

#define LANG_KEYWORDS
    #include "Lists/Language_keywords.hpp"
#undef LANG_KEYWORDS

#undef LANG_KEYWORD



}
