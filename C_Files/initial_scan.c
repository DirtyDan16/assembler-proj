#include "initial_scan.h"

int IC = 100; /* The Instruction Counter. */
int DC = 0; /* The Data Counter. */

/* TODO: ignore indention in indented lines*/
void initial_scan(FILE* start_of_am_file_pointer,key_data_structures* key_nodes) {
	FILE* input_file_pointer = start_of_am_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]= {0}; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\tGoing through first scan of the file!.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line,key_nodes);
	}
}

static void go_over_read_line(char* chosen_line,key_data_structures* key_nodes) {
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
		if (has_label) add_label_to_table(label_name, 'd',key_nodes->label_nodes); /* Add the label name to the table with the 'd' type that stands for data*/
		handle_directive(chosen_line,key_nodes);		/* Handle the directive */
	} else if (is_valid_command(chosen_line)) {
		if (has_label) add_label_to_table(label_name, 'c',key_nodes->label_nodes); /* Add the label name to the table with the 'c' type that stands for command*/
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

void handle_command(char* command) {
	int L = 1; /* The amount of Milas this command will take in the instruction table. Can go up to 3 */

	/* This struct will be handy and convieneient since it gathers all usefull info of this sentence in a singular place! */
	command_sentence* cur_command_sentence = make_command_sentence_struct(command);

	mila binary_value_of_command,binary_value_for_first_argument,binary_value_for_second_argument;
	binary_value_of_command.v = 0; binary_value_for_first_argument.v = 0; binary_value_for_second_argument.v = 0; /* Initialize the binary values to 0 */


	

	if (!check_if_num_of_arguments_are_valid(cur_command_sentence)) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return;
	}


	binary_value_of_command.v += get_binary_value_of_command_name(cur_command_sentence->index_of_command); /* Get the binary value of the command name*/
	/* Set the A value of the command to 1 (Absolute). No need to check for anything, the first Mila will always have the R bit on! */
	binary_value_of_command.v += 1 << INDEX_OF_THE_A_BYTE;


	if (cur_command_sentence->first_argument != NULL) {
		deal_with_first_parameter(cur_command_sentence,&binary_value_of_command,&binary_value_for_first_argument,&L);

		if (cur_command_sentence->second_argument != NULL) {
			deal_with_second_parameter(cur_command_sentence,&binary_value_of_command,&binary_value_for_second_argument,&L);

		}
	} 

	IC += L;
}

command_sentence* make_command_sentence_struct(char* command_line) {
		/* Gather all feilds required to make a strcut for a Command Sentence. */
		char* command_name = strtok(command_line," ");
		char* first_argument = NULL;
		char* second_argument = NULL;
	
		int num_of_arguments = 0;
		int index_of_command = get_index_of_command(command_name); /* Get the index of the command in the array of commands*/
	
		command_sentence* cur_command_sentence = (command_sentence*)malloc(sizeof(command_sentence));
		if (cur_command_sentence == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			exit(1);
		}

		/* Determine amount of arguments in this command. Based on the amount, give the different argument variables those values*/
		/* If there is 2 arguments*/
		if (strchr(command_line,',')) { 
			first_argument = strtok(NULL,",");
			second_argument = strtok(NULL,"\n");
	
			num_of_arguments = 2;
		/* If there is 1 or less arguments. If there is no value to give (0 arguments), it'll just give back null to the var (or a value that won't be registered as a type of 
		an argument, ie. white space)*/
		} else {
			first_argument = strtok(NULL,"\n");
			num_of_arguments = 1;
		}
	
		/*---------------------------------------------------------------------*/
	
		/* Fill the struct with the gathered info */
		cur_command_sentence->command_name = command_name;
		cur_command_sentence->num_of_arguments = num_of_arguments;
		cur_command_sentence->index_of_command = index_of_command;
		cur_command_sentence->first_argument = first_argument;
		cur_command_sentence->second_argument = second_argument;

		return cur_command_sentence;
}

bool check_if_num_of_arguments_are_valid(command_sentence* cur_command_sentence) {
	int index_of_command = cur_command_sentence->index_of_command;
	char* first_argument = cur_command_sentence->first_argument;
	char* second_argument = cur_command_sentence->second_argument;
	/* If the command is invalid, return false*/
	if (index_of_command == -1) {
		return false;
	}

	/* If the command has 2 arguments, check if there are actually 2 arguments*/
	if (num_of_arguments_each_command_has[index_of_command] == 2) {
		if (first_argument == NULL || second_argument == NULL) {
			return false;
		}
	} else if (num_of_arguments_each_command_has[index_of_command] == 1) {
		/* If the command has 1 argument, check if there is 1 argument*/
		if (first_argument == NULL) {
			return false;
		}
	} else {
		/* If the command has 0 arguments, check if there are 0 arguments*/
		if (first_argument != NULL) {
			return false;
		}
	}

	return true;

}

void deal_with_second_parameter(command_sentence* cur_command_sentence,mila* cur_binary_value_of_command,mila* binary_value_for_second_argument,int* L) {
	int type_of_second_argument = determine_type_of_asm_argument(cur_command_sentence->second_argument); /* Determine the type of the second argument*/
	/* If the second argument is not valid, output an error and return*/
	if (type_of_second_argument == NOT_VALID) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return;
	}
	if (!is_argument_valid_for_this_specific_command(cur_command_sentence,type_of_second_argument,2)) {
		fprintf(stderr, "The program got an invalid assembly instruction. The second argument is not valid for this command.");
		return;
	}

	
	/* Logic that deals with the type of argument here ~~*/

	/* If the second argument is a register, get the register number and add it to the binary value of the command*/
	if (type_of_second_argument == REGISTER) {
		 deal_with_register_type_value(cur_binary_value_of_command,cur_command_sentence,2);
	/* All other types of arguments require to add an extra Mila, so we'll increment the L value.*/
	} else {
		*L += 1;
		/* At this point of time, in the first scan, we only know how to deal with direct values. So lets check this type only*/
		if (type_of_second_argument == DIRECT) {
			deal_with_direct_type_value(binary_value_for_second_argument,cur_command_sentence->second_argument);
		}

	}
}	

void deal_with_first_parameter(command_sentence* cur_command_sentence,mila* cur_binary_value_of_command,mila* binary_value_for_first_argument,int* L) {
	int type_of_first_argument = determine_type_of_asm_argument(cur_command_sentence->first_argument); /* Determine the type of the first argument*/
	/* If the first argument is not valid, output an error and return*/
	if (type_of_first_argument == NOT_VALID) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return;
	}
	if (!is_argument_valid_for_this_specific_command(cur_command_sentence,type_of_first_argument,1)) {
		fprintf(stderr, "The program got an invalid assembly instruction. The first argument is not valid for this command.");
		return;
	}

	
	/* Logic that deals with the type of argument here ~~*/

	/* If the first argument is a register, get the register number and add it to the binary value of the command*/
	if (type_of_first_argument == REGISTER) {
		 deal_with_register_type_value(cur_binary_value_of_command,cur_command_sentence,1);
	/* All other types of arguments require to add an extra Mila, so we'll increment the L value.*/
	} else {
		*L += 1;
		/* At this point of time, in the first scan, we only know how to deal with direct values. So lets check this type only*/
		if (type_of_first_argument == DIRECT) {
			deal_with_direct_type_value(binary_value_for_first_argument,cur_command_sentence->first_argument);
		}

	}

		
}

void deal_with_direct_type_value(mila* additional_mila,char* argument) {
	char* argument_without_hash = strtok(argument,"#"); /* Get the argument without the '#' char*/
	int value_of_direct_argument = atoi(argument_without_hash); /* Convert the direct argument to a decimal int */

	/* Take the decimal value of the direct argument and bit shift it to the left so it doesn't collide with the A.R.E bits!*/
	mila binary_value_of_direct_argument;
	binary_value_of_direct_argument.v = value_of_direct_argument << INDEX_OF_THE_BIT_AFTER_A; 
	binary_value_of_direct_argument.v += 1 << INDEX_OF_THE_A_BYTE; /* Set the A value of the direct argument to 1 (Absolute) */

	/* Add the binary value of the direct argument to the binary value of the additional mila */
	(*additional_mila).v = binary_value_of_direct_argument.v;

	/* Since we are dealing with a direct value, we need to set the addressing mode to direct to the Mila,
	HOWEVER the value of the addressing mode is convinently 0, which is the default value of the mila.
	So we don't need to do anything here. */
}

void deal_with_register_type_value(mila* cur_binary_value_of_command,command_sentence* cur_command_sentence,int argument_number) {
	int i;

	/* The number of the register in the registers array*/
	int register_number=0;

	/* Get the register name from the argument number*/
	char* register_name = argument_number == 1 ? cur_command_sentence->first_argument : cur_command_sentence->second_argument;

	/* Check which register the argument is and return the register number*/
	for (i = 0; i < sizeof(registers) / sizeof(registers[0]); i++) {
		if (strcmp(register_name,registers[i]) == 0) {
			register_number = i;
			break;
		}
	}

	/* Determine the type of register (source or target). based on that, use bit shifting with the according index and
	add that value unto the binary value of the first mila.*/
	if (cur_command_sentence->num_of_arguments == 2) {
		if (argument_number == 1) { /* Source*/
			(*cur_binary_value_of_command).v += register_number << INDEX_OF_SOURCE_REGISTER_BYTE;
			(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_SOURCE_ADDRESING_MODE_BYTE;
		} else if (argument_number == 2) /* Target*/ {
			(*cur_binary_value_of_command).v += register_number << INDEX_OF_TARGET_REGISTER_BYTE;
			(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_TARGET_ADDRESING_MODE_BYTE;
		}
	} else if (cur_command_sentence->num_of_arguments == 1) { /* If there is only 1 argument, it has to be the target*/
		(*cur_binary_value_of_command).v += register_number << INDEX_OF_TARGET_REGISTER_BYTE;
		(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_TARGET_ADDRESING_MODE_BYTE;
	} else {
		return; /* If there are no arguments, do nothing */
	}
	
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

bool is_argument_valid_for_this_specific_command(command_sentence* cur_command_sentence,int argument_type,int argument_number) {
	int num_of_arguments = cur_command_sentence->num_of_arguments; /* Get the number of arguments the command has*/

	/* Check if the argument type is valid for this specific command.
	 * Note that we cast the argument type to a char since the addressing modes are stored as chars in the struct.
	*/

	/* If the command has 2 arguments, check if the argument type is valid. First arg has to be for source, and Second for target.*/
	if (num_of_arguments == 2) {
		if (argument_number == 1) {
			if (strchr(addressing_modes_for_commands[cur_command_sentence->index_of_command].source_addressing_modes,(char)argument_type) != NULL) {
				return true;
			}
		} else if (argument_number == 2) {
			if (strchr(addressing_modes_for_commands[cur_command_sentence->index_of_command].target_addressing_modes,(char)argument_type) != NULL) {
				return true;
			}
		}
	/* If the command has 1 argument, check if the argument type is valid. The argument has to be for target.*/
	} else if (num_of_arguments == 1) {
		if (strchr(addressing_modes_for_commands[cur_command_sentence->index_of_command].target_addressing_modes,(char)argument_type) != NULL) {
			return true;
		}
	/* If the command has 0 arguments, no need to check since there is no arg.*/
	} else return true;
		


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

int get_binary_value_of_command_name(int index_of_command) {
	int machine_code_rep_of_command = 0;

	if (index_of_command == -1) {
		fprintf(stderr, "The program got an invalid assembly instruction.");
		return -1;
	}

	/* Get the opcode and funct values of the command and add them to the machine code representation of the command.
	Do it via bit shifting to the desired index*/
	machine_code_rep_of_command += asmCommands[index_of_command].opcode << INDEX_OF_OPCODE_BYTE;
	machine_code_rep_of_command += asmCommands[index_of_command].funct << INDEX_OF_FUNCT_BYTE;	
	
	return machine_code_rep_of_command;
}


void handle_directive(char* line,key_data_structures* key_nodes) {
	char* directive_name = strtok(line, " "); /* Get the directive name */
	char* directive_value = strtok(NULL, "\n"); /* Get the directive value. Recall that we need to look for a new line char SPECIFICALLY since values can be seperated by spaces*/

	if (directive_name == NULL || directive_value == NULL) {
		fprintf(stderr, "A directive is missing a name or a value.\n");
		return;
	}

	/* Check which directive it is and handle it accordingly */
	if (strcmp(directive_name, ".data") == 0) {
		handle_data_directive(directive_value,key_nodes->data_table);		
	} else if (strcmp(directive_name, ".string") == 0) {
		handle_string_directive(directive_value,key_nodes->data_table);		
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


void handle_string_directive(char* string,mila* data_table) {
	 
	if (string == NULL) {
		fprintf(stderr, "String directive is missing a value.\n");
		return;
	}

	/* As long as there are more characters to read from, read the next character*/
	while (string != NULL) { 
		data_table[DC].v = int_to_binary( (int)(*string) );/* Convert the character to an integer and Add the binary representation of the character to the data table */
		DC++; /* Increment the Data Counter */
		string++; /* Get the next character */
	}

}


void handle_data_directive(char* data,mila* data_table) {
	char* data_value = strtok(data, ",");
	 
	if (data_value == NULL) {
		fprintf(stderr, "Data directive is missing a value.\n");
		return;
	}

	/* As long as there are more data values to read from, read the next data value*/
	while (data_value != NULL) {
		data_table[DC].v = int_to_binary( atoi(data_value) );/* Convert the data value which is an ascii char to an integer and Add the binary representation of the data value to the data table */
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

void add_label_to_table(char* label_name, char type,key_label_nodes* key_nodes) {
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
	if (key_nodes->head_of_label_storage == NULL) {
		key_nodes->head_of_label_storage = new_label_node;
		key_nodes->head_of_label_storage = new_label_node;
	} else {
		/* If the table is not empty, add the new label node to the end of the table */
		key_nodes->last_label_node->next = new_label_node;
		key_nodes->last_label_node = new_label_node;
	}

}

int int_to_binary(int value) {
    /* Take the unsigned value of value and mask to 24 bits.
    This is done to ensure that the value is not negative and does not exceed 24 bits */
    unsigned int unsigned_value = (unsigned int)value & 0xFFFFFF;

    int binary_value = 0;
    int base = 1;
    int remainder;

    if (value == 0) {
        return 0;
    }

    /* Convert the integer to binary */
    while (unsigned_value > 0) {
        remainder = unsigned_value % 2;
        binary_value += remainder * base;
        unsigned_value /= 2;
        base *= 10;
    }

    /* Shift the binary value to the left by the specified number of bits.
    * This is done to ensure that the binary value does not collide with the A.R.E bits.
    * The A.R.E bits are 3 bits, so we need to shift the binary value by 3 bits.
    * The number of bits to shift is determined by the number of bits in the binary value.
    */
    binary_value <<= INDEX_OF_THE_BIT_AFTER_A; /* Shift the binary value to the left by 3 bits */

    return binary_value;
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