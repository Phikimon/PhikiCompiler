#ifndef LABELS_HPP_INCLUDED
#define LABELS_HPP_INCLUDED

 #include "Common/Common.hpp"

#define standardLabel "Philiscoolguy"
#define MAX_CMD_LBL_LEN 64
#define MAX_LBL_QT 64

//Stores label's name and where it points to
struct myLabel 
{
    
    char name[MAX_CMD_LBL_LEN];
    
    int ptr = -1;
};

/**
 * Verificator and dump for myLabel struct
 */
 bool _labelOk(myLabel* label, bool verbose = false, const char labelName[MAX_LINE_LEN] = "name");
#define labelOk(lbl, verb) (assert(strlen(#lbl) <= MAX_LINE_LEN), _labelOk(lbl, verb, #lbl))

/**
 * Constructor for myLabel struct
 */
void labelsctor(myLabel* labels[MAX_LBL_QT]);

/**
 * Destructor for myLabel struct
 */
void labelsdtor(myLabel* labels[MAX_LBL_QT]);

#endif
