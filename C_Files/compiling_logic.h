#ifndef compiling_logic
#define compiling_logic

/* Define the struct representing an ASM command*/
typedef struct instruction_machine_code_rep{
    const char *name;  /* Assembly command name*/
    int opcode;        /* Opcode number*/
    int funct;         /* Funct number*/
} instruction_machine_code_rep;

/* List of valid ASM commands*/
static const instruction_machine_code_rep instructions_machine_code_rep[] = {
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

/*
 ASM Command values in an array format.

const int list_of_opcode_numbers[] = {0,1,2,4,5,9,12,13,14,15};
const int list_of_funct_numbers[] = {1,2,3,4};
*/


/* The different addressing modes that the operands can have */
enum addressing_mode {
    IMMEDIATE=0,
    DIRECT=1,
    REGISTER=3,
    RELATIVE=2,
    NOT_VALID=-1
};

enum directive_type {
    DATA,
    STRING,
    ENTRY,
    EXTERN
};

/* The different states for when you go over a read line in the sceond scan.*/
enum read_line_type {
    COMMAND,
    DIRECTIVE,
    LABEL,
    EMPTY
};

typedef struct addressing_modes_for_command{
    char *source_addressing_modes;   /* The addressing modes that the source operand can have */
    char *target_addressing_modes;  /* The addressing modes that the target operand can have */
} addressing_modes_for_command;

/* The valid addressing modes for each command's arguments */
static const addressing_modes_for_command addressing_modes_for_commands[16] = {
        {"0,1,3", "1,3"},
        {"0,1,3", "0,1,3"},
        {"0,1,3", "1,3"},
        {"0,1,3", "1,3"},
        {"1", "1,3"},
        {NULL, "1,3"},
        {NULL, "1,3"},
        {NULL, "1,3"},
        {NULL, "1,3"},
        {NULL, "1,2"},
        {NULL, "1,2"},
        {NULL, "1,2"},
        {NULL, "1,3"},
        {NULL, "0,1,3"},
        {NULL, NULL},
        {NULL, NULL}
};

static const int num_of_arguments_each_command_has[16] = {2,2,2,2,2,1,1,1,1,1,1,1,1,1,0,0};

/* The number of bits the compiler shall shift when wanting to get to a specific position of an Instruction Ta */
#define INDEX_OF_OPCODE_BYTE 18
#define INDEX_OF_FUNCT_BYTE 3
#define INDEX_OF_SOURCE_ADDRESING_MODE_BYTE 16
#define INDEX_OF_TARGET_ADDRESING_MODE_BYTE 11
#define INDEX_OF_SOURCE_REGISTER_BYTE 13
#define INDEX_OF_TARGET_REGISTER_BYTE 8
#define INDEX_OF_THE_A_BYTE 2
#define INDEX_OF_THE_R_BYTE 1
#define INDEX_OF_THE_E_BYTE 0
#define INDEX_OF_THE_BIT_AFTER_A 3

/* */
#define NUM_OF_ASM_COMMANDS 16


#endif