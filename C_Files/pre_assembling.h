#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"

/**
 * The first stage of compiling an ASM file.
 * This stage takes a raw ASM file, detects any defined macros in it and converts them to their definition.
 * The function creates a new After Macro file that stores those changes.
 * 
 * @param 1st a pointer to the start of the ASM file. used to read lines from the ASM file in order to build the AM file.
 * @param 2nd a name of the base ASM file, without its suffix. used to build the name of the AM file.
 * @param 3rd an easy access to important macro nodes that are gonna be filled up throughout.
 * 
 * @return the AM file, if succeeds. otherwise, NULL. 
 */
FILE* pre_assembling(FILE* start_of_assembly_file_pointer, char* input_file_name, key_macro_nodes* key_nodes);

/**
 * This function is being excecuted if the code detects that it is currently outside of macro declaration lines.
 * Basically a state that the code is at, and does specific stuff while at that state.
 * 
 * Checks for macro declartions, and macro names. also outputs to the AM file regular text that is being read.
 * 
 * @param 1st the current line that is read from the ASM file.
 * @param 2nd the AM file that is being written to.
 * @param 3rd the key macro pointers that are needed to be accessed on a global scope.
 */
char* outside_macro_declaration(char* line,FILE* am_file,key_macro_nodes* key_nodes);

/**
 * This function is being excecuted if the code detects that it is currently inside of macro declaration lines.
 * Basically a state that the code is at, and does specific stuff while at that state.
 * 
 * Appends to a macro node its contents, and checks for when the macro declaration ends. does NOT output to the AM file any text that is being read.
 * 
 * @param 1st the current line that is read from the ASM file.
 * @param 2nd the key macro pointers that are needed to be accessed on a global scope.
 */
char* inside_macro_declaration_state(char* line,key_macro_nodes* key_nodes);

/**
 * A helper function that checks if a read line is a taken word of a defined macro. 
 * @param 1st the read line
 * @param 2nd the macro pointers that point to the list of already-defined macros.
 * 
 * @return a boolean value if this line is a macro name or not.
 */
bool check_if_line_is_a_defined_macro(char* chosen_line, key_macro_nodes* key_nodes);

/**
 * A helper function that checks if the given name for a macro is valid or not.
 * Checks if the name is constructed correctly, and if it is a name of a reserved word or not (such as an instruction name)
 * 
 * @param 1st the name
 * @return a boolean value that represents if the name is okay or not. 
 */
bool is_valid_macro_name(char* macro_name);

/**
 * Adds to the list of macros a new macro node. that macro node's val is a macro struct which hold the name of the macro, and its contents.
 * 
 * @param 1st the macro name
 * @param 2nd the pointers to the list of macro nodes.
 * 
 * @return a boolean value whether the process of adding the value was secceeded or not.
 */
bool add_a_macro_node_to_list(char* found_macro_name,key_macro_nodes* key_nodes);

/**
 * This func Frees all the storage that was allocated for the macro nodes. Used typically when we done with the macro stage, however will also be
 * used in situations where memory allocation failed.
 */
void free_macro_storage(key_macro_nodes* key_nodes);


/*from a different file*/
FILE* create_after_macro_file(char* assembly_file_name);
void remove_am_file(char* am_file_name);
bool is_empty(char* line); 

