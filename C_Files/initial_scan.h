#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void initial_scan(FILE* start_of_am_file_pointer);
static void go_over_read_line(char* chosen_line);

void handle_directive(char* line);
void handle_data_directive(char* data);
void handle_string_directive(char* string);
void handle_entry_directive(char* entry);
void handle_extern_directive(char* extern_name);

void handle_command(char* command);
void deal_with_first_parameter(command_sentence* cur_command_sentence, mila* cur_binary_value_of_command, mila* binary_value_for_first_argument, mila* L);
void deal_with_second_parameter(command_sentence* cur_command_sentence, mila* cur_binary_value_of_command, mila* binary_value_for_second_argument, mila* L);
void deal_with_direct_type_value(mila* cur_binary_value_of_command, mila* additional_mila, char* direct_type_argument);
void deal_with_register_type_value(mila* cur_binary_value_of_command, command_sentence* cur_command_sentence,int argument_number);

int determine_type_of_asm_argument(char *argument);
int get_binary_value_of_command_name(int index_of_command);
int get_index_of_command(char* command_name);

void add_label_to_table(char* label_name, char type);

bool check_if_num_of_arguments_are_valid(command_sentence* cur_command_sentence);
bool is_argument_valid_for_this_specific_command(command_sentence* cur_command_sentence,int argument_type, int argument_number);
char* is_valid_label(char* line);
bool is_valid_command(char* chosen_line);
bool is_directive(char* line);
bool is_empty(char* line);
bool is_comment(char* line);

command_sentence* make_command_sentence_struct(char* command_line);

const char* list_of_asm_command_names[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
const char* registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};