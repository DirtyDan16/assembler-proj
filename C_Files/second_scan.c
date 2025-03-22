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
	printf("\n\t%d\n",compile_asm_command_to_machine_code(chosen_line));
}

int compile_asm_command_to_machine_code(char* read_command) {
	int machine_code_rep_of_command = 0,i;
	char* read_command_name = strtok(read_command,"("); /* Get the command name from the command line. We know that we should look for a '(' which indicated the end of the command name. if no '(' present, return null */
	bool has_command_been_found = false;
	for (i = 0; i< NUM_OF_ASM_COMMANDS; i++) {
		/* Check if the read command name is an actual command name that's part of our imaginary assembly lang.
		   If it is, get the machine code rep of the command via bit shifting to get to the wanted places of the Mila.
		 */
		
		if (strcmp(asmCommands[i].name,read_command_name) == 0) {
			machine_code_rep_of_command += asmCommands[i].opcode << INDEX_OF_OPCODE_BYTE;
			machine_code_rep_of_command += asmCommands[i].opcode << INDEX_OF_FUNCT_BYTE;
			has_command_been_found=true;
			break;
		}
	}
	/* returning 0 if the command is invalid */
	if (!has_command_been_found) {
		fprintf(stderr, "The program got an invalid assembly instruction. representing 0.");
		return 0; 
	}
	
	
	
	/*temp*/
	return machine_code_rep_of_command;
}


