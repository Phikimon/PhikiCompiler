#include "Stack.hpp"
#include <iostream>
#include <cstdio>

void CPUStack::dump()
{
    printf("\t Stack dump[%p]\n", this);
    printf("\t{\n");
    printf("\t stackSize = %i\n", c.capacity());
    printf("\t stackCount = %i\n", c.size());

    printf("\t 'data' content:\n");
    int i = 0;
    for(int n : c)
    {
        printf("\n\t%-3d: ", i);
        std::cout << n;
        i++;
    }
    printf("\t\n}\n");
};

stack_t CPUStack::getRandAccess(int offset)
{
    return c.at(c.size() - offset);
}
