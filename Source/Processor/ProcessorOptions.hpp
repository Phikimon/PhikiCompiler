#ifndef PROCESSOR_OPTOINS_HPP_INCLUDED
#define PROCESSOR_OPTOINS_HPP_INCLUDED

enum CPUOptionsErrors
{
    CPUOPTERR_OK,
    CPUOPTERR_NOT_SPECIFIED,
    CPUOPTERR_NO_ARGUMENT,
    CPUOPTERR_UNKNOWN_OPTION
};

struct ProcessorOptions
{
    ProcessorOptions(int argc, char** argv);

    bool vflag, pflag, tflag;
    char* ivalue;
};

#endif
