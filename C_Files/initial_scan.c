#include "initial_scan.h"

static label_node*  head_of_label_table = NULL, * tail_of_label_table = NULL, * current_label_node = NULL;

void initial_scan(FILE* start_of_am_file_pointer) {
	FILE* input_file_pointer = start_of_am_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\tGoing through first scan of the file!.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		/* Remove newline if it's there*/
		line[strcspn(line, "\n")] = '\0';
		go_over_read_line(line);
	}
}

static void go_over_read_line(char* chosen_line) {
	bool has_label = false;
	char* label_name;

	/* If the line is empty or a comment, skip it*/
	if (is_empty(chosen_line) || is_comment(chosen_line)) return;

	/* Check if the line has a label attached to it. if it has, already get the name from it in the if condition*/
	if (label_name = is_valid_label(chosen_line)) {
		has_label = true;
		chosen_line = strchr(chosen_line, " "); /* Skip to after the label*/

		if (chosen_line == NULL) {
			fprintf(stderr, "There's a Label without a command or directive.\n");
			return;
		}
	}

	/* Determine which type this sentence is.*/
	if (is_directive(chosen_line)) {
		if (has_label) add_label_to_table(label_name, 'd'); /* Add the label name to the table with the 'd' type that stands for data*/
		handle_directive(chosen_line);
	} else {
		if (has_label) add_label_to_table(label_name, 'c'); /* Add the label name to the table with the 'c' type that stands for command*/
		handle_command(chosen_line);		/* Handle the command */
	}

}

void handle_directive(char* line) {
	char* directive_name = strtok_copy(line, " ");
	char* directive_value = strtok_copy(NULL, " ");

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
	char* cur_char = NULL;
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

	/* Look for the label indentifier (:). if we have found it, it means this is a label.
		Note that we use strtok_copy() to get the label name because strtok() changes the string it's working on.
	*/
	label_name = strtok_copy(line, ':');

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

	/* Allocate memory for the new label */
	new_label.label_name = (char*)malloc(strlen(label_name) + 1);
	new_label.label_address = malloc(sizeof(int));
	new_label.label_type = malloc(sizeof(char));

	if (new_label.label_name == NULL || new_label.label_address == NULL || new_label.label_type == NULL) {
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
