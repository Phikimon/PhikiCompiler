#ifndef TREES_H_INCLUDED
#define TREES_H_INCLUDED

#include "Common/Common.hpp"
#include "Lists/Language_keywords_and_types_enum.hpp"
Govnocode
extern FILE* treeDumpFileForVisitor;
extern FILE* mathDumpFile;

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

enum nodeCtorMode 
{
    addNodeUp = 0,
    addNodeLeft = -1,
    addNodeRight = 1,
    addNodeInit = -2,
    fillData = -3,
};

enum treeVisitMode 
{
    prefixMode,
    infixMode,
    postfixMode,
};

enum treeMathOpType 
{
    #define LANG_MATH_OPERATORS
    
    #define LANG_MATH_OP(name, char, prior) \
    treeMath_##name = char,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_MATH_OP
    
    #undef LANG_MATH_OPERATORS
};

enum treeMathOpPrior 
{
    #define LANG_MATH_OPERATORS
    
    #define LANG_MATH_OP(name, char, prior) \
    treeMathPrior_##name = prior,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_MATH_OP
    
    #undef LANG_MATH_OPERATORS
};

enum treeNodeDataType 
{
    #define LANG_TYPES
    
    #define LANG_TYPE(name) \
    tree_##name,
    #include "Lists/Language_keywords.hpp"
    
    #undef LANG_TYPES
};

struct treeNode_t 
{ // double-connected tree node

    void*       data;
    treeNodeDataType type;
    
    treeNode_t* uppr;
    treeNode_t* left;
    treeNode_t* rght;
    
};

/**
    Node constructor. 
 */
treeNode_t* _nodeRetCtor(treeNodeDataType type, void* data = NULL, treeNode_t* uppr = NULL, treeNode_t* left = NULL, treeNode_t* rght = NULL);
#define nodeRetCtor(type, data, uppr, left, rght) _nodeRetCtor(type, (void*)&(data), uppr, left, rght)

/**
 Math optimizations
 */
void checkSmthPlusZero(treeNode_t** treeToChange);
void checkSmthMultZero(treeNode_t** treeToChange);
void checkSmthMultOne(treeNode_t** treeToChange);
void calculateConstant(treeNode_t** treeToChange);
void _mathTreeOptimize(treeNode_t** nodeToChange);
void mathTreeOptimize(treeNode_t** treeToChange);


/**
 * Operators that adds new node to tree and concatenates node1 and node2 via new node
 */
treeNode_t operator + (treeNode_t &node1, treeNode_t &node2);
treeNode_t operator - (treeNode_t &node1, treeNode_t &node2);
treeNode_t operator * (treeNode_t &node1, treeNode_t &node2);
treeNode_t operator / (treeNode_t &node1, treeNode_t &node2);
treeNode_t operator ^ (treeNode_t &node1, treeNode_t &node2);

/**
 Private function. Node constructor. 
 */
void _nodeCtor(treeNode_t* newNode, treeNodeDataType type, void* data = NULL, treeNode_t* uppr = NULL, treeNode_t* left = NULL, treeNode_t* rght = NULL);
#define nodeCtor(newNode, type, data, uppr, left, rght) \
{ \
    void* temp = &data; \
    _nodeCtor(newNode, type, temp, uppr, left, rght); \
}

/**
 Dumps node data to stream 'stream'
 */
void printNodeData(treeNode_t* node, FILE* stream = stdout);

/**
 Print information about node on screen
 */
void _dumbDump(treeNode_t* node, char nodeName[MAX_LINE_LEN] = "name");
#define dumbDump(node) _dumbDump(node, #node)

/**
 Returns differential of tree 
 */
treeNode_t* treeDiffer(treeNode_t* node);

/**
 Print information about all the tree nodes to the screen
 */
void _dumbTreeDump(treeNode_t* tree, char nodeName[MAX_LINE_LEN] = "name");
#define dumbTreeDump(node) _dumbTreeDump(node, #node)    

/**
  Redispatch Upper pointers in tree
*/
void redispatchUppr(treeNode_t* tree);

/**
  Returns if the tree is a numeric constant or it contains variables
  */
bool isTreeConst(treeNode_t* node);

/**
 Constructor for tree. Takes configuration string that describes tree structure
*/
void diffTreector();
 
/**
 Destructor for tree. Can be used for subtree
*/
void treedtor(treeNode_t* node);

/**
 subtree verificator and dump
*/
void treeDump(treeNode_t* tree, void dumpFunc(treeNode_t* node));

/**
 Copies all the data from one node to another
 */
int nodecpy();

/**
 Returns copy of tree
 */
treeNode_t* treeCopy(treeNode_t* tree);

/**
 Syntax tree simple dump(dot)
 */
void syntDump(treeNode_t* node);

/**
 Dump the whole tree
 */
void dumpAll(treeNode_t* tree);

/**
 One-node dump
 */
void nodeDump(treeNode_t* node);

/**
 Add equation to tex dump
 */
void addMathDump(treeNode_t* tree, bool addStroke = false);
 
/**
 Function to compile and show TexFile 
 */
void viewTexFile();

/**
 Apply optimizations to tree
 */
void mathTreeOptimize(treeNode_t* tree);

/**
 Returns if two trees are similar
 */
bool treecmp(treeNode_t* tree1, treeNode_t* tree2);

//PRIVATE:

/**
 Private function. Redispatch upper pointers in node
 */
void _redispatchUppr(treeNode_t* node);

/**
 Private function. Add common data to the texDumpFile
 */
void initTexFile();

/**
 Private function. Recursively optimize tree
 */
void _mathTreeOptimize(treeNode_t* node);

/**
 Private function. Recursively calls itself and dumps each node
 */
void _mathTreeDump(treeNode_t* node);

/**
 Private function. Return priority of mathematical operation from treeMathOpPrior enum
 */
int mathOpPrior(unsigned mathOp);
    
/**
 Private function. Node destructor. 
 */
void nodeDtor(treeNode_t* node);

/**
 Private function. Returns if passed operator is a function
 */
bool isTreeFunc(unsigned mathOp);

/**
 Private function. Recursively copies tree
 */
void _treecpy(treeNode_t** newNode, treeNode_t* node);

/**
 Recursively visit all tree nodes and perform function 'act'. It catches any 'throw' in func that is performed and prints small dump.
 Returns -1 if error occures and 0 otherwise
 */
 int treeVisitor(treeNode_t* node, void act(treeNode_t* node), treeVisitMode mode);
 
/**
 Private function that visits all tree nodes. Is called by treeVisitor
 */
 void _treeRecVisit(treeNode_t* node, void act(treeNode_t* node), treeVisitMode mode);
 
//UNIT TESTS:
/**
Unit tests for tree
*/
void ut_tree(bool verbose = false);

#endif
