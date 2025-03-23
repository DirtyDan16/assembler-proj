#ifndef assembler_data_structures
#define assembler_data_structures

#define GEN_STORAGE_SIZE 100 /* The default size of the instruction and data tables. */

typedef struct macro {
	char* macro_name;
	char* macro_content;
} macro;

/* Define the struct representing a macro node*/
typedef struct macro_node {
	macro val;
	struct macro_node* next;
} macro_node;

/* Define the struct representing a label*/
typedef struct label {
	char* label_name;
	int label_address;
	char label_type;
} label;

/* Define the struct representing a label node*/
typedef struct label_node {
	label val;
	struct label_node* next;
} label_node;

/* Define the struct representing an ASM command sentence.*/
typedef struct command_sentence {
	char* command_name;
	int num_of_arguments;
	int index_of_command;
	char* first_argument;
	char* second_argument;
} command_sentence;



extern mila instrcution_table[GEN_STORAGE_SIZE]; /* The instruction table that will be used to store the instructions of the assembly file. */
extern mila data_table[GEN_STORAGE_SIZE]; /* The data table that will be used to store the data of the assembly file. */
extern mila IC; /* The Instruction Counter. */
extern mila DC; /* The Data Counter. */

#endif
