#ifndef SEMANTICAL_ANALYZER_HPP_INCLUDED
#define SEMANTICAL_ANALYZER_HPP_INCLUDED

#include "Processor/Processor.hpp" Govnocode //Delete this line after Semantical_analyzer refactoring. This include is just for CPU_REGISTERS_QT

#include "Common/Trees.hpp""
#include "Lists/Language_keywords_and_types_enum.hpp"

#define assertSynt(node, type, data) _assertSynt(node, type, (void*)(&data), __LINE__, __func__)
#define assertSyntType(node, type) _assertSyntType(node, type, __LINE__, __func__)

struct syntaxTreeTranslator_t
{
    FILE* asmFile;
    char asmFileName[MAX_LINE_LEN];
    
    treeNode_t* root; //syntax tree root
    
    size_t usedIdsNum;
    char* usedIds[CPU_REGISTERS_QT];
};

void _assertSyntType                (treeNode_t* node, treeNodeDataType type, unsigned line, const char* func);
void _assertSynt                    (treeNode_t* node, treeNodeDataType type, void* data, unsigned line, const char* func);

void transCtor                      (syntaxTreeTranslator_t* trans, treeNode_t* root);
void transDtor                      (syntaxTreeTranslator_t* trans);

size_t transIdNum                   (syntaxTreeTranslator_t* trans, char* id = NULL);

void translateTree                  (syntaxTreeTranslator_t* trans, treeNode_t* root);

void translateExpressionRecursive   (syntaxTreeTranslator_t* trans, treeNode_t* node);
int translateExpression             (syntaxTreeTranslator_t* trans, treeNode_t* node);

int exprCountFunctions              (treeNode_t* exprRoot);
void translateFunctionCall          (syntaxTreeTranslator_t* trans, treeNode_t* funcNode, int returnVarNum);
void translatePushValue             (syntaxTreeTranslator_t* trans, treeNode_t* exprRoot);
void printVarNumOrDouble            (syntaxTreeTranslator_t* trans, treeNode_t* node);

#endif
