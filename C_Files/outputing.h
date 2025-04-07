#include "general.h"
#include "compiling_logic.h"
#include "assembler_data_structures.h"


/**
 * This function creates all the necessary output files after both scans have been completed.
 * 
 * It generates:
 * - '.ob' file: for object code (machine code and data).
 * - '.ent' file: for labels marked with '.entry'.
 * - '.ext' file: for labels marked with '.extern' that were used.
 * 
 * It uses the key_resources struct to gather all necessary data for writing into the files.
 * 
 * Note that it won't create the .ent and .ext files if not needs be to create them (if they'll be empty)
 *
 * @param input_file_name The original input file name (without extension), used to name the output files.
 * @param key_resources Reference to the global struct that contains compiled machine code and label information.
 */
void create_output_files(char* input_file_name, key_resources* key_resources);

/**
 * Writes compiled machine code into the '.ob' file, including instruction and data memory.
 * 
 * The first line are numbers representing the length of the Instruction Milas, and the Data Milas, respectfully.
 * 
 * Format is masked hexadecimal for both the instruction counter (IC) and the value.
 * 
 * @param object_file The file pointer to the '.ob' file.
 * @param key_resources Reference to the struct holding all instruction and data lines to output.
 */
void write_into_object_file(FILE* object_file, key_resources* key_resources);

/**
 * Helper function that prints an address and value in a masked hexadecimal format.
 * restrcits the printed values to be 6 digits long.
 * 
 * Used specifically for printing lines into the '.ob' file.
 * 
 * @param file The file pointer to write into.
 * @param address The current address (the machine code line placement)
 * @param value The value to be printed in hex format.
 */
void print_masked_hex(FILE* file, int address, int value);

/**
 * Writes all '.entry' labels into the '.ent' file, along with their resolved memory addresses.
 * 
 * Only writes if there are labels marked as '.entry' in the key_resources struct.
 * 
 * @param entries_file File pointer to the '.ent' file.
 * @param key_resources Struct that contains a list of all '.entry' labels and their final addresses.
 */
void write_into_entries_file(FILE* entries_file, key_resources* key_resources);

/**
 * Writes all '.extern' labels used in the file into the '.ext' file.
 * 
 * For each external label used, the file includes the name and the memory address where it was used.
 * 
 * @param externals_file File pointer to the '.ext' file.
 * @param key_resources Struct containing a list of all '.extern' labels and their usages.
 */
void write_into_externals_file(FILE* externals_file, key_resources* key_resources);

/**
 * Creates a '.am' file that is the result of macro expansion (After Macro).
 * 
 * This file is passed to the first and second scan, and contains the fully expanded source code
 * without any macro definitions or calls.
 * 
 * @param assembly_file_name The original '.as' file name (without extension).
 * @return A file pointer to the created '.am' file, ready to be read by the initial and second scan.
 */
FILE* create_after_macro_file(char* assembly_file_name);

/**
 * Removes the '.am' file after it is no longer needed (once both scans are completed/ or if the file gets errors in the macro phase).
 * 
 * Helps keep the working directory clean after successful assembly.
 * 
 * @param assembly_file_name The original '.as' file name (without extension) — used to locate the corresponding '.am' file.
 */
void remove_am_file(char* assembly_file_name);