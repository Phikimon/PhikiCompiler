//UT_COMP(testNum0, testName0)

#define quot(a) ( #a )
#define _UT_COMP(testNum, testName, fileName) DEF_UT_COMP(testNum, #testName, #fileName) 
#define UT_COMP(testNum, testName) _UT_COMP(testNum, testName, testName.holy)

UT_COMP(1, EmptyMain) 
UT_COMP(2, Zhest)
UT_COMP(3, Square_Lite)
UT_COMP(4, MultipleFunctions)
UT_COMP(5, Loops)
UT_COMP(6, Function_Call)
UT_COMP(7, Factorial)
UT_COMP(8, Recursive)
UT_COMP(9, Series)


#undef quot
#undef _UT_COMP
#undef UT_COMP
