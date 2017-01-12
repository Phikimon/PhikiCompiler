#include "Tree.hpp"
#include <cstring>
#include <cmath>

//---------------------TREENODEDATA-MEMBERS-----------------------------------------
TreeNodeData::TreeNodeData(double num)
{
    number = num;
};
TreeNodeData::TreeNodeData(char*  name)
{
    if (!name) { idName = NULL; return; }

    size_t nameLen = strlen(name);
    ALLOC(idName = (char*)calloc(nameLen + 1,
                                 sizeof(char)));
    strcpy(idName, name);
};
TreeNodeData::TreeNodeData(size_t num)
{
    kwdNum = num;
};
TreeNodeData::TreeNodeData()
{
    idName = NULL;
}

//-------------------------TREENODE-MEMBERS-----------------------------------------
    /*
        (\___/)  Kitty, I am so sorry for all those
        (='.'= ) defines, but I am all to pieces due to
        (")_(")  copy-paste, so I hope you understand me
    */
#define TREENODE_CTOR(TYPE, TYPE_TO_CAST, DATA_TYPE)\
TreeNode::TreeNode(LangKeywordType type,            \
                   TYPE            data,            \
                   TreeNode*       uppr,            \
                   TreeNode*       left,            \
                   TreeNode*       rght):           \
    data_((TYPE_TO_CAST)data),                      \
    type_(type),                                    \
    uppr_(uppr),                                    \
    left_(left),                                    \
    rght_(rght)                                     \
{                                                   \
    f_ckTheTemplates<DATA_TYPE>(type,                    \
                                uppr,                    \
                                left,                    \
                                rght);                   \
}

TREENODE_CTOR( char*,             char*,  TYPE_CHAR_PTR )
TREENODE_CTOR( double,            double, TYPE_DOUBLE   )
TREENODE_CTOR( size_t,            size_t, TYPE_SIZE_T   )
TREENODE_CTOR( unsigned,          size_t, TYPE_SIZE_T   )
TREENODE_CTOR( char,              size_t, TYPE_SIZE_T   )
TREENODE_CTOR( LangKeyword,       size_t, TYPE_SIZE_T   )
TREENODE_CTOR( LangSecondary,     size_t, TYPE_SIZE_T   )
TREENODE_CTOR( LangMathOperators, size_t, TYPE_SIZE_T   )

#undef TREENODE_CTOR

TreeNode::~TreeNode()
{
    if (getDataType(type_) ==
        TYPE_CHAR_PTR        )
    {
        free(data_.idName);
    }

    type_ = LANGKWT_nil;
    left_ = rght_ = uppr_ = NULL;
}

bool operator == (const TreeNode &node1, const TreeNode &node2)
{
    if ( ( ( node1.type_        != node2.type_       ) ) ||
         ( ( (bool)node1.left_) != ((bool)node2.left_) ) ||
         ( ( (bool)node1.rght_) != ((bool)node2.rght_) ) )
        return false;

    switch (getDataType(node1.type_))
        {
            case TYPE_CHAR_PTR:
            {
                assert(node1.data_.idName);
                assert(node2.data_.idName);
                return (!strcmp(node1.data_.idName, node1.data_.idName));
            };
            case TYPE_DOUBLE:
            {
                return D_EQ(node1.data_.number, node2.data_.number);
            };
            case TYPE_SIZE_T:
            {
                return (node1.data_.kwdNum == node2.data_.kwdNum);
            };
            case TYPE_NIL:
            {
                return false;
            }
            default:
            {
                printf("\nUnknown type of node == '%i' "
                        "is passed to the '%s' method\n",
                        node1.type_, __func__);
                myThrow(TREEERR_UNKTYPE);
            }; break;
        }
}

void TreeNode::dataDump(FILE* stream)
{
    assert(stream);
    switch (getDataType(type_))
    {
        case TYPE_DOUBLE:
        {
            fprintf(stream, "%lg",
                   data_.number);
        }; break;

        case TYPE_CHAR_PTR:
        {
            assert(data_.idName);
            fprintf(stream, "'%s'",
                    data_.idName);
        }; break;

        case TYPE_SIZE_T:
        {
            char stringToPrint[MAX_LINE_LEN] = {};
            switch (type_)
            {
                case LANGKWT_mathOperator:
                {
                    assert(data_.kwdNum >= 0           &&
                           data_.kwdNum < LANGMATH_MATHQT);
                    fprintf(stream, "'%s'",
                            langMathOperatorsStrings((LangMathOperators)data_.kwdNum));
                }; break;
                case LANGKWT_num_bracket:
                case LANGKWT_delim:
                {
                    assert(data_.kwdNum >= 0           &&
                           data_.kwdNum < LANGSEC_SECQT);
                    fprintf(stream, "'%s'",
                            langSecondaryStrings((LangSecondary)data_.kwdNum));
                }; break;
                default:
                {
                    assert(data_.kwdNum >= 0           &&
                           data_.kwdNum < LANGKWD_KWDQT);
                    fprintf(stream, "'%s'",
                            langKeywordsStrings((LangKeyword)data_.kwdNum));

                }; break;
            };

        }; break;

        case TYPE_NIL:
        {
            fprintf(stream, "(nil)");
            return;
        }; break;

        default:
        {
            prln();
            printf("Unknown node type in Tree::dataDump");
            myThrow(TREEERR_UNKTYPE);
        }
    }
    assert(type_ >= 0 && type_ < LANGKWT_KWTQT);
    fprintf(stream, " (%s)",
           langKeywordsTypesStrings(type_));
}

void TreeNode::dump(FILE* stream)
{
    fprintf( stream, "Node [%p]", this);
    fprintf( stream, "{\n");
    fprintf( stream, "\tuppr = %p\n", uppr_);
    fprintf( stream, "\tleft = %p\n", left_);
    fprintf( stream, "\trght = %p\n", rght_);
    dataDump(stream);
    fprintf( stream, "}\n");
}

void TreeNode::dotDump    (FILE* stream, bool briefDump)
{
    assert(stream);
    fprintf(stream, "treeNode_%p [label=\"node[%p]\\l", this, this);
    if (!briefDump)
    {
        fprintf(stream, "{\\l"
                          "\tuppr = %p;\\l"
                          "\tleft = %p;\\l"
                          "\trght = %p;\\l",
                             uppr_,
                             left_,
                             rght_);
    }

    fprintf(stream, "\tdataContent = ");
    dataDump(stream);


    if (!briefDump)
    {
        fprintf(stream, "\\l}");
    }

    fprintf(stream, "\\l\"]\n");

    if (!left_)
    {
        fprintf(stream, "treeNode_%p_left [label=\"(nil)\"]\n", this);
        fprintf(stream, "treeNode_%p -> treeNode_%p_left\n", this, this);
    }
    else
        fprintf(stream, "treeNode_%p -> treeNode_%p\n", this, left_);

    if (!this->rght_)
    {
        fprintf(stream, "treeNode_%p_rght [label=\"(nil)\"]\n", this);
        fprintf(stream, "treeNode_%p -> treeNode_%p_rght\n", this, this);
    }
    else
        fprintf(stream, "treeNode_%p -> treeNode_%p\n", this, rght_);

}

void TreeNode::redispatchUppr()
{
    if (left_) left_->uppr_ = this;
    if (rght_) rght_->uppr_ = this;
}

bool TreeNode::isLeftChild()
{
    if (!uppr_) return false;

    if (this == uppr_->left_)
        return true; else
        return false;
}
