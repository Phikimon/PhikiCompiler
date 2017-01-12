#ifndef TREE_HPP_INCLUDED
#define TREE_HPP_INCLUDED
#include "CommonTree.hpp"
#include "TreeNode.hpp"

class Tree
{
    public:
        TreeNode* root_;

        Tree (TreeNode* node)
        {
            root_ = node;
        };

        ~Tree();

        operator TreeNode() const
        {
            return *root_;
        }
        operator TreeNode*() const
        {
            return root_;
        }

        template <class... Args>
        void visitor(TreeVisitMode mode,
                     void (TreeNode::*act)(Args... args),
                     Args... args);

        void dotDump       (bool briefDump = false);
        void redispatchUppr();
    private:
        bool friend operator ==   (const Tree& tree1,
                                   const Tree& tree2);
};

template <class... Args>
void Tree::visitor(TreeVisitMode mode,
                   void (TreeNode::*act)(Args... args),
                   Args... args)
{
    root_->visitor(mode, act, args...);
}

#endif
