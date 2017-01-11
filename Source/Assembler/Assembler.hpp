#ifndef ASM_H_INCLUDED
#define ASM_H_INCLUDED

#define MAX_ARG_QT 3

#include "Common/Common.hpp"
#include "Common/Char_buffers.hpp"
#include "Labels.hpp"

enum { cpuSignature = 0x00 };
extern const char* utResFileName;
extern const char* utDisFileName;
extern const char *asmVersion;
 
//Assembler type
enum asmType
{
    asm_Translate = 1,
    asm_Untranslate = 0,
};

//Commands for myAsm
enum myAsmCmdNum
{
    #define DEF_CMD(CMD_NAME, CMD_ARG_STRING, CMD_CODE) \
    CPU_##CMD_NAME, 
    
    #include "Lists/Processor_cmd_list.hpp"
    
    #undef DEF_CMD
    ASMCMDQT, // my assembler command quantity
};

//My assembler commands' arguments
extern const char* myAsmCmdArg[];

enum ASM_argType
{
    at_reg = 1,
    at_imm = 0
};

//My assembler commands' arguments
extern const char* myAsmCmd[];

enum ASMERRORS_T
{
    ASMERR_EMPTY = 0,      //< No error occured
    ASMERR_UNKCMD = -1,    //< Error: Unknown assembler command
    ASMERR_TOOFEWARG = -2, //< Error: Too few arguments
    ASMERR_UNKARGTYPE = -3,//< Error: Unknown argument type
    ASMERR_TWOCMD = -4,    //< Error: Two commands in one line or extra argument
    ASMERR_EMPPROG = -5,   //< Error: Empty program is in
    ASMERR_WRGARGTYPE = -6,//< Error: Wrong argument type
    ASMERR_TOOLNGLBL = -7, //< Error: Too long label
    ASMERR_NULLLENLBL = -8,//< Error: Label with null length
    ASMERR_UNDECLLBL = -9, //< Error: Undeclared label
    ASMERR_WRSIGN = -10,
};

struct asm_t
{
    unsigned checkSum;
    unsigned leftCanary;
    unsigned seed[2];
        
    charBuf_t *srcBuf = NULL;
    charBuf_t *resBuf = NULL;
    
    int cmdWrittenQt = 0; // How many commands are already in hex
    
    int labelPtr; // it stores number of label struct that is being currently filled
    myLabel* labels[MAX_LBL_QT];
    
    unsigned rightCanary;
};

/**
 constructor for 'assembler' structure
 */
int asmctor(asm_t *assm, FILE* srcFile, asmType T, int verbose = 0);
/**
 destructor for 'assembler' structure
 */
void asmdtor(asm_t *assm);
/**
 Calculates the checkSum
 */
unsigned _checkSum(asm_t* assm, bool verbose = false, const char asmName[MAX_LINE_LEN] = "name");
/**
  Look for labels. Fill the 'labels' array.
 */
void fillLabelsTranslate(asm_t *assm);
/**
  Look for labels in the hex file. Fill the 'labels' array.
 */
void fillLabelsUntranslate(asm_t *assm);
/**
 Translates assembler code to known commands
 */
int translate(asm_t *assm);
/**
 Disassembles machine codes
 */
int unTranslate(asm_t *assm);
/**
 validates the asm structure and prints dump if needed
 */
int asmOk(asm_t *assm, int verbose);
/**
 Unit tests for assembler
 */
void ut_asm_disasm(bool verbose = false, unsigned int mask = 0xffffffff);

#endif
