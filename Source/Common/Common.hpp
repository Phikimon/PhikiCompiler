#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <cstdlib>

#ifndef __cplusplus
typedef enum { false, true } bool;
#endif


//What needs refactoring
#define GovnoCode
#define Govnocode

//Debug
#undef NDEBUG

#ifdef __cplusplus

#ifndef NDEBUG

#define myThrow(a) { prln(); throw(a); }

#else //~NDEBUG

#define myThrow(a) { throw(a); }
//#define myThrow { throw; }

#endif //~NDEBUG

#endif //~__cplusplus

#define SUPERCHECKS
#ifdef  SUPERCHECKS

#define prln() printf("\n FILE %s; LINE %i\n", __FILE__, __LINE__)
#define prdel() printf("\n___________________________________________________________________\n")
#define prhello() printf("\n\033[1m\033[31m\033[4m\033[5mI am here! Line №%i\033[0m\n", __LINE__);

#else //~SUPERCHECKS

#define prln()

#define prdel()

#define prhello()

#endif //~STUPIDUSER

//Some constants

enum { MAX_LINE_LEN = 256 };
#define MACH_EPS 1e-10

//Some error messages/status codes

enum ERRORS_T
{
    Govnocode // Make errors' values positive
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

typedef enum ERRORS_T ERRORS_T;

extern const char* _ERR_ST[-ERR_QT];

//Returns string that represents error
const char* ERR_ST(ERRORS_T ERR);

//My collection of potions

extern const char POISON_CHAR;
extern const int  POISON_INT;
extern const size_t POISON_SIZE_T;

#ifdef __cplusplus
#define POISON_DOUBLE (std::numeric_limits<double>::quiet_NaN())
#else  //__cplusplus
#define POISON_DOUBLE 0xDEADBEEFDEADBEEF
#endif //__cplusplus

//Useful defines

#define printfTo(__command, ...)       \
{                                      \
   char __temporal[MAX_LINE_LEN] = ""; \
   sprintf(__temporal, __VA_ARGS__);   \
   __command(__temporal);              \
}

//#define  DEBUGPRINTF(...) printf("\nDEBUG:" __VA_ARGS__)
//#define DEBUGFPRINTF(...) fprintf(__VA_ARGS__)
#define  DEBUGPRINTF(...)
#define DEBUGFPRINTF(...)
#define FREE(a)   { free(a);   (a) = NULL; }

#define FCLOSE(a) { fclose(a); (a) = NULL; }

#ifdef __cplusplus

#define FOPEN(a)            \
{                           \
    if (!(a))               \
    {                       \
        prln();             \
        myThrow(ERR_OPFILE); \
    }                       \
}

#define ALLOC(a)      \
if (!(a))             \
{                     \
    prln();           \
    myThrow(ERR_ALLOC); \
}

#else //__cplusplus

#define FOPEN(a)            \
{                           \
    if (!(a))               \
    {                       \
        prln();             \
        myThrow(ERR_OPFILE); \
    }                       \
}

#define ALLOC(a)      \
if (!(a))             \
{                     \
    prln();           \
    return ERR_ALLOC; \
}

#endif

//Common functions

size_t myRandom(int seed);

int genSeed();

bool D_EQ(double a, double y);

#endif
