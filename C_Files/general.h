#ifndef general
#define general

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define GEN_LENGTH_OF_STRINGS 100 /* Default length of defining new strings.*/


#define DIRECTORY_TO_INPUT_FILES "Input_Files/"
#define DIRECTORY_TO_OUTPUT_FILES "Output_Files/"

/* Basic boolean stuff*/
typedef int bool;
#define true 1
#define false 0

typedef struct {
    int v : 24; /* 24 bits for the value */
} mila;

/* Method names that are needed to be known on a global scope. Those are utility functions that come from utils.c*/
extern char* concatenate_strings(char* str1,char* str2);

extern int advance_to_char_in_file(FILE *file, char target);

extern void replace_char_with_null(char *str, char target);

extern char* strtok_copy(char* passed_in_str, char* token);

extern char* look_for_first_non_whitespace_char(char* line);

extern int int_to_binary(int value);
#endif
