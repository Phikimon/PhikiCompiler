class TreeWrappper
{
    public:
    Tree* tree_;

    operator + (TreeWrapper& tree1, TreeWrapper& tree2);
    operator - (TreeWrapper& tree1, TreeWrapper& tree2);
    operator * (TreeWrapper& tree1, TreeWrapper& tree2);
    operator / (TreeWrapper& tree1, TreeWrapper& tree2);

    /**
     Tree math optimizations
     */
    void simplifyMultAdd        (TreePointerWrapper treeToChange);
    void calculateConstant      (TreePointerWrapper treeToChange);
    void visitorMathTreeOptimize(TreePointerWrapper treeToChange);
    void mathTreeOptimize       (TreePointerWrapper treeToChange);
}

#define KILLOPERAND(toKill)                                       \
{                                                                 \
    TreeNode* branchToKill = NULL;                                \
    TreeNode* result       = NULL;                                \
                                                                  \
    if (operatorNode->toKill == operatorNode->left)               \
    {                                                             \
        branchToKill = operatorNode->left;                        \
        result       = operatorNode->rght;                        \
    } else                                                        \
    {                                                             \
        branchToKill = operatorNode->rght;                        \
        result       = operatorNode->left;                        \
    }                                                             \
                                                                  \
    nodeDtor(branchToKill);                                       \
                                                                  \
    result->uppr = operatorNode->uppr;                            \
                                                                  \
    if (operatorNode->uppr)                                       \
    {                                                             \
        if (isLeftChild(operatorNode))                            \
        {                                                         \
            operatorNode->uppr->left = result;                    \
        } else                                                    \
        {                                                         \
            operatorNode->uppr->rght = result;                    \
        }                                                         \
    }                                                             \
                                                                  \
    nodeDtor(operatorNode);                                       \
                                                                  \
    *treeToChange = result;                                       \
}
void simplifyMultAdd(TreeWrapper treeToChange)
{
    // Check and reassign
    assert(treeToChange); assert(*treeToChange);
    TreeNode* operatorNode = *treeToChange;

    // Useful nodes to compare with
    double zero = 0;
    TreeNode zeroNode = {};
    _nodeCtor(&zeroNode, LANGKWT_double, &zero);

    double one  = 1;
    TreeNode oneNode = {};
    _nodeCtor(&oneNode, LANGKWT_double, &one);

    //if operatorNode actually contains operator
    if (operatorNode->type == LANGKWT_mathOperator) 
    {
        assert(operatorNode->left); assert(operatorNode->rght);

        switch (*(unsigned*)operatorNode->data)
        {
            case '+': 
            {
                //Kill left operand if it is equal to zero
                if ( *operatorNode->left == zeroNode )
                    {
                        KILLOPERAND( left ) 
                    } else

                //Kill rght operand if it is equal to zero
                if ( *operatorNode->rght == zeroNode )
                    {
                        KILLOPERAND( rght )
                    };
            }
            case '*':
            {
                //Kill rght operand if 
                //      ↓↓↓↓(left IS equal to zero)↓↓↓  OR
                if ( ( *operatorNode->left == zeroNode) || 

                //      ↓↓↓↓(rght IS equal to one)↓↓↓
                     ( *operatorNode->rght == oneNode ) )
                {
                    KILLOPERAND( rght )
                } else

                //Kill left operand that
                //      ↓↓↓↓(rght IS equal to zero)↓↓↓  OR
                if ( ( *operatorNode->rght == zeroNode) || 

                //      ↓↓↓↓(left IS equal to one)↓↓↓
                     ( *operatorNode->left == oneNode ) )
                {
                    KILLOPERAND( left )
                };
            }
            case '^':
            {
                //Kill left operand if it is equal to one
                if ( *operatorNode->left == oneNode )
                    {
                        KILLOPERAND( left ) 
                    } else

                //Kill rght operand if it is equal to one
                if ( *operatorNode->rght == oneNode )
                    {
                        KILLOPERAND( rght )
                    };
            }
            default:
            {
                printf("Error occured in switch in %s", __func__);
                break;
            }
        }
    }
}
#undef KILLOPERAND

void calculateConstant(TreeWrappper treeToChange)
{  
    assert(treeToChange); assert(*treeToChange);
    TreeNode* tree = *treeToChange;
    
    if ((tree->type != LANGKWT_mathOperator) || (!isTreeConst(tree))) 
        return;
    
    if ( ((tree->left) && (tree->left->type != LANGKWT_double)) || 
         ((tree->rght) && (tree->rght->type != LANGKWT_double)) )
        return;
        
    assert(tree->left);
    assert(tree->data);
    
    double resVal = 0;
    TreeNode* result = NULL;

#define lval (*(double*)(tree->left->data))
#define rval (tree->rght ? (*(double*)tree->rght->data) : 0)
#define operCase(operChar, operation, operString) \
case operChar: \
{ \
    resVal = operation; \
    printf("%lg = " #operString "\n", resVal, lval, rval); \
    result = nodeRetCtor(LANGKWT_double, resVal, NULL, NULL, NULL); \
} break;

    switch (*(unsigned*)tree->data)
    {
        operCase('*', lval * rval,      %lg * %lg)
        operCase('/', lval / rval,      %lg / %lg)
        operCase('+', lval + rval,      %lg + %lg)
        operCase('-', lval - rval,      %lg - %lg)
        operCase('^', pow(lval, rval),  %lg ^ %lg)
        operCase('l', log(lval),        ln(%lg); rval = %lg)

        default: 
        {
            prln();
            printf("\nError: Unknown type of mathOp == '%i' is passed to the '%s'\n",
            tree->type, __func__); 
            myThrow(ERR_COMMON);
        }; break;
    };

#undef operCase
#undef lval
#undef rval
                        
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
    
    *treeToChange = tree;
}

bool Tree::isConst()
{
    treeVisittor(infixMode, TreeNode::isConst, root_);
}

void TreeWrapper::mathOptimize_forVisitor()
{
    calculateConstant(this);
    simplifyMultAdd(this);
}

void TreeWrapper::mathOptimize()
{
    treeVisitorPtr(postfixMode, visitorMathTreeOptimize, this);
}

bool TreeNode::isConst()
{
    return (node->type != LANGKWT_id);
}
