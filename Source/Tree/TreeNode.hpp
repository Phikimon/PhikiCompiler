#ifndef TREENODE_HPP_INCLUDED
#define TREENODE_HPP_INCLUDED
#include "CommonTree.hpp"
#include <cstdio>
#include <cassert>

class Tree;

class TreeNode
{
    public:
        TreeNode(LangKeywordType type = LANGKWT_nil,
                 char*           data = NULL,
                 TreeNode*       uppr = NULL,
                 TreeNode*       left = NULL,
                 TreeNode*       rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 double            data = 0,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 size_t            data = 0,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 unsigned          data = 0,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 char              data = '\0',
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 LangKeyword       data = LANGKWD_KWDQT,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 LangSecondary     data = LANGSEC_SECQT,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        TreeNode(LangKeywordType   type = LANGKWT_nil,
                 LangMathOperators data = LANGMATH_MATHQT,
                 TreeNode*         uppr = NULL,
                 TreeNode*         left = NULL,
                 TreeNode*         rght = NULL);

        ~TreeNode();

        void dump (FILE* stream = stdout);

        bool friend operator == (const TreeNode &node1,
                                 const TreeNode &node2);

        TreeNodeData    data_;

        LangKeywordType type_;

        TreeNode* uppr_;
        TreeNode* left_;
        TreeNode* rght_;

    private:
        void dataDump(FILE* stream = stdout);
        void dotDump (FILE* stream, bool briefDump = false);
        void redispatchUppr();
        bool isLeftChild();

        template <DataType DATA_T>
        void f_ckTheTemplates(LangKeywordType type,
                              TreeNode*       uppr,
                              TreeNode*       left,
                              TreeNode*       rght);

        template <class... Args>
        void visitor(TreeVisitMode mode,
                     void (TreeNode::*act)(Args... args),
                     Args... args);
        friend class Tree;
};

template <DataType DATA_T>
void TreeNode::f_ckTheTemplates(LangKeywordType type,
                                TreeNode*       uppr,
                                TreeNode*       left,
                                TreeNode*       rght)
{
    assert((left != rght) || (!left));
    assert((left != uppr) || (!left));
    assert((rght != uppr) || (!rght));

    switch (getDataType(type))
    {
        case DATA_T:
        break;

        case TYPE_NIL:
        {
            data_.idName = NULL;
            type_        = LANGKWT_nil;
        }; break;

        default:
        {
            printf("\nError: Unknown type of node == '%i'"
                   " is passed to the 'nodeCtor method\n", type);
            myThrow(TREEERR_UNKTYPE);
        }; break;
    }
    if (left) left->uppr_ = this;
    if (rght) rght->uppr_ = this;
}


template <class... Args>
void TreeNode::visitor(TreeVisitMode mode,
                       void (TreeNode::*act)(Args... args),
                       Args... args)
{
    if (!this)
    {
        return;
    }


    switch (mode)
    {
        case prefixMode:
        {
            (this->*act)(args...);
            left_->visitor<Args...>(mode, act, args...);
            rght_->visitor<Args...>(mode, act, args...);
        }; break;
        case infixMode:
        {
            left_->visitor<Args...>(mode, act, args...);
            (this->*act)(args...);
            rght_->visitor<Args...>(mode, act, args...);
        }; break;
        case postfixMode:
        {
            left_->visitor<Args...>(mode, act, args...);
            rght_->visitor<Args...>(mode, act, args...);
            (this->*act)(args...);
        }; break;
        default:
        {
            DEBUGPRINTF("Wrong mode is passed to the %s",
                        __func__);
            prln();
            myThrow(TREEERR_UNKVISMD);
        }; break;
    }
}
#endif
