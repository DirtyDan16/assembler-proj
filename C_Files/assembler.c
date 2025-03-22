#include "assembler.h"

int main(int argc, char *argv[]) {
	FILE *assembly_file_pointer; /* Pointer for the input file */
	char* assembly_file_name;
	char* input_file_path;


	int i;/*for index loop*/
	
	
	/* Let's start with checking if the input files are valid, and output error otherwise. */

	/* Check if the program received a file to process */
	if (argc == 1) {
		fprintf(stderr, "the program did not get an assembly file ");
		return 1; 
	}

	


	/* Check if the Assembly files are openable, and if yes, let's start compiling them one file at a time */
	for (i = 1; i < argc; i++) {
		FILE* am_file = NULL;

		input_file_path = concatenate_strings(DIRECTORY_TO_INPUT_FILES, argv[i]);
		
		printf("path: %s\n", input_file_path);
		
		if ((assembly_file_pointer = fopen(input_file_path, "r")) == NULL && (assembly_file_pointer = fopen(argv[i], "r")) == NULL) {
			fprintf(stderr, "the program did not get a file that can be opened!\n");
			continue;
		}

		assembly_file_name = strtok(argv[i], "."); /* Get the current assembly file name (without the ".as" suffix). Will be used to name the output files. */

		am_file = pre_assembling(assembly_file_pointer, assembly_file_name);
		initial_scan(am_file);
		compile(assembly_file_pointer, assembly_file_name);
	}
	return 0;
}
