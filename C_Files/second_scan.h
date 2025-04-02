#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);
static void go_over_read_line(char* chosen_line);


/*created from a different file*/
bool is_empty(char* line);
bool is_comment(char* line);
