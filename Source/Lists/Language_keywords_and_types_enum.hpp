#ifndef LANG_KEYWDS_AND_TYPES_HPP_INCLUDED
#define LANG_KEYWDS_AND_TYPES_HPP_INCLUDED

#include "Common/Common.hpp"

enum LangErrors
{
    LANGERR_OK = 0, //< No error occured
    LANGERR_UNKTYPE //< Unknown data type passed to the
                    //  'getDataType'
};

enum LangKeyword
{
    #define LANG_KEYWORDS

    #define LANG_KEYWORD(keywd, type) LANGKWD_##keywd,
    #include "Lists/Language_keywords.hpp"
    #undef LANG_KEYWORD

    #undef LANG_KEYWORDS
    LANGKWD_KWDQT,
};

enum LangSecondary
{
    #define LANG_SECONDARIES

    #define LANG_SECONDARY(name, type) LANGSEC_##name,
    #include "Lists/Language_keywords.hpp"
    #undef LANG_SECONDARY

    #undef LANG_SECONDARIES
    LANGSEC_SECQT,
};

enum LangMathOperators
{
    #define LANG_MATH_OPERATORS

    #define LANG_MATH_OP(name, priority) LANGMATH_##name,
    #include "Lists/Language_keywords.hpp"
    #undef LANG_MATH_OP

    #undef LANG_MATH_OPERATORS
    LANGMATH_MATHQT,
};

enum LangKeywordType
{
        #define LANG_TYPES

        #define LANG_TYPE(type) LANGKWT_##type,
        #include "Lists/Language_keywords.hpp"
        #undef LANG_TYPE

        #undef LANG_TYPES
        LANGKWT_KWTQT,
};

enum DataType
{
    TYPE_DOUBLE,
    TYPE_CHAR_PTR,
    TYPE_SIZE_T,
    TYPE_NIL
};

/**
 Returns information about data type(double, char*, size_t),
 according to the keyword type(LANGKWT_xxx)
 */
DataType getDataType(LangKeywordType type);

const char* langKeywordsStrings     (LangKeyword kwd);
const char* langMathOperatorsStrings(LangMathOperators op);
const char* langSecondaryStrings    (LangSecondary sec);
const char* langKeywordsTypesStrings(LangKeywordType type);

LangKeywordType getKeywordType(LangKeyword   kwd);

LangKeywordType getKeywordType(LangSecondary name);

#endif
