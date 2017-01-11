#define DO_OR_THROW(code) \
    try \
    { \
    code; \
    } \
    catch (ERRORS_T errCode) { printf("\n%s\n", ERR_ST(errCode));  throw(ERR_EMPTY); } \
    catch (ASMERRORS_T errCode) { printf("\nError ASMERRORS_T #(%i)\n", errCode); throw(ERR_EMPTY); } \
    catch (STACKERRORS_T errCode) { printf("\nError STACKERRORS_T #(%i)\n", errCode); throw(ERR_EMPTY); }; 
    
#undef NDEBUG

#include "Compiler/Compiler_unit_test.hpp"

int main(/*const int argc, const char *argv[]*/) Govnocode
{
    /*
        (\___/)
        (='.'= )
        (")_(")
    */

    ut_comp( (0 << UT_COMP_TOKEN_LIST_DUMP) | (1 << UT_COMP_SYNT_TREE_DUMP) , (1 << 0) );
    return 0;
}
