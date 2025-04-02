#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h" 


key_resources* init_data_structures();
void free_data_structures(key_resources* key_nodes);

/*from a different file*/
FILE* pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name,key_macro_nodes* macro_nodes);
void initial_scan(FILE* start_of_am_file_pointer,key_resources* key_nodes);
void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);


void free_macro_storage(key_macro_nodes* key_nodes);
