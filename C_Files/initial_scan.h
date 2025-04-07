#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"

#include <math.h>

/**
 * The first scan of the ASM file goes over a given After Macro file and gathers information about its lines. 
 * 
 * The function stores in its key_resources struct (a struct that holds importnat info of stuff that needs to be accessed on a global scale)
 * Machine Code of Instructions and Data that it reads from [from what it can at least]
 * 
 * Also stores labels that it detects in a list. 
 * 
 * Outputs errors of problems it detects in the file, and pinpoints the line number.
 * 
 * @param 1st the start of the AM pointer. used in order to read lines from the AM file, so the compiling can be done.
 * @param 2nd a reference to the key resources that needs to be accessed on a global scope.
 */
void initial_scan(FILE* start_of_am_file_pointer,key_resources* key_resources);

/**
 * This function gets a read line and dictates what that line is, and what dhould be done with it. 
 * it detects if there's a label attached to the line and takes note of it. 
 * directs to function that knows how to deal with a directive / instruction (based on what it thinks the line is)
 * 
 * @param 1st the read AM line
 * @param 2nd a reference to the key resources that needs to be accessed on a global scope.
 */
static void go_over_read_line(char* chosen_line,key_resources* key_resources);

/*---------------------------------DIRECTIVE RELATED FUNCTIONS -----------------------------------------------------------------------------*/

/**
 * this function gets a directive line and determines which type of directive it is - rediecting to a function that knows how to deal with that type. 
 * 
 * 
 * @param 1st the read AM line
 * @param 2nd a reference to the key resources that needs to be accessed on a global scope.
 */
void handle_directive(char* line,key_resources* key_resources);

/**
 * deals with the .data directive type. 
 * inspects the value attached to the data directive symbol, and adds each number inside it to the Data Table. Each number occupies a different Data Mila.
 * 
 * @param 1st the line. 
 * @param 2nd the table that stores Machine Code of Data.
 */
void handle_data_directive(char* data,mila* data_table);

/**
 * deals with the .string directive type. 
 * inspects the value attached to the string directive symbol, and adds each character inside the quatation marks to the Data Table. Each character
 * occupies a different Data Mila. Adds at the end of the string a Mila for the '\0' , which is just 0.
 * 
 * @param 1st the line. 
 * @param 2nd the table that stores Machine Code of Data.
 */
void handle_string_directive(char* string,mila data_table[]);

/**
 * a helper function to get the actual value of the string in the string directive.abort
 * checks the validity of the string as well  , and acts accordingly. 
 * 
 * deals with the .data directive type. 
 * inspects the value attached to the data directive symbol, and adds each number inside it to the Data Table. Each number occupies a different Data Mila.
 * 
 * @param 1st the string (with quo marks)
 * 
 * @return returns the string without the quotation marks attached, or NULL if the function notices that the string isn't formatted ok.
 */
char* get_string_directive_value(char* value);

/**
 * deals with the .extern directive type. 
 * inspects the value attached to the extern directive symbol, and adds the symbol to the Labels list, with an ".external" mark attached to.
 * 
 * @param 1st the name of the extern. 
 * @param 2nd a reference to the key resources that needs to be accessed on a global scope.
 */
void handle_extern_directive(char *extern_name, key_resources *key_resources);

/*--------------------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------INSTRUCTIONS FUNCTIONS ---------------------------------------------------------------------------------*/

/**
 * deals with an instruction sentence. 
 * Checks if the instruction sentence is valid, and if it is, it codes it into the the Instruction Table ( a table that stores machine code of instructions)
 * what it can do. 
 * Knows how to deal with arguments of type Immediate and of type Register at this stage.
 * 
 * @param 1st the instruction sentence
 * @param 2nd a reference to the key resources that needs to be accessed on a global scope.
 */
static void handle_command(char* command,key_resources* key_resources);

/**
 * a helper function that adds to a free space of the Instruction Table a new element to it.
 * the element is of type of Instruction Struct - which formats in a nice way all the Milas 
 * related to a given instruction sentence, as well as its address and length of Milas.
 * 
 * @param general the different fields needed for the Instruction struct.
 */
void add_instruction_to_table(instruction* instruction,mila binary_value_of_command,mila binary_value_for_first_argument,mila binary_value_for_second_argument,int IC,int L);

/**
 * Deals with creating the machine code (that it can) to a given instruction, and its additional info Milas, while taking note that this is the argument number 1. 
 * 
 * @param 1st a pointer that holds values in a nice formatted way of an instruction line. also holds important information that helps decide what to do (such as amount of arguments)
 * @param 2nd a reference to the place where the machine code of the Main Mila is at. Adds machine code to it depending on what it knows to do.
 * @param 3rd a reference to the place where the machine code of the the additional info-Mila for argument number 1 is at. Adds machine code to it depending on what it knows to do.
 * @param 4th a reference to the length in Milas (machine code lines) this instruction uses. might be increased depending on the context.
 */
static void deal_with_first_parameter(instruction_sentence* cur_command_sentence, mila* cur_binary_value_of_command, mila* binary_value_for_first_argument, int* L);

/**
 * Deals with creating the machine code (that it can) to a given instruction, and its additional info Milas, while taking note that this is the argument number 2. 
 * 
 * @param 1st a pointer that holds values in a nice formatted way of an instruction line. also holds important information that helps decide what to do (such as amount of arguments)
 * @param 2nd a reference to the place where the machine code of the Main Mila is at. Adds machine code to it depending on what it knows to do.
 * @param 3rd a reference to the place where the machine code of the the additional info-Mila for argument number 2 is at. Adds machine code to it depending on what it knows to do.
 * @param 4th a reference to the length in Milas (machine code lines) this instruction uses. might be increased depending on the context.
 */
static void deal_with_second_parameter(instruction_sentence* cur_command_sentence, mila* cur_binary_value_of_command, mila* binary_value_for_second_argument, int* L);

/**
 * a fucntion that deals and codes into a Mila (which is a machine code line) of an argument that is the type - Immediate.
 * 
 * @param 1st a reference to the place where the machine code of the additional info-Mila is at. Adds machine code to it depending on what it knows to do.
 * @param 2nd the name of the argument. uses its value and translates it to machine code.
 */
void deal_with_immediate_type_value(mila* additional_mila,char* argument);

/**
 * a fucntion that deals and codes into a Mila (which is a machine code line) of an argument that is the type - Register.
 * Modifies the machine code the the Mila of the command, and not of additional info-milas. 
 * 
 * @param 1st a reference to the place where the machine code of the Main Mila is at. Adds machine code to it depending on what it knows to do.
 * @param 2nd a pointer that holds values in a nice formatted way of an instruction line.  also holds important information that helps decide what to do (such as amount of arguments)
 * @param 3rd the argument number. helps choose if the current argument is a scource argument, or a target argument, with the help of knowing from the 2nd param the amount of arguments.
 * knowing the differnce is needed to add correct machine code of the instruction.
 */
void deal_with_register_type_value(mila* cur_binary_value_of_command, instruction_sentence* cur_command_sentence,int argument_number);

/**
 * helper fucntion that pinpoint what type of argument a given argument is. the types are: Immediate,Direct,Relative,Register.
 * @param 1st the argument name.
 * 
 * @return the type, represented as an integer (we have an enum that converts this correctly). if the type is not valid somehow, returns -1.
 */
int determine_type_of_asm_argument(char *argument);

/**
 * Calulates the signature machine code of a given instruction name.
 * a signature of a instruction is its opcode and its fucnt values. 
 * each valid instruction name has a unique signature (a unique combination of these vlaues)
 * 
 * this function returns the numerical representation of a wanted instruction
 * @param 1st the index of the instruction. the index is predefined in a constant array. this index is used to access the value of its opcode and its funct. 
 * 
 * @return the numberical representation of the instruction signature.
 */
int get_binary_value_of_command_name(int index_of_command);

/**
 * a helper funciton that gives the index of an instruction.
 * the instruction names are ordered in a predefined order, and knowing a given index helps for different functions that use this order.
 * 
 * @param 1st the command name
 * 
 * @return the index of the command in the predefined order, or -1 if the name isn't a valid name
 */
int get_index_of_command(char* command_name);

/*--------------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------LABELS -------------------------------------------------------------------------------------------*/
/**
 * a function that adds to the Label Nodes list a new Label Node. 
 * a label node has a val, that is a Label Struct. the struct holds value that are needed to know about a label, such as its name, its address, any given identifiers
 * and if it is external or not.
 * 
 * @param general the different fields used to make the new Label struct.
 * @param key_resources a reference to the key resources that needs to be accessed on a global scope.
 */
void add_label_to_table(char* label_name,int label_address, char* type,key_label_nodes* key_resources);

/**
 * This is called at the end of the first scan. 
 * This updates the address of every symbol that points to a Data or String directive, adding to its address ICF,
 * which is the value of the last address of Instruction Mila.
 * 
 * @param 1st the address of the end of the instruction table.
 * @param 2nd pointers to the list of label nodes.
 */
void update_all_symbols_addresses_of_type_data(int ICF,key_label_nodes* labels);
/*--------------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------BOOLEAN FUCNTIONS------------------------------------------------------------------------------*/
/**
 * Checks whether the number of arguments in a given instruction sentence matches
 * the expected number for its command.
 *
 * @param cur_command_sentence A pointer to the current instruction sentence structure. this struct holds in a nice formatted way usefull information of the instruction.
 * @return true if the number of arguments is valid, false otherwise.
 */
bool check_if_num_of_arguments_are_valid(instruction_sentence* cur_command_sentence);

/**
 * Validates whether a specific argument in an instruction sentence is valid
 * based on the instruction's requirements (the types of arguments it can take. for example: cmp takes Immediate,Direct,And Register, but NOT Relative.).
 *
 * @param cur_command_sentence A pointer to the instruction sentence.
 * @param argument_type The type of the argument (e.g., immediate, direct, register).
 * @param argument_number The argument number. used to dictate if the argument is a source, or a target argument which is important since a 
 * certain type might be valid as a source, but not as a target, and vice versa. 
 * @return true if the argument is valid for the command at the given position, false otherwise.
 */
bool is_argument_valid_for_this_specific_command(instruction_sentence* cur_command_sentence, int argument_type, int argument_number);

/**
 * Checks whether a given string represents a valid label, and verifies that it is not 
 * a reserved word or invalid by syntax.
 *
 * @param line The input line or potential label string.
 * @param key_resources Pointer to important info, including the macro list which will be used for validation.
 * @return A pointer to the label string if valid, NULL otherwise.
 */
char* is_valid_label(char* line, key_resources* key_resources);

/**
 * Checks if a given label name has already been defined as a macro name.
 *
 * @param checked_name The name of the label to check.
 * @param key_nodes Pointer to the list of defined macro names.
 * @return true if the label name is already used as a macro, false otherwise.
 */
bool is_label_already_defined_as_macro_name(char* checked_name, key_macro_nodes* key_nodes);

/**
 * Checks if a label name is already defined in the label table.
 *
 * @param label_name The name of the label to check.
 * @param key_labels Pointer to the list of currently defined labels.
 * @return true if the label name already exists, false otherwise.
 */
bool is_label_already_defined(char* label_name, key_label_nodes* key_labels);

/**
 * Validates whether a given line starts with a known command.
 *
 * @param chosen_line The input line containing a potential command.
 * @return true if the line starts with a valid command, false otherwise.
 */
bool is_valid_command(char* chosen_line);

/**
 * Checks whether a line contains a directive (e.g., '.data', '.string', '.extern', '.entry').
 * Outputs an error as well if the was an attempt to create a directive (starts with a dot)
 * 
 * @param line The input line to analyze.
 * @return true if the line contains a directive, false otherwise.
 */
bool is_directive(char* line);

/**
 * Validates the structure of a '.data' directive sentence.
 * Ensures it only contains integers separated by commas and valid syntax.
 *
 * @param data_sentence The input sentence to validate.
 * @return true if the structure is valid, false otherwise.
 */
bool has_good_structure_for_data_directive_sentence(char* data_sentence);

/**
 * Checks whether a line is empty (only whitespace or nothing at all).
 *
 * @param line The input line.
 * @return true if the line is empty, false otherwise.
 */
bool is_empty(char* line);

/**
 * Checks whether a line is a comment (i.e., starts with a semicolon).
 *
 * @param line The input line.
 * @return true if the line is a comment, false otherwise.
 */
bool is_comment(char* line);

/**
 * Checks if a line contains a semicolon in the middle of it.
 *
 * @param line The input line.
 * @return true if the line contains a semicolon, false otherwise.
 */
bool has_semicolon(char* line);
/*--------------------------------------------------------------------------------------------------------------------------------------------*/

/**
 * a helper function that takes up a command line and makes out of it a Struct that stores information of it in a nice and convienient way. 
 * Includes: name of the command, name of the arguments, num of arguments, and the index of the command name in a predefined order.abort
 * 
 * @param 1st the isntruction sentence,
 * @return a pointer of said newly creted struct.
 */
instruction_sentence* make_command_sentence_struct(char* command_line);

extern int IC; /* The Instruction Counter. */
extern int DC; /* The Data Counter. */


extern int current_line_number; /* This will hold the current line number. */

const char* registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char* list_of_reserved_words[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "mcro", "mcroend",
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
    "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
    ".data", ".string", ".entry", ".extern"
};
