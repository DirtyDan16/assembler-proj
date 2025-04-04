#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);
static void go_over_read_line(char* chosen_line,key_resources* k_resources);

void handle_entry_directive(char* entry_sentence,key_resources* k_resources);
void look_for_label_in_table(char* label_name,key_resources* k_resources);
char* skip_label_name(char* line);


/*created from a different file (particularly from the first scan file, since some stuff between the two tasks are similar)*/
bool is_empty(char* line);
bool is_comment(char* line);

bool is_directive(char* line);
