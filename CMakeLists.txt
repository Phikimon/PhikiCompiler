cmake_minimum_required(VERSION 3.6)
project(Compiler)

set ( SRCPATH "./Source/" )
# set ( INCLUDEOPT "-I./Source/ )
include_directories(./Source)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#set( CFLAGS -c ${INCLUDEOPT} -lasan -O2 -g -std=c++14 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fchkp-first-field-has-own-bounds -fchkp-narrow-to-innermost-array -flto-odr-type-merging -fno-omit-frame-pointer -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -fPIE -pie 
#LDFLAGS=-lasan -O2 -g -std=c++14 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fchkp-first-field-has-own-bounds -fchkp-narrow-to-innermost-array -flto-odr-type-merging -fno-omit-frame-pointer -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -fPIE -pie )

set( CMAKE_CXX_FLAGS        "-w" )
# set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${INCLUDEOPT}"       )

set	(SOURCE_FILES 
        ${SRCPATH}Common/Char_buffers.cpp
        ${SRCPATH}Common/Trees.hpp
        ${SRCPATH}Common/Char_buffers.hpp
        ${SRCPATH}Common/Trees.cpp
        ${SRCPATH}Common/Common.cpp
        ${SRCPATH}Common/Common.hpp

        ${SRCPATH}Processor/Stack.hpp
        ${SRCPATH}Processor/Processor.hpp
        ${SRCPATH}Processor/Processor.cpp
        ${SRCPATH}Processor/Stack.cpp

        ${SRCPATH}Compiler/Recursive_descent.cpp
        ${SRCPATH}Compiler/Semantical_analyzer.cpp
        ${SRCPATH}Compiler/Lexical_analyzer.cpp
        ${SRCPATH}Compiler/Semantical_analyzer.hpp
        ${SRCPATH}Compiler/Compiler_unit_test.hpp
        ${SRCPATH}Compiler/Transliterate.py
        ${SRCPATH}Compiler/Compiler_unit_test.cpp
        ${SRCPATH}Compiler/Syntax_declaration.cpp
        ${SRCPATH}Compiler/Recursive_descent.hpp
        ${SRCPATH}Compiler/Lexical_analyzer.hpp

        ${SRCPATH}Lists/Processor_cmd_list.hpp
        ${SRCPATH}Lists/Language_keywords_and_types_enum.hpp
        ${SRCPATH}Lists/Language_keywords_and_types_enum.cpp
        ${SRCPATH}Lists/Language_keywords.hpp
        ${SRCPATH}Lists/ut_compiler.hpp
        ${SRCPATH}Lists/ut_proc.hpp
        ${SRCPATH}Lists/ut_asm.hpp

        ${SRCPATH}Assembler/Labels.cpp
        ${SRCPATH}Assembler/Labels.hpp
        ${SRCPATH}Assembler/Assembler.cpp
        ${SRCPATH}Assembler/Assembler.hpp

        ${SRCPATH}main.cpp
	)

add_executable  ( Compiler ${SOURCE_FILES} )
#add_dependencies( Compiler ${DEPENDENCIES} )
#add_subdirectory(tests)

