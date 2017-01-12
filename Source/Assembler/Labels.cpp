#include "Labels.hpp"
#include <cstdio>
#include <cstring>
#include <cassert>

void AsmLabels::dump()
{
    for (int i = 0; i < labelNum_; i++)
        if ((addresses_[i] != -1) || (names_[i][0] != '\0'))
        {
            printf("\n\tlabel[%02d]:\n", i);
            printf(  "\tlabelName = %s\n",    names_[i]);
            printf(  "\tlabel addr = %d\n",   addresses_[i]);
        }
}

void AsmLabels::appendLabel(const char* name, unsigned addr)
{
    assert(labelNum_ <= MAX_LBL_QT);
    strncpy(names_[labelNum_], name, MAX_LBL_LEN - 1);
    assert((int)addr >= 0);
    addresses_[labelNum_] = (int)addr; 
    labelNum_++;
};

int AsmLabels::num2Addr(unsigned num)
{
    return addresses_[num];
}

int  AsmLabels::name2Addr(const char* name)
{
    for (int p = 0; p < labelNum_; p++)
        if (strncasecmp(names_[p], name, MAX_LBL_LEN - 1) == 0)
            return addresses_[p];
    return -1;
}

const char* AsmLabels::num2Name(unsigned num)
{
    return names_[num];
}

const char* AsmLabels::stdLabel()
{
    return STD_LABEL;
}

int AsmLabels::getNum()
{
    return labelNum_;
}
