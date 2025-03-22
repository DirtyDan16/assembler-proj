#include "second_scan.h"

enum read_line_type state_of_read_line;

void compile(FILE* start_of_assembly_file_pointer,char* input_file_name) {
	FILE* input_file_pointer = start_of_assembly_file_pointer; /* Have a tracker of which line we are corrently reading from. */
	char line[GEN_LENGTH_OF_STRINGS]; /* This line stores each time a line from the asm file. (fgets() puts the info in it) */

	printf("\n\tcompiling file.\n");
	/* As long as there are more lines to read from, read the next line*/
	while (fgets(line, GEN_LENGTH_OF_STRINGS, input_file_pointer) != NULL) {
		/* Remove newline if it's there*/
		line[strcspn(line, "\n")] = '\0';
		go_over_read_line(line);
	}

	create_output_files(input_file_name);
}

/*
This function gets a recived line that comes from an ASM file and determines which type of line it is.
*/
static void go_over_read_line(char* chosen_line) {
	/*char* machine_code_representation_of_this_line;*/
	
	if (is_empty(chosen_line) || is_comment(chosen_line) ) return;


	switch (state_of_read_line)
	{
	case COMMAND:
		/* code */
		break;
	case DIRECTIVE:
		/* code */
		break;
	case LABEL:
		/* code */
		break;
	case EMPTY:
		
		break;	
	default:
		break;
	}
	/*if ()*/
	
	
	
/*	machine_code_representation_of_this_line = compile_asm_command_to_machine_code(chosen_line);*/
	
/*	printf("\n\t%s\n",machine_code_representation_of_this_line);*/
	/*printf("\n\t%d\n",compile_asm_command_to_machine_code(chosen_line));*/
}




