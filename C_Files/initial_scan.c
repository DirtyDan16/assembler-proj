#include "initial_scan.h"

static label_node*  head_of_label_table = NULL, * tail_of_label_table = NULL;
/* * current_label_node = NULL;*/

mila instrcution_table[GEN_STORAGE_SIZE] = {0}; /* The instruction table that will be used to store the instructions of the assembly file. */
mila data_table[GEN_STORAGE_SIZE] = {0}; /* The data table that will be used to store the data of the assembly file. */
mila IC = 100; /* The Instruction Counter. */
mila DC = 0; /* The Data Counter. */


void initial_scan(FILE* start_of_am_file_pointer) {
	FILE* input_file_pointer = start_of_am_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]= {0}; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\tGoing through first scan of the file!.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line);
	}
}

static void go_over_read_line(char* chosen_line) {
	bool has_label = false;
	
	char* label_name = NULL; /* If we have found a label, we'll store it in here*/

	/* If the line is empty or a comment, skip it*/
	if (is_empty(chosen_line) || is_comment(chosen_line)) return;

	/* Check if the line has a label attached to it. if it has, already get the name from it in the if condition*/
	if ((label_name = is_valid_label(chosen_line)) != NULL) {
		has_label = true;
		chosen_line = strchr(chosen_line, ' '); /* Skip to after the label*/

		if (chosen_line == NULL) {
			fprintf(stderr, "There's a Label without a command or directive.\n");
			return;
		}
	}

	/* Determine which type this sentence is.*/
	if (is_directive(chosen_line)) {
		if (has_label) add_label_to_table(label_name, 'd'); /* Add the label name to the table with the 'd' type that stands for data*/
		handle_directive(chosen_line);
	} else if (is_valid_command(chosen_line)) {
		if (has_label) add_label_to_table(label_name, 'c'); /* Add the label name to the table with the 'c' type that stands for command*/
		handle_command(chosen_line);		/* Handle the command */
	} else {

	}

}


bool is_valid_command(char* chosen_line) {
	char* inspected_word = strtok_copy(chosen_line," ");
	int i;

	if (inspected_word == NULL) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return false; 
	}

	for (i = 0; i < NUM_OF_ASM_COMMANDS; i++) {
		/* Check if the read command name is an actual command name that's part of our imaginary assembly lang.
		 */
		if (strcmp(asmCommands[i].name,chosen_line) == 0) {
			return true;
		}
	}
	/* returning 0 if the command is invalid */
	fprintf(stderr, "The program got an invalid assembly instruction.");
	return false; 
}

/* TODO: have a checker for if num of arguments is valid for a given command.*/
void handle_command(char* command) {
	mila binary_value_of_command = 0,binary_value_for_first_argument=0,binary_value_for_second_argument=0;
	mila L = 1; /* The amount of Milas this command will take in the instruction table. Can go up to 3 */
	
	/* Get the command name and the arguments */
	char* command_name = strtok(command," ");
	char* first_argument = NULL;
	char* second_argument = NULL;

	/* Determine amount of arguments in this command. Based on the amount, give the different argument variables those values*/
	/* If there is 2 arguments*/
	if (strchr(command,',')) { 
		first_argument = strtok(NULL,",");
		second_argument = strtok(NULL,"\n");
	/* If there is 1 or less arguments. If there is no value to give (0 arguments), it'll just give back null to the var (or a value that won't be registered as a type of
	an argument, ie. white space)*/
	} else {
		first_argument = strtok(NULL,"\n");
	}

	binary_value_of_command += get_binary_value_of_command_name(command_name);

	if (first_argument != NULL) {
		deal_with_first_parameter(first_argument,command_name,&binary_value_of_command,&L);

		if (second_argument != NULL) {
			deal_with_second_parameter(second_argument,command_name,&binary_value_of_command,&L);

		}
	} 


	IC += L;
}

void deal_with_second_parameter(char* second_argument,char* command_name,mila* cur_binary_value_of_command,mila* L) {
}	

void deal_with_first_parameter(char* first_argument,char* command_name,mila* cur_binary_value_of_command,mila* L) {
	int type_of_first_argument = determine_type_of_asm_argument(first_argument); /* Determine the type of the first argument*/

	/* If the first argument is not valid, output an error and return*/
	if (type_of_first_argument == NOT_VALID) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return;
	}
	if (!is_argument_valid_for_this_specific_command(type_of_first_argument,command_name,1)) {
		fprintf(stderr, "The program got an invalid assembly instruction. The first argument is not valid for this command.");
		return;
	}

	
	/* Logic that deals with the type of argument here ~~*/
		
}

int get_index_of_command(char* command_name) {
	int i;

	/* Check if the read command name is an actual command name that's part of our imaginary assembly lang.
	*/
	for (i = 0; i < NUM_OF_ASM_COMMANDS; i++) {
		if (strcmp(asmCommands[i].name,command_name) == 0) {
			return i;
		}
	}
	return -1;
}

bool is_argument_valid_for_this_specific_command(int argument_type, char* command_name,int argument_number) {
	int index_of_command = get_index_of_command(command_name); /* Get the index of the command in the array of commands*/

	/* Check if the argument type is valid for this specific command.
	 * If the argument number is 1, check if the argument type is valid for the target addressing modes of the command.
	 * If the argument number is 2, check if the argument type is valid for the source addressing modes of the command.
	 * 
	 * Note that we cast the argument type to a char since the addressing modes are stored as chars in the struct.
	*/
	if (argument_number == 1) {
		if (strchr(addressing_modes_for_commands[index_of_command].target_addressing_modes,(char)argument_type) != NULL) {
			return true;
		}
	} else if (argument_number == 2) {
		if (strchr(addressing_modes_for_commands[index_of_command].source_addressing_modes,(char)argument_type) != NULL) {
			return true;
		}
	}

	return false;
}

int determine_type_of_asm_argument(char* argument) {
	int i;

	/* Check for a singular '#' at the start to determine the type is immediate*/
	if (argument[0] == '#' && (strchr(argument+1,'#') == NULL)) return IMMEDIATE;

	/* Check for a singular '&' at the start to determine the type is relative*/
	if (argument[0] == '&' && (strchr(argument+1,'&') == NULL)) return RELATIVE;

	/* Check if the argument is a register*/
	for (i = 0; i < sizeof(registers) ; i++) {
		if (strcmp(argument,registers[i]) == 0) return REGISTER;
	}

	/* If the argument is none of the above, it's a direct argument... 
	However check if there is actually a word in here*/
	if (!is_empty(argument)) return DIRECT;

	return NOT_VALID;
}

int get_binary_value_of_command_name(char* read_command) {
	int machine_code_rep_of_command = 0;

	int index_of_command = get_index_of_command(read_command);
	if (index_of_command == -1) {
		fprintf(stderr, "The program got an invalid assembly instruction. representing 0.");
		return 0; 
	}

	/* Get the opcode and funct values of the command and add them to the machine code representation of the command.
	Do it via bit shifting to the desired index*/
	machine_code_rep_of_command += asmCommands[index_of_command].opcode << INDEX_OF_OPCODE_BYTE;
	machine_code_rep_of_command += asmCommands[index_of_command].opcode << INDEX_OF_FUNCT_BYTE;	
	
	return machine_code_rep_of_command;
}


void handle_directive(char* line) {
	char* directive_name = strtok(line, " "); /* Get the directive name */
	char* directive_value = strtok(NULL, "\n"); /* Get the directive value. Recall that we need to look for a new line char SPECIFICALLY since values can be seperated by spaces*/

	if (directive_name == NULL || directive_value == NULL) {
		fprintf(stderr, "A directive is missing a name or a value.\n");
		return;
	}

	/* Check which directive it is and handle it accordingly */
	if (strcmp(directive_name, ".data") == 0) {
		handle_data_directive(directive_value);		
	} else if (strcmp(directive_name, ".string") == 0) {
		handle_string_directive(directive_value);		
	} else if (strcmp(directive_name, ".entry") == 0) {
		handle_entry_directive(directive_value);
	} else if (strcmp(directive_name, ".extern") == 0) {
		handle_extern_directive(directive_value);
	} else {
		fprintf(stderr, "Unknown directive.\n");
	}
	
}

void handle_extern_directive(char* extern_name) {
}

void handle_entry_directive(char* entry) {
	char* entry_name = strtok_copy(entry, " ");

	if (entry_name == NULL) {
		fprintf(stderr, "Entry directive is missing a value.\n");
		return;
	}

	/* Add the entry name to the table of entries */
	/*add_entry_to_table(entry_name);*/
}


void handle_string_directive(char* string) {
	int char_in_str_value_as_int;
	 
	if (string == NULL) {
		fprintf(stderr, "String directive is missing a value.\n");
		return;
	}

	/* As long as there are more characters to read from, read the next character*/
	while (string != NULL) {
		char_in_str_value_as_int = (int)(*string); /* Convert the character to an integer */
		data_table[DC] = char_in_str_value_as_int; /* Add the character to the data table */
		DC++; /* Increment the Data Counter */
		string++; /* Get the next character */
	}

}

void handle_data_directive(char* data) {
	char* data_value = strtok(data, ",");
	int data_value_as_int;
	 
	if (data_value == NULL) {
		fprintf(stderr, "Data directive is missing a value.\n");
		return;
	}

	/* As long as there are more data values to read from, read the next data value*/
	while (data_value != NULL) {
		data_value_as_int = atoi(data_value); /* Convert the data value which is an ascii char to an integer */
		data_table[DC] = data_value_as_int; /* Add the data value to the data table */
		DC++; /* Increment the Data Counter */
		data_value = strtok(NULL, ","); /* Get the next data value */
	}
}

/**
 * This function checks if the line has a label attached to it.
 * If it does, it returns the label name. If it doesn't, it returns NULL.
 */
char* is_valid_label(char* line) {
	char* label_name = NULL;

	/* Check if the line has a label identifier (:). If it doesn't, return NULL */
	if (strchr(line, ':') == NULL) {
		return NULL;
	}

	/* 
		Get the label name 
		Note that we use strtok_copy() to get the label name because strtok() changes the string it's working on.
	*/
	label_name = strtok_copy(line, ":");

	if (label_name == NULL) {
		return NULL;
	}

	return label_name;
}

bool is_directive(char* line) {
	char* directive_name = strtok_copy(line, " ");

	if (directive_name == NULL) {
		return false;
	}

	/* Check if the directive name is one of the known directives*/
	if (strcmp(directive_name, ".data") == 0 || strcmp(directive_name, ".string") == 0 || strcmp(directive_name, ".entry") == 0 || strcmp(directive_name, ".extern") == 0) {
		return true;
	}

	return false;
}

void add_label_to_table(char* label_name, char type) {
	label new_label;
	label_node* new_label_node;

	/* Allocate memory for the new label name */
	new_label.label_name = (char*)malloc(strlen(label_name) + 1);

	if (new_label.label_name == NULL) {
		fprintf(stderr, "Memory allocation failed. A Label has not managed to be added to table of Labels.\n");
		exit(1);
	}

	/* Give the label its name, address, and type */
	new_label.label_name = label_name;
	new_label.label_address = IC;
	new_label.label_type = type;

	/* Allocate memory for the new label node */
	new_label_node = (label_node*)malloc(sizeof(label_node));
	if (new_label_node == NULL) {
		fprintf(stderr, "Memory allocation failed. A Label has not managed to be added to table of Labels.\n");
		exit(1);
	}

	/* Give the label node its value and point to the next label node */
	new_label_node->val = new_label;
	new_label_node->next = NULL;

	/* If the table is empty, make the new label node the head and tail of the table */
	if (head_of_label_table == NULL) {
		head_of_label_table = new_label_node;
		tail_of_label_table = new_label_node;
	} else {
		/* If the table is not empty, add the new label node to the end of the table */
		tail_of_label_table->next = new_label_node;
		tail_of_label_table = new_label_node;
	}

}

/* checks if this Sentence is empty or not and return a boolean val accordingly*/
bool is_empty(char* line) {
	while (*line != '\0') {
		if (!isspace(*line)) {
			return false;
		}
		line++; /* Move to the next character in the line */
	}
	return true;
}

/* checks if this Sentence is a comment or not and return a boolean val accordingly*/
bool is_comment(char* line) {
	return line[0] == ';';
}