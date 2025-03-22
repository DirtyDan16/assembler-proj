#include "outputing.h"

/**
* This method copies the content of one file to another. 
* first var is the copied file, and the second is the file you want to copy to
*/
void copy_content_from_a_file_to_another(FILE* copied_file, FILE* file_to_copy_to) {
	int copied_char;
	/*Go over character by character, untill you've reache the EOF, and paste it unto the wanted file */
	while ((copied_char = fgetc(copied_file)) != EOF) {
		fputc(copied_char, file_to_copy_to);
	}
}


void create_output_files(char* input_file_name) {
	/*FILE* object_file;*/
	/*entries_file,externals_file;*/

	/* The variables which will hold all of the output file names.*/
	char object_file_name[GEN_LENGTH_OF_STRINGS];
	

	strcat(object_file_name,strcat(input_file_name,".ob")); /* Name the output object file to the name of the assembly file we read from, and add the '.ob' suffix*/

	fopen(object_file_name,"w"); /* Create the object file (the file which will have the machine code) and make it writeable.*/
	
}


FILE* create_after_macro_file(char* assembly_file_name) {
	FILE* am_file;
	char *am_file_path = malloc(strlen(DIRECTORY_TO_INPUT_FILES)+strlen(assembly_file_name)+strlen(".am"));
	
	/* Name the output object file to the name of the assembly file we read from, and add the '.am' suffix*/
	strcpy(am_file_path,DIRECTORY_TO_OUTPUT_FILES);
	strcat(am_file_path,assembly_file_name);
	strcat(am_file_path,".am");

	am_file = fopen(am_file_path,"w+"); /* Create the after-macro file (the file which has the asm code after translating macros.) and make it writeable.*/
		
	return am_file;
}



