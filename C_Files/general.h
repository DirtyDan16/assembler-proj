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
#define T 1
#define false 0
#define F 0

typedef struct mila {
    int v : 24; /* 24 bits for the value */
} mila;

extern bool does_file_have_errors; /* This will be used to determine if the file has errors or not. If it does, we won't create the object file. */

/* ----------> Method names that are needed to be known on a global scope. Those are utility functions that come from utils.c  <---------------------------*/


/**
 * Function that concatenates two strings without modifying the passed in strings.
 * @param 1st2nd the two strings to concatenate
 * @return the concatenation of the 2 strings
 */
extern char* concatenate_strings(char* str1,char* str2);

/**
 * replaces in a string a certain target character with the null terminator
 * the target is specified as a parameter
 * 
 * @param 1st the string that needs to be modified
 * @param 2nd the character to look for to replace
 */
extern void replace_char_with_null(char *str, char target);

/**
 * This function behaves like strtok(), except it doesn't modify the passed in string.
 * Can't call subsequent calls properly for the same string.
 *
 * The purpose is to get create and get a substring from a given string
 * 
 * @param 1st the string to get a substring from, without modifying it!
 * @param 2nd the token that splits the string
 * 
 * @return the wanted substring.
 */
extern char* strtok_copy(char* passed_in_str, char* token);

/**
 * This function ignore all leading whitespace to a string, and returns the trimed string
 * 
 * @param 1st the line to be trimmed in the front
 * @return a pointer to the first non-whitespace character.
 */
extern char* look_for_first_non_whitespace_char(char* line);

/**
 * This function ignores all whitespaces that surround a given string. 
 * This means that this function puts a null terminator to first non-whitespace char after the main bit of the string.
 * 
 * @param 1st the string to trim behind and in the front
 * @return a string that doesn't include any whitespace characters surrounding the main part of the string.
 */
extern char* trim_whitespace(char* str);

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#endif
