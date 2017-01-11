#include "Recursive_descent.hpp"

GR_ASSIGN(N,    {
                    assertTokenType(curTok, tree_double);
                    result = nodeRetCtor(tree_double, curTok->data, NULL, NULL, NULL);
                    movePtr();
                }
         )

GR_ASSIGN(VAR, {
                    if (curTok->type == tree_id)
                    {
                        result = nodeRetCtor(tree_id, curTok->idName, NULL, NULL, NULL);
                        if ( (curTok->next->type == tree_num_bracket) && (D_EQ(curTok->next->data, '(')) )
                        {
                            printif("\tI've found FUNCTION CALL!\n");
                            result->type = tree_function;
                            movePtr();
                            result->left = GR(ARGS); movePtr();
                        } else
                        {
                            movePtr();
                            printif("\tI've found VAR!\n");
                        }
                    } else
                    {
                        printif("\tI've found N!\n");
                        result = GR(N);
                    }
               }
         )

GR_ASSIGN(P,
              {
                     if ( ((int)curTok->type == (int)tree_num_bracket) && (curTok->data == '(') )
                     {
                         assertToken(curTok, tree_num_bracket, '('); movePtr();
                         result = GR(EXPR);
                         assertToken(curTok, tree_num_bracket, ')'); movePtr();
                     } else
                     {
                         result = GR(VAR);
                     }
              }
         )

GR_ASSIGN(MATH_FUNCS,
              {
                    if ((int)curTok->type == (int)tree_mathOperator)
                    {
                        if (D_EQ(curTok->data, '~'))
                        {
                            movePtr();
                            
                            unsigned mathOp = '^';
                            result = nodeRetCtor(tree_mathOperator, mathOp, NULL, GR(EXPR), NULL);
                            
                            double exponentValue = 0.5;
                            result->rght = nodeRetCtor(tree_double, exponentValue, NULL, NULL, NULL);
                            
                        } else
                        if (D_EQ(curTok->data, 'l'))
                        {
                            movePtr(); assertToken(curTok, tree_num_bracket, '(');
                            
                            unsigned mathOp = 'l';
                            result = nodeRetCtor(tree_mathOperator, mathOp, NULL, GR(EXPR), NULL);
                            
                            assertToken(curTok, tree_num_bracket, ')'); movePtr();
                        }
                    } else
                    {
                        result = GR(P);
                    }
              })

GR_ASSIGN(UNMIN,
              {
                    if ( (curTok->type == tree_mathOperator) && (curTok->data == '-') )
                    {
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result = nodeRetCtor(tree_mathOperator, opChar, NULL, NULL, NULL);
                        
                        double zero = 0;
                        result->left = nodeRetCtor(tree_double, zero, result, NULL, NULL);
                        
                        movePtr(); result->rght = GR(EXPR);
                    } else
                    {
                        result = GR(MATH_FUNCS); 
                    }
              }
         )

GR_ASSIGN(NEG,
              {
                    if ( (curTok->type == tree_mathOperator) && (curTok->data == 'f') )
                    {
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result = nodeRetCtor(tree_mathOperator, opChar, NULL, NULL, NULL);
                        
                        movePtr(); result->left = GR(EXPR);
                    } else
                    {
                        result = GR(UNMIN); 
                    }
              }
         )

GR_ASSIGN(D,
              {
                   result = GR(NEG); 
                    
                   while ( (curTok->type == tree_mathOperator) && (curTok->data == '^') )
                   {
                        assert(result);
                        
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result->left = _nodeRetCtor(result->type, result->data, result, result->left, result->rght);
                        
                        nodeCtor(result, tree_mathOperator, opChar, NULL, result->left, NULL);
                       
                        movePtr(); result->rght = GR(NEG);
                   }
              }
         )

GR_ASSIGN(T,
              {
                   result = GR(D); 
                    
                   while ( (curTok->type == tree_mathOperator) && ( (curTok->data == '*') || (curTok->data == '/') ) )
                   {
                        assert(result);
                        
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result->left = _nodeRetCtor(result->type, result->data, result, result->left, result->rght);
                        
                        nodeCtor(result, tree_mathOperator, opChar, NULL, result->left, NULL);
                       
                        movePtr(); result->rght = GR(D);
                   }
              }
         )

GR_ASSIGN(S,
              {
                   result = GR(T); 
                    
                   while ( (curTok->type == tree_mathOperator) && ( (curTok->data == '+') || (curTok->data == '-') ) )
                   {
                        assert(result);
                        
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result->left = _nodeRetCtor(result->type, result->data, result, result->left, result->rght);
                        
                        nodeCtor(result, tree_mathOperator, opChar, NULL, result->left, NULL);
                       
                        movePtr(); result->rght = GR(T);
                   }
              }
         )

GR_ASSIGN(EQ,
              {
                   result = GR(S); 
                    
                   if ( (curTok->type == tree_mathOperator) && ( (curTok->data == 'e') || (curTok->data == 'n') ) )
                   {
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result->left = _nodeRetCtor(result->type, result->data, result, result->left, result->rght);
                        
                        nodeCtor(result, tree_mathOperator, opChar, NULL, result->left, NULL);
                       
                        movePtr(); result->rght = GR(S);
                   }
              }
         )
         
GR_ASSIGN(GL,
              {
                   result = GR(EQ); 
                    
                   if ( (curTok->type == tree_mathOperator) && ( (curTok->data == 'G') || (curTok->data == 'L') || (curTok->data == '>') || (curTok->data == '<')) )
                   {
                        unsigned opChar = (unsigned)curTok->data;
                        
                        result->left = _nodeRetCtor(result->type, result->data, result, result->left, result->rght);
                        
                        nodeCtor(result, tree_mathOperator, opChar, NULL, result->left, NULL);
                       
                        movePtr(); result->rght = GR(EQ);
                   }
              }
          )
 
GR_ASSIGN(EXPR,
              {
                  return GR(GL);
              }
         )

GR_ASSIGN(RET, 
              {
                    assertToken(curTok, tree_return, LANGKWD_return); 
                    unsigned value = (unsigned)LANGKWD_return;
                    result = nodeRetCtor(tree_return, value, result, NULL, NULL);
                    movePtr();
                    result->left = GR(EXPR);
              } 
         )
         
GR_ASSIGN(LOOP, 
              {
                    assertToken(curTok, tree_loop, LANGKWD_while); movePtr();
                    unsigned value = (unsigned) LANGKWD_while;
                    result = nodeRetCtor(tree_loop, value, NULL, NULL, NULL);
                    
                    result->left = GR(EXPR);
                    result->rght = GR(OPBLOCK);
              }
         )
         
GR_ASSIGN(COND, 
              {
                    assertToken(curTok, tree_conditional, LANGKWD_if);
                    unsigned value = (unsigned)LANGKWD_if;
                    result = nodeRetCtor(tree_conditional, value, NULL, NULL, NULL);
                    movePtr();
                    result->left = GR(EXPR);
                    result->rght = nodeRetCtor(tree_conditional, value, result, NULL, NULL);
                    result->rght->left = GR(OPBLOCK);
                    if ( (curTok->next->type == tree_conditional) && (curTok->next->data == LANGKWD_else) )
                    {
                        assertToken(curTok, tree_delim, '.'); movePtr();
                        assertToken(curTok, tree_conditional, LANGKWD_else); movePtr();
                        result->rght->rght = GR(OPBLOCK);
                    }
              }
         )

GR_ASSIGN(IO,
              {
                  assertTokenType(curTok, tree_io);
                  if ( D_EQ(curTok->data, LANGKWD_scan) )
                  {
                    assertToken(curTok, tree_io, LANGKWD_scan);
                    unsigned value = (unsigned) LANGKWD_scan;
                    result = nodeRetCtor(tree_io, value, NULL, NULL, NULL);
                    
                    movePtr(); assertTokenType(curTok, tree_id);
                    assertToken(curTok->next, tree_delim, '.');
                    result->left = nodeRetCtor(tree_id, curTok->idName, result, NULL, NULL);
                    movePtr();
                  } else
                  {
                    assertToken(curTok, tree_io, LANGKWD_print);
                    unsigned value = (unsigned) LANGKWD_print;
                    result = nodeRetCtor(tree_io, value, NULL, NULL, NULL);
                    movePtr();
                    result->left = GR(EXPR);
                  }
              }
         )
         
GR_ASSIGN(NEWVAR,
              {
                  assertToken(curTok, tree_new_variable, LANGKWD_assign);
                  if ( D_EQ(curTok->data, LANGKWD_assign) )
                  {
                    assertToken(curTok, tree_new_variable, LANGKWD_assign);
                    unsigned value = (unsigned) LANGKWD_assign;
                    result = nodeRetCtor(tree_new_variable, value, NULL, NULL, NULL);
                    
                    movePtr(); assertTokenType(curTok, tree_id);
                    assertToken(curTok->next, tree_delim, '.');
                    result->left = nodeRetCtor(tree_id, curTok->idName, result, NULL, NULL);
                    movePtr();
                  };
              }
         )
         
GR_ASSIGN(VARASS,
              {
                  assertToken(curTok, tree_assign, LANGKWD_declare);
                  if ( D_EQ(curTok->data, LANGKWD_declare) )
                  {
                    assertToken(curTok, tree_assign, LANGKWD_declare);
                    unsigned value = (unsigned) LANGKWD_declare;
                    result = nodeRetCtor(tree_assign, value, NULL, NULL, NULL);
                    
                    movePtr(); assertTokenType(curTok, tree_id);
                    result->left = nodeRetCtor(tree_id, curTok->idName, result, NULL, NULL);
                    movePtr();
                    
                    result->rght = GR(EXPR);
                    
                    assertToken(curTok, tree_delim, '.');
                  };
              }
         )

GR_ASSIGN(OP, 
              {
                switch (curTok->type)
                {
                   case LANGKWT_new_variable:      {
                                                    result = GR(NEWVAR);
                                                }; break;
                                                
                   case LANGKWT_assign:            {
                                                    result = GR(VARASS);
                                                }; break;
                                                
                   case LANGKWT_operator_bracket:  {
                                                    if (D_EQ(curTok->data, LANGKWD_begin))
                                                        result = GR(OPBLOCK); else
                                                        assert(!"Unrecognized closing bracket");
                                                }; break;
                                                
                   case LANGKWT_conditional:       {
                                                    result = GR(COND);
                                                }; break;
                                                
                   case LANGKWT_return:            {
                                                    result = GR(RET);
                                                }; break;
                                                
                   case LANGKWT_double:
                   case LANGKWT_id:
                                                {
                                                    result = GR(EXPR);
                                                }; break;
                                                
                   case LANGKWT_loop:              {
                                                    result = GR(LOOP);
                                                }; break;
                                                
                   case LANGKWT_delim:             {
                                                    ;//Do nothing
                                                }; break;
                                                
                   case LANGKWT_io:                {
                                                    result = GR(IO);
                                                }; break;
                                                
                   default:                     {
                                                    printf("\n\033[31mError: unrecognized operator\033[0m\n\n");
                                                    tokenDump(curTok);
                                                    printf("\n\n\033[31mIn Line\033[0m\n");
                                                    printLine(curTok->line);
                                                    
                                                    throw(COMPERR_UNKOP);
                                                }; break;
                };
                assertToken(curTok, tree_delim, '.'); movePtr();
          }
        )

GR_ASSIGN(OPBLOCK,
              {
                    //OPERATOR_BRACKET_OPENING
                    assertToken(curTok, tree_operator_bracket, LANGKWD_begin);
                    movePtr();
                    //OPERATORS
                    
                    treeNode_t* temp = NULL;
                    
                    size_t opCounter = 0;
                    while ( ( (int)curTok->type != (int)tree_operator_bracket ) && (opCounter < COMP_MAX_OP_QT) )
                    {
                        if (opCounter > 0)
                        {
                            temp->rght = nodeRetCtor(tree_delim, dot, NULL, NULL, NULL);
                            temp = temp->rght;
                        } else
                        {
                            result = nodeRetCtor(tree_delim, dot, NULL, NULL, NULL);
                            temp = result;
                        }
                        temp->left = GR(OP);
                        opCounter++;
                    }
                    assert(opCounter < COMP_MAX_OP_QT);
                        
                    //OPERATOR_BRACKET_CLOSING
                    assertToken(curTok, tree_operator_bracket, LANGKWD_end);
                    movePtr(); assertToken(curTok, tree_delim, '.');
              }
         )

GR_ASSIGN(ARGS, 
              {
                        assertToken(curTok, tree_num_bracket, '('); movePtr();
                        //treeNode_t* result = (treeNode_t*)calloc(1, sizeof(*result));
                        size_t argCounter = 0;
                        treeNode_t* funcArgs = NULL;//result;
                        if ( (int)curTok->type != (int)tree_num_bracket )
                        {
                            assertToken(curTok, tree_new_function, LANGKWD_pass); movePtr();
                            assertTokenType(curTok, tree_id);
                            result = nodeRetCtor(tree_id, curTok->idName, NULL, NULL, NULL); movePtr();
                            funcArgs = result;
                        }
                        while ( ( (int)curTok->type != (int)tree_num_bracket ) && (argCounter < COMP_MAX_FUNC_ARG_QT) )
                        {
                            assertToken(curTok, tree_delim, ','); movePtr(); 
                            assertToken(curTok, tree_new_function, LANGKWD_pass); movePtr();
                            assertTokenType(curTok, tree_id);
                            funcArgs->left = nodeRetCtor(tree_id, curTok->idName, NULL, NULL, NULL);
                            funcArgs = funcArgs->left;
                            movePtr(); argCounter++;
                        };
                        assert(argCounter < COMP_MAX_FUNC_ARG_QT);
                        //NUM_BRACKET_CLOSING
                        assertToken(curTok, tree_num_bracket, ')');
              }
          )

GR_ASSIGN(PROGRAM,
              {
                  /**SEE /Tree_structure_syntax/Function.pdf*/
                    //PISANIE
                    assertToken(curTok, tree_new_program, LANGKWD_program);
                    unsigned value = (unsigned) LANGKWD_program;
                    result = nodeRetCtor(tree_new_program, value, NULL, NULL, NULL);
                    
                    treeNode_t* temp = result;
                    //PISANIE NAME
                    movePtr(); assertTokenType(curTok, tree_id);
                    temp->left = nodeRetCtor(tree_id, curTok->idName, temp, NULL, NULL); movePtr();
                    //FUNCTIONS
                    size_t funcCounter = 0;
                    while ( ( (int)curTok->type == (int)tree_new_function ) && (funcCounter < COMP_MAX_FUNC_QT) )
                    {
                        temp->rght = nodeRetCtor(tree_delim, dot, temp, NULL, NULL);
                        temp = temp->rght;
                        treeNode_t* newFunction = temp;
                        
                        //IMENEM GOSPODA BOGA
                        unsigned value = (unsigned)LANGKWD_function;
                        assertToken(curTok, tree_new_function, LANGKWD_function);
                        newFunction->left = nodeRetCtor(tree_new_function, value, newFunction, NULL, NULL);
                        newFunction = newFunction->left;
                        
                        //FUNCTION NAME
                        movePtr(); assertTokenType(curTok, tree_id);
                        newFunction->left = nodeRetCtor(tree_id, curTok->idName, newFunction,  NULL, NULL);
                        //IMENEM GOSPODA BOGA 2
                        movePtr();
                        newFunction->rght = nodeRetCtor(tree_new_function, value, newFunction, NULL, NULL);
                        newFunction = newFunction->rght;
                        
                        newFunction->left = GR(ARGS);
                        
                        //OP_BLOCK
                        movePtr(); newFunction->rght = GR(OPBLOCK);
                        if (curTok->next)
                        {
                            movePtr();
                        }
                    }
                    assert(funcCounter < COMP_MAX_FUNC_QT);
              }
         )

GR_ASSIGN(G0,
              {
                    result = GR(PROGRAM); redispatchUppr(result);
              }
         )
