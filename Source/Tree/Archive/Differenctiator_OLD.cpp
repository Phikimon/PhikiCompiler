enum TreeMathOperator
{
    #define LANG_MATH_OPERATORS

    #define LANG_MATH_OP(name, char, prior) \
    treeMath_##name = char,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_MATH_OP

    #undef LANG_MATH_OPERATORS
};

enum TreeMathOperatorsPriorities
{
    #define LANG_MATH_OPERATORS

    #define LANG_MATH_OP(name, char, prior) \
    treeMathPrior_##name = prior,

    #include "Lists/Language_keywords.hpp"

    #undef LANG_MATH_OP

    #undef LANG_MATH_OPERATORS
};

#ifdef DIFFERENTIATOR
FILE* mathDumpFile = NULL;

/**
 Additional math tree functions
 */
bool isOperFunc (unsigned mathOp);
int  mathOpPrior(unsigned mathOp); //< Return priority of
                                       //< mathematical operation from
                                       //< treeMathOpPrior enum
                                       //
bool isOperFunc(unsigned mathOp)
{
    return (mathOp == treeMath_ln);
}

int mathOpPrior(unsigned mathOp)
{
    switch (mathOp)
    {
    #define opPrior(a) return treeMathPrior_##a;
        case treeMath_ln:    opPrior(ln);
        case treeMath_mult:  opPrior(mult);
        case treeMath_div:   opPrior(div);
        case treeMath_plus:  opPrior(plus);
        case treeMath_minus: opPrior(minus);
        case treeMath_deg:   opPrior(deg);
        default:
        {
            printf("\nError: Unknown mathOp == '%c' is"
                   " passed to the 'mathOpPrior' method.\n", mathOp);
            myThrow(TREEERR_UNKTYPE);
        }
    #undef opPrior
    }
}

/**
 * Operators that add new node to tree and concatenates node1 and node2 via new node
 */
TreeNode operator +  (TreeNode &node1, TreeNode &node2);
TreeNode operator -  (TreeNode &node1, TreeNode &node2);
TreeNode operator *  (TreeNode &node1, TreeNode &node2);
TreeNode operator /  (TreeNode &node1, TreeNode &node2);
TreeNode operator ^  (TreeNode &node1, TreeNode &node2);

/**
 Returns differential of tree
 */
TreeNode* treeDiffer(TreeNode* node);

/**
 Dump the whole tree
 */
void dumpAll(TreeNode* tree);

/**
 Add equation to tex dump
 */
void addMathDump(TreeNode* tree, bool addStroke = false);

/**
 Private function. Add common data to the texDumpFile
 */
void initTexFile();

/**
 Function to compile and show TexFile
 */
void viewTexFile();

/**
 Private function. Recursively copies tree
 */
void _treecpy(TreeNode** newNode, TreeNode* node);

/**
 Returns copy of tree
 */
TreeNode* treeCopy(TreeNode* tree);

/**
 Private function. Recursively calls itself and dumps each node
 */
void _mathTreeDump(TreeNode* node);

_______SOURCES____________________________________________________________________________

void dumpAll(TreeNode* tree)
{
    while (tree->uppr)
    {
        tree = tree->uppr;
    }
    addMathDump(tree);
}

TreeNode* treeDiffer(TreeNode* node)
{
    assert(node);
    //If tree is totally constant, return 0
    if (isTreeConst(node))
    {
        double derivative = 0;
        return nodeRetCtor(LANGKWT_double, derivative, NULL, NULL, NULL);
    }
    //
    #define CALLOC_TREE(tree) TreeNode* tree = (TreeNode*)calloc(1, sizeof(*tree))
    CALLOC_TREE(result);

    switch (node->type)
    {
        case LANGKWT_double:
        {
            assert(node->data);
            double derivative = 0;
            free(result);
            result = nodeRetCtor(LANGKWT_double, derivative, NULL, NULL, NULL);
        }; break;
        case LANGKWT_mathOperator:
        {
            assert(node->data);

            TreeNode* leftBranch = treeCopy(node->left);
            TreeNode* rghtBranch = node->rght ? treeCopy(node->rght) : NULL;
            TreeNode* leftDiffer = treeDiffer(node->left);
            TreeNode* rghtDiffer = node->rght ? treeDiffer(node->rght) : NULL;

            #define dL (*leftDiffer)
            #define dR (*rghtDiffer)
            #define L  (*leftBranch)
            #define R  (*rghtBranch)
            switch (*(unsigned*)node->data)
            {
            case '*':
            {
                CALLOC_TREE(augend1); CALLOC_TREE(augend2);
                *augend1 = dL * R;
                *augend2 = dR * L;

                *result = *augend1 + *augend2;
            } break;
            case '/':
            {
                CALLOC_TREE(augend1); CALLOC_TREE(augend2);
                *augend1 = dL * R;
                *augend2 = dR * L;

                CALLOC_TREE(numerator); CALLOC_TREE(denominator);
                *numerator = *augend1 - *augend2;
                TreeNode* RCpy1 = treeCopy(&R);
                TreeNode* RCpy2 = treeCopy(&R);
                *denominator = *RCpy1 * *RCpy2;

                *result = *numerator / *denominator;
            }; break;
            case '+':
            {
                *result = dL + dR;
            }; break;
            case '-':
            {
                *result = dL - dR;
            }; break;
            case '^':
            {
                assert( (isTreeConst(&L)) || (isTreeConst(&R)) );

                if (isTreeConst(&L))
                //(a^f(x))' = f(x)' * (a^f(x)) * ln(a) // f(x) =  R; a = L
                {
                    prhello();
                    unsigned mathOperator = 'l';
                    TreeNode* lnA = _nodeRetCtor(LANGKWT_mathOperator, &mathOperator, NULL, &L, NULL);
                    CALLOC_TREE(factor1);
                    *factor1 = dR * *lnA;

                    CALLOC_TREE(A); CALLOC_TREE(factor2);
                    A = treeCopy(&L);
                    *factor2 = *A ^ R;
                    *result = *factor1 * *factor2;
                } else
                //R=const  //(f(x)^a)' = a * f(x)' * (f(x))^(a-1)  // f(x) = L; a = R
                {
                    TreeNode* factor = treeCopy(&R);

                    CALLOC_TREE(degree); CALLOC_TREE(mult); CALLOC_TREE(degreeMinusOne);

                    double OneValue = 1;
                    TreeNode* One = nodeRetCtor(LANGKWT_double, OneValue, NULL, NULL, NULL);

                    *degreeMinusOne = R - *One;

                    *degree = L ^ *degreeMinusOne;
                    *mult = *degree * *factor;
                    *result = dL * *mult;
                }
            }; break;
            case 'l':
            { // ln(f(x))' = f'(x) / f(x)
                *result = dL / L;
            }; break;
            default:
            {
                prln();
                printf("\nError: Unknown type of mathOp == '%i' is passed to the '%s'\n", node->type, __func__);
                myThrow(ERR_COMMON);
            }; break;
        };
            #undef L
            #undef R
            #undef dL
            #undef dR
        }; break;
        case LANGKWT_id:
        {
            double derivative = 1;
            result = nodeRetCtor(LANGKWT_double, derivative, NULL, NULL, NULL);
        }; break;
        default:
        {
            printf("\nUnknown type of node == '%i' is passed to the '%s' method\n",
                                           node->type,            __func__);
            myThrow(TREEERR_UNKTYPE);
        }; break;
    }
    redispatchUppr(result);
    return result;
}

void initTexFile()
{
    //If called twice then throw
    static bool isAlreadyCalled = false;

    if (isAlreadyCalled)
    {
        prln();
        myThrow(TREEERR_TOOMANYDUMPS);//< Throw if more than one dump is called
    };

    isAlreadyCalled = true;

    assert(!mathDumpFile);

    mathDumpFile = fopen("mathDump.tex", "w");
    if (!mathDumpFile)
    {
        prln();
        myThrow(ERR_OPFILEW);
    }

    fprintf(mathDumpFile,
    "\\documentclass{article}\n"
    "\\usepackage[a4paper,margin=6mm]{geometry}\n"
    "\\usepackage{amsmath}\n"
    "\\usepackage[utf8]{inputenc}"
    "\\usepackage[T2A]{fontenc}"
    "\\usepackage[russian]{babel}"
    "\\usepackage{hyperref}\n"
    "\\title{\\LaTeX\\  Derivation}\n"
    "\\author{Prof Phikimon}\n"
    "\\begin{document}\n"
    "\\fontsize{12}{12pt}\\selectfont\n"
    "\\maketitle\n"

    "\\begin{flushleft}\n"
    "Здравствуйте, товарищ \\%%USERNAME\\%%! \\\\\n"
    "Высылаю Вам свои скромные выкладки по обсуждаемому нами вчера вопросу. "
    "Удачно разобраться Вам в этой жести! \\\\"
    "\\end{flushleft}\n"
    "\\begin{center}\n"
    "\\fontsize{16}{16pt}\\selectfontХА-"
    "\\fontsize{24}{24pt}\\selectfontХА-"
    "\\fontsize{32}{32pt}\\selectfontХА!!! \\\\"
    "\\end{center}\n"
    "\\fontsize{16}{16pt}\\selectfont\n"

    "\\begin{center}\n"
    " Что же, приступим к преобразованиям:\n"
    "\\fontsize{8}{8pt}\\selectfont\n"
    "\\begin{equation}\n");
}

void viewTexFile()
{
    assert(mathDumpFile);

    fprintf(mathDumpFile, "\\end{equation}\n"
                          "\\fontsize{16}{16}\\selectfont\n"
                          "Спешу вас поздравить. Вы, конечно, не стали разбираться в выкладках и пропустили ошибку. Удачи на экзамене!"
                          "*И что, получается, я писал эти 1,5к строк дифференциатора и упростителя зря?... =(*"
    "\\end{center}\n\n\n\\end{document}");
    fclose(mathDumpFile);

    //Is to be switched manually
    if (1)
    {
        //Compile Tex File
        system("pdflatex ./mathDump.tex > dev\\null");

        //View Tex File
        system("evince ./mathDump.pdf");

    } else
    {
        //Or open texFile in the editor
        system("texmaker ./mathDump.tex");
    }
}

void _treecpy(TreeNode** newNode, TreeNode* node)
{
    *newNode = (TreeNode*)calloc(1, sizeof(**newNode));
    **newNode = (*node);

    switch (node->type)
        {
            case LANGKWT_id:
            case LANGKWT_string:
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(strlen((char*)node->data) + 1, sizeof(char));
                                  strcpy((char*)(*newNode)->data, (char*)node->data);
                              }; break;
            case LANGKWT_double:
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(1, sizeof(double));
                                  *(double*)(*newNode)->data = *(double*)node->data;
                              }; break;
            case LANGKWT_num_bracket:
            case LANGKWT_delim:
            case LANGKWT_operator_bracket:
            case LANGKWT_conditional:
            case LANGKWT_io:
            case LANGKWT_new_program:
            case LANGKWT_new_function:
            case LANGKWT_new_variable:
            case LANGKWT_return:
            case LANGKWT_assign:
            case LANGKWT_loop:
            case LANGKWT_mathOperator:
                              {
                                  assert(node->data);
                                  (*newNode)->data = calloc(1, sizeof(unsigned));
                                  *(unsigned*)(*newNode)->data = *(unsigned*)node->data;
                              }; break;
            case LANGKWT_nil:
                              {
                                  (*newNode)->data = NULL;
                              }; break;
            default:
                              {
                                    printf("\nError: Unknown type of node == '%i' is passed to the 'dumbDump' method\n", node->type);
                                    myThrow(TREEERR_UNKTYPE);
                              }; break;
        }

    if ((*newNode)->left)
        _treecpy(&(*newNode)->left, node->left);
    if ((*newNode)->rght)
        _treecpy(&(*newNode)->rght, node->rght);
}

TreeNode* treeCopy(TreeNode* tree)
{
    _assert(tree);

    TreeNode* newTree = NULL;
    _treecpy(&newTree, tree);
    return newTree;
}

void _mathTreeDump(TreeNode* node)
{
    assert(node);
        switch (node->type)
        {
            case LANGKWT_double:
                              {
                                  assert(node->data);
                                  fprintf(mathDumpFile, "%lg", *(double*)node->data);
                              }; break;
            case LANGKWT_mathOperator:
                #define treeOp(a) *(unsigned*)(a)
                              {
                                assert(node->data);
                                switch (treeOp(node->data))
                                {
                                    case treeMath_ln: {
                                                            fprintf(mathDumpFile, "\\ln{(");
                                                            _mathTreeDump(node->left);
                                                            assert(!node->rght);
                                                            fprintf(mathDumpFile, ")}");
                                                      }; break;

                                    case treeMath_deg:
                                                          if ((node->rght->type == LANGKWT_double) && (*(double*)(node->rght->data) == 0.5))
                                                          {
                                                            fprintf(mathDumpFile, "\\sqrt{");
                                                            _mathTreeDump(node->left);
                                                            fprintf(mathDumpFile, "}");
                                                            break;
                                                          };
                                    case treeMath_minus:
                                    case treeMath_plus:
                                    case treeMath_div:
                                    case treeMath_mult: {
                                                        assert(node->type == LANGKWT_mathOperator);
                                                        assert((!node->uppr) || (node->uppr->type == LANGKWT_mathOperator));

                                                        if ( (node->uppr) && (!isTreeFunc(treeOp(node->uppr->data))) &&
                                                        ( mathOpPrior(treeOp(node->uppr->data)) < mathOpPrior(treeOp(node->data)) ) )
                                                        {
                                                            fprintf(mathDumpFile, "\\left(");
                                                            assert(node->left);
                                                            _mathTreeDump(node->left);
                                                            fprintf(mathDumpFile, "%c", treeOp(node->data));
                                                            assert(node->rght);
                                                            _mathTreeDump(node->rght);
                                                            fprintf(mathDumpFile, "\\right)");
                                                        } else
                                                        {
                                                            if ((treeOp(node->data) == treeMath_div))
                                                            {
                                                                fprintf(mathDumpFile, "\\frac{");
                                                                _mathTreeDump(node->left);
                                                                fprintf(mathDumpFile, "}");
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->rght);
                                                                fprintf(mathDumpFile, "}");
                                                            } else
                                                            {
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->left);
                                                                fprintf(mathDumpFile, "}");
                                                                fprintf(mathDumpFile, "%c", treeOp(node->data));
                                                                fprintf(mathDumpFile, "{");
                                                                _mathTreeDump(node->rght);
                                                                fprintf(mathDumpFile, "}");
                                                            }
                                                        }
                                                        }; break;

                                    default:  {
                                                printf("\nError: Unknown mathOp == '%c' is passed to the '_mathTreeDump' method.\n", treeOp(node->data));
                                                myThrow(TREEERR_UNKTYPE);
                                              }; break;
                                }
                              }; break;
                #undef treeOp
            case LANGKWT_id:
                              {
                                  fprintf(mathDumpFile, "x");
                              }; break;
            default:
                              {
                                    prln();
                                    printf("\nError: Unknown type of node == '%i' is passed to the 'nodeDump' method.\n", node->type);
                                    myThrow(TREEERR_UNKTYPE);
                              }; break;
        }
}

char* someBosh()
{
    static unsigned callNum = 0;
    callNum++;
    const unsigned dumpLimit = 15;

    if (callNum == dumpLimit)
        return "Дальнейшие переходы не вызовут вопросов даже у Тер-Крикорова, в связи с чем наше радио \"Фил-твои-шутки-уже-не-торт-FM\""
               "прекращает своё вещание";
    if (callNum > dumpLimit)
        return "";

    int toChoose = 0;

    if (callNum <= 12)
    {
        toChoose = callNum;
    } else
    {
        toChoose = rand() % 13;
    }


    switch (toChoose)
    {
        #define ADD_STRING(num, str) \
        case num: { return str; };
    ADD_STRING(0, "Я знаю, дети, вы хотите спать/ботать матан/не ботать матан, но мы с вами-таки добьём этот номер!")
    ADD_STRING(1, "Данный переход даже Беклемишеву очевиден:")
    ADD_STRING(2, "Откровенно говоря, тот, кто это писал(mr.\\%GODNAME\\%) списал этот пример из Демидовича и сам не до конца понял данный переход")
    ADD_STRING(3, "А в данном преобразовании намеренно допущена ошибка, чтобы вы попытались её найти(это не шутка!)(шутка)")
    ADD_STRING(4, "Передаю привет маме, папе, Дединскому, Кнуту - всем, кто учил меня уважать космос")
    ADD_STRING(5, "А чтобы научиться совершать этот переход, мне пришлось продать \\%ROOTNAME\\% душу")
    ADD_STRING(6, "Если вы один из моих NaN друзей ВК, который недоумевает, что здесь происходит, можете просто понегодовать, что ВК нет кнопки dislike")
    ADD_STRING(7, "\\end{center}\\begin{flushleft}\n"
                  "Звонит, значит, программист беременной жене:\\\\— Ты где?\\\\— На сохранении.\\\\— Ну ладно, позвонишь, когда сохранишься."
                  "\\end{flushleft}\\begin{center}\n")
    ADD_STRING(8, "Думал, это ландафшиц? Нет, мальчик, мы здесь преобразовываем производные")
    ADD_STRING(9, "Точку меняем на птичку, птичку на точку(с)")
    ADD_STRING(10,"Хо-хо, а разобраться в данном переходе - то, что Донов прописал")
    ADD_STRING(11,"I send regards to you, Phil2022, who is showing at the moment this 6-year-old work to some interview holder from Google/Intel/\\textbf{1C}?")
    ADD_STRING(12,"Права на этот переход принадлежат Филиппу из П-412. Если вы хотите узнать, как он был произведён(не Филипп, а переход),"
                  " то принесите уже наконец Филиппу что-нибудь сладенькое!")
    }
}
//#define CRAZYMODE
void addMathDump(TreeNode* tree, bool addStroke)
{
    assert(tree);

    static int isAlreadyCalled = 0;

    srand(isAlreadyCalled*32+50);

    if (isAlreadyCalled)
    {
    #ifdef CRAZYMODE
        if (isAlreadyCalled == 13)
        {
            fprintf(mathDumpFile, ".\n"
                                  "\\end{equation}\n"
                                  "\\fontsize{16}{16pt}\\selectfont"
                                  "\\textbf{Чёрт возьми, господа, что мы с вами c какой-то легкотнёй возимся! Физтехи мы или кто?!?!"
                                  " Айда ещё одну производную возьмём! Да чтобы в экран не вместилась!}\n"
                                  "\\fontsize{8}{8pt}\\selectfont"
                                  "\\begin{equation}\n");
            //
            tree = treeDiffer(tree);
        } else
    #endif
            fprintf(mathDumpFile, "%s=\n\\end{equation}\n"
                                  "\\fontsize{16}{16}\\selectfont\n"
                                  "%s\n"
                                  "\\fontsize{8}{8}\\selectfont\n"
                                  "\\begin{equation}\n\t", (addStroke) ? "'" : " ", someBosh());
    } else
    {
        initTexFile();
    }

    fprintf(mathDumpFile, "\\left(");
    _mathTreeDump(tree);
    fprintf(mathDumpFile, "\\right)");

    isAlreadyCalled++;
}

#define OPERATOR_DECLARE(oper, operChar)                                    \
TreeNode operator oper (TreeNode &node1, TreeNode &node2)                   \
{                                                                           \
    assert(&node1 != &node2);                                               \
    assert(&node1); assert(&node2);                                         \
                                                                            \
    unsigned mathOp = operChar;                                             \
                                                                            \
    TreeNode newNode = {};                                                  \
    nodeCtor(&newNode, LANGKWT_mathOperator, mathOp, NULL, &node1, &node2); \
    node1.uppr = node2.uppr = NULL;                                         \
                                                                            \
    return newNode;                                                         \
}

OPERATOR_DECLARE(+, '+')
OPERATOR_DECLARE(*, '*')
OPERATOR_DECLARE(-, '-')
OPERATOR_DECLARE(/, '/')
OPERATOR_DECLARE(^, '^')

#endif
