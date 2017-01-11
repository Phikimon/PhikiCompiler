#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <typeinfo>
#include <limits.h>
#include <math.h>
#include <clocale>

//What needs refactoring
#define GovnoCode
#define Govnocode

//Debug
#undef NDEBUG
#define STUPIDUSER

#define _assert(cond) \
{ \
    if (!(cond)) \
    { \
        int* a = NULL; \
        *a = 0; \
    }; \
    assert(cond); \
}


//Debug checks

#ifdef STUPIDUSER

#define throw(a) { prln(); throw(a); }
#define prln() printf("\n FILE %s; LINE %i\n", __FILE__, __LINE__)
#define prdel() printf("\n___________________________________________________________________\n")
#define prhello() printf("\n\033[1m\033[31m\033[4m\033[5mI am here! Line №%i\033[0m\n", __LINE__);

 /*
  Assert and print error message
 */
#define assMes(cond, mes)                        \
 if (!(cond))                                    \
 {                                               \
   assert(!mes);                                 \
 }

 /*
  Check for array overrun, or for validity of counter. 'belTo' stands for 'belongs to'
 */
#define belTo(index, left, right, mes)           \
 if ( ( index < left ) || ( index >  right ) )   \
 {                                               \
   assert(!mes);                                 \
 }

#define belToLeft(index, left, mes)              \
 if ( index < left )                             \
 {                                               \
   assert(!mes);                                 \
 }

#define belToRight(index, right, mes)            \
 if ( index > right )                            \
 {                                               \
   assert(!mes);                                 \
 }

#define belToEx(index, left, right, code)        \
 if ( ( index < left ) || ( index >  right ) )   \
 {                                               \
   throw(code);                                  \
 }

#define belToLeftEx(index, left, code)           \
 if ( index < left )                             \
 {                                               \
   throw(code);                                  \
 }

#define belToRightEx(index, right, code)         \
 if ( index > right )                            \
 {                                               \
   throw(code);                                  \
 }

#else

#define assMes(cond, mes)

#define belTo(index, left, right, mes)

#define belToLeft(index, left, mes)

#define belToRight(index, right, mes)

#define belToLeftEx(index, left, code)

#define belToRightEx(index, right, code)

#endif

//Some constants

#define MAX_LINE_LEN 256
Govnocode //Some constants not needed in common.hpp
#define START_FILE_SIZE 16
#define ARGTYP_MAX_DIGITS 10
#define MACH_EPS 1e-10

//Some error messages/status codes

typedef unsigned char hashType;

#define CNTR_INV "Error: Counter is invalid"

enum ERRORS_T
{
    ERR_EMPTY    =   0, //< Nothing is printed
    ERR_NOERR    =  -1, //< Error: no such error exists :)
    ERR_STD      =  -2, //< Error: user, call the administrator
    ERR_ALLOC    =  -3, //< Error: failed to allocate memory
    ERR_OPFILER  =  -4, //< Error: failed to open file for reading(doesn't exist)
    ERR_OPFILEW  =  -5, //< Error: failed to open file for writing(already busy)
    ERR_LONG     =  -6, //< Error: too long file name
    ERR_MANY     =  -7, //< Error: too many arguments/parameters
    ERR_MISSO    =  -8, //< Error: missing '-o' parameter
    ERR_WRINPAR  =  -9, //< Error: wrong input option
    ERR_EXCHAR   = -10, //< Error: Excess character in option
    ERR_NOUTNAME = -11, //< Error: no output file name found
    ERR_UNKPAR   = -12, //< Error: Unknown option
    ERR_TWICE    = -13, //< Error: twice entered option
    ERR_SCANF    = -14, //< Error: scanf can't read what you asked for
    ERR_FREAD    = -15, //< Error: can't read from file
    ERR_COMMON   = -16, //< Error: not specified error occured
    ERR_OPFILE   = -17, //< Error: failed to open file
    ERR_QT        = -18,
};

extern const char* _ERR_ST[-ERR_QT];

//Returns string that represents error
const char* ERR_ST(ERRORS_T ERR);

//My collection of potions

#define POISON_CHAR '\0'
#define POISON_INT 0xDEADBEEF

//Useful defines

#define free(a)   { free(a);   (a) = NULL; }

#define fclose(a) { fclose(a); (a) = NULL; }

#define FOPEN(a) \
{ \
    if (!(a)) \
    { \
        prln(); \
        throw (ERR_OPFILE); \
    } \
}

#define ALLOC(a) \
if (!(a)) \
{ \
    prln(); \
    throw(ERR_ALLOC); \
}

#define checkSum(smth, verb) { assert(strlen(#smth) <= MAX_LINE_LEN); _checkSum(smth, verb, #smth); }

//Common functions

unsigned myRandom(unsigned seed);

unsigned genSeed();

bool D_EQ(double a, double y);

#endif
