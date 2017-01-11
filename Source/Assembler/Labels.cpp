#include "Labels.hpp"

bool _labelOk(myLabel* label, bool verbose, const char labelName[MAX_LINE_LEN])
{
    bool ok = false;
    try
    {
        ok = (! ( (label->ptr == -1) || (!label->name) ) ) && label;
    }
    catch (...) {ok = false;};

    if (verbose) 
    {
        if (1)//(ok)
        {
        printf("\n\tLabel \"%s\" is okay[%p]\n", labelName, &label);
        printf("\t{\n");
        printf("\t label->ptr  = %i[%p]\n", label->ptr, &label->ptr);
        printf("\t label->name = %s[%p]\n", label->name, &label->name);
        printf("\t}\n");
        } else
        {
            printf("\tLabel \"%s\" is NOT okay! [%p]\n", labelName, &label);
        }
    }

    return !ok;
}

void labelsctor(myLabel* labels[MAX_LBL_QT])
{
    for (int i = 0; i < MAX_LBL_QT; i++)
    {
        labels[i] = (myLabel*)calloc(1, sizeof(**labels));
        labels[i]->ptr = -1;
        strcpy(labels[i]->name, ":");
        //_labelOk(labels[i], true, "labels[i]");
    };
};

void labelsdtor(myLabel* labels[MAX_LBL_QT])
{
    for (int i = 0; i < MAX_LBL_QT; i++)
    {
        labels[i]->ptr = -1;
        free(labels[i]);
        labels[i] = NULL;
    };
};
