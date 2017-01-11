//UT ASM stencil: UT ASM(testNum, testName, "inputFileName")
#define ut_asm_path /home/philipp/Documents/Ded/ut_asm
#define quot(a) ( #a )
#define _UT_ASM(testNum, testName, dir, path) DEF_UT_ASM(testNum, testName, quot(dir/path)  ) 
#define UT_ASM(testNum, testName, path) _UT_ASM(testNum, testName, ut_asm_path, path/testName )
//    Correct:
UT_ASM(1, Read%, Correct)
UT_ASM(2, Read$, Correct)
UT_ASM(3, No argument, Correct)
UT_ASM(4, Three different arguments, Correct)
UT_ASM(5, Comments and indentations, Correct)
UT_ASM(6, Complex, Correct)
UT_ASM(7, ASM_Lupj, Correct)
//UT_ASM(7, Label, Correct)

//    Incorrect:
UT_ASM(8, EMPTY, Incorrect)
UT_ASM(9, EXTRAARG, Incorrect)
UT_ASM(10, TOOFEW, Incorrect)
UT_ASM(11, TWOCMD, Incorrect)
UT_ASM(12, UNKCMD, Incorrect)
UT_ASM(13, WRGARGTYPE, Incorrect)
UT_ASM(14, COMINSARG, Incorrect)
#undef ut_asm_path
#undef quot
#undef UT_ASM
#undef _UT_ASM