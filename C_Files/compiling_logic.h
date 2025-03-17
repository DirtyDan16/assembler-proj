#ifndef compiling_logic
#define compiling_logic
/* Define the struct representing an ASM command*/
typedef struct {
    const char *name;  /* Assembly command name*/
    int opcode;        /* Opcode number*/
    int funct;         /* Funct number*/
} AsmCommand;

/* List of valid ASM commands*/
static const AsmCommand asmCommands[] = {
    {"mov",  0,  0}, 
    {"cmp",  1,  0}, 
    {"add",  2,  1}, 
    {"sub",  2,  2}, 
    {"lea",  4,  0}, 
    {"clr",  5,  1}, 
    {"not",  5,  2}, 
    {"inc",  5,  3}, 
    {"dec",  5,  4}, 
    {"jmp",  9,  1}, 
    {"bne",  9,  2}, 
    {"jsr",  9,  3}, 
    {"red",  12, 0}, 
    {"prn",  13, 0}, 
    {"rts",  14, 0}, 
    {"stop", 15, 0}
};

/* The number of bits the compiler shall shift when wanting to get to a specific position of an Instruction Ta */
#define INDEX_OF_OPCODE_BYTE 18
#define INDEX_OF_FUNCT_BYTE 3
#define INDEX_OF_SOURCE_ADDRESING_MODE_BYTE 16
#define INDEX_DEST_ADDRESING_MODE_BYTE 11
#define INDEX_OF_SOURCE_REGISTER 13
#define INDEX_OF_DEST_REGISTER_BYTE 8
#define INDEX_OF_THE_A_BYTE 2
#define INDEX_OF_THE_R_BYTE 1
#define INDEX_OF_THE_E_BYTE 0

/* */
#define NUM_OF_ASM_COMMANDS 16


#endif

