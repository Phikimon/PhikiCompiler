	Initial commit. Source files are collected from different projects, written by me during sept-dec 2016, and what I'm going to do is to spend few weeks refactoring this bull****. 
	The worst places in the code are marked with the keywords(empty  #define) 'Govnocode' and 'GovnoCode'

Project structure:

Source
├── Assembler
│   ├── Assembler.cpp
│   ├── Assembler.hpp
│   ├── Labels.cpp
│   └── Labels.hpp
├── Common
│   ├── Char_buffers.cpp
│   ├── Char_buffers.hpp
│   ├── Common.cpp
│   ├── Common.hpp
│   ├── Trees.cpp
│   └── Trees.hpp
├── Compiler
│   ├── Compiler_unit_test.cpp
│   ├── Compiler_unit_test.hpp
│   ├── Lexical_analyzer.cpp
│   ├── Lexical_analyzer.hpp
│   ├── Recursive_descent.cpp
│   ├── Recursive_descent.hpp
│   ├── Semantical_analyzer.cpp
│   ├── Semantical_analyzer.hpp
│   ├── Syntax_declaration.cpp
│   └── Transliterate.py
├── Lists
│   ├── Language_keywords_and_types_enum.cpp
│   ├── Language_keywords_and_types_enum.hpp
│   ├── Language_keywords.hpp
│   ├── Processor_cmd_list.hpp
│   ├── ut_asm.hpp
│   ├── ut_compiler.hpp
│   └── ut_proc.hpp
├── main.cpp
├── Processor
│   ├── Processor.cpp
│   ├── Processor.hpp
│   ├── Stack.cpp
│   └── Stack.hpp
└── UT
    ├── UT_ASM
    │   ├── Correct
    │   │   ├── ASM_Lupj
    │   │   ├── Comments and indentations
    │   │   ├── Complex
    │   │   ├── From Compiler
    │   │   ├── Label
    │   │   ├── No argument
    │   │   ├── Read$
    │   │   ├── Read%
    │   │   └── Three different arguments
    │   └── Incorrect
    │       ├── COMINSARG
    │       ├── EMPTY
    │       ├── EXTRAARG
    │       ├── NULLLENLBL
    │       ├── TOOFEW
    │       ├── TWOCMD
    │       ├── UNKCMD
    │       └── WRGARGTYPE
    ├── UT_COMPILER
    │   ├── Conditionals.holy
    │   ├── EmptyMain.holy
    │   ├── Factorial.holy
    │   ├── Function_Call.holy
    │   ├── Loops.holy
    │   ├── MultipleFunctions.holy
    │   ├── Recursive.holy
    │   ├── Series.holy
    │   ├── Square_Lite.holy
    │   └── Zhest.holy
    └── UT_PROC
        └── Correct
            ├── ASM_Lupj
            ├── Complex
            ├── DoubleComplex
            ├── Factorial
            ├── Factorial_recursive
            └── Stack_operations
