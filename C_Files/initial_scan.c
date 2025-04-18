#include "initial_scan.h"

int IC = START_OF_IC; /* The Instruction Counter. */
int DC = START_OF_DC; /* The Data Counter. */

int current_line_number = 1; /* Tracks the current line number for error reporting */
/* Why this is ok as a global variable? 
This is acceptable because the 'current_line_number' is a shared resource that needs to be accessed and updated across multiple functions 
during the processing of the file. Making it global avoids the need to pass it as a parameter to every function, simplifying the code. 
Additionally, its scope is limited to this file, ensuring it doesn't interfere with other parts of the program. 

Also, its value is only changed at a single function.
*/
bool does_file_have_errors = false; /* This variable is used to check if the file has any errors. If it does, we won't create the object file. (and its relatives) */

void initial_scan(FILE* start_of_am_file_pointer,key_resources* key_resources) { 
	FILE* input_file_pointer = start_of_am_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	 /* This line stores each time a line from the asm file. (fgets() puts the info in it)

	 	Previously, this was a local (non-static) array, so its stack memory
		could be reclaimed and reused by malloc(). That led to two pointers 
		referencing the same memory.

		Making it static ensures the memory remains valid and is not overwritten.
	 */
	static char line[GEN_LENGTH_OF_STRINGS]= {0};

	printf("\n----> Going through first scan of the file!. <-----\n");

	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line,key_resources);
		current_line_number++; /* Increment the line number */
	}

	/* Save the final values of IC and DC. will be used for the object file.*/
	key_resources->ICF = IC;
	key_resources->DCF = DC;

	/*Reset the IC and DC*/
	IC = START_OF_IC;
	DC = START_OF_DC;

	update_all_symbols_addresses_of_type_data(key_resources->ICF,key_resources->label_nodes);

	fseek(input_file_pointer, 0, SEEK_SET); /* Reset the file pointer to the beginning of the file */
	current_line_number = 1; /* Reset the line number */
}

void update_all_symbols_addresses_of_type_data(int ICF,key_label_nodes* labels) {
	label_node* pos = labels->head_of_label_storage;

	while (pos != NULL) {
		/*only if the type is .data*/
		if (strcmp(pos->val.label_type,".data") == 0) {
			pos->val.label_address += ICF; /*Add the ICF to the address.*/
		}

		pos = pos->next;
	}
}

static void go_over_read_line(char* chosen_line,key_resources* key_resources) {
	bool has_label = false;
	
	char* label_name = NULL; /* If we have found a label, we'll store it in here*/
	char* line_without_indentation = look_for_first_non_whitespace_char(chosen_line); /* This will hold the line without indentation. Will be used to get the raw value of the line so we can compare correctly with certain keywords. */


	/* If the line is empty or a comment, skip it*/
	if (is_empty(chosen_line) || is_comment(line_without_indentation)) return;

	if (has_semicolon(line_without_indentation)) return; /* If the line has a comment in the middle, skip it.*/


	
	/* Check if the line has a label attached to it. if it has, already get the name from it.*/
	if ((label_name = is_valid_label(line_without_indentation,key_resources)) != NULL) {
		has_label = true;		
	}

	/* Skip to after the label*/
	if (strchr(line_without_indentation, ':') != NULL ) {
		line_without_indentation = strchr(line_without_indentation,':') + 1;
		line_without_indentation = look_for_first_non_whitespace_char(line_without_indentation); /* Get the line without the whitespace that is given between the label and the rest of sentence. */
		if (line_without_indentation == NULL) {
			fprintf(stderr, "There's a Label without a command or directive. \n LINE: %d\n", current_line_number);
			does_file_have_errors = true; 
			return;
		}
	}

	/* Determine which type this sentence is.*/
	if (is_directive(line_without_indentation)) {
		if (has_label) add_label_to_table(label_name,DC,".data",key_resources->label_nodes); /* Add the label name to the table*/
		handle_directive(line_without_indentation,key_resources);		/* Handle the directive */
	} else if (is_valid_command(line_without_indentation)) {
		if (has_label) add_label_to_table(label_name,IC,".code",key_resources->label_nodes); /* Add the label name to the table*/
		handle_command(line_without_indentation,key_resources);		/* Handle the command */
	} else {

	}
}


bool is_valid_command(char* line_without_indentation) {
	char* inspected_word = strtok_copy(line_without_indentation," ");
	int i;

	if (inspected_word == NULL) {
		fprintf(stderr, "The program got an invalid assembly instruction. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return false; 
	}

	/* If the inspected word has a newline attached to it, we would want to get rid of it since it isn't accounted for in the comparison.
		(specifically can happen for stop/rts commands)	
	*/
	replace_char_with_null(inspected_word,'\n');

	for (i = 0; i < NUM_OF_ASM_COMMANDS; i++) {
		/* Check if the read command name is an actual command name that's part of our imaginary assembly lang.
		 */
		if (strcmp(instructions_machine_code_rep[i].name,inspected_word) == 0) {
			return true;
		}
	}
	/* returning 0 if the command is invalid */
	fprintf(stderr, "The program got an invalid assembly instruction.\n LINE: %d\n", current_line_number);
	does_file_have_errors = true;
	return false; 
}

static void handle_command(char* command,key_resources* key_resources) {
	int L = 1; /* The amount of Milas this command will take in the instruction table. Can go up to 3 */

	/* This struct will be handy and convieneient since it gathers all usefull info of this sentence in a singular place! */
	instruction_sentence* cur_command_sentence = make_command_sentence_struct(command);

	mila binary_value_of_command,binary_value_for_first_argument,binary_value_for_second_argument;
	binary_value_of_command.v = 0; binary_value_for_first_argument.v = 0; binary_value_for_second_argument.v = 0; /* Initialize the binary values to 0 */


	if (!check_if_num_of_arguments_are_valid(cur_command_sentence)) {
		fprintf(stderr, "The program got an assembly instruction with wrong amount of arguments. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true; 
		return;
	}


	binary_value_of_command.v += get_binary_value_of_command_name(cur_command_sentence->index_of_command); /* Get the binary value of the command name*/
	/* Set the A value of the command to 1 (Absolute). No need to check for anything, the first Mila will always have the R bit on! */
	binary_value_of_command.v += 1 << INDEX_OF_THE_A_BIT;


	/* If there are arguments added alongside the instruction, deal with the Machine Code they give appropriately.*/
	if (cur_command_sentence->first_argument != NULL) {
		deal_with_first_parameter(cur_command_sentence,&binary_value_of_command,&binary_value_for_first_argument,&L);

		if (cur_command_sentence->second_argument != NULL) {
			deal_with_second_parameter(cur_command_sentence,&binary_value_of_command,&binary_value_for_second_argument,&L);
		}
	} 

	add_instruction_to_table(&key_resources->instruction_table[key_resources->index_of_instruction_table],binary_value_of_command,binary_value_for_first_argument,binary_value_for_second_argument,IC,L); /* Add the instruction to the instruction table */
	key_resources->index_of_instruction_table++; /* Increment the index of the instruction table */
	
	/* Update the IC by the Length of the instruction Machine Code.*/
	IC += L;
}

void add_instruction_to_table(instruction* instruction,mila binary_value_of_command,mila binary_value_for_first_argument,mila binary_value_for_second_argument,int IC,int L) {
	/* Add the command to the instruction table */
	instruction->code_of_command = binary_value_of_command;
	instruction->code_of_first_argument = binary_value_for_first_argument;
	instruction->code_of_second_argument = binary_value_for_second_argument;
	instruction->IC = IC;
	instruction->L = L;
}

instruction_sentence* make_command_sentence_struct(char* command_line) {
		instruction_sentence* cur_command_sentence = NULL; /* This will be the struct that holds all the info of the command sentence. */

		/* Gather all feilds required to make a strcut for a Command Sentence. */
		char* command_name = NULL, * first_argument = NULL, * second_argument = NULL;
		int num_of_arguments = 0, index_of_command = 0;

		char* command_line_copy = strdup(command_line); /* Make a copy of the command line so we can modify it without affecting the original string */
		if (command_line_copy == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			exit(1);
		}
		command_name = strtok(command_line_copy," ");

		/* If the command name has a newline attached to it, we would want to get rid of it since it isn't accounted for in the comparison.
		(specifically can happen for stop/rts commands)	
		*/
		replace_char_with_null(command_name,'\n');
	
		index_of_command = get_index_of_command(command_name); /* Get the index of the command in the array of commands*/
	
		cur_command_sentence = (instruction_sentence*)malloc(sizeof(instruction_sentence));
		if (cur_command_sentence == NULL) {
			fprintf(stderr, "Memory allocation failed.\n");
			exit(1);
		}

		/* Determine amount of arguments in this command. Based on the amount, give the different argument variables those values*/
		/* If there is 2 arguments*/
		if (strchr(command_line,',')) { 
			first_argument = strtok(NULL,",");
			/* Get rid of whitespaces surrounding the argument*/
			first_argument = trim_whitespace(first_argument);

			second_argument = strtok(NULL,"\n");
			/* Get rid of whitespaces surrounding the argument*/
			second_argument = trim_whitespace(second_argument);

	
			num_of_arguments = 2;
		/* If there is 1 or less arguments. If there is no value to give (0 arguments), it'll just give back null to the var (or a value that won't be registered as a type of 
		an argument, ie. white space), and in such scenario, we won't increase the num of arguments var to 1 */
		} else {
			first_argument = strtok(NULL,"\n");
			if (first_argument != NULL) num_of_arguments = 1;
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

bool check_if_num_of_arguments_are_valid(instruction_sentence* cur_command_sentence) {
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

static void deal_with_second_parameter(instruction_sentence* cur_command_sentence,mila* cur_binary_value_of_command,mila* binary_value_for_second_argument,int* L) {
	int type_of_second_argument = determine_type_of_asm_argument(cur_command_sentence->second_argument); /* Determine the type of the second argument*/
	/* If the second argument is not valid, output an error and return*/
	if (type_of_second_argument == NOT_VALID) {
		fprintf(stderr, "The program got an invalid assembly instruction. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}
	if (!is_argument_valid_for_this_specific_command(cur_command_sentence,type_of_second_argument,2)) {
		fprintf(stderr, "The program got an invalid assembly instruction. The second argument is not valid for this command. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}

	
	/* Logic that deals with the type of argument here ~~*/

	/* If the second argument is a register, get the register number and add it to the binary value of the command*/
	if (type_of_second_argument == REGISTER) {
		 deal_with_register_type_value(cur_binary_value_of_command,cur_command_sentence,2);
	/* All other types of arguments require to add an extra Mila, so we'll increment the L value.*/
	} else {
		*L += 1;
		/* At this point of time, in the first scan, we only know how to deal with immediate values. So lets check this type only*/
		if (type_of_second_argument == IMMEDIATE) {
			deal_with_immediate_type_value(binary_value_for_second_argument,cur_command_sentence->second_argument);
		}

	}
}	

static void deal_with_first_parameter(instruction_sentence* cur_command_sentence,mila* cur_binary_value_of_command,mila* binary_value_for_first_argument,int* L) {
	int type_of_first_argument = determine_type_of_asm_argument(cur_command_sentence->first_argument); /* Determine the type of the first argument*/
	/* If the first argument is not valid, output an error and return*/
	if (type_of_first_argument == NOT_VALID) {
		fprintf(stderr, "The program got an invalid assembly instruction. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}
	if (!is_argument_valid_for_this_specific_command(cur_command_sentence,type_of_first_argument,1)) {
		fprintf(stderr, "The program got an invalid assembly instruction. The first argument is not valid for this command. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}

	
	/* Logic that deals with the type of argument here ~~*/

	/* If the first argument is a register, get the register number and add it to the binary value of the command*/
	if (type_of_first_argument == REGISTER) {
		 deal_with_register_type_value(cur_binary_value_of_command,cur_command_sentence,1);
	/* All other types of arguments require to add an extra Mila, so we'll increment the L value.*/
	} else {
		*L += 1;
		/* At this point of time, in the first scan, we only know how to deal with immediate values. So lets check this type only*/
		if (type_of_first_argument == IMMEDIATE) {
			deal_with_immediate_type_value(binary_value_for_first_argument,cur_command_sentence->first_argument);
		}

	}

		
}

void deal_with_immediate_type_value(mila* additional_mila,char* argument) {
	mila binary_value_of_immediate_argument;
	unsigned int value_of_immediate_argument;
	char* argument_without_hash = strtok(argument,"#"); /* Get the argument without the '#' char*/
	if (argument_without_hash == NULL) {
		fprintf(stderr, "The program got an immediate argument that has a '#' but isn't followed with anything after. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}
	value_of_immediate_argument = atoi(argument_without_hash); /* Convert the direct argument to a decimal int */

	/* Take the decimal value of the direct argument and bit shift it to the left so it doesn't collide with the A.R.E bits!*/
	binary_value_of_immediate_argument.v = value_of_immediate_argument << INDEX_OF_THE_BIT_AFTER_A; 
	binary_value_of_immediate_argument.v += 1 << INDEX_OF_THE_A_BIT; /* Set the A value of the immediate argument to 1 (Absolute) */

	/* Add the binary value of the immediate argument to the binary value of the additional mila */
	(*additional_mila).v = binary_value_of_immediate_argument.v;

	/* Since we are dealing with a immediate value, we need to set the addressing mode to immediate to the Mila,
	HOWEVER the value of the addressing mode is convinently 0, which is the default value of the mila.
	So we don't need to do anything here. */
}

void deal_with_register_type_value(mila* cur_binary_value_of_command,instruction_sentence* cur_command_sentence,int argument_number) {
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
			(*cur_binary_value_of_command).v += register_number << INDEX_OF_SOURCE_REGISTER_BIT;
			(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_SOURCE_ADDRESING_MODE_BIT;
		} else if (argument_number == 2) /* Target*/ {
			(*cur_binary_value_of_command).v += register_number << INDEX_OF_TARGET_REGISTER_BIT;
			(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_TARGET_ADDRESING_MODE_BIT;
		}
	} else if (cur_command_sentence->num_of_arguments == 1) { /* If there is only 1 argument, it has to be the target*/
		(*cur_binary_value_of_command).v += register_number << INDEX_OF_TARGET_REGISTER_BIT;
		(*cur_binary_value_of_command).v += REGISTER << INDEX_OF_TARGET_ADDRESING_MODE_BIT;
	} else {
		return; /* If there are no arguments, do nothing */
	}
	
}

int get_index_of_command(char* command_name) {
	int i;

	/* Check if the read command name is an actual command name that's part of our imaginary assembly lang.
	*/
	for (i = 0; i < NUM_OF_ASM_COMMANDS; i++) {
		if (strcmp(instructions_machine_code_rep[i].name,command_name) == 0) {
			return i;
		}
	}
	return -1;
}

bool is_argument_valid_for_this_specific_command(instruction_sentence* cur_command_sentence,int argument_type,int argument_number) {
	int num_of_arguments = cur_command_sentence->num_of_arguments; /* Get the number of arguments the command has*/
	int index = cur_command_sentence->index_of_command; /* Get the index of the command in the array of commands*/

	/* Check if the argument type is valid for this specific command.
	  We go to the index of the command in the array of commands, and check if the argument type is valid for this
	  command via going to the index of a wanted addressing mode and checking if the value is true or false.
	*/

	/* If the command has 2 arguments, check if the argument type is valid. First arg has to be for source, and Second for target.*/
	if (num_of_arguments == 2) {
		if (argument_number == 1) {
			if (addressing_modes_for_commands[index].source_addressing_modes[argument_type]) {
				return true;
			}
		} else if (argument_number == 2) {
			if (addressing_modes_for_commands[index].target_addressing_modes[argument_type]) {
				return true;
			}
		}
	/* If the command has 1 argument, check if the argument type is valid. The argument has to be for target.*/
	} else if (num_of_arguments == 1) {
		if (addressing_modes_for_commands[index].target_addressing_modes[argument_type]) {
			return true;
		}
	/* If the command has 0 arguments, no need to check since there is no arg.*/
	} else if (num_of_arguments == 0) return true;

	return false; 
}

int determine_type_of_asm_argument(char* argument) {
	int i;

	/* Check for a singular '#' at the start to determine the type is immediate*/
	if (argument[0] == '#' && (strchr(argument+1,'#') == NULL)) return IMMEDIATE;

	/* Check for a singular '&' at the start to determine the type is relative*/
	if (argument[0] == '&' && (strchr(argument+1,'&') == NULL)) return RELATIVE;

	/* Check if the argument is a register*/
	for (i = 0; i < sizeof(registers)/sizeof(registers[0]) ; i++) {
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
		fprintf(stderr, "The program got an invalid assembly instruction. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return -1;
	}

	/* Get the opcode and funct values of the command and add them to the machine code representation of the command.
	Do it via bit shifting to the desired index*/
	machine_code_rep_of_command += instructions_machine_code_rep[index_of_command].opcode << INDEX_OF_OPCODE_BIT;
	machine_code_rep_of_command += instructions_machine_code_rep[index_of_command].funct << INDEX_OF_FUNCT_BIT;	
	
	return machine_code_rep_of_command;
}


void handle_directive(char* line,key_resources* key_resources) {
	char* directive_name = strtok(line, " "); /* Get the directive name */
	char* directive_value = strtok(NULL, "\n"); /* Get the directive value. Recall that we need to look for a new line char SPECIFICALLY since values can be seperated by spaces*/

	if (directive_name == NULL || directive_value == NULL) {
		fprintf(stderr, "A directive is missing a name or a value.\n \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}

	/* Check which directive it is and handle it accordingly */
	if (strcmp(directive_name, ".data") == 0) {
		handle_data_directive(directive_value,key_resources->data_table);		
	} else if (strcmp(directive_name, ".string") == 0) {
		handle_string_directive(directive_value,key_resources->data_table);		
	} else if (strcmp(directive_name, ".entry") == 0) {
		/* This is a placeholder for now. We will handle the entry directive in the second scan */
	} else if (strcmp(directive_name, ".extern") == 0) {
		handle_extern_directive(directive_value,key_resources);
	} else {
		fprintf(stderr, "Unknown directive. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
	}
	
}

void handle_extern_directive(char* extern_name,key_resources* key_resources) {
	trim_whitespace(extern_name); /* Get rid of any whitespace surrounding the extern name*/
	if (extern_name == NULL) {
		fprintf(stderr, "Extern directive is missing a value.\n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return;
	}
	add_label_to_table(extern_name,0,".external",key_resources->label_nodes); /* Add the label name to the table*/
}


void handle_string_directive(char* value,mila data_table[]) {
	char* value_without_quo_marks = get_string_directive_value(value);
	char* ptr_to_start = value_without_quo_marks; /*to free*/
	if (value_without_quo_marks == NULL) return;

	/* As long as there are more characters to read from, read the next character*/
	while (*value_without_quo_marks != '\0') {
		data_table[DC].v = (*value_without_quo_marks);
		DC++; /* Increment the Data Counter */
		value_without_quo_marks++; /* Get the next character */
	}
	data_table[DC].v = '\0';
	DC++;

	free(ptr_to_start); /* Free the memory allocated for the string value */
}

char* get_string_directive_value(char* value) {
	/* The end of the actual value (including quo marks)*/
	char* end;

	if (value == NULL) {
		fprintf(stderr, "String directive is missing a value. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}

	value = trim_whitespace(value);
	end = value;

	if (*value != '\"') {
		fprintf(stderr, "String directive's value doesn't correctly open and close with: ' \" '. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	} else {
		*value = '\0';
		value++;
	};
	
	/* Get to the last char that isn't null.*/
	while (*(end+1) != '\0') {
		end++;
	}

	if (*end != '\"') {
		fprintf(stderr, "String directive's value doesn't correctly open and close with: ' \" '. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	} else {
		*end = '\0';
	}

	return (strdup(value));
}

void handle_data_directive(char* data,mila* data_table) {
	char* bit_of_data;
	 
	 if (!has_good_structure_for_data_directive_sentence(data)) {
		return;
	 }
	 

	 bit_of_data = strtok(data,",");


	/* As long as there are more data values to read from, read the next data value*/
	while (bit_of_data != NULL) {
		int numeric_val_of_data = atoi(bit_of_data);
		if (numeric_val_of_data == 0 && strchr(bit_of_data,'0') == NULL) {
			fprintf(stderr,"Data directive has a value that can't be parsed into a number at all.\n LINE: %d", current_line_number);
			return;
		}

		if (numeric_val_of_data < MIN_NUMERIC_VALUE_FOR_DATA_IN_DATA_DIRECTIVE ) {
			fprintf(stderr, "Data directive has a value that's smaller then the smallest valid number: -(2^23-1).\n LINE: %d\n", current_line_number);
			does_file_have_errors = true;
			return;
		} else if (numeric_val_of_data > MAX_NUMERIC_VALUE_FOR_DATA_IN_DATA_DIRECTIVE) {
			fprintf(stderr, "Data directive has a value that's bigger then the biggest valid number: 2^23-1.\n LINE: %d\n", current_line_number);
			does_file_have_errors = true;
			return;
		}

		data_table[DC].v = numeric_val_of_data;

		DC++; /* Increment the Data Counter */
		bit_of_data = strtok(NULL, ","); /* Get the next data value */
	}
}

bool has_good_structure_for_data_directive_sentence(char* data_sentence) {
    char *ptr = data_sentence;


	if (data_sentence == NULL) {
        fprintf(stderr, "Data directive is missing a value.\nLINE: %d\n", current_line_number);
        does_file_have_errors = true;
        return false;
    }


    while (*ptr != '\0') {

        /* Skip any leading whitespace */
        while (isspace(*ptr)) {
			ptr++;
		}

        /* Handle optional minus sign for negative numbers */
        if (*ptr == '-') {
            ptr++;
        }

        /* Ensure there's at least one digit after optional '-' */
        if (!isdigit(*ptr)) {
            fprintf(stderr, "Expected a number in data directive. are there any excess commas?\nLINE: %d\n", current_line_number);
            does_file_have_errors = true;
            return false;
        }

        /* Consume the full numeric part */
        while (isdigit(*ptr)) {
            ptr++;
        }

        /* Skip trailing whitespace after number */
        while (isspace(*ptr)) {
            ptr++;
        }

        /* If a comma follows, move to next number */
        if (*ptr == ',') {
         	ptr++;
		/* If we reached the end of string, structure is valid so far */	
        } else if (*ptr == '\0') {
            break;
        } else {
        /* If it's any other character, it's invalid */
            fprintf(stderr, "Invalid character in data directive.\nLINE: %d\n", current_line_number);
            does_file_have_errors = true;
            return false;
        }
    }

    return true;
}

char* is_valid_label(char* line,key_resources* key_resources) {
	int i;

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

	/*----------------------------------------------------------------------------------------------------------------------------------------*/

	/* Check if the label name is a reserved word (like an instruction name)*/
	for (i = 0; i < NUM_OF_RESERVED_WORDS; i++) {
		if (strcmp(label_name, list_of_reserved_words[i]) == 0) {
			fprintf(stderr, "The label name is a reserved word. \n LINE: %d\n", current_line_number);
			does_file_have_errors = true;
			return NULL;
		}
	}
	
	/* Check if the label name is already defined or not*/
	if (is_label_already_defined(label_name,key_resources->label_nodes)) {
		fprintf(stderr, "The label name is already defined. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}

	/* Check if the label is a reserved name of a macro*/
	if (is_label_already_defined_as_macro_name(label_name,key_resources->macro_nodes)) {
		fprintf(stderr, "The label name is already defined as a macro name. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}

	/* Check if the label name is too long*/
	if (strlen(label_name) > MAX_LENGTH_OF_LABEL) {
		fprintf(stderr, "The label name is too long. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}

	/* Check if the label name is valid (only contains alphanumeric characters and numbers)*/
	if (!isalpha(label_name[0])) {
		fprintf(stderr, "The label name is invalid. It must start with a letter. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}
	for (i = 1; i < strlen(label_name); i++) {
		if (!isalnum(label_name[i])) {
			fprintf(stderr, "The label name is invalid. It must only contain letters and numbers. \n LINE: %d\n", current_line_number);
			does_file_have_errors = true;
			return NULL;
		}
	}

	/*------------------------------------------------------------------------------------------------------------------------------------------*/

	return label_name;
}

bool is_label_already_defined_as_macro_name(char* checked_name,key_macro_nodes* key_nodes) {
	macro_node* pos = key_nodes->head_of_macro_storage;

	while (pos != NULL) {
		/* return true only if the name is a macro name*/
		if (strcmp(checked_name,pos->val.macro_name) == 0) {
			return true;
		}

		pos = pos->next;
	}

	return false;
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

	if (directive_name[0] == '.') {
		fprintf(stderr,"Got invalid directive type (line starts with '.').");
	}

	return false;
}

bool is_label_already_defined(char* label_name,key_label_nodes* key_resources) {
	label_node* pos = key_resources->head_of_label_storage;

	while (pos != NULL) {
		if (strcmp(pos->val.label_name, label_name) == 0) {
			return true; /* Label already defined */
		}
		pos = pos->next;
	}
	return false; /* Label not defined */
}

void add_label_to_table(char* label_name,int label_address, char* type,key_label_nodes* key_resources) {
	label new_label;
	label_node* new_label_node;

	/* Give the label its name, address, and type */
	new_label.label_name = strdup(label_name);
	if (new_label.label_name == NULL) {
		fprintf(stderr, "Memory allocation failed. A Label has not managed to be added to table of Labels.\n");
		exit(1);
	}
	new_label.label_address = label_address;
	new_label.label_type = type;
	new_label.is_entry = false; /* Set the is_entry flag to false by default */

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
	if (key_resources->head_of_label_storage == NULL) {
		key_resources->head_of_label_storage = new_label_node;
		key_resources->last_label_node = new_label_node;
	} else {
		/* If the table is not empty, add the new label node to the end of the table */
		key_resources->last_label_node->next = new_label_node;
		key_resources->last_label_node = new_label_node;
	}

}

/* checks if this Sentence is empty or not and return a boolean val accordingly*/
bool is_empty(char* line) {
	if (line == NULL) return true;

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

bool has_semicolon(char* line) {
	line++; /*skip the intended semi-colon if it is there*/
	if (strchr(line, ';') != NULL) {
		fprintf(stderr, "The program got a sentence with a comment in the middle of it. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return true;
	}

	return false;
}

