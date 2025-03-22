#include "general.h"
#include "compiling_logic.h"


void copy_content_from_a_file_to_another(FILE* copied_file, FILE* file_to_copy_to);
void create_output_files(char* input_file_name);
FILE* create_after_macro_file(char* assembly_file_name);

extern bool is_asm_file_with_errors;
