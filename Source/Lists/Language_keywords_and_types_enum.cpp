#include "Language_keywords_and_types_enum.hpp"

const char* LangKeywordsStrings[] = 
{
    #define LANG_KEYWORDS
    
    #define LANG_KEYWORD(keywd, type) #keywd,
    
    #include "Lists/Language_keywords.hpp"
    
    #undef LANG_KEYWORD
    
    #undef LANG_KEYWORDS
};

const char* LangKeywordsTypesStrings[] =
{
    #define LANG_TYPES 
    
    #define LANG_TYPE(type) #type,
    
    #include "Lists/Language_keywords.hpp"
    
    #undef LANG_TYPE
    
    #undef LANG_TYPES
};
