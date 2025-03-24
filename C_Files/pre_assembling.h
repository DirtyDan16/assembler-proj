#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"

FILE* pre_assembling(FILE* start_of_assembly_file_pointer, char* input_file_name, key_macro_nodes* key_nodes);

char* outside_macro_declaration(char* line,FILE* am_file,char* state,key_macro_nodes* key_nodes);
char* inside_macro_declaration_state(char* line,FILE* am_file,char* state,key_macro_nodes* key_nodes);

bool check_line_for_macro(char* chosen_line, key_macro_nodes* key_nodes);

void add_a_macro_node_to_list(char* found_macro_name, key_macro_nodes* key_nodes);

void free_macro_storage(key_macro_nodes* key_nodes);

FILE* create_after_macro_file(char* assembly_file_name); /*from a different file*/
bool is_empty(char* line); /*from a different file*/

