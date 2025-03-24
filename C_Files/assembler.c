#include "assembler.h"

int main(int argc, char *argv[]) {
	FILE *assembly_file_pointer; /* Pointer for the input file */
	char* assembly_file_name;
	char* input_file_path;


	int i;/*for index loop*/
	
	/* Initialize the data structures that will hold pointers to valuable nodes in the data structures / arrays */
	key_data_structures* key_nodes = init_data_structures();

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

		am_file = pre_assembling(assembly_file_pointer, assembly_file_name,key_nodes->macro_nodes);
		initial_scan(am_file,key_nodes);
	}
	return 0;
}

key_data_structures* init_data_structures() {
	key_data_structures* key_nodes = (key_data_structures*)malloc(sizeof(key_data_structures));

	key_nodes->macro_nodes = (key_macro_nodes*)malloc(sizeof(key_macro_nodes));
	key_nodes->label_nodes = (key_label_nodes*)malloc(sizeof(key_label_nodes));
	
	if (key_nodes->macro_nodes == NULL || key_nodes->label_nodes == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	
	key_nodes->macro_nodes->head_of_macro_storage = NULL;
	key_nodes->macro_nodes->cur_macro_node = NULL;
	key_nodes->macro_nodes->last_macro_node = NULL;
	
	key_nodes->label_nodes->head_of_label_storage = NULL;
	key_nodes->label_nodes->cur_label_node = NULL;
	key_nodes->label_nodes->last_label_node = NULL;

	memset(&key_nodes->instruction_table, 0, sizeof(key_nodes->instruction_table));
    memset(&key_nodes->data_table, 0, sizeof(key_nodes->data_table));


	return key_nodes;
}
