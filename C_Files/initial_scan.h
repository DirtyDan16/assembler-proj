#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void initial_scan(FILE* start_of_am_file_pointer);
static void go_over_read_line(char* chosen_line);

void handle_directive(char* line);
void handle_data_directive(char* data);
void handle_string_directive(char* string);
void handle_entry_directive(char* entry);
void handle_extern_directive(char* extern_directive);

void handle_command(char* line);
int get_binary_value_of_command_name(char* command_name);
void deal_with_first_parameter(char* first_argument,char* command_name,mila* cur_binary_value_of_command,mila* L);
void deal_with_second_parameter(char* second_argument,char* command_name,mila* cur_binary_value_of_command,mila* L);


int determine_type_of_asm_argument(char *argument);

int get_index_of_command(char* read_command);

void add_label_to_table(char* label_name, char type);

bool is_argument_valid_for_this_specific_command(int argument_type, char *command_name, int argument_number);
char* is_valid_label(char* line);
bool is_valid_command(char* line);
bool is_directive(char* line);
bool is_empty(char* line);
bool is_comment(char* line);


const char* list_of_asm_command_names[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
const char* registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};