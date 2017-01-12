#include "Tree.hpp"
#include <cassert>
#include <cstdio>

void Tree::dotDump(bool briefDump)
{
    static int dumpNumber = 0;
    dumpNumber++;

    char dumpFileName[sizeof("./temp/treeDump000.gv") + 1] = {};
    sprintf(dumpFileName, "./temp/treeDump%03i.gv", dumpNumber);

    FILE* dumpFile = NULL;
    FOPEN(dumpFile = fopen(dumpFileName, "w"));

    fprintf(dumpFile, "digraph G {\n"
                      "graph [ dpi = 300 ]");
    visitor(prefixMode, (&TreeNode::dotDump),
            (FILE*)dumpFile,
            false);
    fprintf(dumpFile, "}\n");

    fclose(dumpFile);

#define XDOT_TREE_DUMP
#ifdef XDOT_TREE_DUMP
    printfTo(system, "xdot %s", dumpFileName);
#endif

#ifdef DOT_TREE_DUMP
    printfTo(system, "dot %s -Tpng -o "
                     "./temp/treeDump%03i.png",
                     dumpFileName, dumpNumber);
#endif
}

Tree::~Tree()
{
    Govnocode
    //visitor<>(postfixMode,
    //          (&TreeNode::~TreeNode()));
}

bool operator == (const Tree& tree1, const Tree& tree2)
{
    TreeNode* root1 = tree1;
    TreeNode* root2 = tree2;
    bool equal = (*root1 == *root2);

    if (root1->left_)
        equal = equal && (root1->left_ == root2->left_);
    if (root1->rght_)
        equal = equal && (root1->rght_ == root2->rght_);

   return equal;
}

void Tree::redispatchUppr()
{
    visitor(prefixMode,
           (&TreeNode::redispatchUppr));
}
