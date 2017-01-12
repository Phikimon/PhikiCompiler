#ifndef COMMON_TREE_HPP_INCLUDED
#define COMMON_TREE_HPP_INCLUDED
#include <cstddef>
#include "Lists/Language_keywords_and_types_enum.hpp"

enum TREEERR_T
{
    TREEERR_UNINIT            =   1,   //Uninitializer tree
    TREEERR_NOERR             =   0,   //Correct tree
    TREEERR_WRCHRCONFSTR      =  -1,   //Unknown char in configuration string
    TREEERR_WRCONFSTR         =  -2,   //Wrong configuration string
    TREEERR_WRWHRPAR          =  -3,   //Wrong 'where' parameter is passed
    TREEERR_UNKTYPE           =  -4,   //Unknown type is passed to the node constructor
    TREEERR_UNKVISMD          =  -5,   //Unknow visiting mode passed
    TREEERR_TOOMANYDUMPS      =  -6,   //< Only one dump at once is supported
};

enum TreeVisitMode
{
    prefixMode,
    infixMode,
    postfixMode,
};

union TreeNodeData
{
    double number;
    size_t kwdNum;
    char*  idName;
    TreeNodeData();
    TreeNodeData(double num);
    TreeNodeData(char*  name);
    TreeNodeData(size_t num);
};

#endif
