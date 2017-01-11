/**
    By converting dumb enums to macros, I learnt how to use regular expressions like this:
    Replace
    '([\w]), ([%*$][%*$][%*$]),'
    with
    '\1, 3, \2,'
     * 
    Replace
    '\(([A-Z2]+), ([\d])([^\d]++)([\d]++)\)'
    with
    '(\4, \1, \2\3)'
     *
    and so on
 */
 // DEF CMD stencil: DEF_CMD(CMD_NAME, CMD_ARG_STRING, CMD_CODE)
//for example, for ADD %1  $123   %2 
//              args = {1,  123,   2}; 
//           argType = {1,  0,     1}.
//1 - register(%); 0 - immediate($)
#define _printf(...) if (verbose) { printf(__VA_ARGS__); } else {;}
#define VAL(argNum) ( (argType[argNum] == at_reg) ? proc->registers[(unsigned)args[argNum]] : args[argNum] )
#define _PRINT_VAL(argNum, string_reg, string_imm) \
if (argType[argNum] == at_reg) \
{ \
    _printf(string_reg, (unsigned)args[argNum], proc->registers[(unsigned)args[argNum]]); \
} else \
{ \
   _printf(string_imm, args[argNum]); \
}; 
#define PRINT_VAL(argNum, string_reg, string_imm) \
if (argType[argNum] == at_reg) \
{ \
    printf(string_reg, (unsigned)args[argNum], proc->registers[(unsigned)args[argNum]]); \
} else \
{ \
   printf(string_imm, args[argNum]); \
}; 
//NOP           - do nothing
DEF_CMD(NOP, 0, {
                    moveProgramCounter(proc, argQt);
                } 
       ) 

//INR %ax $a    - put value $a in register %ax
DEF_CMD(INR, 2%$,{
                    unsigned value = 0; 
                    proc->registers[(unsigned)args[0]] = args[1]; 
                    _printf("\nINR:   reg[%%%u] = $%lg\n", (unsigned)args[0], args[1]); 
                    
                    moveProgramCounter(proc, argQt);
                 } 
       )
       
//CPUID          - PUSH 'signature' (see processor datasheet)
DEF_CMD(CPUID, 0, {
                    _printf("\nCPUID: $0x%x\n", CPUID_VAL);
                    
                    moveProgramCounter(proc, argQt);
                  } 
       )
       
//MOV %ax %bx   - move value from %bx to %ax
DEF_CMD(MOV, 2%%, { 
                    proc->registers[(unsigned)args[0]] = proc->registers[(unsigned)args[1]];
                    _printf("\nMOV:   %%%u($%lg) -> %%%u\n", (unsigned)args[1], proc->registers[(unsigned)args[1]], (unsigned)args[0]); 
                    
                    moveProgramCounter(proc, argQt);
                  } 
       ) 
       
//POP %ax       - Pops value from stack. ax: index of the register
DEF_CMD(POP, 1%, { 
                    proc->registers[(unsigned)args[0]] = stackPop(proc->dataStack); 
                    _printf("\nPOP:   ($%lg) -> %%%u\n", proc->registers[(unsigned)args[0]], (unsigned)args[0]); 
                    
                    moveProgramCounter(proc, argQt);
                 } 
        )
        
//PUSH [%/$]ax  - Pushs value in stack. ax - index of the register or integer
DEF_CMD(PUSH, 1*, { 
                    stackPush(proc->dataStack, VAL(0)); 
                    
                    _PRINT_VAL(0, "\nPUSH:  stack <- %%%u($%lg)\n", "\nPUSH:  stack <- $%lg\n"); 
                    
                    moveProgramCounter(proc, argQt);
                  } 
       )

//JMP $label    - Unconditional jump to 'label'
DEF_CMD(JMP, 1$, { 
                    _printf("\nJUMP\n");
                    CPU_CMD_JUMP(proc, args[0]);
                 } 
       )


/*
 Conditional jumps. All comparisons are signed.
 'a' and 'b' can be either indexes of registers
 or integers. 'addr' can be integer or named label
 */
DEF_CMD(JE, 3**$, { 
                    _PRINT_VAL(0, "\nJE:    ( %%%u($%lg) == ", "JE:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf((D_EQ(VAL(0), VAL(1))) ? "true => goto 'label'\n" : "false => programCounter++\n");
                    if (D_EQ(VAL(0), VAL(1)))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                  } 
       )

DEF_CMD(JNE, 3**$, { 
                    _PRINT_VAL(0, "\nJNE:   ( %%%u($%lg) != ", "JNE:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf(!D_EQ(VAL(0), VAL(1)) ? "true => goto 'label'\n" : "false => programCounter++\n");
                    if (!D_EQ(VAL(0), VAL(1)))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                   } 
       )
/*
DEF_CMD(JL, 3**$, { 
                    _PRINT_VAL(0, "\nJL:    ( %%%u($%lg) < ", "JL:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf((VAL(0) < VAL(1)) ? "true => goto 'label'\n" : "false => programCounter++\n");

                    if (VAL(0) < VAL(1))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                  } 
       )

DEF_CMD(JG, 3**$, { 
                    _PRINT_VAL(0, "\nJG:    ( %%%u($%lg) > ", "JG:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf((VAL(0) > VAL(1)) ? "true => goto 'label'\n" : "false => programCounter++\n");
                    if (VAL(0) > VAL(1))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                  } 
       )

DEF_CMD(JGE, 3**$, { 
                    _PRINT_VAL(0, "\nJGE:   ( %%%u($%lg) >= ", "JGE:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf((VAL(0) >= VAL(1)) ? "true => goto 'label'\n" : "false => programCounter++\n");
                    if (VAL(0) >= VAL(1))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                   } 
       )
*/
DEF_CMD(JLE, 3**$, { 
                    _PRINT_VAL(0, "\nJLE:   ( %%%u($%lg) <= ", "JLE:    ( $%lg < ");
                    _PRINT_VAL(1, "%%%u($%lg) ) == ", " $%lg ) == ");
                    _printf((VAL(0) <= VAL(1)) ? "true => goto 'label'\n" : "false => programCounter++\n");
                    if (VAL(0) <= VAL(1))
                        CPU_CMD_JUMP(proc, args[2]);      else
                        moveProgramCounter(proc, argQt);
                   } 
       )

       
//CALL $label   - Call function 'label' 
DEF_CMD(CALL, 1$, { 
                    _printf("\nCALL\n");
                    stackPush(proc->callStack, proc->programCounter);
                    CPU_CMD_JUMP(proc, args[0]);
                  } 
       )
       
//RET           - Return to place from where 'CALL' was called last time
DEF_CMD(RET, 1*, { 
                    _printf("\nRET\n");
                    
                    stackPush(proc->dataStack, VAL(0));
                    
                    proc->programCounter = stackPop(proc->callStack);

                    moveProgramCounter(proc, 1/*argQt(CALL)*/);

                 } 
       )

//IN %ax        - Put value from keyboard into %ax
DEF_CMD(IN, 1%, {
                    _printf("\nIN:    reg[%%%u] = $", (unsigned)args[0]); 
                    scanf("%lg", &proc->registers[(unsigned)args[0]]); 
                    
                    moveProgramCounter(proc, argQt);
                } 
       )
       
//OUT %ax       - Printf value from ax to keyboard
DEF_CMD(OUT, 1*, { 
                    _PRINT_VAL(0, "\nOUT:   %%%u($%lg)\n", "\nOUT:   $%lg\n"); 

                    if (verbose == 0) printf("\n%lg\n", VAL(0));
                    
                    moveProgramCounter(proc, argQt);
                 } 
       )
       
//ADD %ax [%/$]bx [%/$]cx - %ax = bx + cx
DEF_CMD(ADD, 3%**, { 
                        //Dump
                        _printf("\nADD:   %%%u <-", (unsigned)args[0]);
                        //Printf first augent
                        unsigned argNum = 1;
                        _PRINT_VAL(1, " ( %%%lu($%lg) +", " $%lg +"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) + VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
          
//SUB %ax [%/$]bx [%/$]cx - %ax = bx - cx
DEF_CMD(SUB, 3%**, { 
                        //Dump
                        _printf("\nSUB:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) -", " $%lg -"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) - VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
         
//MUL %ax [%/$]bx [%/$]cx - %ax = bx * cx
DEF_CMD(MUL, 3%**, { 
                        //Dump
                        _printf("\nMUL:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) *", " $%lg *"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) * VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//DIV %ax [%/$]bx [%/$]cx - %ax = bx / cx 
DEF_CMD(DIV, 3%**, { 
                        //Dump
                        _printf("\nDIV:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) /", " $%lg /"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) / VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

/*//AND %ax [%/$]bx [%/$]cx - %ax = bx && cx
DEF_CMD(AND, 3%**, { 
                        //Dump
                        _printf("\nAND:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) &&", " $%lg &&"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) && VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//OR  %ax [%/$]bx [%/$]cx - %ax = bx ||  cx
DEF_CMD(OR, 3%**, { 
                        //Dump
                        _printf("\nOR:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) ||", " $%lg ||"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = VAL(1) || VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//XOR %ax [%/$]bx [%/$]cx - %ax = bx (+) cx
DEF_CMD(XOR, 3%**, { 
                        //Dump
                        _printf("\nXOR:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) (+)", " $%lg (+)"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = (unsigned)VAL(1) ^ (unsigned)VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//NEG %ax [%/$]bx         - %ax = ¬ bx
DEF_CMD(NEG, 2%*, { 
                        //Dump
                        _printf("\nXOR:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( ¬%%%u($%lg)", " ¬$%lg"); 
                        
                        proc->registers[(unsigned)args[0]] = !(VAL(1));
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//SHL %ax [%/$]bx [%/$]cx - %ax = (bx<<cx)
DEF_CMD(SHL, 3%**, { 
                        //Dump
                        _printf("\nSHL:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) <<", " $%lg <<"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = (unsigned)VAL(1) << (unsigned)VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//SHR %ax [%/$]bx [%/$]cx - %ax = (bx>>cx)
DEF_CMD(SHR, 3%**, { 
                        //Dump
                        _printf("\nSHR:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) >>", " $%lg >>"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = (unsigned)VAL(1) >> (unsigned)VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
*/
//LAND %ax [%/$]bx [%/$]cx - %ax = bx & cx
DEF_CMD(LAND, 3%**, { 
                        //Dump
                        _printf("\nLAND:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) &", " $%lg &"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = (unsigned)VAL(1) & (unsigned)VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//LOR  %ax [%/$]bx [%/$]cx - %ax = bx |  cx
DEF_CMD(LOR, 3%**, { 
                        //Dump
                        _printf("\nOR:   %%%u <-", args[0]);
                        //Printf first augent
                        _PRINT_VAL(1, " ( %%%u($%lg) |", " $%lg |"); 
                        //Printf second augent
                        _PRINT_VAL(2, " %%%u($%lg) )", " $%lg )");
                        
                        proc->registers[(unsigned)args[0]] = (unsigned)VAL(1) | (unsigned)VAL(2);
                        
                        //Printf result
                        _printf("($%lg)\n", proc->registers[(unsigned)args[0]]);
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//MULST
DEF_CMD(MULST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double mult = factor1*factor2;
                        stackPush(proc->dataStack, mult);
                        
                        //Dump
                        _printf("\nMULST:   $%lg <- $%lg * $%lg ", mult, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//DIVST
DEF_CMD(DIVST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1/factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nMULST:   $%lg <- $%lg / $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//ADDST
DEF_CMD(ADDST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 + factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nMULST:   $%lg <- $%lg + $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//SUBST
DEF_CMD(SUBST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 - factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nMULST:   $%lg <- $%lg - $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//LNEST
DEF_CMD(LNEST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = !D_EQ(factor1, factor2);
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLNEST:   $%lg <- $%lg != $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//LEST
DEF_CMD(LEST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = D_EQ(factor1, factor2);
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLEST:   $%lg <- $%lg == $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//LGST
DEF_CMD(LGST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 > factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLGST:   $%lg <- $%lg > $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//LGEST
DEF_CMD(LGEST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 >= factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLGEST:   $%lg <- $%lg >= $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//LLST
DEF_CMD(LLST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 < factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLLST:   $%lg <- $%lg < $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

//LLEST
DEF_CMD(LLEST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = factor1 <= factor2;
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nLLEST:   $%lg <- $%lg <= $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )
                   
//POWST
DEF_CMD(POWST, 0, { 
                        double factor2 = stackPop(proc->dataStack);
                        double factor1 = stackPop(proc->dataStack);
                        double res = pow(factor1, factor2);
                        stackPush(proc->dataStack, res);
                        
                        //Dump
                        _printf("\nPOWST:   $%lg <- $%lg <= $%lg ", res, factor1, factor2);
                        
                        //Go on
                        moveProgramCounter(proc, argQt);
                   } )

/*DEF_CMD(SIN, 2%*, { ;} )
DEF_CMD(COS, 2%*, { ;} )
DEF_CMD(TG, 2%*, { ;} )
DEF_CMD(CTG, 2%*, 20, { ;} )
DEF_CMD(FLDL2E, 0, { ;} )
DEF_CMD(FLDL2T, 0, { ;} )
DEF_CMD(FLDLG2, 0, 30, { ;} )
DEF_CMD(FLDLN2, 0, { ;} )
DEF_CMD(FLDPI, 0, { ;} )*/


#undef _printf
#undef VAL
#undef _PRINT_VAL
#undef PRINT_VAL