#include "assembler.h"

/**
 * @publisher: Stav Gordeev
 * ID: 333241651
 * 
 * NOTE: ALL Function Documentation is found on their respective header files. (besides main)
 */

/**
 * The Main program.
 * Takes as arguments file names to be compiled. 
 * Goes over file by file, into the pre-assembling stage, the first-scan stage and the second-scan stage.
 * 
 * If no errors were detected in the process of gathering info about the file, the output files will be outputed accordingly.
 * 
 * returns 0 if it finished the excecution without exciting in the middle.
 */
int main(int argc, char *argv[]) {
	FILE *assembly_file_pointer; /* Pointer for the input file */
	char* assembly_file_name;
	char* input_file_path;


	int i;/*for index loop*/
	
	/* Initialize the data structures that will hold pointers to valuable nodes in the data structures / arrays for a given ASM file. */
	key_resources* key_resources = init_data_structures();

	/* Let's start with checking if the input files are valid, and output error otherwise. */

	/* Check if the program received a file to process */
	if (argc == 1) {
		fprintf(stderr, "the program did not get an assembly file ");
		return 1; 
	}

	


	/* Check if the Assembly files are openable, and if yes, let's start compiling them one file at a time */
	for (i = 1; i < argc; i++) {
		FILE* am_file = NULL;
	    key_resources = init_data_structures();	/* Initialize the data structures that will hold pointers to valuable nodes in the data structures / arrays for a given ASM file. */

		input_file_path = concatenate_strings(DIRECTORY_TO_INPUT_FILES, argv[i]);
		
		printf("\n\n~~~~~~~~~~~~~~~~ Path: %s ~~~~~~~~~~~~~~~~\n\n", input_file_path);
		
		if ((assembly_file_pointer = fopen(input_file_path, "r")) == NULL && (assembly_file_pointer = fopen(argv[i], "r")) == NULL) {
			fprintf(stderr, "the program did not get a file that can be opened!\n");
			continue;
		}

		assembly_file_name = strtok(argv[i], "."); /* Get the current assembly file name (without the ".as" suffix). Will be used to name the output files. */

		/* Check if the file can be expanded into its after macro form. If returned null, it means the file has errors in its macros, so we'll skip it 
		and advance to the next inputted ASM file. We'll also free all of the memory allocated for our data that we want to access in all sorts of places*/
		if ((am_file = pre_assembling(assembly_file_pointer, assembly_file_name,key_resources->macro_nodes)) == NULL) {
			free_data_structures(key_resources);
			continue;
		}

		initial_scan(am_file,key_resources);
		second_scan(am_file,key_resources);

		/* Create the output files */
		if (does_file_have_errors) {
			fprintf(stderr, "the file %s has errors, and will not be compiled.\n", assembly_file_name);
		} else {
			create_output_files(assembly_file_name,key_resources); /* Create the output files */
		}

		/* Free the data structures */
		free_data_structures(key_resources);
		fclose(am_file); /* Close the after macro file */

	}


	return 0;
}

key_resources* init_data_structures() {
	key_resources* resources = (key_resources*)malloc(sizeof(key_resources));

	resources->macro_nodes = (key_macro_nodes*)malloc(sizeof(key_macro_nodes));
	resources->label_nodes = (key_label_nodes*)malloc(sizeof(key_label_nodes));
	resources->extern_label_nodes = (key_extern_label_nodes*)malloc(sizeof(key_extern_label_nodes));
	
	if (resources->macro_nodes == NULL || resources->label_nodes == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	
	/* Macros*/
	resources->macro_nodes->head_of_macro_storage = NULL;
	resources->macro_nodes->cur_macro_node = NULL;
	resources->macro_nodes->last_macro_node = NULL;
	
	/* Labels*/
	resources->label_nodes->head_of_label_storage = NULL;
	resources->label_nodes->last_label_node = NULL;

	/* Extern Labels*/
	resources->extern_label_nodes->head_of_extern_label_storage = NULL;
	resources->extern_label_nodes->last_extern_label_node = NULL;

	/* Instruction Table*/
	memset(&resources->instruction_table, 0, sizeof(resources->instruction_table));
	resources->index_of_instruction_table = 0; /* The index of the first free space of the instruction table. starts at 0, and grows by 1 each time an instruction is added. */

	/* Data Table*/
    memset(&resources->data_table, 0, sizeof(resources->data_table));

	resources->ICF = START_OF_IC; /* The instruction counter. This is the address of the next instruction to be added. */
	resources->DCF = 0; /* The data counter. This is the address of the next data to be added. */
	resources->is_there_any_entry = false; /* This will be set to true if there is at least one entry directive in the file. */
	resources->is_there_any_externs = false; /* This will be set to true if there is at least one extern directive in the file. */


	return resources;
}

void free_data_structures(key_resources* resources) {
	/* Free the macro nodes */
	free_macro_storage(resources->macro_nodes);
	free(resources->macro_nodes);

	/* Free the label nodes */
	free_label_storage(resources->label_nodes);
	free(resources->label_nodes);

	/* Free the extern labels nodes*/
	free_extern_storage(resources->extern_label_nodes);
	free(resources->extern_label_nodes);

	free(resources); /* Free the key nodes struct itself */
}

void free_label_storage(key_label_nodes* resources) {
	label_node* pos = resources->head_of_label_storage, *next;

	while (pos != NULL) {
		next = pos->next;

		free(pos->val.label_name);
		free(pos);

		pos = next;
	}

	resources->head_of_label_storage = NULL;
	resources->last_label_node = NULL;
}

void free_extern_storage(key_extern_label_nodes* resources) {
	extern_label_node* pos = resources->head_of_extern_label_storage, *next;

	while (pos != NULL) {
		next = pos->next;

		free(pos->val.label_name);
		free(pos);

		pos = next;
	}

	resources->head_of_extern_label_storage = NULL;
	resources->last_extern_label_node = NULL;
}

