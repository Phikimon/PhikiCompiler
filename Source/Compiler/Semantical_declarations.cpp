#include "Semantical_analyzer.hpp"
#include "Processor/Processor.hpp" //For CPU_REGISTERS_QT constant
#include "Recursive_descent.hpp" //For COMPILER_MAX_FUNC_ARG_QT and
                                 //COMPILER_MAX_FUNC_VAR_QT constants
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#define ADD_TO_FILE(...) fprintf(asmFile_, __VA_ARGS__)

#define  assertSynt(node, type, data) \
        _assertSynt(node, type, data, __LINE__, __func__)

#define  assertSyntType(node, type)   \
        _assertSyntType(node, type, __LINE__, __func__)

#define CUR_FUNC functions_[functionCounter_]

#define SEMANT_NDEBUG

#ifndef SEMANT_NDEBUG

#define _HELLO_FUNCPRINT { printf("\n\tI am %s!\n", __func__); }
#define _BYE_FUNCPRINT { printf("\n\tBye from %s!\n", __func__); }

#else

#define _HELLO_FUNCPRINT
#define _BYE_FUNCPRINT

#endif

/* Before reading this file it's necessary to see
 * "Source/Compiler/Syntax\ tree\ standard/*.png" files */
void SyntaxTreeTranslator::compileProgram(TreeNode* node)
{
    _HELLO_FUNCPRINT
    while (node)
    {
        assertSynt(node, LANGKWT_delim, LANGSEC_dot);
        compileFunction(node->left_);
        node = node->rght_;
    }
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileFunction(TreeNode* node)
{
    _HELLO_FUNCPRINT
    TreeNode* funcName = node->left_;
    assertSyntType(funcName, LANGKWT_id);

    TreeNode* linker1  = node->rght_;
    assertSynt(linker1, LANGKWT_new_function, LANGKWD_function);

    TreeNode* linker2  = linker1->left_;
    assertSynt(linker2, LANGKWT_new_function, LANGKWD_function);

    TreeNode* operators = linker1->rght_;
    assertSynt(operators, LANGKWT_delim, LANGSEC_dot);

    TreeNode* arguments = linker2->left_;
    assertSynt(operators, LANGKWT_delim, LANGSEC_dot);

    TreeNode* variables = linker2->rght_;

    ADD_TO_FILE("\n;FUNCTION %s\n"
                "%s:\n",
                funcName->data_.idName,
                funcName->data_.idName);

    strncpy(CUR_FUNC.functionName,
            funcName->data_.idName,
            MAX_LINE_LEN);
    for (int i = 0; i < functionCounter_; i++)
    {
        if (!strncmp(CUR_FUNC.functionName,
                     functions_[i].functionName,
                     MAX_LINE_LEN))
        {
            prln();
            printf("\nError:\n"
                   "Phila's language prohibits usage of "
                   "functions with similar names:\n"
                   "%s found\n",
                   CUR_FUNC.functionName);
            myThrow(STTERR_SIMNAMES);
        }
    }

    CUR_FUNC.varsQt = getVariables(variables,
                                   CUR_FUNC.names);

    CUR_FUNC.argsQt = getArguments(arguments,
                                   CUR_FUNC.names + CUR_FUNC.varsQt);

    checkNamesCollision();

    compileOperatorsBlock(operators);

    ADD_TO_FILE("; END OF FUNCTION %s\n", funcName->data_.idName);
    functionCounter_++;
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::checkNamesCollision()
{
    _HELLO_FUNCPRINT
    //TODO: add hashes
    for (int i = 0; i < CUR_FUNC.namesQt(); i++)
    {
        char* firstName = CUR_FUNC.names[i];
        for (int j = 0; j < CUR_FUNC.namesQt(); j++)
        {
            char* secondName = CUR_FUNC.names[j];
            if ((i != j) && (!strncmp(firstName, secondName, MAX_LINE_LEN)))
            {
                prln();
                printf("\nError:\n"
                       "Phila's language prohibits usage of "
                       "variables or arguments with similar names:\n"
                       "%s found\n", firstName);
                myThrow(STTERR_SIMNAMES);
            }
        }
    }
    _BYE_FUNCPRINT
}

//Returns 'smth' quantity
#define getSmth(WHAT, WHAT1, DIRECTION)                                          \
int SyntaxTreeTranslator::get##WHAT(TreeNode* smth,                              \
                                    char* names[COMPILER_MAX_FUNC_##WHAT1##_QT]) \
{                                                                                \
    _HELLO_FUNCPRINT                                                             \
    int i = 0;                                                                   \
    while (smth)                                                                 \
    {                                                                            \
        assert(i < COMPILER_MAX_FUNC_##WHAT1##_QT);                              \
        assertSyntType(smth, LANGKWT_id);                                        \
        size_t idLen = strnlen(smth->data_.idName, MAX_LINE_LEN);                \
        assert(idLen);                                                           \
        names[i] = (char*)calloc(idLen + sizeof('\0'),                           \
                                 sizeof(char));                                  \
        strncpy(names[i], smth->data_.idName, idLen);                            \
        smth  = smth->DIRECTION##_;                                              \
        i++;                                                                     \
    }                                                                            \
    return i;                                                                    \
    _BYE_FUNCPRINT                                                               \
}

getSmth(Variables, VAR, rght)
getSmth(Arguments, ARG, left)

#undef getSmth

void SyntaxTreeTranslator::compileOperatorsBlock(TreeNode* currentDot)
{
    _HELLO_FUNCPRINT
    while (currentDot)
    {
        assertSynt(currentDot, LANGKWT_delim, LANGSEC_dot);
        TreeNode* oper = currentDot->left_;
        assert(oper);
        switch (oper->type_)
        {
#define compileOperator(typeName, funcName)  \
case LANGKWT_##typeName:                     \
{                                            \
    compile##funcName(oper);                 \
} break;
            compileOperator(assign,      AssignmentOperator)
            compileOperator(conditional, ConditionalOperator)
            compileOperator(return,      ReturnOperator)
            compileOperator(loop,        LoopOperator)
            compileOperator(io,          InputOrOutputOperator)
#undef compileOperator
            default:
            {
                prln();
                printf("\n\nUnknown operator type %d(%s)",
                       oper->type_,
                       langKeywordsTypesStrings(oper->type_));

                printf(" passed to the %s function.\n\n",
                        __func__);

                myThrow(STTERR_UNKTYPE);
           }
        }
        currentDot = currentDot->rght_;
    }
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileConditionalOperator(TreeNode* node)
{
    _HELLO_FUNCPRINT
    TreeNode* condition = node->left_;

    TreeNode* linker = node->rght_;
    assertSynt(linker, LANGKWT_conditional, LANGKWD_if);

    TreeNode*  trueBlock = linker->left_;
    assertSynt(trueBlock, LANGKWT_delim, LANGSEC_dot);

    TreeNode* falseBlock = linker->rght_;
    if (falseBlock)
    {
        assertSynt(falseBlock, LANGKWT_delim, LANGSEC_dot);
    }

    static long unsigned ifCounter = 0;
    ifCounter++;

    ADD_TO_FILE("\n; Computing value for IF%lu\n", ifCounter);
    compileExpression(condition);
    ADD_TO_FILE("\nPOP %%%lu\n", CUR_FUNC.namesQt() + 1);
    ADD_TO_FILE("JE %%%lu $0 $ELSE%lu\n",
                 CUR_FUNC.namesQt() + 1,
                 ifCounter);

    compileOperatorsBlock(trueBlock);

    ADD_TO_FILE("\nJMP $IFEND%lu\n", ifCounter);
    ADD_TO_FILE("\nELSE%lu:\n", ifCounter);

    compileOperatorsBlock(falseBlock);

    ADD_TO_FILE("\nIFEND%lu:\n", ifCounter);
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileReturnOperator(TreeNode* node)
{
    _HELLO_FUNCPRINT
    assertSynt(node, LANGKWT_return, LANGKWD_return);
    ADD_TO_FILE("\n; RETURN from %s\n", CUR_FUNC.functionName);

    TreeNode* returnExpression = node->left_;
    compileExpression(returnExpression);
    ADD_TO_FILE("\nPOP %%0\n");
    ADD_TO_FILE("RET\n");
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileLoopOperator(TreeNode* node)
{
    _HELLO_FUNCPRINT
    static size_t loopCounter = 0;
    loopCounter++;

    assertSynt(node, LANGKWT_loop, LANGKWD_while);

    TreeNode* condition = node->left_;

    TreeNode* block = node->rght_;
    assertSynt(node, LANGKWT_loop, LANGKWD_while);

    ADD_TO_FILE("\nLOOPBEGIN%lu:\n"
                "\n; Calculating condition for loop %lu\n",
                 loopCounter, loopCounter);

    compileExpression(condition);
    ADD_TO_FILE("; End of calculation\n");

    ADD_TO_FILE("\nPOP %%%lu\n", CUR_FUNC.namesQt() + 1);
    ADD_TO_FILE("\nJE %%%lu $0 $LOOPEND%lu\n",
                 CUR_FUNC.namesQt() + 1,
                 loopCounter);

    compileOperatorsBlock(block);

    ADD_TO_FILE("\nJMP $LOOPBEGIN%lu\n", loopCounter);
    ADD_TO_FILE("\nLOOPEND%lu:\n", loopCounter);
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileInputOrOutputOperator(TreeNode* node)
{
    _HELLO_FUNCPRINT
    assertSyntType(node, LANGKWT_io);
    TreeNode* value = node->left_;
    if (node->data_.kwdNum == LANGKWD_print)
    {
        ADD_TO_FILE("\n; Computing expression for OUT\n");
        compileExpression(value);
        ADD_TO_FILE("POP %%%lu\n", CUR_FUNC.namesQt() + 1);
        ADD_TO_FILE("OUT %%%lu\n", CUR_FUNC.namesQt() + 1);
        ADD_TO_FILE("\n");
    } else
    if (node->data_.kwdNum == LANGKWD_scan)
    {
        assertSyntType(value, LANGKWT_id);
        assert(value->data_.idName);
        ADD_TO_FILE("\n IN %%%lu ; %s\n",
                    findName(value),
                    value->data_.idName);
    };
    _BYE_FUNCPRINT
}

int SyntaxTreeTranslator::findName(TreeNode* nodeWithName)
{
    _HELLO_FUNCPRINT
    assert(nodeWithName);
    assert(nodeWithName->data_.idName);
    for (int i = 0; i < CUR_FUNC.namesQt(); i++)
    {
        if (!strncmp(nodeWithName->data_.idName,
                     CUR_FUNC.names[i],
                     MAX_LINE_LEN))
        {
            _BYE_FUNCPRINT
            return i + 1;
        }
    }
    prln();
    printf("Undeclared variable with name '%s' in assignment",
            nodeWithName->data_.idName);
    myThrow(STTERR_UNKNAME);
    return -1;
}

//Performs calculations using stack and
//%<namesQt + 1> and %<namesQt + 2> registers.
//Result is stored in %<namesQt>
void SyntaxTreeTranslator::compileExpression(TreeNode* node)
{
    _HELLO_FUNCPRINT
    if (!node)
        return;

    if (node->type_ != LANGKWT_function)
    {
        compileExpression(node->left_);
        compileExpression(node->rght_);
    }

    if (node->type_ == LANGKWT_mathOperator)
    {
        //Pop two values from stack in
        //%<namesQt + 1> and %<namesQt + 2> registers,
        //perform calculation and push back
        ADD_TO_FILE("\nPOP %%%lu\n",
                     CUR_FUNC.namesQt() + 2);
        ADD_TO_FILE("POP %%%lu\n",
                     CUR_FUNC.namesQt() + 1);
        assert(node->data_.kwdNum >= 0 &&
               node->data_.kwdNum <  LANGMATH_MATHQT);
        ADD_TO_FILE("\n%sBIN %%%lu %%%lu\n",
                     asmMathInstr((LangMathOperators)node->data_.kwdNum),
                     CUR_FUNC.namesQt() + 1,
                     CUR_FUNC.namesQt() + 2);
        ADD_TO_FILE("\nPUSH %%%lu\n",
                     CUR_FUNC.namesQt() + 1);
    } else
    if ( (node->type_ == LANGKWT_id    ) ||
         (node->type_ == LANGKWT_double) )
    {
        //Push value in stack
        ADD_TO_FILE("PUSH ");
        compileRValue(node);
        if (node->type_ == LANGKWT_id)
        {
            ADD_TO_FILE(" ; %s", node->data_.idName);
        }
        ADD_TO_FILE("\n");
    } else
    if (node->type_ == LANGKWT_function)
    {
        ADD_TO_FILE("\n; CALL %s\n", node->data_.idName);
        compileFunctionCall(node);
        ADD_TO_FILE("; CALL_END %s\n", node->data_.idName);
    } else
    {
        prln();
        printf("Unexpected node type %d(%s)",
                node->type_,
                langKeywordsTypesStrings(node->type_));
        myThrow(STTERR_UNKTYPE);
    }
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileFunctionCall(TreeNode* node)
{
    _HELLO_FUNCPRINT
    assertSyntType(node, LANGKWT_function);
    assert(node->data_.idName);
    int funcNumber = -1;
    for (int i = 0; i <= functionCounter_; i++)
    {
        if (!strncmp(functions_[i].functionName,
                     node->data_.idName,
                     MAX_LINE_LEN))
        {
            funcNumber = i;
            break;
        }
    }
    if (funcNumber == -1)
    {
        prln();
        printf("\nFunction with name '%s', "
                 "which is called in '%s', "
                 "is undeclared.\n",
                node->data_.idName,
                functions_[functionCounter_].functionName);
        myThrow(STTERR_UNDECLFUNC);
    }
    compileFunctionCallPrologue(node, funcNumber);
    ADD_TO_FILE("CALL $%s\n", node->data_.idName);
    compileFunctionCallEpilogue(node, funcNumber);
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileFunctionCallEpilogue(TreeNode* node,
                                                       int funcNumber)
{
    _HELLO_FUNCPRINT
    ADD_TO_FILE("\n; Epilogue for '%s' function call\n",
                node->data_.idName);
    ADD_TO_FILE("; Swap %%0 and stack top\n");
    ADD_TO_FILE("POP %%%lu\n", CUR_FUNC.namesQt() + 1);
    for (int i = 1;
             i < functions_[funcNumber].namesQt() + 3;
             i++)
    {
        ADD_TO_FILE("POP %%%lu\n", i);
    }
    ADD_TO_FILE("PUSH %%0\n");
    ADD_TO_FILE("MOV %%0 %%%lu\n", CUR_FUNC.namesQt() + 1);
    ADD_TO_FILE("; Epilogue end\n");
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileFunctionCallPrologue(TreeNode* node,
                                                       int funcNumber)
{
    _HELLO_FUNCPRINT
    ADD_TO_FILE("\n; Prologue for '%s' function call\n",
                node->data_.idName);
    //Store registers in stack
    for (int i = 0;
             i < functions_[funcNumber].namesQt() + 3;
             i++)
    {
        ADD_TO_FILE("PUSH %%%lu\n", i);
    }
    //Fill arguments' registers
    TreeNode* dotNode = node->rght_;
    for (int i = 1;
             i <= functions_[funcNumber].argsQt;
             i++)
    {
        if (!dotNode)
        {
            prln();
            printf("\nNot enough arguments are passed "
                   "to function %s from function %s\n",
                   functions_[functionCounter_].functionName,
                   CUR_FUNC.functionName);
            myThrow(STTERR_TOOFEWARGS);
        }
        TreeNode* argNode = dotNode->left_;
        compileExpression(argNode);
        ADD_TO_FILE("POP %%%lu\n", i);
        dotNode = dotNode->rght_;
    }
    if (dotNode)
    {
        prln();
        printf("\nToo many arguments are passed "
               "to function %s from function %s\n",
               functions_[functionCounter_].functionName,
               CUR_FUNC.functionName);
        myThrow(STTERR_TOOFEWARGS);
    }
    ADD_TO_FILE("; Prologue end\n");
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileRValue(TreeNode* node)
{
    _HELLO_FUNCPRINT
    if (node->type_ == LANGKWT_id)
    {
        assert(node->data_.idName);
        ADD_TO_FILE("%%%lu", findName(node));
    } else
    if (node->type_ == LANGKWT_double)
    {
        long unsigned registerNumber = node->data_.number;
        assert(registerNumber < CPU_REGISTERS_QT &&
               registerNumber >= 0               );
        ADD_TO_FILE("$%lu", registerNumber);
    } else
    {
        prln();
        printf("Unexpected node type %d(%s) in %s",
                node->type_,
                langKeywordsTypesStrings(node->type_),
                __func__);
        myThrow(STTERR_UNKTYPE);
    }
    _BYE_FUNCPRINT
}

void SyntaxTreeTranslator::compileAssignmentOperator(TreeNode* node)
{
    _HELLO_FUNCPRINT
    assertSynt(node, LANGKWT_assign, LANGKWD_assign);

    TreeNode* leftOperand = node->left_;
    assertSyntType(leftOperand, LANGKWT_id);

    TreeNode* rghtOperand = node->rght_;

    int registerNum = findName(leftOperand);
    //Compile expression and push value it in stack
    compileExpression(rghtOperand);
    //Result of calculations is pushed in stack
    ADD_TO_FILE("\nPOP %%%lu ; %s\n",
                   registerNum,
                   leftOperand->data_.idName);
    _BYE_FUNCPRINT
}

#undef ADD_TO_FILE
#undef assertSynt
#undef assertSyntType
#undef CUR_FUNC
#undef _HELLO_FUNCPRINT
#undef _BYE_FUNCPRINT
