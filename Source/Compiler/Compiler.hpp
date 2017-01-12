#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include <Common/Common.hpp>
#include "getopt.h"
/*
 *    Compiler options:
 *
 * Options with arguments:
 * ┌───────────────────┬───────────────────┐
 * │        -i         │        -o         │
 * ├───────────────────┼───────────────────┤
 * │     set input     │     set output    │
 * │       file        │       file        │
 * └───────────────────┴───────────────────┘
 * Options without arguments(flags):
 * ┌───────────────────┬───────────────────┬───────────────────┬───────────────────┐
 * │       -a          │        -s         │         -r        │        -t         │
 * ├───────────────────┼───────────────────┼───────────────────┼───────────────────┤
 * │  leave ASM file   │ leave SYNTAX tree │  detailed syntax  │ leave TOKEN list  │
 * │                   │     dot file      │(RECURSIVE-descent)│     dot file      │
 * └───────────────────┴───────────────────┤   analyzer dump   ├───────────────────┘
 *                                         └───────────────────┘
 */

enum CompilerErrors
{
    COMPERR_OK,          //< No error occured
    COMPERR_WRNGTYPE,    //< Wrong token type
    COMPERR_WRNGDATA,    //< Wrong token data
    COMPERR_NULLTOK,     //< Attempt to call non-existent list node
    COMPERR_UNKCHAR,       //< Unknown character
    COMPERR_UNKOP,         //< Unknown operator
    COMPERR_UNTERMCOMMENT, //< Unterminated comment
};

struct CompilerOptions
{
    bool aflag,  sflag,  rflag,  tflag;
    char *ovalue, *ivalue;
};

int compilerOptionsCtor(CompilerOptions* opts, int argc, char** argv);

int compile(int argc, char** argv);

#endif //COMPILER_HPP_INCLUDED
