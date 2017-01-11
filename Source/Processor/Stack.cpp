#include "Stack.hpp"

int stackctor(stack_t* stack, int Size)
{
    assert(stack);
    
    //Generate random seeds
    for (int i = 0; i < 2; i++)
    {
        stack->seed[i] = genSeed();
    };
    //Check input pointer
    if (Size == 0)
    {
        prln();
        printf("\nStack error: size can't be equal to zero\n");
        throw(STACKERR_NULL_SIZE);
    };

    stack->leftCanary = myRandom(stack->seed[0]);
    stack->rightCanary= myRandom(stack->seed[1]);
    stack->count = 0;
    stack->size = Size;
    ALLOC(stack->data = (stackData_t*)calloc(stack->size, sizeof(*stack->data)));
    stack->checkSum = _checkSum(stack, false);
    //checkSum(stack, true);
    return 0;
}

int stackdtor(stack_t* stack) 
{
    assert(stack);
    
    free(stack->data);
    
    for (int i = 0; i < 2; i++)
        stack->seed[i] = 0xBADF00D;
        
    stack->rightCanary = 0xBADCAFE;
    stack->leftCanary = 0xC0CAC01A;
    
    stack->size = -1;
    stack->count = -1;
    
    stack->checkSum = 0xDEADBEEF;
    
    free(stack);
    
    return 0;
}

unsigned _checkSum(stack_t* stack, bool verbose, const char stackName[MAX_LINE_LEN]) 
{
    assert(stack);
    
    unsigned checkSum = 0;
    if (verbose) prdel();
                                    if (verbose) printf("\n\tCalculating CheckSum of '%s'\n", stackName);
    checkSum ^= stack->leftCanary;  if (verbose) printf("\t stack->leftCanary = %x\n", stack->leftCanary);
    checkSum ^= stack->rightCanary; if (verbose) printf("\t stack->rightCanary = %x\n", stack->rightCanary);

    checkSum ^= stack->count;       if (verbose) printf("\t stack->count = %i\n", stack->count);
    checkSum ^= stack->size;        if (verbose) printf("\t stack->size = %i\n", stack->size);
    
    for (int i = 0; i < 2; i++)
    {
        checkSum ^= stack->seed[i]; if (verbose) printf("\t stack->seed[%i] = %x\n", i, stack->seed[i]);
    };
    
    assert(stack->data);
    if (verbose) printf("\t stack->data = {");
    for (int i = 0; i < stack->size; i++) 
    {
        checkSum ^= (unsigned)stack->data[i]; if (verbose) printf("%2x(%d); ", stack->data[i], stack->data[i]);
    }
    if (verbose) printf("\n\t CheckSum = %u\n     CheckSumPrev = %u\n", checkSum, stack->checkSum);
    if (verbose) printf("\t  CheckSum is %s\n\n", (checkSum == stack->checkSum) ? "okay" : "NOT OKAY");
    if (verbose) printf("}\n");
    if (verbose) prdel();
    
    return checkSum;
}

int reallocStack(stack_t* stack) 
{
    assert(stack);
    assert(!_stackOk(stack, false));
    if (_stackOk(stack, false))
        return -1;
    
    stackData_t* newStackData = (stackData_t*)calloc(stack->size * 2, sizeof(*newStackData));
    for (unsigned i = 0; i < stack->size*2; i++)
    {
        newStackData[i] = ((i < stack->size) ? stack->data[i] : 0);
    };
    
    free(stack->data);
    stack->data = newStackData;
    
    /*stackData_t *oldstack;
    printf("\n__stack->data = %p__", stack->data);
    if ((oldstack = (stackData_t*)realloc(stack->data, stack->size * 2)) == NULL) throw ERR_ALLOC;
    printf("\n__oldstack    = %p__\n", oldstack);
    stack->data = oldstack;
    
    //Initialize memory
    for (stackData_t* i = stack->data + stack->size; i < stack->data + stack->size*2; i++)
    {
        printf("%i %i \n", i-stack->data, *i);
        *i = 0;
    };*/
    
    stack->size *= 2;
    
    return 0;
}

bool _stackOk(stack_t* stack, bool verbose, const char stackName[MAX_LINE_LEN])
{
    assert(stack);

    bool ok = (stack->leftCanary == myRandom(stack->seed[0])) && (stack->rightCanary == myRandom(stack->seed[1])) && 
              (stack->checkSum == _checkSum(stack)) && (stack->size > 0) && (stack->data) && (stack->count >= 0);
    
    //Shout if neccesary
    if (verbose)
    {
        printf("\t Stack '%s' is %s [%p] \n", stackName, (ok ? "okay" : "NOT OKAY"), stack);
        printf("\t{\n");
        printf("\t stackSize = %i\n", stack->size);
        printf("\t stackCount = %i\n", stack->count);
        #ifdef NDEBUG
        printf("\t Sorry, I can't show you canaries' values, but:\n");
        printf("\t  leftCanary is %s\n", ((stack->leftCanary == myRandom(stack->seed[0])) ? "okay" : "NOT OKAY"));
        printf("\t  rightCanary is %s\n", ((stack->rightCanary == myRandom(stack->seed[1])) ? "okay" : "NOT OKAY"));
        printf("\t  CheckSum is %s\n\n", (_checkSum(stack) == stack->checkSum) ? "okay" : "NOT OKAY");
        #else
        printf("\n\t Canaries:\n");
        printf("\t leftCanarySeed        == %x\n", stack->seed[0]);
        printf("\t  leftCanary           == %x\n", stack->leftCanary);
        printf("\t  leftCanary has to be == %x\n", (myRandom(stack->seed[0])));
        printf("\t  leftCanary is %s\n\n", ((stack->leftCanary == myRandom(stack->seed[0])) ? "okay" : "NOT OKAY"));
        
        printf("\t rightCanarySeed        == %x\n", stack->seed[1]);
        printf("\t  rightCanary           == %x\n", stack->rightCanary);
        printf("\t  rightCanary has to be == %x\n", myRandom(stack->seed[1]));
        printf("\t  rightCanary is %s\n\n", ((stack->rightCanary == myRandom(stack->seed[1])) ? "okay" : "NOT OKAY"));
        checkSum(stack, true);
        #endif
        assert(stack->data);
        printf("\t 'data' content: [%p]\n", stack->data);
        int i = 0;
        while (((stack->data[i] != 0) || (i < stack->count)) && (i < stack->size))
        {
          /*  if (typeid(*stack->data) == typeid(double))
            {
                printf("\t   %s %4lg  \n", (i <= stack->count) ? "*" : " ", stack->data[i]);
                
            };
            if (typeid(*stack->data) == typeid(int))
            {
                printf("\t   %s %4i  \n", (i <= stack->count) ? "*" : " ", stack->data[i]);
                i++;
            };*/
            if (typeid(*stack->data) == typeid(unsigned))
            {
                printf("\t   %s %4u  \n", (i <= stack->count) ? "*" : " ", stack->data[i]);
            };
            i++;
        };
        printf("\n\t}\n\n");
    }
    return !ok;
};

int stackPush(stack_t* stack, stackData_t value, bool verbose)
{
    assert(stack);
    assert(!_stackOk(stack, false, "from stackPush"));
    if (_stackOk(stack, false))
        return -1;
     //   if (stack->count + 1 == stack->size) 
    //stackOk(stack, true);
    if (stack->count + 1 == stack->size) 
    {
        reallocStack(stack);
    }
     //       if (stack->count + 1== stack->size) 
    //stackOk(stack, true);
    stack->data[stack->count++] = value;

    stack->checkSum = _checkSum(stack);
    
    stackOk(stack, verbose);
    
    return 0;
}

stackData_t stackPop(stack_t* stack, bool verbose)
{
    assert(stack);
    assert(!_stackOk(stack, verbose, "from stackPop"));
    if (_stackOk(stack, false))
        return -1;
    
    if (!stack->count)
    {
        prln();
        printf("\nStack error: nothing to pop\n");
        throw (STACKERR_NOPOP);
    }
    
    if (stack->count + 1 >= stack->size) 
    {
        reallocStack(stack);
    }
    stack->count--;
    stack->checkSum = _checkSum(stack);

    return stack->data[stack->count];
}

Govnocode //Replace with static
bool STACK_ERROR_HAPPENED = false;
/**
 Unit tests for stack
 */
void ut_stack(bool verbose)
{    
            try
            {
                prdel();
                printf("\nStack unit test.\n");
                stack_t* stack;

                if (! ( stack = (stack_t*)calloc(1, sizeof(*stack) ) ) )
                {
                    throw ERR_ALLOC;
                }
                //Pass random seed to the stackctor
                stackctor(stack, INIT_STACK_SIZE);
                //First test
                bool testIsOk = true;

               // stackOk(stack, verbose);
                                stackPush(stack, 0);
                                checkSum(stack, true);
               // stackOk(stack, verbose);
                                stackPush(stack, 21);
                                checkSum(stack, true);
               // stackOk(stack, verbose);
                                stackPop(stack);
                                checkSum(stack, true);
               // stackOk(stack, verbose);
                                stackPop(stack);
                                checkSum(stack, true);
               // stackOk(stack, verbose);
#define CALLDUMPS() \
                if (!STACK_ERROR_HAPPENED) \
                { \
                    STACK_ERROR_HAPPENED = true; \
                    printf("\nStack unit test is NOT okay. There are some dumps for you:\n"); \
                    prdel(); \
                    ut_stack(true); \
                }
                
                if (testIsOk)
                {
                    printf("\n\nStack unit test is okay");
                    prdel();
                }
                else
                {
                    CALLDUMPS();
                }
                
            }
            catch (ERRORS_T errCode)
            {
                printf("\n%s\n", ERR_ST(errCode));
                CALLDUMPS();
                throw(ERR_EMPTY);
            }
            catch (STACKERRORS_T errCode)
            {
                printf("\nError STACKERRORS_T #(%i)\n", errCode);
                CALLDUMPS();
                throw(ERR_EMPTY);
            };
#undef CALLDUMPS
}
