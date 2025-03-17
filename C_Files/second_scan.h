#include "general.h"
#include "compiling_logic.h"


void compile(FILE* start_of_assembly_file_pointer,char* input_file_name);
void go_over_read_line(char* chosen_line);
int compile_asm_command_to_machine_code(char* read_command);


void create_output_files(char* input_file_name);/*created from a different file*/
