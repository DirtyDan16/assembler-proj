#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h" 

/**
 * The function initializes for a given ASM file the key_resources struct, which is full of important info that needs to be accessed on a global scale in order to compile the file.
 * 
 * Is called only when a new ASM is inspected.
 * 
 * @return a ptr to the struct full of usefull information, initialized.
 */
key_resources* init_data_structures();

/**
 * The function frees and resets information that is available in the key_resources struct.
 * must be called in-between reading ASM files.
 * 
 * @param resources - the used up struct that's full of info that's needed to be accessed on a global scope.
 */
void free_data_structures(key_resources* resources);

/*from a different file. Documentation is available on their header file....*/

FILE* pre_assembling(FILE* start_of_assembly_file_pointer,char* input_file_name,key_macro_nodes* macro_nodes);
void initial_scan(FILE* start_of_am_file_pointer,key_resources* key_nodes);
void second_scan(FILE* start_of_assembly_file_pointer,key_resources* key_resources);
void create_output_files(char* input_file_name,key_resources* key_resources);


/**
 * Frees the dynamically allocated storage for macro nodes.
 */
void free_macro_storage(key_macro_nodes* resources);

/**
 * Frees the dynamically allocated storage for label nodes.
 */
void free_label_storage(key_label_nodes* resources);

/**
 * Frees the dynamically allocated storage for extern label nodes. 
 */
void free_extern_storage(key_extern_label_nodes* resources);
