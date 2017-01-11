#include "./Semantical_analyzer.hpp"

void _assertSyntType(treeNode_t* node, treeNodeDataType type, unsigned line, const char* func)
{
    if (node->type != type)
    {
        printf("\033[31m\033[1mIn func '%s' in line #%u syntax-tree node type(%d)(%s) is not equal to %d(%s).\n", 
                func, line, node->type, LangKeywordsTypesStrings[node->type], type, LangKeywordsTypesStrings[type]);
        dumbDump(node);
        printf("\033[0m\n");
        assert(!"Assertion failed");
    }
};

void _assertSynt(treeNode_t* node, treeNodeDataType type, void* data, unsigned line, const char* func)
{
    if (node->type != type)
    {
        printf("\033[0mIn func '%s' in line #%u syntax-tree node isn't equal to expected. Node:\033[31m\033[1m\n", func, line);
        dumbDump(node);
        treeNode_t* expected = _nodeRetCtor(type, data);
        printf("\n\033[0mExpected node:\033[31m\033[1m\n");
        dumbDump(expected);
        printf("\033[0m\n");
        assert(!"Assertion failed");
    }
};


void transCtor(syntaxTreeTranslator_t* trans, treeNode_t* root)
{
    unsigned program = (unsigned)LANGKWD_program;
    assertSynt(root, tree_new_program, program);
    assertSyntType(root->left, tree_id);
    
    sprintf(trans->asmFileName, "./temp/ASM_%s", *(char**)root->left->data);
    
    FOPEN(trans->asmFile = fopen(trans->asmFileName, "w"));
    
    fprintf(trans->asmFile, "CALL $main\n"
                            "POP  %%0\n"
                            "OUT  %%0\n"
                            "JMP  $END\n");
    
    trans->root = root->rght;
    trans->usedIdsNum = 0;
}

void transDtor(syntaxTreeTranslator_t* trans)
{
    trans->usedIdsNum = 0;
    trans->root = NULL;
    
    fprintf(trans->asmFile, "\n\nEND:"
                            "\nNOP\n");
    
    fclose(trans->asmFile); trans->asmFile = NULL;
}


size_t transIdNum(syntaxTreeTranslator_t* trans, char* id)
{
    assert(trans); assert(trans->asmFile);
    
    if (!id)
    {
        trans->usedIdsNum++;
        return trans->usedIdsNum - 1;
    }
    
    for (size_t i = 0; i < trans->usedIdsNum; i++)
    {
        if ( (trans->usedIds[i]) && (!strcmp(trans->usedIds[i], id)) )
            return i;
    }
    
    if (trans->usedIdsNum >= CPU_REGISTERS_QT - 1)
    {
        prln();
        printf("\nToo many variables is used in asm program. Please, wait for update\n");
        throw(CPU_TOOMANYVARS);
    }
    trans->usedIds[trans->usedIdsNum] = (char*)calloc(strlen(id) + 1, sizeof(*trans->usedIds[trans->usedIdsNum]));
    strcpy(trans->usedIds[trans->usedIdsNum], id);
    trans->usedIdsNum++;
    return trans->usedIdsNum - 1;
}


void translateTree(syntaxTreeTranslator_t* trans, treeNode_t* root)
{
    assert(trans); assert(trans->asmFile);
    
    if (!root)
    {
        return;
    }
    
    treeNode_t* currentDot = root;
    while (currentDot)
    {
        unsigned dot = '.'; assertSynt(currentDot, tree_delim, dot);
        
        treeNode_t* currentOp = currentDot->left;
        
        switch (currentOp->type)
        {
            case tree_new_variable:{
                                        
                                   }; break;
            case tree_assign:      {
                                        unsigned assign = (unsigned)LANGKWD_assign;
                                        assertSynt(currentOp, tree_assign, assign);
                                        assertSyntType(currentOp->left, tree_id);
                                        assert(exprCountFunctions(currentOp->rght) <= 1); GovnoCode
                                        
                                        if (currentOp->rght->type == tree_function)
                                        {
                                            translateFunctionCall(trans, currentOp->rght, transIdNum(trans, *(char**)currentOp->left->data));
                                        } else
                                        {
                                            translatePushValue(trans, currentOp->rght);
                                            fprintf(trans->asmFile, "\nPOP %%%lu\n", transIdNum(trans, *(char**)currentOp->left->data));
                                        }
                                   }; break;
            case tree_conditional: {
                                        fprintf(trans->asmFile, "\n; Calculating condition for 'if'\n");
                                        unsigned If = (unsigned)LANGKWD_if;
                                        assertSynt(currentOp, tree_conditional, If);
                                        assertSynt(currentOp->rght, tree_conditional, If);
                                        size_t condition = translateExpression(trans, currentOp->left);
                                        static size_t ifCounter = 0;
                                        size_t currentIfCounter = ifCounter;
                                        ifCounter++;
                                        fprintf(trans->asmFile, "\nJE %%%lu $0 $ELSE%lu\n", condition, currentIfCounter);
                                        currentOp = currentOp->rght;
                                        translateTree(trans, currentOp->left);
                                        fprintf(trans->asmFile, "\nJMP $IFEND%lu", currentIfCounter);
                                        fprintf(trans->asmFile, "\nELSE%lu:\n", currentIfCounter);
                                        translateTree(trans, currentOp->rght);
                                        fprintf(trans->asmFile, "\nIFEND%lu:\n", currentIfCounter);
                                   }; break;
            case tree_new_function:{
                                        fprintf(trans->asmFile, "\n;FUNCTION %s\n"
                                                                "%s:\n", *(char**)currentOp->left->data, *(char**)currentOp->left->data);
                                        currentOp = currentOp->rght;
                                        treeNode_t* argumentNode = currentOp->left;
                                        currentOp = currentOp->rght;
                                        fprintf(trans->asmFile, "\n  ;Pop function arguments\n");
                                        
                                        while ((argumentNode) && (argumentNode->left))
                                        {
                                            argumentNode = argumentNode->left;
                                        };
                                        while ((argumentNode) && (argumentNode->type != tree_new_function))
                                        {
                                            fprintf(trans->asmFile, "POP %%%lu\n", transIdNum(trans, *(char**)argumentNode->data));
                                            argumentNode = argumentNode->uppr;
                                        };
                                        
                                        fprintf(trans->asmFile, "\n  ;Function itself\n");
                                        translateTree(trans, currentOp);
                                        fprintf(trans->asmFile, "\n\nRET $0\n");
                                        fprintf(trans->asmFile, ";END OF FUNCTION %s\n", *(char**)currentDot->left->left->data);
                                   }; break;
            case tree_return:      {
                                        fprintf(trans->asmFile, " ;return\n");
                                        unsigned Return = (unsigned)LANGKWD_return;
                                        assertSynt(currentOp, tree_return, Return);
                                        size_t varNum = translateExpression(trans, currentOp->left);
                                        fprintf(trans->asmFile, "\nRET %%%lu\n", varNum);
                                        fprintf(trans->asmFile, " ;end of return\n");
                                   }; break;
            case tree_loop:        {
                                        static size_t loopCounter = 0;
                                        size_t currentLoopCounter = loopCounter;
                                        loopCounter++;
                                        
                                        fprintf(trans->asmFile, "\n; Calculating condition for loop\n");
                                        fprintf(trans->asmFile, "\nLOOPBEGIN%lu:\n", currentLoopCounter);
                                        unsigned While = (unsigned)LANGKWD_while;
                                        assertSynt(currentOp, tree_loop, While);
                                        size_t condition = translateExpression(trans, currentOp->left);

                                        fprintf(trans->asmFile, "\nJE %%%lu $0 $LOOPEND%lu\n", condition, currentLoopCounter);
                                        currentOp = currentOp->rght;
                                        translateTree(trans, currentOp);
                                        fprintf(trans->asmFile, "\nJMP $LOOPBEGIN%lu\n", currentLoopCounter);
                                        fprintf(trans->asmFile, "\nLOOPEND%lu:\n", currentLoopCounter);
                                   }; break;
            case tree_io:          {
                                        assertSyntType(currentOp, tree_io);
                                        if (*(unsigned*)currentOp->data == LANGKWD_print)
                                        {
                                            fprintf(trans->asmFile, "\n OUT "); 
                                            printVarNumOrDouble(trans, currentOp->left); 
                                            fprintf(trans->asmFile, "\n");
                                        } else
                                        if (*(unsigned*)currentOp->data == LANGKWD_scan)
                                        {
                                            assertSyntType(currentOp->left, tree_id);
                                            fprintf(trans->asmFile, "\n IN %%%lu \n", transIdNum(trans, *(char**)currentOp->left->data)); 
                                        };
                                   }; break;
            default:               {
                                        prln();
                                        printf("\n\nUnknown operator type %d(%s) passed to the %s function.\n\n", currentOp->type, LangKeywordsTypesStrings[currentOp->type], __func__);
                                        assert(!"Assertion failed"); GovnoCode
                                   }
        }
        
        currentDot = currentDot->rght;
    }
}

void printVarNameOrDouble(syntaxTreeTranslator_t* trans, treeNode_t* node)
{
    char returnStr[MAX_LINE_LEN] = "";
    
    if (node->type == tree_id)
    {
        sprintf(returnStr, "%s", *(char**)node->data);
    } else
    if (node->type == tree_double)
    {
        sprintf(returnStr, "$%lg", *(double*)node->data);
    } else
    {
        prln();
        printf("Unexpected node type %d(%s)", node->type, LangKeywordsTypesStrings[node->type]);
        assert(!"Assertion failed");
    }
    
    fprintf(trans->asmFile, " %s ", returnStr); 
}

void printVarNumOrDouble(syntaxTreeTranslator_t* trans, treeNode_t* node)
{
    if (node->type == tree_id)
    {
        fprintf(trans->asmFile, " %%%lu ", transIdNum(trans, *(char**)node->data));
    } else
    if (node->type == tree_double)
    {
        fprintf(trans->asmFile, " $%lg ", *(double*)node->data);
    } else
    {
        prln();
        printf("Unexpected node type %d(%s)", node->type, LangKeywordsTypesStrings[node->type]);
        assert(!"Assertion failed");
    }
}

void translateFunctionCall(syntaxTreeTranslator_t* trans, treeNode_t* funcNode, int returnVarNum)
{
    //PUSH arguments
    treeNode_t* argumentNode = funcNode;
    while (argumentNode->left)
    {
        argumentNode = argumentNode->left;
        fprintf(trans->asmFile, "PUSH"); 
        printVarNumOrDouble(trans, argumentNode);
        fprintf(trans->asmFile, "\n");
    }

    //Print function call
    fprintf(trans->asmFile, "\n"
                            "CALL $%s%30s\t;| \n"
                            "POP %%%lu%30s\t;| VAR%lu = %s( ",
                            *(char**)funcNode->data, "",
                            returnVarNum, "", returnVarNum, *(char**)funcNode->data);
    //Print arguments
    argumentNode = funcNode;
    while (argumentNode->left)
    {
        argumentNode = argumentNode->left;
        printVarNameOrDouble(trans, argumentNode);
        fprintf(trans->asmFile, ", ");
    }
    fprintf(trans->asmFile, ")\n");
}

int exprCountFunctions(treeNode_t* exprRoot)
{
    if (!exprRoot)
    {
        return 0;
    }
    return exprCountFunctions(exprRoot->left) + exprCountFunctions(exprRoot->rght) + ( (exprRoot->type == tree_function) ? 1 : 0 );
}

void translatePushValue(syntaxTreeTranslator_t* trans, treeNode_t* exprRoot)
{
    if (!exprRoot)
        return;
       
    translatePushValue(trans, exprRoot->left);
    translatePushValue(trans, exprRoot->rght);
    
    if (exprRoot->type == tree_mathOperator)
    {
        fprintf(trans->asmFile, "%sST\n", procMathCmd(*(unsigned*)exprRoot->data));
    } else
    if ( (exprRoot->type == tree_id) || (exprRoot->type == tree_double) )
    {
        fprintf(trans->asmFile, "PUSH"); 
        printVarNumOrDouble(trans, exprRoot);
        if (exprRoot->type == tree_id)
        {
            fprintf(trans->asmFile, "; %s", *(char**)exprRoot->data);
        }
        fprintf(trans->asmFile, "\n");
    } else
    {
        prln();
        printf("Unexpected node type %d(%s)", exprRoot->type, LangKeywordsTypesStrings[exprRoot->type]);
        assert(!"Assertion failed");
    }
}

int translateExpression(syntaxTreeTranslator_t* trans, treeNode_t* exprRoot)
{
    assert(trans); assert(trans->asmFile); assert(exprRoot);
    
    assert(exprCountFunctions(exprRoot) <= 1); GovnoCode
    
    int returnExpNum = transIdNum(trans);
    
    if (exprRoot->type == tree_function)
    {
        translateFunctionCall(trans, exprRoot, returnExpNum);
    } else
    {
        translatePushValue(trans, exprRoot);
        fprintf(trans->asmFile, "\nPOP %%%lu\n", returnExpNum);
    }
    //return 
    return returnExpNum;
}
