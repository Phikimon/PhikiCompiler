#include "Common/Common.hpp"

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

unsigned myRandom(unsigned seed)
{
    return seed*0xC0CAC01A-0xF00DCAFE; //I was hungry =(
};

unsigned genSeed()
{
    unsigned seed = 0;
    unsigned *ptr = (unsigned*)malloc( sizeof(*ptr) );
    srand((*ptr + (unsigned)rand()));
    seed = (unsigned)rand();
    free(ptr);
    return seed;
}

bool D_EQ(double a, double y)
{
    return fabsl(a - y) <= MACH_EPS;
}
