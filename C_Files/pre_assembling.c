#include "pre_assembling.h"

/**
 * This function is the first step of the assembly process. It goes over the assembly file and looks for macros. If it finds a macro, it will store it in a list of known macros.
 * If it finds a macro name in the assembly file, it will replace it with the contents of the macro.
 * 
 * @param start_of_assembly_file_pointer - The pointer to the start of the assembly file.
 * @param input_file_name - The name of the assembly file.
 * @param key_nodes - A struct that holds pointers to the head, current, and last nodes of the list of known macros.
 * @return FILE* - The pointer to the start of the after-macro file.
 */
FILE* pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name,key_macro_nodes* key_nodes) {
	FILE* cur_asm_file_line_ptr = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	FILE* am_file = create_after_macro_file(input_file_name); /* Create the after-macro file. we will write unto it each line we go from from the og asm file. */
	char line[GEN_LENGTH_OF_STRINGS] = {0}; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */
	
	
	char* state;
	state = "OUTSIDE"; /* Declare and initialize the state variable */
	
	printf("\n\tlooking for macros in file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, cur_asm_file_line_ptr) != NULL) {

		/* Check the state of the program, and act accordingly. */
		if (strcmp(state,"OUTSIDE") == 0) {
			state = outside_macro_declaration(line,am_file,state,key_nodes);
		} else if (strcmp(state,"INSIDE_MACRO_DECLARATION") == 0) {
			state = inside_macro_declaration_state(line,am_file,state,key_nodes);
		}


		 /* Check for an error state. We'll go to next file */
		 if (strcmp(state, "ERROR") == 0) {
            fclose(am_file); /* Close the file*/
            remove_am_file(input_file_name); /* Delete the file*/
            return NULL; 
        }
	}



	fseek(am_file, 0, SEEK_SET); /* Go back to the start of the file so we can read from the start of the file.*/
	return am_file;
}


char* outside_macro_declaration(char* line,FILE* am_file,char* state,key_macro_nodes* key_nodes) {
	char* ptr_to_remaining_of_macro_line=NULL; /* When we find a mcro declaration, this will hold the macro name that is in the same line as the declaration*/

	/* If the line is empty, just print it and continue to the next line.*/
	if (is_empty(line)) {
		fprintf(am_file, "%s", line);
		return state;
	}

	/* If we found a macro, then take the contents of the macro and write them unto the am_file*/
	if (check_line_for_macro(line,key_nodes)) {
		/* cur_macro_node is the node that we found has a macro_name that is the same as this line. so we take the contents of the macro and paste them unto the am_file*/
		fprintf(am_file, "%s", key_nodes->cur_macro_node->val.macro_content);

		return state; /* Continue so you don't print the macro name*/
	} 
	/* Check if the line is a macro declaring line
	we use strtok() to devide an inspected line with a space in it, since macro declaring lines come in the format: "mcro <name>", so we want to check the first word only*/
	if (strcmp(strtok_copy(line," "),"mcro") == 0) {
		printf("New macro being declared!\n");

		ptr_to_remaining_of_macro_line = (strchr(line,' ')+1); /* Point at the remaining part of the line (the actual name of the macro)*/

		printf(" macro name: %s\n",ptr_to_remaining_of_macro_line);

		/* Create a new macro node into the list and give its macro field a macro name that is the name that is in 'line'.*/
		if (add_a_macro_node_to_list(ptr_to_remaining_of_macro_line,key_nodes)) {
			/* If the macro node was added successfully, we can now change the state to inside a macro declaration. */
			state = "INSIDE_MACRO_DECLARATION"; 
		} else {
			fprintf(stderr, "The program got an invalid assembly instruction.");
			state = "ERROR"; /* Change state to error since we can't have a macro name like this*/
		}

		return state;
	}

	free_macro_storage(key_nodes); 
	fprintf(am_file, "%s", line); /* If you got here, the line that got registered is a regular line, so just print it.*/
	return state;
}

char* inside_macro_declaration_state(char* line,FILE* am_file,char* state,key_macro_nodes* key_nodes) {
	/* Get the last macro node in the list of known macros. */
	macro_node* last_node = key_nodes->last_macro_node;

	/* If the line is empty, just print it and continue to the next line.*/
	if (is_empty(line)) {
		fprintf(am_file, "%s", line);
		return state;
	}
	/* Check if the line is an end of a macro declaration. if yes, change state to outside of macro declaration and continue reading the next line*/
	if (strcmp(strtok_copy(line,"\n"), "mcroend") == 0) {
		state = "OUTSIDE";
		return state;
	}

	/* Concatenate this line of information to the macro_content field of this macro. a check if it is null is needed since strlen(NULL) is a segfault.*/
	if (last_node->val.macro_content == NULL) {
		last_node->val.macro_content = malloc(strlen(line)+1);
		last_node->val.macro_content[0] = '\0'; /* Init the macro content so we can concatenate to it*/
	} else {
		last_node->val.macro_content = realloc(last_node->val.macro_content,
			strlen(last_node->val.macro_content) + strlen(line) + 1);
	}

	/* If the program did not manage to get enough storage to store the macro in the list of macro symbols, output an error and free the storage.*/
	if (last_node->val.macro_content == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		free_macro_storage(key_nodes);
		exit(1);
	}
	strcat(last_node->val.macro_content, line);

	return state;
}

bool check_line_for_macro(char* chosen_line,key_macro_nodes* key_nodes) {
	key_nodes->cur_macro_node = key_nodes->head_of_macro_storage; /* Initialize the current macro node to the head of the list of known macros. */
	
	/* If the chosen_line does not end with a newline character, add one. (so comarision is with two strings with a \n) */
	if (strchr(chosen_line, '\n') == NULL) {
		strcat(chosen_line, "\n");
	}
	
	
	/* Go over all the elements in the storage of known macros. if the name of the line correspounds to a known macro, return true (there is a macro name like this name) otherwise return false*/
	while (key_nodes->cur_macro_node != NULL) {
		if (strcmp(chosen_line,key_nodes->cur_macro_node->val.macro_name)==0) {
			printf("read line is a macro! (%s)\n",chosen_line);
			return true;
		}
		key_nodes->cur_macro_node = key_nodes->cur_macro_node->next; /* advance to the next macro node in the list*/

	}
	
	/* If we got here, it means there isn't a macro in this line. returning false so we don't try to translate anything.*/
	return false;
}

bool add_a_macro_node_to_list(char* found_macro_name,key_macro_nodes* key_nodes) {
	/* Create and initialize the new macro node. its macro name will be the name we found in 'line', and we make sure its next field is null. */
	macro_node* new_macro_node = malloc(sizeof(macro_node));

	/* Check if the macro name is even valid. If it isn't we will have to output an error and jump to the next file since the current file is not valid*/
	if (!is_valid_macro_name(found_macro_name)) {
		free_macro_storage(key_nodes);
		return false;
	}

	/* Check if we have space for the information or not. */
	if (new_macro_node == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		free_macro_storage(key_nodes);
		exit(1);
	}
	
	/* Allocate storage to where the macro name will be stored, and if can be done, copy the ~value~ of found_macro_name unto the feild. */
	new_macro_node->val.macro_name = malloc(strlen(found_macro_name) + 1);
	if (new_macro_node->val.macro_name == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		free_macro_storage(key_nodes);
		exit(1);
	}

	strcpy(new_macro_node->val.macro_name, found_macro_name);

	new_macro_node->val.macro_content = NULL;
	new_macro_node->next = NULL;
	

	/* Attach the new node to the list */
	/* Check if there is actually a node*/
	if (key_nodes->last_macro_node != NULL) {
		key_nodes->last_macro_node->next = new_macro_node;
	} else {
		key_nodes->head_of_macro_storage = new_macro_node;  /* Initialize head if list is empty*/
	}

	/* Update the last node pointer */
	key_nodes->last_macro_node = new_macro_node;


	/* If we got here, it means the the process was succesful. so we'll return 'true'*/
	return true;
}

bool is_valid_macro_name(char* macro_name) {
	char* list_of_invalid_macro_names[] = {
		 "mcro", "mcroend",
		 "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
		 "jmp", "bne", "jsr", "red", "prn", "rts", "stop",
		 ".data", ".string", ".entry", ".extern"
	};

	
	int i;

	if (macro_name == NULL) {
		fprintf(stderr, "A macro name can't be null\n");
		return false;
	}

	/* Check if the macro name is one of the invalid names */
	for (i = 0; i < sizeof(list_of_invalid_macro_names) / sizeof(list_of_invalid_macro_names[0]); i++) {
		if (strcmp(macro_name, list_of_invalid_macro_names[i]) == 0) {
			fprintf(stderr, "A macro name is can't be a kept keyword\n");
			return false;
		}
	}

	/* Check if the macro name starts with a letter or an underscore */
	if (!isalpha(macro_name[0]) && macro_name[0] != '_') {
		fprintf(stderr, "A macro name must start with a letter or an underscore.\n");
		return false;
	}

	/* Check if the macro name contains only letters, numbers, and underscores. we check one less char than the length of the string since we check if the very last character is a \n or not.*/
	for (i = 1; i < strlen(macro_name) - 1; i++) {
		if (!isalnum(macro_name[i]) && macro_name[i] != '_') {
			fprintf(stderr, "A macro name can only contain letters, numbers, and underscores.\n");
			return false;
		}
	}
	if (macro_name[strlen(macro_name) - 1] != '\n') {
		fprintf(stderr, "A macro name must end with a newline character.\n");
		return false;
	}


	return true;
}

/**
 * This func Frees all the storage that was allocated for the macro nodes. Used typically when we done with the macro stage, however will also be
 * used in situations where memory allocation failed.
 */
void free_macro_storage(key_macro_nodes* key_nodes) {
    macro_node *current = key_nodes->head_of_macro_storage;
    macro_node *next;

	/* Free each node, and its fields...*/
    while (current != NULL) {
        next = current->next;
        free(current->val.macro_name);
        free(current->val.macro_content);
        free(current);
        current = next;
    }

    key_nodes->head_of_macro_storage = NULL;
    key_nodes->cur_macro_node = NULL;
    key_nodes->last_macro_node = NULL;
}