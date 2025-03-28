#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h" 


key_data_structures* init_data_structures();


/*from a different file*/
void compile(FILE* assembly_file_pointer,char* input_file_name);
FILE* pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name,key_macro_nodes* macro_nodes);
void initial_scan(FILE* start_of_am_file_pointer,key_data_structures* key_nodes);


void free_data_structures(key_data_structures* key_nodes);

void free_macro_storage(key_macro_nodes* key_nodes);
