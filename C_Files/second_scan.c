#include "second_scan.h"

static int current_line_number = 1; /* Tracks the current line number for error reporting */

void second_scan(FILE* start_of_assembly_file_pointer,key_resources* k_resources) {
	FILE* input_file_pointer = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\t Going through second scan of the file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line,k_resources);
		current_line_number++; /* Increment the line number*/
	}

}

static void go_over_read_line(char* chosen_line,key_resources* k_resources) {
	char* directive_type = NULL; /* This will hold the type of directive we are currently looking at. (if we're dealing with directives)*/

	/* If the line is empty or a comment, skip it*/
	if (is_empty(chosen_line) || is_comment(chosen_line)) return;

	chosen_line = look_for_first_non_whitespace_char(chosen_line); /* This will hold the line without indentation. Will be used to get the raw value of the line so we can compare correctly with certain keywords. */
	chosen_line = skip_label_name(chosen_line);

	if (is_directive(chosen_line)) {
		directive_type	= strtok(strdup(chosen_line),"");
		if (strcmp(directive_type, ".entry") == 0) {
			handle_entry_directive(chosen_line,k_resources);
		} else {
			return;
		}
		free(directive_type);
	};

}

void handle_entry_directive(char* entry_sentence,key_resources* k_resources) {
	char* entry_value = NULL; /* This will hold the value of the entry directive. */
	
	entry_value = strtok(entry_sentence, " ");
	

	entry_value = trim_whitespace(entry_value);

	if (entry_value == NULL) {
		fprintf(stderr, "Entry directive is missing a value. \n LINE: %d\n", current_line_number);
		return;
	}
	look_for_label_in_table(entry_value,k_resources); /* This will look for the label in the label table. */

}

void look_for_label_in_table(char* label_name,key_resources* k_resources) {
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

	ptr = look_for_first_non_whitespace_char(ptr);

	return ptr;
}

