#include "second_scan.h"

static int current_line_number = 1; /* Tracks the current line number for error reporting */

void second_scan(FILE* start_of_assembly_file_pointer,key_resources* k_resources) {
	FILE* input_file_pointer = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\t Going through second scan of the file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		go_over_read_line(line);
		current_line_number++; /* Increment the line number*/
	}

}

static void go_over_read_line(char* chosen_line) {
	
}



