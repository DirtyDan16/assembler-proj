#include "general.h"
#include "compiling_logic.h"
#include "data_types.h"


void pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name);
int check_line_for_macro(char* line);
void add_a_macro_node_to_list(char* found_macro_name);
void free_macro_storage();


FILE* create_after_macro_file(char* assembly_file_name); /*from a different file*/

enum READING_ASM_FILE_STATES {OUTSIDE,INSIDE_MACRO_DECLARATION};