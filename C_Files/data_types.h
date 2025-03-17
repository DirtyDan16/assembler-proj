#ifndef data_types
#define data_types

typedef struct {
	char* macro_name;
	char* macro_content;
} macro;



struct macro_node {
	macro val;
	struct macro_node* next;
};

typedef struct macro_node macro_node;



#endif
