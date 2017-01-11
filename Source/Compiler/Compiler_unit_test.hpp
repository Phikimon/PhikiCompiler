#ifndef COMPILER_UNIT_TEST_HPP_INCLUDED
#define COMPILER_UNIT_TEST_HPP_INCLUDED

//lib
#include "Processor/Processor.hpp"

//Current project
#include "Compiler/Lexical_analyzer.hpp"
#include "Compiler/Recursive_descent.hpp"
#include "Compiler/Semantical_analyzer.hpp"

 /*
  * Unit test function for compiler
  */
void ut_comp(size_t verbose, unsigned mask);

/**
 *                                                                      UT_COMP VERBOSE MASK
 * +-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+
 * |   PROC_VERB_OPT   |    HEX_BUF_DUMP   |   OPEN_ASM_FILE   |  SYNT_TREE_DUMP   |   SYNT_ANAL_DUMP  |  TOKEN_LIST_DUMP  |    SRC_FILE_DUMP  |   OPEN_SRC_FILE   |
 * +-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+
 * |                   |                   |                   |                   |                   |                   |                   |                   |
 * |   PROCESSOR'S     |     HEX FILE      |  OPEN ASM FILE    |    SYNTAX TREE    |   DETAILED SYNT   |    TOKEN LIST     |    SOURCE FILE    | OPEN SOURCE FILE  |
 * |  VERBOSE OPTION   |   BUFFER DUMP     |     VIA GEDIT     |     DOT DUMP      |   ANALYZER DUMP   |       DUMP        |    BUFFER DUMP    |     VIA GEDIT     |
 * +-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+-------------------+
 */

enum UT_COMP_VERBOSE_MASK_BITS
{
    UT_COMP_OPEN_SRC_FILE   = 0,
    UT_COMP_SRC_FILE_DUMP   = 1,
    UT_COMP_TOKEN_LIST_DUMP = 2,
    UT_COMP_SYNT_ANAL_DUMP  = 3,
    UT_COMP_SYNT_TREE_DUMP  = 4,
    UT_COMP_OPEN_ASM_FILE   = 5,
    UT_COMP_HEX_BUF_DUMP    = 6,
    UT_COMP_PROC_VERB_OPT   = 7,
};

#endif

