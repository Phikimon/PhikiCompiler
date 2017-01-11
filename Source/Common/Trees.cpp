#include "Trees.hpp"

FILE* treeDumpFileForVisitor = NULL;
FILE* mathDumpFile = NULL;

void _treecpy(treeNode_t** newNode, treeNode_t* node)
{
    *newNode = (treeNode_t*)calloc(1, sizeof(**newNode));
    **newNode = (*node);
    
    switch (node->type)
        {
            case tree_id:
            case tree_string: 
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(strlen((char*)node->data) + 1, sizeof(char));
                                  strcpy((char*)(*newNode)->data, (char*)node->data);
                              }; break;
            case tree_double: 
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(1, sizeof(double));
                                  *(double*)(*newNode)->data = *(double*)node->data;
                              }; break;
            case tree_num_bracket:
            case tree_delim:
            case tree_operator_bracket:
            case tree_conditional:
            case tree_io:
            case tree_new_program:
            case tree_new_function:
            case tree_new_variable:
            case tree_return:
            case tree_assign:
            case tree_loop:
            case tree_mathOperator:
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(1, sizeof(unsigned));
                                  *(unsigned*)(*newNode)->data = *(unsigned*)node->data;
                              }; break;
            case tree_nil:
                              {
                                  (*newNode)->data = NULL;
                              }; break;
            default: 
                              {
                                    printf("\nError: Unknown type of node == '%i' is passed to the 'dumbDump' method\n", node->type); 
                                    throw(TREEERR_UNKTYPE);
                              }; break;
        }
    
    if ((*newNode)->left)
        _treecpy(&(*newNode)->left, node->left);
    if ((*newNode)->rght)
        _treecpy(&(*newNode)->rght, node->rght);
}

treeNode_t* treeCopy(treeNode_t* tree)
{
    _assert(tree);
    
    treeNode_t* newTree = NULL;
    _treecpy(&newTree, tree);
    return newTree;
}

void treedtor(treeNode_t* node)
{
    treeVisitor(node, nodeDtor, postfixMode);
}

void treeDump(treeNode_t* tree, void dumpFunc(treeNode_t* node))
{
    assert(tree);

    static int dumpNumber = 0;
    dumpNumber++;
    
    treeDumpFileForVisitor = NULL;
    
    char* dumpFileName = (char*)calloc(strlen("./temp/treeDump.gv") + 3 + 1, sizeof(*dumpFileName));
    sprintf(dumpFileName, "./temp/treeDump%03i.gv", dumpNumber);
    
    if (!(treeDumpFileForVisitor = fopen(dumpFileName, "w"))) throw(ERR_OPFILEW);
    
    fprintf(treeDumpFileForVisitor, "digraph G {\n"
                                    "graph [ dpi = 300 ]"); 
    
    treeVisitor(tree, dumpFunc/*nodeDump/syntDump*/, prefixMode);
    
    fprintf(treeDumpFileForVisitor, "}\n"); 
    
    fclose(treeDumpFileForVisitor); 
    
    char command[100] = "";
    
    sprintf(command, "xdot %s", dumpFileName);
    system(command);
    
    char pngFileName[MAX_LINE_LEN] = "";
    sprintf(pngFileName, "./temp/treeDump%03i.png", dumpNumber);
    sprintf(command, "dot %s -Tpng -o %s", dumpFileName, pngFileName);
    system(command);
    
    free(dumpFileName);
}

void printNodeData(treeNode_t* node, FILE* stream)
{
    assert(node); assert(stream);
    switch (node->type)
    {
                case LANGKWT_double:           {
                                                fprintf(stream, "%lg (%s)", *(double*)node->data, LangKeywordsTypesStrings[node->type]);
                                            }; break;
                case LANGKWT_mathOperator:
                case LANGKWT_num_bracket:
                case LANGKWT_delim:            {
                                                fprintf(stream, "'%c' (%s)", *(unsigned*)node->data, LangKeywordsTypesStrings[node->type]);
                                            }; break;
                                            
                case LANGKWT_operator_bracket:
                case LANGKWT_conditional:
                case LANGKWT_io:
                case LANGKWT_new_program:
                case LANGKWT_new_function:
                case LANGKWT_new_variable:
                case LANGKWT_return:
                case LANGKWT_assign:
                case LANGKWT_loop:             {
                                                fprintf(stream, "'%s' ( '%s' )", LangKeywordsStrings[*(unsigned*)node->data], LangKeywordsTypesStrings[node->type]);
                                            }; break;
                case LANGKWT_function:
                case LANGKWT_id:               {
                                                fprintf(stream, "'%s' ( '%s' )", *(char**)node->data, LangKeywordsTypesStrings[node->type]);
                                            }; break;
                case LANGKWT_nil:              {
                                                fprintf(stream, "ERROR");
                                            }; break;
                                            
                default:                    {
                                                prln();
                                                printf("Unknown token type in tokenListDump");
                                                throw (TREEERR_UNKTYPE);
                                            }
    }
}
#define dotTreeDumpConnectNodes() \
    if (!node->left) \
    { \
        fprintf(treeDumpFileForVisitor, "treeNode_%p_left [label=\"(nil)\"]\n", node); \
        fprintf(treeDumpFileForVisitor, "treeNode_%p -> treeNode_%p_left\n", node, node); \
    } \
    else \
        fprintf(treeDumpFileForVisitor, "treeNode_%p -> treeNode_%p\n", node, node->left); \
     \
    if (!node->rght) \
    { \
        fprintf(treeDumpFileForVisitor, "treeNode_%p_rght [label=\"(nil)\"]\n", node); \
        fprintf(treeDumpFileForVisitor, "treeNode_%p -> treeNode_%p_rght\n", node, node); \
    } \
    else \
        fprintf(treeDumpFileForVisitor, "treeNode_%p -> treeNode_%p\n", node, node->rght);

void nodeDump(treeNode_t* node)
{
    assert(treeDumpFileForVisitor); assert(node); 
    #define shortPtr(ptr) (ptr ? ((long unsigned)(ptr) & (0xFFFF)) : NULL)
    fprintf(treeDumpFileForVisitor, "treeNode_%p [label=\"node[%p]\\l"
                                                            "{\\l"
                                                            "\tuppr = %p;\\l"
                                                            "\tleft = %p;\\l"
                                                            "\trght = %p;\\l"
                                                            "\tdata = %p;\\l",
                                                            node, shortPtr(node),
                                                            shortPtr(node->uppr),
                                                            shortPtr(node->left),
                                                            shortPtr(node->rght),
                                                            shortPtr(node->data));
    printf("\tdataContent = ");
    printNodeData(node, treeDumpFileForVisitor);

    fprintf(treeDumpFileForVisitor, "\\l}\\l\"]\n");
    dotTreeDumpConnectNodes();

}

void syntDump(treeNode_t* node)
{
    assert(treeDumpFileForVisitor); assert(node); 
    #define shortPtr(ptr) (ptr ? ((long unsigned)(ptr) & (0xFFFF)) : NULL)
    fprintf(treeDumpFileForVisitor, "treeNode_%p [label=\"", node);

    printNodeData(node, treeDumpFileForVisitor);

    fprintf(treeDumpFileForVisitor, "\\l\"]\n");
    
    dotTreeDumpConnectNodes();
}
#undef dotTreeDumpConnectNodes

void dumpAll(treeNode_t* tree)
{
    while (tree->uppr)
    {
        tree = tree->uppr;
    }
    addMathDump(tree);
}

void _dumbTreeDump(treeNode_t* tree, char nodeName[MAX_LINE_LEN])
{
    dumbDump(tree);
    if (tree->left)
        dumbTreeDump(tree->left);
    if (tree->rght)
        dumbTreeDump(tree->rght);
}

void _dumbDump(treeNode_t* node, char nodeName[MAX_LINE_LEN])
{
    assert(node);
    printf("Node \"%s\"[%p] \n{\n", nodeName, node);
    printf("\tuppr = %p\n", node->uppr);
    printf("\tleft = %p\n", node->left);
    printf("\trght = %p\n", node->rght);
    printNodeData(node);
    printf("}\n");
    
}

treeNode_t* _nodeRetCtor(treeNodeDataType type, void* data, treeNode_t* uppr, treeNode_t* left, treeNode_t* rght)
{
    treeNode_t* node = (treeNode_t*)calloc(1, sizeof(*node));
    assert((left != rght) || (!left)); assert((left != uppr) || (!left)); assert((rght != uppr) || (!rght));
    _nodeCtor(node, type, data, uppr, left, rght);
    
    return node;
}

//PRIVATE
void _nodeCtor(treeNode_t* newNode, treeNodeDataType type, void* data, treeNode_t* uppr, treeNode_t* left, treeNode_t* rght)
{
    assert(newNode);
    //Fill data field
    switch (type)
    {
        case tree_id:
        case tree_string: 
                          {
                                assert(data);
                                
                                char* temp = (char*)calloc(strlen(*(char**)data) + 1, sizeof(char));
                                assert(temp);
                                strcpy(temp, *(char**)data);
                                
                                newNode->data = calloc(1, sizeof(char*));
                                *(char**)newNode->data = temp;
                          }; break;
        case tree_double: 
                          {
                                assert(data);
                                double* temp = (double*)calloc(1, sizeof(double));
                                assert(temp);
                                *temp = *(double*)data;
                                
                                newNode->data = temp;
                          }; break;
        case tree_num_bracket:
        case tree_delim:
        case tree_operator_bracket:
        case tree_conditional:
        case tree_io:
        case tree_new_program:
        case tree_new_function:
        case tree_new_variable:
        case tree_return:
        case tree_assign:
        case tree_loop:
        case tree_mathOperator:
                          {
                                assert(data);
                                unsigned* temp = (unsigned*)calloc(1, sizeof(unsigned));
                                
                                *temp = *(unsigned*)data;
                                
                                newNode->data = temp;
                          }; break;
        case tree_nil:
                          {
                                newNode->data = NULL;
                                newNode->type = tree_nil;
                          }; break;
        default: 
                          {
                                printf("\nError: Unknown type of node == '%i' is passed to the 'nodeCtor method\n", type); 
                                throw(TREEERR_UNKTYPE);
                          }; break;
    
    }
    newNode->left = left;
    if (left)
    {
        left->uppr = newNode;
    }
    newNode->rght = rght;
    if (rght)
    {
        rght->uppr = newNode;
    }
    newNode->uppr = uppr;
    newNode->type = type;
}

void nodeDtor(treeNode_t* node)
{
    assert(node);
    if ((node->type != tree_nil) && (node->type != tree_id) && (node->type != tree_string) && (node->type != tree_function))
    {
        free(node->data);
    } else
    if ( (node->type == tree_id) || (node->type == tree_string) || (node->type == tree_function) )
    {
        free(*(char**)node->data);
        free(node->data);
    }
    node->type = tree_nil;
    
    node->left = node->rght = node->uppr = NULL;
    free(node);
}

void _treeRecVisit(treeNode_t* node, void act(treeNode_t* node), treeVisitMode mode)
{
    assert(node);
    #define _treeRecVisit(node1, act, mode) {if (node1) _treeRecVisit(node1, act, mode); }
    assert(node != node->left);
    assert(node != node->rght);
    
            switch (mode)                                                     
            {                                                                 
                case prefixMode: {                                            
                                    act(node);
                                    _treeRecVisit(node->left, act, mode);
                                    _treeRecVisit(node->rght, act, mode);
                                 }; break;
                case infixMode:  {
                                    _treeRecVisit(node->left, act, mode);
                                    act(node);
                                    _treeRecVisit(node->rght, act, mode);
                                 }; break;
                case postfixMode:{
                                    _treeRecVisit(node->left, act, mode);
                                    _treeRecVisit(node->rght, act, mode);
                                    act(node);
                                 }; break;
                default: prln(); throw(TREEERR_UNKVISMD);
            }  
            
    #undef _treeRecVisit
}

int treeVisitor(treeNode_t* node, void act(treeNode_t* node), treeVisitMode mode)
{
    if (!node)
        return -1;
    try
    {
        _treeRecVisit(node, act, mode);
    } catch(...)
    {
        prln();
        throw(ERR_COMMON);
    }
    
    return 0;
}

void redispatchUppr(treeNode_t* tree)
{
    assert(tree);
    treeVisitor(tree, _redispatchUppr, prefixMode);
}

void _redispatchUppr(treeNode_t* node)
{
    assert(node);
    if (node->left)
    {
        node->left->uppr = node;
    };
    if (node->rght)
    {
        node->rght->uppr = node;
    }
}

bool treecmp(treeNode_t* tree1, treeNode_t* tree2)
{
    if ( (tree1->type != tree2->type) || ( (tree1->left) && (!tree2->left) ) || ( (tree1->rght) && (!tree2->rght) ) )
        return false;
        
    bool equal = false;
    
    switch (tree1->type)
        {
            case tree_id:
            case tree_string: 
                              {
                                  assert(tree1->data); assert(tree2->data);
                                  equal = (! strcmp((char*)tree1->data, (char*)tree2->data) );
                              }; break;
            case tree_double: 
                              {
                                  assert(tree1->data); assert(tree2->data);
                                  equal = D_EQ(*(double*)tree1->data, *(double*)tree2->data);
                              }; break;
            case tree_num_bracket:
            case tree_delim:
            case tree_operator_bracket:
            case tree_conditional:
            case tree_io:
            case tree_new_program:
            case tree_new_function:
            case tree_new_variable:
            case tree_return:
            case tree_assign:
            case tree_loop:
            case tree_mathOperator:
                              {
                                  assert(tree1->data); assert(tree2->data);
                                  equal = (*(unsigned*)tree1->data == *(unsigned*)tree2->data);
                              }; break;
            case tree_nil:
                              {
                                  equal = true;
                              }; break;
            default: 
                              {
                                    printf("\nError: Unknown type of node == '%i' is passed to the '%s' method\n", tree1->type, __func__); 
                                    throw(TREEERR_UNKTYPE);
                              }; break;
        }
    if (tree1->left)
        equal = equal && treecmp(tree1->left, tree2->left);
    if (tree1->rght)
        equal = equal && treecmp(tree1->rght, tree2->rght);
    
   return equal;
}

void checkSmthPlusZero(treeNode_t** treeToChange)
{
    assert(treeToChange); assert(*treeToChange);
    treeNode_t* tree = *treeToChange;
    
    assert(tree);
    if ( (tree->type == tree_mathOperator) && (*(unsigned*)tree->data == '+') )
    {
        assert(tree->left); assert(tree->rght);
        if ( (tree->left->type == tree_double) && D_EQ(*(double*)tree->left->data, 0) )
        {
            nodeDtor(tree->left);
            
            tree->rght->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->rght;
                } else
                {
                    tree->uppr->rght = tree->rght;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->rght;
            nodeDtor(toRemove);
            dumpAll(tree);
        } else
        if ( (tree->rght->type == tree_double) && D_EQ(*(double*)tree->rght->data, 0) )
        {
            nodeDtor(tree->rght);
            
            tree->left->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->left;
                } else
                {
                    tree->uppr->rght = tree->left;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->left;

            nodeDtor(toRemove);
            dumpAll(tree);
        }
    }
    *treeToChange = tree;
}

void checkSmthMultZero(treeNode_t** treeToChange)
{
    assert(treeToChange); assert(*treeToChange);
    treeNode_t* tree = *treeToChange;
    
    assert(tree);
    if ( (tree->type == tree_mathOperator) && (*(unsigned*)tree->data == '*') )
    {
        assert(tree->left); assert(tree->rght);
        if ( (tree->left->type == tree_double) && D_EQ(*(double*)tree->left->data, 0) )
        {
            nodeDtor(tree->rght);
            
            tree->left->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->left;
                } else
                {
                    tree->uppr->rght = tree->left;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->left;
            nodeDtor(toRemove);
            dumpAll(tree);
        } else
        if ( (tree->rght->type == tree_double) && (D_EQ(*(double*)tree->rght->data, 0)) )
        {
            nodeDtor(tree->left);
            
            tree->rght->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->rght;
                } else
                {
                    tree->uppr->rght = tree->rght;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->rght;
            nodeDtor(toRemove);
            dumpAll(tree);
        }
    }
    
    *treeToChange = tree;
}

void checkSmthMultOne(treeNode_t** treeToChange)
{
    assert(treeToChange); assert(*treeToChange);
    treeNode_t* tree = *treeToChange;
    
    assert(tree);
    if ( (tree->type == tree_mathOperator) && ((*(unsigned*)tree->data == '*') || (*(unsigned*)tree->data == '^')) )
    {
        assert(tree->left); assert(tree->rght);
        if ( (tree->left->type == tree_double) && D_EQ(*(double*)tree->left->data, 1) )
        {
            nodeDtor(tree->left);
            
            tree->rght->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->rght;
                } else
                {
                    tree->uppr->rght = tree->rght;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->rght;
            nodeDtor(toRemove);
            dumpAll(tree);
        } else
        if ( (tree->rght->type == tree_double) && D_EQ(*(double*)tree->rght->data, 1) )
        {
            nodeDtor(tree->rght);
            
            tree->left->uppr = tree->uppr;
            if (tree->uppr)
            {
                bool isLeftBranch = (tree == tree->uppr->left);
                if (isLeftBranch)
                {
                    tree->uppr->left = tree->left;
                } else
                {
                    tree->uppr->rght = tree->left;
                }
            }
            treeNode_t* toRemove = tree;
            tree = tree->left;

            nodeDtor(toRemove);
            dumpAll(tree);
        }
    

    }
    *treeToChange = tree;
}

void calculateConstant(treeNode_t** treeToChange)
{  
    assert(treeToChange); assert(*treeToChange);
    treeNode_t* tree = *treeToChange;
    
    if ((tree->type != tree_mathOperator) || (!isTreeConst(tree))) 
        return;
    
    if ( ((tree->left) && (tree->left->type != tree_double)) || ((tree->rght) && (tree->rght->type != tree_double)) )
        return;
        
    assert(tree->left);
    assert(tree->data);
    
    double resVal = 0;
    treeNode_t* result = NULL;
    
    #define val(data) 
    #define lval (*(double*)(tree->left->data))
    #define rval (tree->rght ? (*(double*)tree->rght->data) : 0)
    #define FILLRES() { result = nodeRetCtor(tree_double, resVal, NULL, NULL, NULL); }
    switch (*(unsigned*)tree->data)
    {
        case '*': {
                        resVal = lval * rval;
                        //printf("%lg = %lg * %lg\n", resVal, lval, rval);
                        FILLRES();
                  } break;
        case '/': {
                        resVal = lval / rval;
                        //printf("%lg = %lg / %lg\n", resVal, lval, rval);
                        FILLRES();
                  }; break;
        case '+': {
                        resVal = lval + rval;
                        //printf("%lg = %lg + %lg\n", resVal, lval, rval);
                        FILLRES();
                  }; break;
        case '-': {
                        resVal = lval - rval;
                        //printf("%lg = %lg - %lg\n", resVal, lval, rval);
                        FILLRES();
                  }; break;
        case '^': {
                        resVal = pow(lval, rval);
                        //printf("%lg = %lg ^ %lg\n", resVal, lval, rval);
                        FILLRES();
                  }; break;
        case 'l': {
                        resVal = log(lval);
                        //printf("%lg = ln(%lg)\n", resVal, lval);
                        FILLRES();
                  }; break;
                  
        default: {
                        prln();
                        printf("\nError: Unknown type of mathOp == '%i' is passed to the '%s'\n", tree->type, __func__); 
                        throw(ERR_COMMON);
                 }; break;
    };
    #undef val
    #undef lval
    #undef rval
    #undef FILLRES
                        
    result->uppr = tree->uppr;
    if (tree->uppr)
    {
        bool isLeftBranch = (tree == tree->uppr->left);
        if (isLeftBranch)
        {
            tree->uppr->left = result;
        } else
        {
            tree->uppr->rght = result;
        }
    }
    treedtor(tree);
    tree = result;
    
    dumpAll(tree);
    
    *treeToChange = tree;
}

void _mathTreeOptimize(treeNode_t** nodeToChange)
{
    assert(nodeToChange); assert(*nodeToChange);
    calculateConstant(nodeToChange);
    checkSmthPlusZero(nodeToChange);
    checkSmthMultZero(nodeToChange);
    checkSmthMultOne(nodeToChange);
}

void mathTreeOptimize(treeNode_t** treeToChange)
{
    if ((*treeToChange)->left)
        mathTreeOptimize(&(*treeToChange)->left);
    if ((*treeToChange)->rght)
        mathTreeOptimize(&(*treeToChange)->rght);
    _mathTreeOptimize(treeToChange);
}

treeNode_t* treeDiffer(treeNode_t* node)
{
    assert(node);
    //If tree is totally constant, return 0
    if (isTreeConst(node))
    {
        double derivative = 0;
        return nodeRetCtor(tree_double, derivative, NULL, NULL, NULL);
    }
    //
    #define CALLOC_TREE(tree) treeNode_t* tree = (treeNode_t*)calloc(1, sizeof(*tree))
    CALLOC_TREE(result);

    switch (node->type)
    {
        case tree_double: 
                            {
                                assert(node->data);
                                double derivative = 0;
                                free(result);
                                result = nodeRetCtor(tree_double, derivative, NULL, NULL, NULL);
                            }; break;
        case tree_mathOperator:
                            {
                                assert(node->data);
                                
                                treeNode_t* leftBranch = treeCopy(node->left);
                                treeNode_t* rghtBranch = node->rght ? treeCopy(node->rght) : NULL; 
                                treeNode_t* leftDiffer = treeDiffer(node->left);
                                treeNode_t* rghtDiffer = node->rght ? treeDiffer(node->rght) : NULL;
                                
                                #define dL (*leftDiffer)
                                #define dR (*rghtDiffer)
                                #define L  (*leftBranch)
                                #define R  (*rghtBranch)
                                switch (*(unsigned*)node->data)
                                {
                                    case '*': {
                                                    CALLOC_TREE(augend1); CALLOC_TREE(augend2);
                                                    *augend1 = dL * R;
                                                    *augend2 = dR * L;
                                                    
                                                    *result = *augend1 + *augend2;
                                              } break;
                                    case '/': {
                                                    CALLOC_TREE(augend1); CALLOC_TREE(augend2);
                                                    *augend1 = dL * R;
                                                    *augend2 = dR * L;
                                                    
                                                    CALLOC_TREE(numerator); CALLOC_TREE(denominator);
                                                    *numerator = *augend1 - *augend2;
                                                    treeNode_t* RCpy1 = treeCopy(&R);
                                                    treeNode_t* RCpy2 = treeCopy(&R);
                                                    *denominator = *RCpy1 * *RCpy2;
                                                    
                                                    *result = *numerator / *denominator;
                                              }; break;
                                    case '+': {
                                                    *result = dL + dR;
                                              }; break;
                                    case '-': {
                                                    *result = dL - dR;
                                              }; break;
                                    case '^': {
                                                    assert( (isTreeConst(&L)) || (isTreeConst(&R)) );

                                                    if (isTreeConst(&L))     //(a^f(x))' = f(x)' * (a^f(x)) * ln(a) // f(x) =  R; a = L
                                                    {
                                                        prhello();
                                                        unsigned mathOperator = 'l';
                                                        treeNode_t* lnA = nodeRetCtor(tree_mathOperator, mathOperator, NULL, &L, NULL);
                                                        CALLOC_TREE(factor1);
                                                        *factor1 = dR * *lnA;
                                                        
                                                        CALLOC_TREE(A); CALLOC_TREE(factor2);
                                                        A = treeCopy(&L);
                                                        *factor2 = *A ^ R;
                                                        *result = *factor1 * *factor2;
                                                    } else //R=const        //(f(x)^a)' = a * f(x)' * (f(x))^(a-1)  // f(x) = L; a = R
                                                    {
                                                        treeNode_t* factor = treeCopy(&R);
                                                        
                                                        CALLOC_TREE(degree); CALLOC_TREE(mult); CALLOC_TREE(degreeMinusOne);
                                                        
                                                        double OneValue = 1;
                                                        treeNode_t* One = nodeRetCtor(tree_double, OneValue, NULL, NULL, NULL);
                                                        
                                                        *degreeMinusOne = R - *One;
                                                        
                                                        *degree = L ^ *degreeMinusOne;
                                                        *mult = *degree * *factor;
                                                        *result = dL * *mult;
                                                    }
                                              }; break;
                                    case 'l': { // ln(f(x))' = f'(x) / f(x)
                                                    *result = dL / L;
                                              }; break;
                                    default:  {
                                                    prln();
                                                    printf("\nError: Unknown type of mathOp == '%i' is passed to the '%s'\n", node->type, __func__); 
                                                    throw(ERR_COMMON);
                                              }; break;
                                };
                                #undef L
                                #undef R
                                #undef dL
                                #undef dR
                            }; break;
        case tree_id:
                            {
                                double derivative = 1;
                                result = nodeRetCtor(tree_double, derivative, NULL, NULL, NULL);
                            }; break;
        default: 
                            {
                                printf("\nError: Unknown type of node == '%i' is passed to the '%s' method\n", node->type, __func__); 
                                throw(TREEERR_UNKTYPE);
                            }; break;
    }
    redispatchUppr(result);
    return result; 
}


void initTexFile()
{
    //If called twice then throw
        static bool isAlreadyCalled = false;
        
        if (isAlreadyCalled)
        {
            prln();
            throw(TREEERR_TOOMANYDUMPS);//< Throw if more than one dump is called
        };
        
        isAlreadyCalled = true;
    //
    
    assert(!mathDumpFile);
        
    mathDumpFile = fopen("mathDump.tex", "w");
    if (!mathDumpFile) 
    {
        prln();
        throw(ERR_OPFILEW);
    }
        
    fprintf(mathDumpFile, 
    "\\documentclass{article}\n"
    "\\usepackage[a4paper,margin=6mm]{geometry}\n"
    "\\usepackage{amsmath}\n"
    "\\usepackage[utf8]{inputenc}"
    "\\usepackage[T2A]{fontenc}"
    "\\usepackage[russian]{babel}"
    "\\usepackage{hyperref}\n"
    "\\title{\\LaTeX\\  Derivation}\n"
    "\\author{Prof Phikimon}\n"
    "\\begin{document}\n"
    "\\fontsize{12}{12pt}\\selectfont\n"
    "\\maketitle\n"

    "\\begin{flushleft}\n"
    "Здравствуйте, товарищ \\%%USERNAME\\%%! \\\\\n"
    "Высылаю Вам свои скромные выкладки по обсуждаемому нами вчера вопросу. "
    "Удачно разобраться Вам в этой жести! \\\\"
    "\\end{flushleft}\n"
    "\\begin{center}\n"
    "\\fontsize{16}{16pt}\\selectfontХА-"
    "\\fontsize{24}{24pt}\\selectfontХА-"
    "\\fontsize{32}{32pt}\\selectfontХА!!! \\\\"
    "\\end{center}\n"
    "\\fontsize{16}{16pt}\\selectfont\n"
    
    "\\begin{center}\n"
    " Что же, приступим к преобразованиям:\n"
    "\\fontsize{8}{8pt}\\selectfont\n"
    "\\begin{equation}\n");
}

void viewTexFile()
{
    assert(mathDumpFile);
        
    fprintf(mathDumpFile, "\\end{equation}\n"
                          "\\fontsize{16}{16}\\selectfont\n"
                          "Спешу вас поздравить. Вы, конечно, не стали разбираться в выкладках и пропустили ошибку. Удачи на экзамене!"
                          "*И что, получается, я писал эти 1,5к строк дифференциатора и упростителя зря?... =(*"
    "\\end{center}\n\n\n\\end{document}");
    fclose(mathDumpFile);
        
    //Is to be switched manually
    if (1)
    {
        //Compile Tex File
        system("pdflatex ./mathDump.tex > dev\\null");
        
        //View Tex File
        system("evince ./mathDump.pdf");
        
    } else
    {
        //Or open texFile in the editor
        system("texmaker ./mathDump.tex");
    }
}

bool isTreeFunc(unsigned mathOp)
{
    return (mathOp == treeMath_ln);
}

int mathOpPrior(unsigned mathOp)
{
    switch (mathOp)
    {
    #define opPrior(a) return treeMathPrior_##a;
        case treeMath_ln:    opPrior(ln);
        case treeMath_mult:  opPrior(mult);
        case treeMath_div:   opPrior(div);
        case treeMath_plus:  opPrior(plus);
        case treeMath_minus: opPrior(minus);
        case treeMath_deg:   opPrior(deg);
        default:  {
                        printf("\nError: Unknown mathOp == '%c' is passed to the 'mathOpPrior' method.\n", mathOp);
                        throw(TREEERR_UNKTYPE);      
                  }
    #undef opPrior
    }
}

void _mathTreeDump(treeNode_t* node)
{
    assert(node);
        switch (node->type)
        {
            case tree_double: 
                              {
                                  assert(node->data);
                                  fprintf(mathDumpFile, "%lg", *(double*)node->data);
                              }; break;
            case tree_mathOperator:
                #define treeOp(a) *(unsigned*)(a)
                              {
                                assert(node->data);
                                switch (treeOp(node->data))
                                {
                                    case treeMath_ln: {
                                                            fprintf(mathDumpFile, "\\ln{(");
                                                            _mathTreeDump(node->left);
                                                            assert(!node->rght);
                                                            fprintf(mathDumpFile, ")}");
                                                      }; break;
                                    
                                    case treeMath_deg:
                                                          if ((node->rght->type == tree_double) && (*(double*)(node->rght->data) == 0.5))
                                                          {
                                                            fprintf(mathDumpFile, "\\sqrt{");
                                                            _mathTreeDump(node->left);
                                                            fprintf(mathDumpFile, "}");
                                                            break;
                                                          };
                                    case treeMath_minus:
                                    case treeMath_plus:
                                    case treeMath_div: 
                                    case treeMath_mult: {
                                                        assert(node->type == tree_mathOperator);
                                                        assert((!node->uppr) || (node->uppr->type == tree_mathOperator));
                                                        
                                                        if ( (node->uppr) && (!isTreeFunc(treeOp(node->uppr->data))) && 
                                                        ( mathOpPrior(treeOp(node->uppr->data)) < mathOpPrior(treeOp(node->data)) ) )
                                                        {
                                                            fprintf(mathDumpFile, "\\left(");
                                                            assert(node->left);
                                                            _mathTreeDump(node->left);
                                                            fprintf(mathDumpFile, "%c", treeOp(node->data));
                                                            assert(node->rght);
                                                            _mathTreeDump(node->rght);
                                                            fprintf(mathDumpFile, "\\right)");
                                                        } else
                                                        {
                                                            if ((treeOp(node->data) == treeMath_div))
                                                            {
                                                                fprintf(mathDumpFile, "\\frac{");
                                                                _mathTreeDump(node->left);
                                                                fprintf(mathDumpFile, "}");
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->rght);
                                                                fprintf(mathDumpFile, "}");
                                                            } else
                                                            {
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->left);
                                                                fprintf(mathDumpFile, "}");
                                                                fprintf(mathDumpFile, "%c", treeOp(node->data));
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->rght);
                                                                fprintf(mathDumpFile, "}");
                                                            }
                                                        }
                                                        }; break;
                                                    
                                    default:  {
                                                printf("\nError: Unknown mathOp == '%c' is passed to the '_mathTreeDump' method.\n", treeOp(node->data));
                                                throw(TREEERR_UNKTYPE);      
                                              }; break;
                                }
                              }; break;
                #undef treeOp
            case tree_id:
                              {
                                  fprintf(mathDumpFile, "x");
                              }; break;
            default: 
                              {
                                    prln();
                                    printf("\nError: Unknown type of node == '%i' is passed to the 'nodeDump' method.\n", node->type); 
                                    throw(TREEERR_UNKTYPE);
                              }; break;
        }
}

char* someBosh()
{
    static unsigned callNum = 0;
    callNum++;
    const unsigned dumpLimit = 15;
    
    if (callNum == dumpLimit)
        return "Дальнейшие переходы не вызовут вопросов даже у Тер-Крикорова, в связи с чем наше радио \"Фил-твои-шутки-уже-не-торт-FM\""
               "прекращает своё вещание";
    if (callNum > dumpLimit)
        return "";
        
    int toChoose = 0;
    
    if (callNum <= 12)
    {
        toChoose = callNum;
    } else
    {
        toChoose = rand() % 13;
    }
        
    
    switch (toChoose)
    {
        #define ADD_STRING(num, str) \
        case num: { return str; };
    ADD_STRING(0, "Я знаю, дети, вы хотите спать/ботать матан/не ботать матан, но мы с вами-таки добьём этот номер!")
    ADD_STRING(1, "Данный переход даже Беклемишеву очевиден:")
    ADD_STRING(2, "Откровенно говоря, тот, кто это писал(mr.\\%GODNAME\\%) списал этот пример из Демидовича и сам не до конца понял данный переход")
    ADD_STRING(3, "А в данном преобразовании намеренно допущена ошибка, чтобы вы попытались её найти(это не шутка!)(шутка)")
    ADD_STRING(4, "Передаю привет маме, папе, Дединскому, Кнуту - всем, кто учил меня уважать космос")
    ADD_STRING(5, "А чтобы научиться совершать этот переход, мне пришлось продать \\%ROOTNAME\\% душу")
    ADD_STRING(6, "Если вы один из моих NaN друзей ВК, который недоумевает, что здесь происходит, можете просто понегодовать, что ВК нет кнопки dislike")
    ADD_STRING(7, "\\end{center}\\begin{flushleft}\n"
                  "Звонит, значит, программист беременной жене:\\\\— Ты где?\\\\— На сохранении.\\\\— Ну ладно, позвонишь, когда сохранишься."
                  "\\end{flushleft}\\begin{center}\n")
    ADD_STRING(8, "Думал, это ландафшиц? Нет, мальчик, мы здесь преобразовываем производные")
    ADD_STRING(9, "Точку меняем на птичку, птичку на точку(с)")
    ADD_STRING(10,"Хо-хо, а разобраться в данном переходе - то, что Донов прописал")
    ADD_STRING(11,"I send regards to you, Phil2022, who is showing at the moment this 6-year-old work to some interview holder from Google/Intel/\\textbf{1C}?")
    ADD_STRING(12,"Права на этот переход принадлежат Филиппу из П-412. Если вы хотите узнать, как он был произведён(не Филипп, а переход),"
                  " то принесите уже наконец Филиппу что-нибудь сладенькое!")
    }
}
//#define CRAZYMODE
void addMathDump(treeNode_t* tree, bool addStroke)
{
    assert(tree);
    
    static int isAlreadyCalled = 0;
    
    srand(isAlreadyCalled*32+50);
    
    if (isAlreadyCalled)
    {
    #ifdef CRAZYMODE
        if (isAlreadyCalled == 13)
        {
            fprintf(mathDumpFile, ".\n"
                                  "\\end{equation}\n"
                                  "\\fontsize{16}{16pt}\\selectfont"
                                  "\\textbf{Чёрт возьми, господа, что мы с вами c какой-то легкотнёй возимся! Физтехи мы или кто?!?!"
                                  " Айда ещё одну производную возьмём! Да чтобы в экран не вместилась!}\n"
                                  "\\fontsize{8}{8pt}\\selectfont"
                                  "\\begin{equation}\n");
            //
            tree = treeDiffer(tree);
        } else
    #endif
            fprintf(mathDumpFile, "%s=\n\\end{equation}\n"
                                  "\\fontsize{16}{16}\\selectfont\n"
                                  "%s\n"
                                  "\\fontsize{8}{8}\\selectfont\n"
                                  "\\begin{equation}\n\t", (addStroke) ? "'" : " ", someBosh());
    } else
    {
        initTexFile();
    }
    
    fprintf(mathDumpFile, "\\left(");
    _mathTreeDump(tree);
    fprintf(mathDumpFile, "\\right)");
    
    isAlreadyCalled++;
}

bool isTreeConst(treeNode_t* node)
{
    assert(node);
    
    return (node->type != tree_id) && ((!node->left) || (isTreeConst(node->left))) && ((!node->rght) || (isTreeConst(node->rght)));
}

Govnocode /// Operators' redefinitions is copy-paste

treeNode_t operator + (treeNode_t &node1, treeNode_t &node2)
{
    assert(&node1 != &node2);
    assert(&node1); assert(&node2);
    
    unsigned mathOp = '+';
    
    treeNode_t newNode = {};
    nodeCtor(&newNode, tree_mathOperator, mathOp, NULL, &node1, &node2);
    node1.uppr = node2.uppr = NULL;
    
    return newNode;
}

treeNode_t operator * (treeNode_t &node1, treeNode_t &node2)
{
    assert(&node1 != &node2);
    assert(&node1); assert(&node2);
    
    unsigned mathOp = '*';
    
    treeNode_t newNode = {};
    nodeCtor(&newNode, tree_mathOperator, mathOp, NULL, &node1, &node2);
    node1.uppr = node2.uppr = NULL;
    
    return newNode;
}

treeNode_t operator - (treeNode_t &node1, treeNode_t &node2)
{
    assert(&node1 != &node2);
    assert(&node1); assert(&node2);
    
    unsigned mathOp = '-';
    
    treeNode_t newNode = {};
    nodeCtor(&newNode, tree_mathOperator, mathOp, NULL, &node1, &node2);
    node1.uppr = node2.uppr = NULL;
    
    return newNode;
}

treeNode_t operator / (treeNode_t &node1, treeNode_t &node2)
{
    assert(&node1 != &node2);
    assert(&node1); assert(&node2);
    
    unsigned mathOp = '/';
    
    treeNode_t newNode = {};
    nodeCtor(&newNode, tree_mathOperator, mathOp, NULL, &node1, &node2);
    node1.uppr = node2.uppr = NULL;
    
    return newNode;
}

treeNode_t operator ^ (treeNode_t &node1, treeNode_t &node2)
{
    assert(&node1 != &node2);
    assert(&node1); assert(&node2);
    
    unsigned mathOp = '^';
    
    treeNode_t newNode = {};
    nodeCtor(&newNode, tree_mathOperator, mathOp, NULL, &node1, &node2);
    node1.uppr = node2.uppr = NULL;
    
    return newNode;
}
