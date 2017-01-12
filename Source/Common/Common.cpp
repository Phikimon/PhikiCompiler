#include "Common.hpp"
#include <ctime> 
#include <math.h>

extern const char POISON_CHAR     = (char)0xDE;
extern const int  POISON_INT      = (int)0xDEADBEEF;
extern const size_t POISON_SIZE_T = 0xDEADBEEFDEADBEEF;

const char* _ERR_ST[-ERR_QT] =
{
    "",
    "\nError: no such error exists :)\n",
    "\nError: user, call the administrator\n",
    "\nError: failed to allocate memory\n",
    "\nError: failed to open file for reading(doesn't exist)\n",
    "\nError: failed to open file for writing(already busy)\n",
    "\nError: too long file name\n",
    "\nError: too many arguments/options\n",
    "\nError: missing '-o' option\n",
    "\nError: wrong input option\n",
    "\nError: Excess character in option\n",
    "\nError: no output file name found\n",
    "\nError: Unknown option\n",
    "\nError: twice entered option\n",
    "\nError: scanf can't read what you asked for\n",
    "\nError: can't read from file\n",
    "\nError: not specified error occured\n",
};

const char* ERR_ST(ERRORS_T ERR)
{
    return _ERR_ST[-ERR];
};

size_t myRandom(int seed)
{
    return ((size_t)seed*0xC0CAC01A - 0xF00DCAFE) + 
          (((size_t)seed*0xDEADF00D - 0xABCDEFEF) << 8 * 4);
            //I was hungry =(
};

int genSeed()
{
    int seed = ((int)clock() * rand());
    srand((unsigned)seed * (unsigned)((size_t)&seed & 0xFFFFFFFF));
    return rand() * seed;
};

bool D_EQ(double a, double y)
{
    return fabsl(a - y) <= MACH_EPS;
};
