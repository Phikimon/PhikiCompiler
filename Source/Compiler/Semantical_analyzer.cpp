#include "Semantical_analyzer.hpp"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

SyntaxTreeTranslator::SyntaxTreeTranslator(TreeNode* root):
    asmFile_(NULL),
    asmFileName_({}),
    functions_({}),
    functionCounter_(0),
    root_   (root)
{}

SyntaxTreeTranslator::~SyntaxTreeTranslator()
{
    functionCounter_ = 0;
    asmFile_ = NULL;
    root_ = NULL;
}

void SyntaxTreeTranslator::_assertSyntType(TreeNode* node,
                                           LangKeywordType type,
                                           unsigned line, const char* func)
{
    if (node->type_ != type)
    {
        printf("\033[31m\033[1m");
        printf("\nIn func '%s' in line #%u",
                func, line);
        printf(" syntax-tree node type(%d)(%s)",
                node->type_,
                langKeywordsTypesStrings(node->type_));
        printf(" is not equal to %d(%s).\n",
                type,
                langKeywordsTypesStrings(type));
        node->dump();
        printf("\033[0m\n");
        assert(!"Assertion failed");
    }
};

const char* SyntaxTreeTranslator::getAsmFileName()
{
    return asmFileName_;
}

void SyntaxTreeTranslator::translate()
{
    _assertSynt(root_, LANGKWT_new_program, LANGKWD_program, 0, "Global");
    _assertSyntType(root_->left_, LANGKWT_id, 0, "Global");

    sprintf(asmFileName_, "./temp/%s.asm", root_->left_->data_.idName);

    FOPEN(asmFile_ = fopen(asmFileName_, "w"));

    fprintf(asmFile_, "CALL $main\n"
                      "OUT  %%0\n"
                      "JMP  $END\n");
    compileProgram(root_->rght_);

    fprintf(asmFile_, "\n\nEND:"
                      "\nNOP\n");

    FCLOSE(asmFile_);
}
