#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"

/**
 * The second scan of the ASM file goes over the already pre-processed and analyzed assembly file,
 * and finalizes code generation by resolving label addresses and dealing with '.entry' directives.
 * 
 * The function assumes that the first scan (initial_scan) has already been done,
 * so it uses the data from key_resources (labels, macros, etc.) to fill in missing
 * machine code parts (e.g., for labels that couldn’t be resolved before).
 * 
 * Outputs errors related to undefined labels or bad references, along with the line numbers.
 *
 * @param start_of_assembly_file_pointer Pointer to the beginning of the original AM file.
 * @param key_resources Reference to the global struct that holds labels, external label, machine code and other shared compilation state.
 */
void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);

/**
 * This function is responsible for analyzing a given line during the second scan phase.
 * 
 * It detects the type of line (directive, instruction) and performs the relevant
 * handling logic using helper functions.
 * 
 * Focus is given to validating and linking label references that were left unresolved in the first scan.
 *
 * @param chosen_line The current line being processed from the AM file.
 * @param k_resources Reference to the global struct that stores label definitions, machine code, and state needed during compilation.
 */
static void go_over_read_line(char* chosen_line,key_resources* k_resources);

/**
 * Handles an '.entry' directive by marking the specified label as an entry if it has been found in the list that was made in the first scan
 * in the label table.
 *
 * @param entry_sentence A line containing the '.entry' directive.
 * @param k_resources Pointer to the structure that holds all key resources including label tables.
 */
void handle_entry_directive(char* entry_sentence, key_resources* k_resources);

/**
 * Searches the label table for a given label name and marks it as an entry.
 * This is used when processing '.entry' directives.
 *
 * @param label_name The name of the label to search for.
 * @param k_resources Pointer to the structure holding label-related resources.
 */
void look_for_label_in_table_for_entries(char* label_name, key_resources* k_resources);

/**
 * Skips the label name at the start of a line and returns a pointer (if it exists ofc. if it isn't it doesn't do a thing)
 * to the remaining part of the line.
 *
 * @param line The input line that may start with a label.
 * @return A pointer to the position in the line right after the label.
 */
char* skip_label_name(char* line);

/*-------------------------------------------INSTRUCTIONS FUNCTIONS --------------------------------------------------*/
/**
 * Gets a command sentence, connects it to the machine code that is found in the Instruction Table,
 *  and completes inside there the machine code of the instruction for it and its arguments.
 *
 * @param command The command to process.
 * @param key_resources Pointer to the key resources.
 */
static void handle_command(char* command, key_resources* key_resources);

/**
 * Returns a pointer to the current instruction table index.
 * Used for managing at what instruction we are at, and get its related information from the table
 *
 * @return A pointer to the current index in the instruction table.
 */
static int* get_instruction_table_index();

/**
 * Resets the instruction table index back to zero or its initial value.
 * Used in-between reading ASM files.
 */
void reset_instruction_table_index();

/**
 * Deals with finishing the machine code (that it started in the first scam) to a given instruction, and its additional info Milas, while taking note that this is the argument number 1. 
 * 
 * @param cur_command_sentence The parsed instruction sentence structure.
 * @param k_resources Pointer to key resources used for label lookup and validation.
 * @param index_in_instruction_table The instruction table index this argument relates to.
 */
static void deal_with_first_parameter(instruction_sentence* cur_command_sentence, key_resources* k_resources, int index_in_instruction_table);

/**
 * Deals with finishing the machine code (that it started in the first scam) to a given instruction, and its additional info Milas, while taking note that this is the argument number 1. 

 *
 * @param cur_command_sentence The parsed instruction sentence structure.
 * @param k_resources Pointer to key resources used for label lookup and validation.
 * @param index_in_instruction_table The instruction table index this argument relates to.
 */
static void deal_with_second_parameter(instruction_sentence* cur_command_sentence, key_resources* k_resources, int index_in_instruction_table);

/**
 * Checks whether a given argument refers to an external label.
 * If so, updates the external label list accordingly.
 *
 * @param argument The argument string to check.
 * @param k_resources Pointer to the resources used for managing external references.
 * @param IC The instruction counter where the external reference occurs.
 */
void check_if_the_argument_is_external(char* argument, key_resources* k_resources, int IC);

/*-----------------------------------------------------------------------------------------------------------------------------------------*/
/**
 * Adds a given external label to the list of external label references.
 * This list holds the names of external labels and their address every time they are refrenced as an argument in the file.
 * A given name can appear multiple times with different addresses, if it is used as an argument multiple times.
 *
 * @param wanted_label Pointer to the label structure.
 * @param k_resources Pointer to the external label resources container.
 * @param IC The instruction counter where the reference occurs.
 */
void add_extern_label_to_list(label* wanted_label, key_extern_label_nodes* k_resources, int IC);

/**
 * Handles an argument of direct type (e.g., a label), validates it,
 * and adds its code to the instruction table accordingly.
 *
 * @param cur_command_sentence The current instruction sentence being processed.
 * @param k_resources Pointer to label-related resources.
 * @param argument_number the argument number. used to know if the argument is a Source argument, or a Target.
 * @param index_in_instruction_table Current index in the instruction table.
 */
void deal_with_direct_type_value(instruction_sentence* cur_command_sentence, key_resources* k_resources, int argument_number, int index_in_instruction_table);

/**
 * Searches for a label in the label table by name.
 *
 * @param label_name The name of the label to search.
 * @param k_resources Pointer to the structure containing all defined labels.
 * @return Pointer to the label struct if found, NULL otherwise.
 */
label* find_label_node(char* label_name, key_label_nodes* k_resources);

/**
 * Adds machine code data associated with a given label argument to a Mila of an additional info-Mila of a direct argument.
 * @param the_mila Pointer to the mila where the label’s machine code will be added.
 * @param label Pointer to the label from which to extract info from (such as if it is external, in which we don't add the address).
 */
void add_machine_code_of_label(mila* the_mila, label* label);

/**
 * Handles a relative-type argument ,
 * computes its relative address, and stores the result in the Mila in the Instruction Table.
 *
 * @param cur_command_sentence The parsed instruction sentence.
 * @param k_resources Pointer to key resources used for label lookup.
 * @param index Instruction index for the instruction table.
 * @param argument_number The position of the argument in the instruction. used to detemine if this is a source or a target argument.
 */
void deal_with_relative_type_value(instruction_sentence* cur_command_sentence, key_resources* k_resources, int index, int argument_number);

/**
 * Adds the relative machine code of a label (relative to the instruction address)
 * to a machine code line (Mila).
 *
 * @param ptr_to_mila The mila object being updated.
 * @param label Pointer to the label whose address is being used.
 * @param address_of_instruction_we_came_from The address of the current instruction (used to compute the offset).
 */
void add_machine_code_of_label_relative_to(mila* ptr_to_mila, label* label, int address_of_instruction_we_came_from);

/*created from a different file (particularly from the first scan file, since some stuff between the two tasks are similar)*/
bool is_empty(char* line);
bool is_comment(char* line);
bool has_semicolon(char* line);
bool is_valid_command(char* chosen_line);
bool is_directive(char* line);


int determine_type_of_asm_argument(char *argument);


instruction_sentence* make_command_sentence_struct(char* command_line);



extern int current_line_number; /* This will hold the current line number. */
