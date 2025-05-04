#include "common.h"
#include "memory.h"
#include <string.h> 
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

/* get the next word from line, return it,  and update the line pointer  to the next word*/
char* read_next_word(char ** read_ptr, int skip_spaces) {
    char* word, *first_letter;
    int len;
    len = 0;
    if (skip_spaces) {
        /* skip the spaces and commas in the start of the word*/
        while (**read_ptr == ' ' || **read_ptr == ',' || **read_ptr == '\t') {
            (*read_ptr)++;
        }    
    }
    first_letter = *read_ptr;
    /* continue to the end of the word*/
    while (**read_ptr != ' ' && **read_ptr != '\t' && **read_ptr != '\0' && **read_ptr != '\n' && **read_ptr != ',') {
        len++;
        (*read_ptr)++;
    }
    word = (char*)handle_malloc((len + 1)*sizeof(char));
    strncpy(word, first_letter, len);
    word[len] = '\0';
    return word;
}

/* get the next word from line without update the line pointer to the next word*/
char* get_next_word(char * read_ptr, int skip_spaces) {
    char* word, *first_letter;
    int len;
    len = 0;
    if (skip_spaces) {
    /* skip the spaces and commas in the start of the word*/
        while (*read_ptr == ' ' || *read_ptr == '\t' || *read_ptr == '\n') {
            read_ptr++;
        }    
    }
    first_letter = read_ptr;
    while (*read_ptr != ' ' && *read_ptr != '\0' && *read_ptr != '\n') {
    /* continue to the end of the word*/
        len++;
        read_ptr++;
    }
    word = (char*)handle_malloc(sizeof(char) * (len + 1));
    strncpy(word, first_letter, len);
    word[len] = '\0';
    return word;
}

/*convert string that represent number with + or - to an integer*/
int str_to_int(char* word) {
    int i,is_minus, len, sum=0;
    is_minus = (*word=='-');
    if (*word =='+' || *word=='-') {
        word++;
    }
    len = strlen(word);
    for (i=0;i<len;i++) {
        sum += (pow(10,len-i-1))*(*word-'0');
        word++;
    }
    if (is_minus) {
        sum = 0-sum;
    }
    return sum;
}

/* check if line is empty or comment*/
int is_empty_or_comment(char* line) {
    if(*line ==';') { /* it is comment line that srarted in ';' */
        return 1;
    }
    while(*line!='\0' && *line !='\n') {
        if (*line!=' ' && *line!='\t') {
            return 0;
        }
        line++;
    }
    return 1;
}

/* check if word represent number with + or -*/
int is_valid_num(char* word) {
    if (*word== '+' || *word=='-') {
        word++;
    }
    while (*word!='\0') {
        if (!isdigit(*word)) {
            return 0;
        }
        word++;
    }
    return 1;
}

/* return the register index that appear in word return -1 if no register*/
char get_register(char* word) {
    if (*word!='r') {
        return -1;
    }
    word++;
    if (isdigit(*word) && (*word)-'0'>=0 && (*word)-'0'<8) {
        return (*word)-'0';
    }
    return -1;
}

/* print errors to console with params, and remember it for not create binary file */
void print_error(const char *format, ...) {
    va_list args;
    printf("Found Error, File: %s, Line %d: ",filename, line_counter);
    va_start(args, format);
    vprintf(format, args);
    vprintf("\n", args);
    va_end(args);
    has_errors=TRUE;
}

/* log data with params to console, */
void log_data(const char *format, ...) {
    va_list args;
    printf("File: %s Line %d: ",filename, line_counter);
    va_start(args, format);
    vprintf(format, args);
    vprintf("\n", args);
    va_end(args);    
}