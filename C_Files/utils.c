#include "general.h"



char* concatenate_strings(char* str1,char* str2) {
    /* Calculate the length of the resulting string */
    size_t length = strlen(str1) + strlen(str2) + 1;  /* +1 for the null terminator */
    
    /* Allocate memory for the concatenated string */
    char* result = (char*)malloc(length * sizeof(char));
    
    /* Check if memory allocation was successful */
    if (result == NULL) {
        return NULL;  /* Return NULL if memory allocation failed */
    }
    
    /* Concatenate the two strings */
    strcpy(result, str1);
    strcat(result, str2);
    
    return result;  /* Return the concatenated string */
}

/* Function to replace the first occurrence of a character with '\0' */
void replace_char_with_null(char *str, char target) {
    /* Find the first occurrence of the target character */
    char *pos = strchr(str, target);
    
    if (pos != NULL) {
        /* Replace it with '\0' */
        *pos = '\0';
    }
}


char* strtok_copy(char* passed_in_str , char* token) {
    char* copy_str = malloc((strlen(passed_in_str) + 1));

    if (copy_str == NULL) {
        return NULL;  /* Return NULL if memory allocation failed */
    }

    strcpy(copy_str, passed_in_str);

    /* get the substring from the duplicate str using strtok (which modifies the duplicate), and return it */
    strtok(copy_str, token);

    return copy_str;
}

char* look_for_first_non_whitespace_char(char* line) {
    char* first_non_whitespace_char = NULL;

    /* Check if the input line is NULL */
    if (line == NULL) {
        return NULL;
    }

    /* Find the first non-whitespace character */
    while (*line != '\0') {
        if (!isspace(*line)) {
            first_non_whitespace_char = line;
            break;
        }
        line++; /* Move to the next character */
    }

    return first_non_whitespace_char;
}

char* trim_whitespace(char* str) {
    char* end;

    str = look_for_first_non_whitespace_char(str); /* Move the pointer to the first non-whitespace character */

    /* If the string is all spaces, return an empty string */
    if (*str == 0) {
        return str;
    }

    /* Get rid of whitespace after the string*/
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    return str;
}