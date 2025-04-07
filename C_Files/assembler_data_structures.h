#ifndef assembler_data_structures
#define assembler_data_structures

#define GEN_STORAGE_SIZE 500 /* The default size of the instruction and data tables. */

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
	char* label_type;
	bool is_entry; /* A boolean that indicates if the label is an entry or not. */
} label;

/* Define the struct representing a label node*/
typedef struct label_node {
	label val;
	struct label_node* next;
} label_node;

/* create a struct that hold pointers to specific needed label_nodes.*/
typedef struct key_label_nodes {
	label_node* head_of_label_storage;
	label_node* last_label_node;
} key_label_nodes;

/*------------------------------------------------------------------------------------*/

/* A struct defining extern labels that we find AS AN ARGUMENT, not when they are declared.*/
typedef struct extern_label {
	char* label_name;
	int label_address; /* The address this label ~appears~ as an argument*/
} extern_label;

/* A struct defining extern label nodes. */
typedef struct extern_label_node {
	extern_label val;
	struct extern_label_node* next;
} extern_label_node;

/* A struct defining extern label nodes. */
typedef struct key_extern_label_nodes {
	extern_label_node* head_of_extern_label_storage;
	extern_label_node* last_extern_label_node;
} key_extern_label_nodes;
/*-----------------------------------------------------------------------------------*/

/* A struct that holds the code of an instruction, plus its IC and length. */
typedef struct instruction {
	mila code_of_command; /* The code of the command. */
	mila code_of_first_argument; /* The code of the first argument (for non registers). */
	mila code_of_second_argument; /* The code of the second argument. (for non registers)*/
	int IC; /* The instruction counter. */
	int L; /* The length of the instruction in Milas*/
} instruction;



/*------------------------------------------------------------------------------------*/
/*A special struct that aggregates pointers to key nodes/arrays of data structures.
	This also holds special information, such as abou0 the instruction and data tables.
   This struct is used to easily access and manage the head, current, and last nodes of all data structures, or a certain index of an array */
typedef struct key_resources {
	key_macro_nodes* macro_nodes;
	key_label_nodes* label_nodes;
	key_extern_label_nodes* extern_label_nodes;
	instruction instruction_table[GEN_STORAGE_SIZE];/* The instruction table that will be used to store the instructions of the assembly file. */
	int index_of_instruction_table; /* The index of the first free space of the instruction table. starts at 0, and grows by 1 each time an instruction is added. */
	mila data_table[GEN_STORAGE_SIZE];/* The data table that will be used to store the data of the assembly file. */



	int ICF; /*The final number of IC*/
	int DCF; /*The final number of DC.*/

	bool is_there_any_entry; /* A boolean that indicates if there is any entry in the label table. If there is, we build the entries file. */
	bool is_there_any_externs; /* A boolean that indicates if there is any extern in the label table. If there is, we build the externals file. */
} key_resources;

/*------------------------------------------------------------------------------------*/

/* A struct that holds useful info about a specific command sentence.*/
typedef struct instruction_sentence {
	char* command_name; /* The name of the command. */
	int num_of_arguments; /* The number of arguments the command has. */
	int index_of_command; /* The index of the command in the asmCommands array */
	char* first_argument; 
	char* second_argument;
} instruction_sentence;



#endif
