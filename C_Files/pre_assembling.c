#include "pre_assembling.h"

static macro_node *head_of_macro_storage=NULL,*cur_macro_node=NULL,*last_macro_node=NULL;




void pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name) {
	FILE* cur_asm_file_line_ptr = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	FILE* am_file = create_after_macro_file(input_file_name); /* Create the after-macro file. we will write unto it each line we go from from the og asm file. */
	char line[GEN_LENGTH_OF_STRINGS] = {0}; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */
	
	char* ptr_to_remaining_of_macro_line=NULL; /* When we find a mcro declaration, this will hold the macro name that is in the same line as the declaration*/
	
	enum READING_ASM_FILE_STATES state = OUTSIDE; /* Declare and initialize the state variable */
	
	printf("\n\tlooking for macros in file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, cur_asm_file_line_ptr) != NULL) {

		switch (state)
		{
			case OUTSIDE:
			/* If we found a macro, then take the contents of the macro and write them unto the am_file*/
				if (check_line_for_macro(line)) {
					printf("found macro\n");
					/* cur_macro_node is the node that we found has a macro_name that is the same as this line. so we take the contents of the macro and paste them unto the am_file*/
					fprintf(am_file, "%s", cur_macro_node->val.macro_content);

					continue; /* Continue so you don't print the macro name*/
				} 
			/* Check if the line is a macro declaring line
			we use strtok() to devide an inspected line with a space in it, since macro declaring lines come in the format: "mcro <name>", so we want to check the first word only*/
				if (strcmp(strtok(line," "),"mcro") == 0) {
					printf("New macro being declared!\n");

					ptr_to_remaining_of_macro_line = (strchr(line,'\0')+1); /* Point at the remaining part of the line (the actual name of the macro)*/

					printf(" macro name: %s\n",ptr_to_remaining_of_macro_line);
			
					/* Create a new macro node into the list and give its macro field a macro name that is the name that is in 'line'.*/
					add_a_macro_node_to_list(ptr_to_remaining_of_macro_line);

					state = INSIDE_MACRO_DECLARATION; /* Change state since we've noticed we're inside a macro declaration*/

					continue; /* Continue so you don't print the 'mcro'*/
				}

				fprintf(am_file, "%s", line); /* If you got here, the line that got registered is a regular line, so just print it.*/


				break;
			case INSIDE_MACRO_DECLARATION:
				/* Check if the line is an end of a macro declaration. if yes, change state to outside of macro declaration and continue reading the next line*/
				if (strcmp(strtok_copy(line,"\n"), "mcroend") == 0) {
					state = OUTSIDE;
					continue;
				}

				/* Concatenate this line of information to the macro_content field of this macro. a check if it is null is needed since strlen(NULL) is a segfault.*/
				if (last_macro_node->val.macro_content == NULL) {
					last_macro_node->val.macro_content = malloc(strlen(line)+1);
					last_macro_node->val.macro_content[0] = '\0'; /* Init the macro content so we can concatenate to it*/
				} else {
					last_macro_node->val.macro_content = realloc(last_macro_node->val.macro_content,
						strlen(last_macro_node->val.macro_content) + strlen(line) + 1);
				}

				/* If the program did not manage to get enough storage to store the macro in the list of macro symbols, output an error and free the storage.*/
				if (last_macro_node->val.macro_content == NULL) {
					fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
					free_macro_storage();
				}
				strcat(last_macro_node->val.macro_content, line);


				break;
			default:
				break;
		}
	}
}


bool check_line_for_macro(char* line) {
	cur_macro_node = head_of_macro_storage; /* Initialize the current macro node to the head of the list of known macros. */
	
	/* If the line does not end with a newline character, add one. (so comarision is with two strings with a \n) */
	if (strchr(line, '\n') == NULL) {
		strcat(line, "\n");
	}
	
	printf("going over all macro nodes and comparing\n");
	
	/* Go over all the elements in the storage of known macros. if the name of the line correspounds to a known macro, return true (there is a macro name like this name) otherwise return false*/
	while (cur_macro_node != NULL) {
		if (strcmp(line,cur_macro_node->val.macro_name)==0) {
			printf("read line is a macro!\n");
			return true;
		}
		printf("%s with %s\n",line,cur_macro_node->val.macro_name);
		cur_macro_node = cur_macro_node->next; /* advance to the next macro node in the list*/

	}
	
	/* If we got here, it means there isn't a macro in this line. returning false so we don't try to translate anything.*/
	printf("read line is not a macro.\n");
	return false;
}

void add_a_macro_node_to_list(char* found_macro_name) {
	/* Create and initialize the new macro node. its macro name will be the name we found in 'line', and we make sure its next field is null. */
	macro_node* new_macro_node = malloc(sizeof(macro_node));
	if (new_macro_node == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		free_macro_storage();
	}
	
	/* Output error if no storage was managed to be given*/
	if (new_macro_node == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		return;
	}
	
	/* Allocate storage to where the macro name will be stored, and if can be done, copy the ~value~ of found_macro_name unto the feild. */
	new_macro_node->val.macro_name = malloc(strlen(found_macro_name) + 1);
	if (new_macro_node->val.macro_name == NULL) {
		fprintf(stderr, "the program did not manage to get enough storage to store a macro in the list of macro symbols.");
		free_macro_storage();
	}

	strcpy(new_macro_node->val.macro_name, found_macro_name);

	new_macro_node->val.macro_content = NULL;
	new_macro_node->next = NULL;
	

	/* Attach the new node to the list */
	/* Check if there is actually a node*/
	if (last_macro_node != NULL) {
		last_macro_node->next = new_macro_node;
	} else {
		head_of_macro_storage = new_macro_node;  /* Initialize head if list is empty*/
	}

	/* Update the last node pointer */
	last_macro_node = new_macro_node;
}

/**
 * This func Frees all the storage that was allocated for the macro nodes. Used typically for crashes.
 */
void free_macro_storage() {
    macro_node *current = head_of_macro_storage;
    macro_node *next;

	/* Free each node, and its fields...*/
    while (current != NULL) {
        next = current->next;
        free(current->val.macro_name);
        free(current->val.macro_content);
        free(current);
        current = next;
    }

    head_of_macro_storage = NULL;
    cur_macro_node = NULL;
    last_macro_node = NULL;

	exit(1);
}