cmake_minimum_required(VERSION 3.6)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -lasan -O2 -g -std=c++14 -Wall -Wno-extra -Wno-conversion-null -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fchkp-first-field-has-own-bounds -fchkp-narrow-to-innermost-array -flto-odr-type-merging -fno-omit-frame-pointer -fsanitize=address -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -fPIE -pie" )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wno-conversion-null" )
##-fsanitize=undefined -fsanitize=alignment
set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan -O2 -g -std=c++14 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fchkp-first-field-has-own-bounds -fchkp-narrow-to-innermost-array -flto-odr-type-merging -fno-omit-frame-pointer -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr -fPIE -pie" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set ( SRCPATH "./Source/" )

include_directories( ${SRCPATH} )

project(PhikiCompiler)

set     (TREE_SOURCE_FILES
    ${SRCPATH}Tree/CommonTree.hpp
    ${SRCPATH}Tree/Tree.cpp
    ${SRCPATH}Tree/Tree.hpp
    ${SRCPATH}Tree/TreeNode.cpp
    ${SRCPATH}Tree/TreeNode.hpp
        )

set     (COMMON_SOURCE_FILES
    ${TREE_SOURCE_FILES}

	${SRCPATH}Common/Char_buffers.hpp
	${SRCPATH}Common/Char_buffers.cpp

	${SRCPATH}Common/Common.hpp
	${SRCPATH}Common/Common.cpp

        ${SRCPATH}Lists/Language_keywords_and_types_enum.hpp
        ${SRCPATH}Lists/Language_keywords_and_types_enum.cpp

        ${SRCPATH}Lists/Language_keywords.hpp
	)

set     (ASSEMBLER_SOURCE_FILES

	${COMMON_SOURCE_FILES}

        ${SRCPATH}Assembler/Labels.hpp
        ${SRCPATH}Assembler/Labels.cpp

        ${SRCPATH}Assembler/Assembler.hpp
        ${SRCPATH}Assembler/Assembler.cpp
        ${SRCPATH}Assembler/Disassembler.hpp
        ${SRCPATH}Assembler/Disassembler.cpp
        ${SRCPATH}Assembler/CommonAssembler.cpp
        ${SRCPATH}Assembler/CommonAssembler.hpp

	${SRCPATH}Lists/ut_asm.hpp
	)

set     (PROCESSOR_SOURCE_FILES

	${COMMON_SOURCE_FILES}
	${ASSEMBLER_SOURCE_FILES}

    ${SRCPATH}Processor/Stack.hpp
        ${SRCPATH}Processor/Stack.cpp

    ${SRCPATH}Processor/Processor.hpp
        ${SRCPATH}Processor/Processor.cpp
        ${SRCPATH}Processor/ProcessorCmdList.cpp
    ${SRCPATH}Processor/ProcessorOptions.hpp
        ${SRCPATH}Processor/ProcessorOptions.cpp


        ${SRCPATH}Lists/ut_proc.hpp
	)

set     (COMPILER_SOURCE_FILES

	${COMMON_SOURCE_FILES}
	${ASSEMBLER_SOURCE_FILES}
	${PROCESSOR_SOURCE_FILES}

    ${SRCPATH}Compiler/TokenList.hpp
        ${SRCPATH}Compiler/TokenList.cpp

    ${SRCPATH}Compiler/TokenListNode.hpp
        ${SRCPATH}Compiler/TokenListNode.cpp

        ${SRCPATH}Compiler/Semantical_analyzer.hpp
        ${SRCPATH}Compiler/Semantical_declarations.cpp
        ${SRCPATH}Compiler/Semantical_analyzer.cpp

        ${SRCPATH}Compiler/Recursive_descent.hpp
        ${SRCPATH}Compiler/Recursive_descent.cpp

        ${SRCPATH}Compiler/Lexical_analyzer.hpp

	${SRCPATH}Compiler/Compiler.hpp
	${SRCPATH}Compiler/Compiler.cpp

        ${SRCPATH}Compiler/Syntax_declaration.cpp

	${SRCPATH}Lists/ut_compiler.hpp
	)


add_executable  ( Compiler ${COMPILER_SOURCE_FILES}  ${SRCPATH}Compiler/main.cpp )
add_executable  ( Processor ${PROCESSOR_SOURCE_FILES} ${SRCPATH}Processor/main.cpp )
#add_executable  ( Assembler ${ASSEMBLER_SOURCE_FILES} )
