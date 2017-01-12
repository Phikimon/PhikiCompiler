#include "ProcessorOptions.hpp"
#include "Common/Common.hpp"
#include "getopt.h"
#include <cassert>
#include <cstdio>
#include <cctype>

ProcessorOptions::ProcessorOptions(int argc, char** argv):
    vflag (false),
    pflag (false),
    tflag (false),
    ivalue(NULL)
{
    assert(argv); assert(argc >= 0);

    int c = 0;
    opterr = 0;
#define caseopt(opt, optchar)            \
    case optchar:                        \
    {                                    \
       opt##flag = true;                 \
    }; break;

#define caseargopt(opt, optchar)         \
    case optchar:                        \
    {                                    \
        opt##value = optarg;             \
    }; break;

    while ((c = getopt (argc, argv, "vtpi:") ) != -1)
    {
        switch (c)
        {
            caseopt(v, 'v');
            caseopt(p, 'p');
            caseopt(t, 't');
            caseargopt(i, 'i');

            case '?':
            {
                if (optopt == 'i')
                {
                    printf ("Option -%c requires an argument.\n",  optopt);
                    myThrow(CPUOPTERR_NO_ARGUMENT);
                }
                else if (isprint (optopt))
                    printf ("Unknown option `-%c'.\n",             optopt);
                else
                    printf ("Unknown option character `\\x%x'.\n", optopt);
                myThrow(CPUOPTERR_UNKNOWN_OPTION);
            }
            default:
                myThrow(CPUOPTERR_NOT_SPECIFIED);
        }
    }

//#define PRINT_SET_OPTIONS
#ifdef PRINT_SET_OPTIONS
    printf ("vflag = %i, pflag = %i, tflag = %i, ivalue = %s\n",
             vflag, pflag, tflag, ivalue);
#endif
}
