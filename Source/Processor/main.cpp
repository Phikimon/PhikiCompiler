#include "Processor/Processor.hpp"

int main(int argc, char** argv)
{
    /*
        (\___/)
        (='.'= )
        (")_(")
    */
    printf("\n  +--------------------------+\n");
    printf(  "  | Phila's processor #%x    |\n", CPU_SIGNATURE);
    printf(  "  +--------------------------+\n");

    //Create processor instance
    Processor proc(argc, argv);

    //Execution itself(verbose if corresponding option is set)
    proc.execute();
    return 0;
}
