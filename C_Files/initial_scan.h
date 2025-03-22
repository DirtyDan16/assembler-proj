#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void initial_scan(FILE* start_of_am_file_pointer);
void go_over_read_line(char* chosen_line);
void handle_directive(char* line);
void handle_command(char* line);
void handle_data_directive(char* data);
void handle_string_directive(char* string);
void handle_entry_directive(char* entry);
void handle_extern_directive(char* extern_directive);
