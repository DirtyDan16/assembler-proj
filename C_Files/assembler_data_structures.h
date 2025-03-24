#ifndef assembler_data_structures
#define assembler_data_structures

#define GEN_STORAGE_SIZE 100 /* The default size of the instruction and data tables. */

/*------------------------------------------------------------------------------------*/
typedef struct macro {
	char* macro_name;
	char* macro_content;
} macro;

/* Define the struct representing a macro node*/
typedef struct macro_node {
	macro val;
	struct macro_node* next;
} macro_node;

/* create a struct that hold pointers to specific needed macro_nodes.*/
typedef struct key_macro_nodes {
	macro_node* head_of_macro_storage;
	macro_node* cur_macro_node;
	macro_node* last_macro_node;
} key_macro_nodes;

/*------------------------------------------------------------------------------------*/
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

/* create a struct that hold pointers to specific needed label_nodes.*/
typedef struct key_label_nodes {
	label_node* head_of_label_storage;
	label_node* cur_label_node;
	label_node* last_label_node;
} key_label_nodes;

/*------------------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------------*/
/*A special struct that aggregates pointers to key nodes/arrays of data structures.
   This struct is used to easily access and manage the head, current, and last nodes of all data structures, or a certain index of an array */
typedef struct key_data_structures {
	key_macro_nodes* macro_nodes;
	key_label_nodes* label_nodes;
	mila instruction_table[GEN_STORAGE_SIZE];/* The instruction table that will be used to store the instructions of the assembly file. */
	mila data_table[GEN_STORAGE_SIZE];/* The data table that will be used to store the data of the assembly file. */
} key_data_structures;

/*------------------------------------------------------------------------------------*/



/* Define the struct representing an ASM command sentence.*/
typedef struct command_sentence {
	char* command_name;
	int num_of_arguments;
	int index_of_command;
	char* first_argument;
	char* second_argument;
} command_sentence;



#endif
