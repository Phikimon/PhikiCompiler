#include "Recursive_descent.hpp"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define DEBUG_PRINTF(...)    \
{                            \
    if (verbose_)            \
        printf(__VA_ARGS__); \
}

#define GR(GR_NAME) get##GR_NAME()

#define GR_ASSIGN(GR_NAME, GR_CONFIG)               \
TreeNode* SyntaxAnalyzer::get##GR_NAME ()           \
{                                                   \
    DEBUG_PRINTF("\033[1m\033[36m"                  \
            "\nI am get%s()! token:\n"              \
            "\033[0m",                 #GR_NAME);   \
    if (verbose_)                                   \
        dump();                                     \
                                                    \
    TreeNode* result = NULL;                        \
    {                                               \
        GR_CONFIG;                                  \
    };                                              \
                                                    \
    DEBUG_PRINTF("\033[1m\033[31m"                  \
            "That was get%s()! Bye-Bye!\n\n"        \
            "\033[0m",                #GR_NAME);    \
    return result;                                  \
}
#define  assertToken(type, data)                   \
        _assertToken(type, data, __LINE__, __func__)

#define  assertTokenType(type)                         \
        _assertTokenType(type,   __LINE__, __func__)

Govnocode //get rid of those strange characters

GR_ASSIGN(N,
{
    assertTokenType(LANGKWT_double);
    result = new TreeNode(LANGKWT_double, curTok_->val_.data);
    movePtr();
}
         )

GR_ASSIGN(VAR,
{
    if (curTok_->type_ == LANGKWT_id)
    {
        result = new TreeNode(LANGKWT_id, curTok_->val_.idName);
        movePtr();

        if (*curTok_ == TokenListNode(LANGKWT_num_bracket,
                                      LANGSEC_left_bracket))
        {
            DEBUG_PRINTF("\tI've found FUNCTION CALL!\n");
            result->type_ = LANGKWT_function;
            result->rght_ = GR(CALLARGS);
        } else
        {
            DEBUG_PRINTF("\tI've found VAR!\n");
        }
    } else
    {
        DEBUG_PRINTF("\tI've found N!\n");
        result = GR(N);
    }
}
         )

GR_ASSIGN(P,
{
    if (*curTok_ == TokenListNode(LANGKWT_num_bracket,
                                  LANGSEC_left_bracket))
    {
        assertToken(LANGKWT_num_bracket, LANGSEC_left_bracket);
        movePtr();

        result = GR(EXPR);

        assertToken(LANGKWT_num_bracket, LANGSEC_rght_bracket);
        movePtr();
    } else
    {
        result = GR(VAR);
    }
}
         )

GR_ASSIGN(MATH_FUNCS,
{
    if (curTok_->type_ == LANGKWT_mathOperator)
    {
        if (D_EQ(curTok_->val_.data, LANGMATH_sqrt))
        {
            movePtr();

            result = new TreeNode(LANGKWT_mathOperator, LANGMATH_degree);
            result->left_ = GR(EXPR);
            result->rght_ = new TreeNode(LANGKWT_double, 0.5);
        } else
        if (D_EQ(curTok_->val_.data, LANGMATH_ln))
        {
            movePtr();
            assertToken(LANGKWT_num_bracket, LANGSEC_left_bracket);

            result = new TreeNode(LANGKWT_mathOperator, LANGMATH_ln);
            result->left_ = GR(EXPR);
            result->rght_ = NULL;

            assertToken(LANGKWT_num_bracket, LANGSEC_rght_bracket);
            movePtr();
        }
    } else
    {
        result = GR(P);
    }
}
)

GR_ASSIGN(UNMIN,
{
    if ( (curTok_->type_ == LANGKWT_mathOperator) &&
         (curTok_->val_.data == LANGMATH_minus) )
    {
        movePtr();
        result       = new TreeNode(LANGKWT_mathOperator, LANGMATH_minus);
        result->left_ = new TreeNode(LANGKWT_double, 0.0);
        result->rght_ = GR(EXPR);
    } else
    {
        result = GR(MATH_FUNCS);
    }
}
         )

GR_ASSIGN(NEG,
{
    if ( (curTok_->type_ == LANGKWT_mathOperator) &&
         (curTok_->val_.data == LANGMATH_negate )  )
    {
        movePtr();
        result = new TreeNode(LANGKWT_mathOperator, LANGMATH_negate);
        result->left_ = GR(EXPR);
        result->rght_ = NULL;
    } else
    {
        result = GR(UNMIN);
    }
}
         )

#define BINARY_MATH_OPERATORS(NAME, CALLEE_NAME, CONDITION)            \
GR_ASSIGN(NAME,                                                        \
{                                                                      \
    result = GR(CALLEE_NAME);                                          \
                                                                       \
    while ( (curTok_->type_ == LANGKWT_mathOperator) &&                 \
            (CONDITION) )                                              \
    {                                                                  \
        assert(result);                                                \
                                                                       \
        TreeNode* operNode = new TreeNode(LANGKWT_mathOperator,        \
                                          (size_t)curTok_->val_.data); \
        operNode->left_ = result;                                       \
                                                                       \
        movePtr(); operNode->rght_ = GR(CALLEE_NAME);                   \
                                                                       \
        result = operNode;                                             \
    }                                                                  \
}                                                                      \
)

BINARY_MATH_OPERATORS(D , NEG, (curTok_->val_.data == LANGMATH_degree) )

BINARY_MATH_OPERATORS(T , D  , (curTok_->val_.data == LANGMATH_mult) ||
                               (curTok_->val_.data == LANGMATH_div ) )

BINARY_MATH_OPERATORS(S , T  , (curTok_->val_.data == LANGMATH_plus ) ||
                               (curTok_->val_.data == LANGMATH_minus) )

BINARY_MATH_OPERATORS(EQ, S  , (curTok_->val_.data == LANGMATH_equal    ) ||
                               (curTok_->val_.data == LANGMATH_not_equal) )

BINARY_MATH_OPERATORS(GL, EQ , (curTok_->val_.data == LANGMATH_greater_or_equal) ||
                               (curTok_->val_.data == LANGMATH_less_or_equal   ) ||
                               (curTok_->val_.data == LANGMATH_greater         ) ||
                               (curTok_->val_.data == LANGMATH_less            ) )
#undef BINARY_MATH_OPERATORS

GR_ASSIGN(EXPR,
{
    return GR(GL);
    assertToken(LANGKWT_delim, LANGSEC_dot);
}
         )

GR_ASSIGN(RET,
{
    assertToken(LANGKWT_return, LANGKWD_return);
    movePtr();

    result = new TreeNode(LANGKWT_return, LANGKWD_return);
    result->left_ = GR(EXPR);
    result->rght_ = NULL;
    assertToken(LANGKWT_delim, LANGSEC_dot);
    movePtr();
}
         )

GR_ASSIGN(LOOP,
{
    assertToken(LANGKWT_loop, LANGKWD_while);
    movePtr();

    result = new TreeNode(LANGKWT_loop, LANGKWD_while);
    result->left_ = GR(EXPR);
    result->rght_ = GR(OPBLOCK);

    assertToken(LANGKWT_delim, LANGSEC_dot);
    movePtr();
}
         )

GR_ASSIGN(COND,
{
    assertToken(LANGKWT_conditional, LANGKWD_if);
    movePtr();

    result = new TreeNode(LANGKWT_conditional, LANGKWD_if);
    result->left_ = GR(EXPR);
    result->rght_ = new TreeNode(LANGKWT_conditional, LANGKWD_if);
    result->rght_->left_ = GR(OPBLOCK);

    if (*curTok_->next_ == TokenListNode(LANGKWT_conditional, LANGKWD_else))
    {
        assertToken(LANGKWT_delim, LANGSEC_dot);
        movePtr();

        assertToken(LANGKWT_conditional, LANGKWD_else);
        movePtr();

        result->rght_->rght_ = GR(OPBLOCK);
    } else
    {
        result->rght_->rght_ = NULL;
    }

    assertToken(LANGKWT_delim, LANGSEC_dot);
    movePtr();
}
         )

GR_ASSIGN(IO,
{
    assertTokenType(LANGKWT_io);
    if ( D_EQ(curTok_->val_.data, LANGKWD_scan) )
    {
        assertToken(LANGKWT_io, LANGKWD_scan);
        result = new TreeNode(LANGKWT_io, LANGKWD_scan);

        movePtr();

        assertTokenType(LANGKWT_id);
        result->left_ = new TreeNode(LANGKWT_id,
                                     curTok_->val_.idName);
        result->rght_ = NULL;

        movePtr();

        assertToken(LANGKWT_delim, LANGSEC_dot);
        movePtr();
    } else
    {
        assertToken(LANGKWT_io, LANGKWD_print);
        result = new TreeNode(LANGKWT_io, LANGKWD_print);

        movePtr();

        result->left_ = GR(EXPR);
        result->rght_ = NULL;

        assertToken(LANGKWT_delim, LANGSEC_dot);
        movePtr();
    }
}
         )

GR_ASSIGN(NEWVAR,
{
    assertToken(LANGKWT_new_variable, LANGKWD_declare);
    if ( D_EQ(curTok_->val_.data, LANGKWD_declare) )
    {
        assertToken( LANGKWT_new_variable, LANGKWD_declare);
        movePtr();

        assertTokenType(LANGKWT_id);
        result = new TreeNode(LANGKWT_id, curTok_->val_.idName);
        movePtr();

        assertToken(LANGKWT_delim, LANGSEC_dot);
        movePtr();
    };
}
         )

GR_ASSIGN(VARASS,
{
    assertToken(LANGKWT_assign, LANGKWD_assign);
    if ( D_EQ(curTok_->val_.data, LANGKWD_assign) )
    {
        assertToken( LANGKWT_assign, LANGKWD_assign);
        result = new TreeNode(LANGKWT_assign, LANGKWD_assign);
        movePtr();

        assertTokenType(LANGKWT_id);
        result->left_ = new TreeNode(LANGKWT_id, curTok_->val_.idName);
        movePtr();

        result->rght_ = GR(EXPR);

        assertToken(LANGKWT_delim, LANGSEC_dot);
        movePtr();
    };
}
         )

GR_ASSIGN(OP,
{
    switch (curTok_->type_)
    {
#define OPERCASE(enumVal, GRName) \
case LANGKWT_##enumVal:           \
{                                 \
result = GR(GRName);          \
}; break;
        OPERCASE(new_variable, NEWVAR)
        OPERCASE(assign,       VARASS)
        OPERCASE(conditional,  COND  )
        OPERCASE(return,       RET   )
        OPERCASE(loop,         LOOP  )
        OPERCASE(io,           IO    )

        case LANGKWT_operator_bracket:
        {
            if (D_EQ(curTok_->val_.data, LANGKWD_begin))
                result = GR(OPBLOCK);
            else
                assert(!"Unrecognized closing bracket");
        }; break;
        case LANGKWT_delim:
        {
            while (*curTok_ == TokenListNode(LANGKWT_delim, LANGSEC_dot))
                curTok_ = curTok_->next_;
        }; break;
        default:
        {
            printf("\n\033[31mError: unrecognized operator\033[0m\n\n");
            dump();
            printf("\n\n\033[31mIn Line\033[0m\n");
            putErr();
            myThrow(COMPERR_UNKOP);
        }; break;
    };
}
            )

GR_ASSIGN(OPBLOCK,
{
    //OPERATOR_BRACKET_OPENING
    assertToken(LANGKWT_operator_bracket, LANGKWD_begin);
    movePtr();
    //OPERATORS

    result = NULL;
    TreeNode* temp = NULL;

    size_t opCounter = 0;
    bool isFirstLoop = true;
    while ( curTok_->type_ != LANGKWT_operator_bracket )
    {
        if (isFirstLoop)
        {
            result = temp = new TreeNode(LANGKWT_delim, LANGSEC_dot);
            isFirstLoop = false;
        } else
        {
            temp->rght_ = new TreeNode(LANGKWT_delim, LANGSEC_dot);
            temp       = temp->rght_;
        }
        assert(opCounter < COMPILER_MAX_OP_QT);
        temp->left_ = GR(OP);
        opCounter++;
    }

    //OPERATOR_BRACKET_CLOSING
    assertToken(LANGKWT_operator_bracket, LANGKWD_end);
    movePtr();

    assertToken(LANGKWT_delim, LANGSEC_dot);
    //Doesn't skip '.'!
}
         )

GR_ASSIGN(CALLARGS,
{
    assertToken(LANGKWT_num_bracket, LANGSEC_left_bracket);
    movePtr();

    TreeNode* callArgs = NULL;

    size_t argCounter  = 0;
    bool   isFirstLoop = true;
    while ( curTok_->type_ != LANGKWT_num_bracket )
    {
        if (!isFirstLoop)
        {
            assertToken(LANGKWT_delim, LANGSEC_comma);
            movePtr();
        }
        assert(argCounter < COMPILER_MAX_FUNC_ARG_QT);

        callArgs = new TreeNode(LANGKWT_delim,
                                LANGSEC_dot);
        if (isFirstLoop)
        {
            result = callArgs;
        }
        callArgs->left_ = GR(EXPR);
        callArgs = callArgs->rght_;

        isFirstLoop = false;
        argCounter++;
    };

    assertToken(LANGKWT_num_bracket, LANGSEC_rght_bracket);
    movePtr();
}
          )

GR_ASSIGN(ARGS,
{
    assertToken(LANGKWT_num_bracket, LANGSEC_left_bracket);
    movePtr();

    TreeNode* funcArgs = NULL;

    size_t argCounter = 0;
    bool isFirstLoop = true;
    while ( curTok_->type_ != LANGKWT_num_bracket )
    {
        if (!isFirstLoop)
        {
            assertToken(LANGKWT_delim, LANGSEC_comma);
            movePtr();
        }
        isFirstLoop = false;
        assert(argCounter < COMPILER_MAX_FUNC_ARG_QT);

        assertToken(LANGKWT_new_function, LANGKWD_pass);
        movePtr();

        assertTokenType(LANGKWT_id);
        if (!funcArgs)
        {
            result = funcArgs = new TreeNode(LANGKWT_id,
                                             curTok_->val_.idName);
        } else
        {
            funcArgs->left_    = new TreeNode(LANGKWT_id,
                                             curTok_->val_.idName);
            funcArgs = funcArgs->left_;
        }
        argCounter++;
        movePtr();
    };

    assertToken(LANGKWT_num_bracket, LANGSEC_rght_bracket);
    movePtr();
}
          )

GR_ASSIGN(PROGRAM,
{
    /**SEE /Tree_structure_syntax/Function.pdf*/
    //Program name
    assertToken( LANGKWT_new_program, LANGKWD_program);
    result = new TreeNode(LANGKWT_new_program, LANGKWD_program);
    movePtr();

    assertTokenType(   LANGKWT_id);
    result->left_ = new TreeNode(LANGKWT_id, curTok_->val_.idName);
    movePtr();
    result->rght_ = new TreeNode(LANGKWT_delim, LANGSEC_dot);
    //Functions
    size_t funcCounter = 0;
    TreeNode* slider = result->rght_;
    while ( curTok_->type_ == LANGKWT_new_function )
    {
        assert(funcCounter < COMPILER_MAX_FUNC_QT);

        TreeNode* newFunction = slider;
        //Function node
        assertToken(LANGKWT_new_function, LANGKWD_function);
        newFunction->left_ = new TreeNode(LANGKWT_new_function, LANGKWD_function);
        newFunction = newFunction->left_;
        movePtr();
        //Function name node
        assertTokenType(LANGKWT_id);
        newFunction->left_ = new TreeNode(LANGKWT_id, curTok_->val_.idName);
        movePtr();
        //Secondary node, that is needed to connect function with
        //both arguments and operators' block
        newFunction->rght_ = new TreeNode(LANGKWT_new_function, LANGKWD_function);
        newFunction = newFunction->rght_;
        //Arguments
        //Third-rate node, that connects secondary node with both
        //arguments and local variables
        newFunction->left_ = new TreeNode(LANGKWT_new_function, LANGKWD_function);
        //Arguments
        newFunction->left_->left_ = GR(ARGS);
        //Local variables
        TreeNode* localVars = newFunction->left_;
        int varCounter = 0;
        while (*curTok_ == TokenListNode(LANGKWT_new_variable,
                                         LANGKWD_declare)    )
        {
            assert(varCounter < COMPILER_MAX_FUNC_VAR_QT);
            localVars->rght_ = GR(NEWVAR);
            localVars = localVars->rght_;
            varCounter++;
        }
        //OP_BLOCK
        newFunction->rght_ = GR(OPBLOCK);
        //Next function
        if (curTok_->next_)
        {
            slider->rght_ = new TreeNode(LANGKWT_delim, LANGSEC_dot);
            slider = slider->rght_;
            movePtr();
        }
    }
}
         )

GR_ASSIGN(G0,
{
    result = GR(PROGRAM);
    //Throughout the program we haven't been caring about
    //stuff like
    //  y->uppr = x;
    //we just wrote
    //  x->left_/rght_ = y;
    //thus improving code readability.
    //So this function is called to 'take care of' all
    //those 'uppr' pointers
    Tree(result).redispatchUppr();
}
         )

#undef movePtr
#undef assertToken
#undef assertTokenType
#undef DEBUG_PRINTF
#undef GR
#undef GR_ASSIGN
#undef assertToken
#undef assertTokenType
