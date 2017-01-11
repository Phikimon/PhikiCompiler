//UT PROC stencil: UT PROC(testNum, testName, "inputFileName")
#define ut_proc_path /home/philipp/Documents/Ded/ut_proc
#define quot(a) ( #a )
#define _UT_PROC(testNum, testName, dir, path) DEF_UT_PROC(testNum, testName, quot(dir/path)  ) 
#define UT_PROC(testNum, testName, path) _UT_PROC(testNum, testName, ut_proc_path, path/testName )
//    Correct:
UT_PROC(1, Complex, Correct)
UT_PROC(2, Factorial, Correct)
UT_PROC(3, Factorial_recursive, Correct)
UT_PROC(4, DoubleComplex, Correct)
UT_PROC(5, Stack_operations, Correct)
UT_PROC(6, ASM_Lupj, Correct)
#undef ut_proc_path
#undef quot
#undef UT_PROC
#undef _UT_PROC