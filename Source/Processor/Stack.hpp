#ifndef CPUSTACK_HPP_INCLUDED
#define CPUSTACK_HPP_INCLUDED
#include <stack>
#include <vector>

typedef double registers_t;
typedef registers_t stack_t;

class CPUStack : public std::stack< stack_t, std::vector<stack_t> >
{
    public:
    using std::stack< stack_t, std::vector<stack_t> >::c;
    void dump();
    stack_t getRandAccess(int offset);
};

#endif
