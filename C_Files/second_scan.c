#include "second_scan.h"

void second_scan(FILE* start_of_assembly_file_pointer,key_resources* k_resources) {
	FILE* input_file_pointer = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\tGoing through second scan of the file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line,k_resources);
		current_line_number++; /* Increment the line number*/
	}

	current_line_number = 1; /* Reset the line number */

}

static void go_over_read_line(char* chosen_line,key_resources* k_resources) {
	char* directive_type = NULL; /* This will hold the type of directive we are currently looking at. (if we're dealing with directives)*/

	/* If the line is empty or a comment, skip it*/
	if (is_empty(chosen_line) || is_comment(chosen_line)) return;

	if (has_semicolon(chosen_line)) return; /* If the line has a comment in the middle, skip it.*/

	chosen_line = look_for_first_non_whitespace_char(chosen_line); /* This will hold the line without indentation. Will be used to get the raw value of the line so we can compare correctly with certain keywords. */
	chosen_line = skip_label_name(chosen_line);

	/* Determine which type this sentence is.*/
	if (is_directive(chosen_line)) {
		directive_type = strtok(strdup(chosen_line),"");
		if (strcmp(directive_type, ".entry") == 0) {
			handle_entry_directive(chosen_line,k_resources);
		} else {
			return;
		}
		free(directive_type);
	} else if (is_valid_command(chosen_line)) {
		handle_command(chosen_line,k_resources);
	} else {
		/*no need to output the same error in both scans*/
	}
	

}

static void handle_command(char* command,key_resources* k_resources) {
	 /* This will hold the index of the cur instruction we haven't went over in the instruction table, and so we get easy access to info we gathered and the machine code of the Milas.
	 	Because we are going over the instruction table in a linear way, we are aligned with the instruction in a way that 
		the index of the instruction table is the same as the chronological-index of the instruction we are currently looking at.
	 	This is a static variable, so it will be the same for all calls to this function.
	 */
	static int index_in_instruction_table = 0;

	/* This struct will be handy and convieneient since it gathers all usefull info of this sentence in a singular place! */
	instruction_sentence* cur_command_sentence = make_command_sentence_struct(command);

	
	if (current_line_number == 1) {
		index_in_instruction_table = 0; /* Reset the index to 0 at the start of the second scan. */
	}

	trim_the_ampersand_symbol(cur_command_sentence);
	

	/* If there are arguments added alongside the instruction, deal with the Machine Code they give appropriately.*/
	if (cur_command_sentence->first_argument != NULL) {
		deal_with_first_parameter(cur_command_sentence,k_resources,index_in_instruction_table);

		if (cur_command_sentence->second_argument != NULL) {
			deal_with_second_parameter(cur_command_sentence,k_resources,index_in_instruction_table);
		}
	}

	/* Update the index of the instruction table. */
	index_in_instruction_table++;
}

void trim_the_ampersand_symbol(instruction_sentence* cur_command_sentence) {
	if (cur_command_sentence->first_argument != NULL) {
		if (cur_command_sentence->first_argument[0] == '&') {
			cur_command_sentence->first_argument++;
		}
	}
	if (cur_command_sentence->second_argument != NULL) {
		if (cur_command_sentence->second_argument[0] == '&') {
			cur_command_sentence->second_argument++;
		}
	}
}

static void deal_with_second_parameter(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index_in_instruction_table) {
	int type_of_second_argument = determine_type_of_asm_argument(cur_command_sentence->second_argument); /* Determine the type of the first argument*/
	
	/* ~~ No need to check the validity of the argument for the specific command if we got to the second scan ~~*/


	/* In the first scan, we dealt with the encoding for the immediate and register types, and now, in the second scan we'll deal with the other 2 types of arguments.*/
	if (type_of_second_argument == DIRECT) {
		deal_with_direct_type_value(cur_command_sentence,k_resources,index_in_instruction_table,2);
		check_if_the_argument_is_external(cur_command_sentence->second_argument,k_resources,k_resources->instruction_table[index_in_instruction_table].IC);
	} else if (type_of_second_argument == RELATIVE) {
		deal_with_relative_type_value(cur_command_sentence,k_resources,index_in_instruction_table,2);
		check_if_the_argument_is_external(cur_command_sentence->second_argument,k_resources,k_resources->instruction_table[index_in_instruction_table].IC);
	}
}

static void deal_with_first_parameter(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index_in_instruction_table) {
	int type_of_first_argument = determine_type_of_asm_argument(cur_command_sentence->first_argument); /* Determine the type of the first argument*/
	
	/* ~~ No need to check the validity of the argument for the specific command if we got to the second scan ~~*/


	/* In the first scan, we dealt with the encoding for the immediate and register types, and now, in the second scan we'll deal with the other 2 types of arguments.*/
	if (type_of_first_argument == DIRECT) {
		deal_with_direct_type_value(cur_command_sentence,k_resources,index_in_instruction_table,1);
		check_if_the_argument_is_external(cur_command_sentence->first_argument,k_resources,k_resources->instruction_table[index_in_instruction_table].IC);
	} else if (type_of_first_argument == RELATIVE) {
		deal_with_relative_type_value(cur_command_sentence,k_resources,index_in_instruction_table,1);
		check_if_the_argument_is_external(cur_command_sentence->first_argument,k_resources,k_resources->instruction_table[index_in_instruction_table].IC);
	}
}

void check_if_the_argument_is_external(char* argument,key_resources* k_resources,int IC) {
	label* wanted_label = find_label_node(argument,k_resources->label_nodes); /* This will hold the label struct. */
	if (wanted_label == NULL) return; /* If the label node is NULL, we can't do anything. */

	/* If the label is external, we add it to the externals list. */
	if (strcmp(wanted_label->label_type,".external") == 0) {
		add_extern_label_to_list(wanted_label,k_resources->extern_label_nodes,IC);
		/* Set the - 'is there any externs' flag to true.
		We only set this flag HERE to true, since if we declared an extern without using it, it means nothing to us. */
		k_resources->is_there_any_externs = true;
	}
}

void add_extern_label_to_list(label* wanted_label,key_extern_label_nodes* k_resources,int IC) {
	extern_label_node* new_extern_label_node = (extern_label_node*)malloc(sizeof(extern_label_node)); /* This will hold the new extern label node. */
	if (new_extern_label_node == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}

	new_extern_label_node->val.label_name = strdup(wanted_label->label_name); /* Allocate memory for the label name and copy it. */
	if (new_extern_label_node->val.label_name == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	new_extern_label_node->val.label_address = IC; /* Set the address of the label to the current instruction counter. */

	new_extern_label_node->next = NULL; /* Set the next pointer to NULL. */

	if (k_resources->head_of_extern_label_storage == NULL) {
		k_resources->head_of_extern_label_storage = new_extern_label_node; /* Set the head of the extern label storage to the new extern label node. */
		k_resources->last_extern_label_node = new_extern_label_node; /* Set the last extern label node to the new extern label node. */
	} else {
		k_resources->last_extern_label_node->next = new_extern_label_node; /* Add the new extern label node to the end of the list. */
		k_resources->last_extern_label_node = new_extern_label_node; /* Update the last extern label node to be the new extern label node. */
	}
}

void deal_with_direct_type_value(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index,int argument_number) {
	instruction (*table)[] = &(k_resources->instruction_table); /* This will hold the instruction table. */

	mila code_of_command = (*table)[index].code_of_command; /* This will hold the code of the command. */
	mila code_of_first_additional_mila = (*table)[index].code_of_first_argument; /* This will hold the code of the first additional mila. */
	mila code_of_second_additional_mila = (*table)[index].code_of_second_argument; /* This will hold the code of the second additional mila. */

	
	char* argument = argument_number == 1 ? cur_command_sentence->first_argument : cur_command_sentence->second_argument; /* This will hold the argument. */
	label* wanted_label = find_label_node(argument,k_resources->label_nodes); /* This will hold the label struct. */
	if (wanted_label == NULL) return; /* If the label node is NULL, we can't do anything. */


	/* We need to distinguish between 2 and 1 arguments scenarios.
		* If there are 2 arguments, the first argument is the source and the second is the target.
		* If there is 1 argument, the first argument is the target.
	 
	   Regardless of the amount of arguments, we need to add the label address (the argument we got) to the machine code in the additional mila.
	*/
	if (cur_command_sentence->num_of_arguments == 2) {
		if (argument_number == 1) {
			code_of_command.v += DIRECT << INDEX_OF_SOURCE_ADDRESING_MODE_BIT; 
			add_machine_code_of_label(&code_of_first_additional_mila,wanted_label); 
		} else if (argument_number == 2) {
			code_of_command.v += DIRECT << INDEX_OF_TARGET_ADDRESING_MODE_BIT; 
			add_machine_code_of_label(&code_of_second_additional_mila,wanted_label);
		}
	} else if (cur_command_sentence->num_of_arguments == 1) {
		code_of_command.v += DIRECT << INDEX_OF_TARGET_ADDRESING_MODE_BIT; 
		add_machine_code_of_label(&code_of_first_additional_mila,wanted_label); 
	} else {
		return; /* If there are no arguments, do nothing */
	}
}


void add_machine_code_of_label(mila* ptr_to_mila,label* label) {
	/* we want to change the A.R.E fields correctly.*/

	/* If the label is external, we set the E (external) bit to 1, and the others we keep at 0 */
	if (strcmp(label->label_type,".external") == 0) {
		(*ptr_to_mila).v += 1 << INDEX_OF_THE_E_BIT;

		/* ~~~An external's address doesn't mean anything in the same source file, so we won't add its address~~~ (keeping it 0)*/
	/* Otherwise, we set the R (relative) bit to 1, and the other two keep at 0*/
	} else {
		(*ptr_to_mila).v += 1 << INDEX_OF_THE_R_BIT;
		/* Add the address of the label to the mila. */
		(*ptr_to_mila).v += label->label_address << INDEX_OF_THE_BIT_AFTER_A;
	}
}

label* find_label_node(char* label_name,key_label_nodes* k_resources) {
	label_node* pos = k_resources->head_of_label_storage; /* This will hold the label node. */

	if (pos == NULL) {
		fprintf(stderr, "The program got a label as an argument for an instruction, but the file doesn't define ANY labels. \n LINE: %d\n", current_line_number);
		does_file_have_errors = true;
		return NULL;
	}

	while (pos != NULL) {
		if (strcmp(pos->val.label_name,label_name) == 0) {
			return &(pos->val);
		}
		pos = pos->next;
	}

	fprintf(stderr, "The program got a label as an argument for an instruction, but the label doesn't exist at all. \n LINE: %d\n", current_line_number);
	does_file_have_errors = true;
	return NULL;
}

void deal_with_relative_type_value(instruction_sentence* cur_command_sentence,key_resources* k_resources,int index,int argument_number) {
	instruction (*table)[] = &(k_resources->instruction_table); /* This will hold the instruction table. */

	mila code_of_command = (*table)[index].code_of_command; /* This will hold the code of the command. */
	mila code_of_first_additional_mila = (*table)[index].code_of_first_argument; /* This will hold the code of the first additional mila. */
	mila code_of_second_additional_mila = (*table)[index].code_of_second_argument; /* This will hold the code of the second additional mila. */

	int current_address = (*table)[index].IC; /* This will hold the current address of the instruction. We kept our IC from last scan, so it's easy to re-refrence it :D */

	char* argument = argument_number == 1 ? cur_command_sentence->first_argument : cur_command_sentence->second_argument; /* This will hold the argument. */
	label* wanted_label = find_label_node(argument,k_resources->label_nodes); /* This will hold the label struct. */
	if (wanted_label == NULL) return; /* If the label node is NULL, we can't do anything. */

	/* We need to distinguish between 2 and 1 arguments scenarios.
		* If there are 2 arguments, the first argument is the source and the second is the target.
		* If there is 1 argument, the first argument is the target.
	 
	   Regardless of the amount of arguments, we need to add the differnce between addresses of the 2 locations to the machine code in the additional mila.
	*/
	if (cur_command_sentence->num_of_arguments == 2) {
		if (argument_number == 1) {
			code_of_command.v += RELATIVE << INDEX_OF_SOURCE_ADDRESING_MODE_BIT; 
			add_machine_code_of_label_relative_to(&code_of_first_additional_mila,wanted_label,current_address); 
		} else if (argument_number == 2) {
			code_of_command.v += RELATIVE << INDEX_OF_TARGET_ADDRESING_MODE_BIT; 
			add_machine_code_of_label_relative_to(&code_of_second_additional_mila,wanted_label,current_address);
		}
	} else if (cur_command_sentence->num_of_arguments == 1) {
		code_of_command.v += RELATIVE << INDEX_OF_TARGET_ADDRESING_MODE_BIT; 
		add_machine_code_of_label_relative_to(&code_of_first_additional_mila,wanted_label,current_address); 
	} else {
		return; /* If there are no arguments, do nothing */
	}
}

void add_machine_code_of_label_relative_to(mila* ptr_to_mila,label* label,int address_of_instruction_we_came_from) {
	int address_of_label = label->label_address;

	/* Get the differences of addresses between the destiny (the label) and the place we come from (the current instruction)
	   Move it to the left of the A.R.E fields.
	*/
	(*ptr_to_mila).v = (address_of_label - address_of_instruction_we_came_from) << INDEX_OF_THE_BIT_AFTER_A;

	/* For Relative additional info Milas, the A bit is the bit that's always on.*/
	(*ptr_to_mila).v += 1 < INDEX_OF_THE_A_BIT;
}

void handle_entry_directive(char* entry_sentence,key_resources* k_resources) {
	char* entry_value = NULL; /* This will hold the value of the entry directive. */
	
	entry_value = strtok(entry_sentence, " ");
	

	entry_value = trim_whitespace(entry_value);

	if (entry_value == NULL) {
		fprintf(stderr, "Entry directive is missing a value. \n LINE: %d\n", current_line_number);
		return;
	}
	look_for_label_in_table_for_entries(entry_value,k_resources); /* This will look for the label in the label table. */

}

void look_for_label_in_table_for_entries(char* label_name,key_resources* k_resources) {
	key_label_nodes* key_labels = k_resources->label_nodes; /* This will hold the label nodes. */


	label_node* pos = NULL;
	
	if (key_labels == NULL) {
		fprintf(stderr, "Entry directive has a value that doesn't exist in the label table. \n LINE: %d\n", current_line_number);
		return;
	}

	pos = key_labels->head_of_label_storage;

	while (pos != NULL) {
		if (strcmp(pos->val.label_name,label_name) == 0) {
			pos->val.is_entry = true; /* If we found the label, we set it to be an entry. */
			k_resources->is_there_any_entry = true; /* If we found an entry, we set the boolean to true. */
			return;
		}
		pos = pos->next;
	}

	fprintf(stderr, "Entry directive has a value that doesn't exist in the label table. \n LINE: %d\n", current_line_number);

}	

char* skip_label_name(char* line) {
	char* ptr = strchr(line, ':');
	/* If we haven't found ":", there aint a label, so we return the whole string.*/
	if (ptr == NULL) {
		return line;
	}
	/* If we found ":", we need to skip it and the whitespace that comes after it. */
	ptr++;
	ptr = look_for_first_non_whitespace_char(ptr);
	return ptr;
}

