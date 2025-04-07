#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void copy_content_from_a_file_to_another(FILE* copied_file, FILE* file_to_copy_to);


void create_output_files(char* input_file_name,key_resources* key_resources);

void write_into_object_file(FILE* object_file,key_resources* key_resources);
void print_masked_hex(FILE* file,int address,int value);
void write_into_entries_file(FILE* entries_file,key_resources* key_resources);
void write_into_externals_file(FILE* externals_file,key_resources* key_resources);

FILE* create_after_macro_file(char* assembly_file_name);
void remove_am_file(char* assembly_file_name);