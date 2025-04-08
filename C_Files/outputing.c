#include "outputing.h"

void create_output_files(char* input_file_name,key_resources* key_resources) {
	FILE* object_file = NULL,* entries_file = NULL,* externals_file = NULL;

	/* The variables which will hold all of the output file names.*/
	char* object_file_name = malloc(strlen(DIRECTORY_TO_OUTPUT_FILES)+strlen(input_file_name)+strlen(".ob")+1);
	char* entries_file_name = malloc(strlen(DIRECTORY_TO_OUTPUT_FILES)+strlen(input_file_name)+strlen(".ent")+1);
	char* externals_file_name = malloc(strlen(DIRECTORY_TO_OUTPUT_FILES)+strlen(input_file_name)+strlen(".ext")+1);
	
	if (object_file_name == NULL || entries_file_name == NULL || externals_file_name == NULL) {
		fprintf(stderr, "The program did not manage to get enough storage to store the output file names.");
		exit(1);
	}

	/* Create the output files, with their file path and suffix*/

	strcpy(object_file_name,DIRECTORY_TO_OUTPUT_FILES);
	strcat(object_file_name,input_file_name); 
	strcat(object_file_name,".ob"); /* Name the output object file to the name of the assembly file we read from, and add the '.ob' suffix*/

	/* Create the object file (the file which will have the machine code) and make it writeable.*/
	object_file = fopen(object_file_name,"w+");
	if (object_file == NULL) { 
		fprintf(stderr, "The program could not create the object file. \n FILE NAME: %s\n", input_file_name);
		exit(1);
	} else {
		write_into_object_file(object_file,key_resources); /* Write the object file. */
	}

	if (key_resources->is_there_any_entry) {
		strcpy(entries_file_name,DIRECTORY_TO_OUTPUT_FILES);
		strcat(entries_file_name,input_file_name); /* Name the output object file to the name of the assembly file we read from, and add the '.ent' suffix*/
		strcat(entries_file_name,".ent");
		/* Create the entries file (the file which will have the entries) and make it writeable.*/
		entries_file = fopen(entries_file_name,"w+");
		if (entries_file == NULL) {
			fprintf(stderr, "The program could not create the entries file. \n FILE NAME: %s\n", input_file_name);
			exit(1);
		} else {
			write_into_entries_file(entries_file,key_resources); /* Write the entries file. */
		}	
	}

	if (key_resources->is_there_any_externs) {
		strcpy(externals_file_name,DIRECTORY_TO_OUTPUT_FILES);
		strcat(externals_file_name,input_file_name); /* Name the output object file to the name of the assembly file we read from, and add the '.ext' suffix*/
		strcat(externals_file_name,".ext");
		 /* Create the externals file (the file which will have the externals) and make it writeable.*/
		externals_file = fopen(externals_file_name,"w+");
		if (externals_file == NULL) {
			fprintf(stderr, "The program could not create the externals file. \n FILE NAME: %s\n", input_file_name);
			exit(1);
		} else {
			write_into_externals_file(externals_file,key_resources); /* Write the externals file. */
		}
	}
	
}

void write_into_object_file(FILE* object_file,key_resources* key_resources) {
	int i = 0;

	instruction *table = key_resources->instruction_table; /* The instruction table that's used to store the instructions of the assembly file. */

	/* Write the length of IC and DC to the object file */
	fprintf(object_file,"%d %d\n",key_resources->ICF - START_OF_IC,key_resources->DCF);

	/* Write the object file. firstly - all instruction Milas*/
	/* We go over every value that isn't empty in the table*/
	while (table[i].code_of_command.v != 0) {
		int length_of_instruction = table[i].L; /* The length of the instruction in Milas*/

		/* Print the machine code of the instruction, and the given additional info-Milas attached to it if they aren't worth to 0*/

		print_masked_hex(object_file,table[i].IC,table[i].code_of_command.v);
		if (length_of_instruction == 2) {
			/* 2 possibilities of this amount of length:*/
			/*WHEN THERE'S 2 ARGUMENTS:
			 	Check from which argument number the additional info-Mila comes from (that makes the length be bigger than 1).
			 	situations such as registers as the argument makes the additional info-Mila be redundant. (and so not increase the L counter.)
			  WHEN THERE'S ONLY 1 ARGUMENT:
			  	in such case the second argument will ofc be worth to 0.
			*/
			if (table[i].code_of_first_argument.v != 0) {
				print_masked_hex(object_file,table[i].IC + 1,table[i].code_of_first_argument.v);
			} else {
				print_masked_hex(object_file,table[i].IC + 1,table[i].code_of_second_argument.v);
			}
		/* when the length is 3, both arguments have an additional info-Mila attached to them. Print them both and account for the IC.*/
		} else if (length_of_instruction == 3) {
			print_masked_hex(object_file,table[i].IC + 1,table[i].code_of_first_argument.v);
			print_masked_hex(object_file,table[i].IC + 2,table[i].code_of_second_argument.v);
		}
		
		i++;
		
	}

	/* Now, write the data table. */
	for (i = 0; i < key_resources->DCF; i++) {
		print_masked_hex(object_file,key_resources->ICF + i,key_resources->data_table[i].v);
	}
}

void print_masked_hex(FILE* file,int address,int value) {
	unsigned int masked_value = (unsigned int)value;
	masked_value &= 0xFFFFFF;

	fprintf(file,"%07d %06x\n",address,masked_value);
}

void write_into_entries_file(FILE* entries_file,key_resources* key_resources) {
	label_node* pos = key_resources->label_nodes->head_of_label_storage; /* This will hold the label node. */

	/* Write the entries file. */
	while (pos != NULL) {
		if (pos->val.is_entry) {
			fprintf(entries_file,"%s %07d\n",pos->val.label_name,pos->val.label_address);
		}
		pos = pos->next;
	}
}

void write_into_externals_file(FILE* externals_file,key_resources* key_resources) {
	extern_label_node* extern_refrence_pos = key_resources->extern_label_nodes->head_of_extern_label_storage; /* this pos node keeps track of all the extern labels refrences as arguments.*/

	/* Write all refrences to those externs info*/
	while (extern_refrence_pos != NULL) {
		fprintf(externals_file,"%s %07d\n",extern_refrence_pos->val.label_name,extern_refrence_pos->val.label_address);

		extern_refrence_pos = extern_refrence_pos->next;
	}
}

FILE* create_after_macro_file(char* assembly_file_name) {
	FILE* am_file;
	char *am_file_path = malloc(strlen(DIRECTORY_TO_OUTPUT_FILES)+strlen(assembly_file_name)+strlen(".am")+1);
	
	/* Name the output object file to the name of the assembly file we read from, and add the '.am' suffix*/
	strcpy(am_file_path,DIRECTORY_TO_OUTPUT_FILES);
	strcat(am_file_path,assembly_file_name);
	strcat(am_file_path,".am");

	am_file = fopen(am_file_path,"w+"); /* Create the after-macro file (the file which has the asm code after translating macros.) and make it writeable.*/
		
	return am_file;
}

void remove_am_file(char* am_file_name) {
	char *am_file_path = malloc(strlen(DIRECTORY_TO_OUTPUT_FILES)+strlen(am_file_name)+strlen(".am")+1);
	
	/* Name the output object file to the name of the assembly file we read from, and add the '.am' suffix*/
	strcpy(am_file_path,DIRECTORY_TO_OUTPUT_FILES);
	strcat(am_file_path,am_file_name);
	strcat(am_file_path,".am");

	remove(am_file_path); /* Remove the after-macro file (the file which has the asm code after translating macros.) */
}