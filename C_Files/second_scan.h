#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);
static void go_over_read_line(char* chosen_line,key_resources* k_resources);

void handle_entry_directive(char* entry_sentence,key_resources* k_resources);
void look_for_label_in_table_for_entries(char* label_name,key_resources* k_resources);
char* skip_label_name(char* line);

static void handle_command(char* command,key_resources* key_resources);
static int* get_instruction_table_index();
void reset_instruction_table_index();
static void deal_with_first_parameter(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index_in_instruction_table);
static void deal_with_second_parameter(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index_in_instruction_table);

void check_if_the_argument_is_external(char* argument,key_resources* k_resources,int IC);
void add_extern_label_to_list(label* wanted_label,key_extern_label_nodes* k_resources,int IC); 

void deal_with_direct_type_value(instruction_sentence* cur_command_sentence,key_resources* k_resources,int argument_number,int index_in_instruction_table);
label* find_label_node(char* label_name,key_label_nodes* k_resources) ;
void add_machine_code_of_label(mila* the_mila,label* label);

void deal_with_relative_type_value(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index,int argument_number);
void add_machine_code_of_label_relative_to(mila* ptr_to_mila,label* label,int address_of_instruction_we_came_from);

/*created from a different file (particularly from the first scan file, since some stuff between the two tasks are similar)*/
bool is_empty(char* line);
bool is_comment(char* line);
bool has_semicolon(char* line);
bool is_valid_command(char* chosen_line);
bool is_directive(char* line);


int determine_type_of_asm_argument(char *argument);


instruction_sentence* make_command_sentence_struct(char* command_line);



extern int current_line_number; /* This will hold the current line number. */
