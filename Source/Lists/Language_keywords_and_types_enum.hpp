#ifndef LANG_KEYWDS_AND_TYPES_HPP_INCLUDED
#define LANG_KEYWDS_AND_TYPES_HPP_INCLUDED

enum LangKeyword 
{
    #define LANG_KEYWORDS
    
    #define LANG_KEYWORD(keywd, type) LANGKWD_##keywd,
    #include "Lists/Language_keywords.hpp"
    #undef LANG_KEYWORD
    
    #undef LANG_KEYWORDS
};

enum LangKeywordType
{
    #define LANG_TYPES
    
    #define LANG_TYPE(type) LANGKWT_##type,
    #include "Lists/Language_keywords.hpp"
    #undef LANG_TYPE
    
    #undef LANG_TYPES
};

extern const char* LangKeywordsStrings[];

extern const char* LangKeywordsTypesStrings[];

#endif
