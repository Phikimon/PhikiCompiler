 // DEF CMD template: DEF_CMD(CMD_NAME, ARG_QT, ARGS_ADDR_MODE)
//3 - any(*); 2 - register(%); 1 - immediate($), 0 - no argument
//
//Set addressing modes
#define NO_ARGS 0
#define REG_ARG(num) (ARG_REG << ((num - 1) * 2))
#define IMM_ARG(num) (ARG_IMM << ((num - 1) * 2))
#define ANY_ARG(num) (ARG_ANY << ((num - 1) * 2))
#define UNSAFE_DEF_CMD(CMD_NAME, ARG_QT,  ARGS_ADDR_MODE ) \
        DEF_CMD(CMD_NAME, ARG_QT, (ARGS_ADDR_MODE))
//--------------------------------------BASIC-------------------------------------------
UNSAFE_DEF_CMD(NOP,     0, NO_ARGS)
UNSAFE_DEF_CMD(CPUID,   0, NO_ARGS)

UNSAFE_DEF_CMD(INR,     2, REG_ARG(1) | IMM_ARG(2))
UNSAFE_DEF_CMD(MOV,     2, REG_ARG(2) | REG_ARG(1))
//--------------------------------------BASIC-END---------------------------------------

//--------------------------------------CALL-RET----------------------------------------
UNSAFE_DEF_CMD(CALL,    1, IMM_ARG(1))
UNSAFE_DEF_CMD(RET,     0, NO_ARGS)
//--------------------------------------CALL-RET END------------------------------------

//--------------------------------------I/O---------------------------------------------
UNSAFE_DEF_CMD(IN,      1, REG_ARG(1))
UNSAFE_DEF_CMD(OUT,     1, ANY_ARG(1))
//--------------------------------------I/O-END-----------------------------------------

//--------------------------------------ARITHM-INSTRUCTIONS-----------------------------
UNSAFE_DEF_CMD(ADD,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(SUB,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(MUL,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(DIV,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(POW,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))

UNSAFE_DEF_CMD(LG,      3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LL,      3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LGE,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LLE,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LE,      3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LNE,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))

UNSAFE_DEF_CMD(LAND,    3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))
UNSAFE_DEF_CMD(LOR,     3, REG_ARG(1) | ANY_ARG(2) | ANY_ARG(3))

//----------------------------------BIN-ARITHM-INSTRUCTIONS-----------------------------
UNSAFE_DEF_CMD(ADDBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(SUBBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(MULBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(DIVBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(POWBIN,  2, REG_ARG(1) | ANY_ARG(2))

UNSAFE_DEF_CMD(LGBIN,   2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LLBIN,   2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LGEBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LLEBIN,  2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LEBIN,   2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LNEBIN,  2, REG_ARG(1) | ANY_ARG(2))

UNSAFE_DEF_CMD(LANDBIN, 2, REG_ARG(1) | ANY_ARG(2))
UNSAFE_DEF_CMD(LORBIN,  2, REG_ARG(1) | ANY_ARG(2))
//--------------------------------------ARITHM-INSTRUCTIONS-END-------------------------
UNSAFE_DEF_CMD(POP,     1, REG_ARG(1))
UNSAFE_DEF_CMD(PUSH,    1, ANY_ARG(1))
UNSAFE_DEF_CMD(MOVST,   2, REG_ARG(1) | ANY_ARG(1))
//--------------------------------STACK-ARITHM-INSTRUCTIONS-----------------------------
UNSAFE_DEF_CMD(ADDST,   0, NO_ARGS)
UNSAFE_DEF_CMD(SUBST,   0, NO_ARGS)
UNSAFE_DEF_CMD(MULST,   0, NO_ARGS)
UNSAFE_DEF_CMD(DIVST,   0, NO_ARGS)
UNSAFE_DEF_CMD(POWST,   0, NO_ARGS)

UNSAFE_DEF_CMD(LGST,    0, NO_ARGS)
UNSAFE_DEF_CMD(LLST,    0, NO_ARGS)
UNSAFE_DEF_CMD(LGEST,   0, NO_ARGS)
UNSAFE_DEF_CMD(LLEST,   0, NO_ARGS)
UNSAFE_DEF_CMD(LEST,    0, NO_ARGS)
UNSAFE_DEF_CMD(LNEST,   0, NO_ARGS)

//UNSAFE_DEF_CMD(LANDST,  0, NO_ARGS)
//UNSAFE_DEF_CMD(LORST,   0, NO_ARGS)
//--------------------------------STACK-ARITHM-INSTRUCTIONS-END-------------------------
UNSAFE_DEF_CMD(JMP,     1, IMM_ARG(1))
//---------------------------CONDITIONAL-JUMPS-INSTRUCTIONS-----------------------------
UNSAFE_DEF_CMD(JE,      3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
UNSAFE_DEF_CMD(JNE,     3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
UNSAFE_DEF_CMD(JL,      3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
UNSAFE_DEF_CMD(JLE,     3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
UNSAFE_DEF_CMD(JG,      3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
UNSAFE_DEF_CMD(JGE,     3, ANY_ARG(1) | ANY_ARG(2) | IMM_ARG(3))
//---------------------------CONDITIONAL-JUMPS-INSTRUCTIONS-END-------------------------

/*UNSAFE_DEF_CMD(SIN, 2%*, { ;} )
UNSAFE_DEF_CMD(COS, 2%*, { ;} )
UNSAFE_DEF_CMD(TG, 2%*, { ;} )
UNSAFE_DEF_CMD(CTG, 2%*, 20, { ;} )
UNSAFE_DEF_CMD(FLDL2E, 0, { ;} )
UNSAFE_DEF_CMD(FLDL2T, 0, { ;} )
UNSAFE_DEF_CMD(FLDLG2, 0, 30, { ;} )
UNSAFE_DEF_CMD(FLDLN2, 0, { ;} )
UNSAFE_DEF_CMD(FLDPI, 0, { ;} )*/


#undef NO_ARGS
#undef REG_ARG
#undef IMM_ARG
#undef ANY_ARG
