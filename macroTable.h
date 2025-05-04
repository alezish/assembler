#ifndef MACROTABLE_H
#define MACROTABLE_H 

#include <stdio.h>
#include "common.h"
/* define the table of all macro*/
/* define single command inside macro*/
typedef struct MacroLine
{
    char line[LINE_MAX_LEN];
    struct MacroLine* next;
}MacroLine;

/* define macro with all lines inside*/
typedef struct MacroNode
{
    char name[LINE_MAX_LEN];
    struct MacroLine* first;
    struct MacroNode * next_macro;
} MacroNode;

typedef struct MacroTable 
{
    MacroNode * firstMacro;

} MacroTable;

/* create macro without data inside*/
MacroNode* create_macro_node(char* macro_name);

/* add line to a macro*/
void add_macro_line(MacroNode* macro, char* line);

/* add new macro to the table*/
void add_macro_to_table(MacroNode* new_macro, MacroTable* table);

/* check if the line contains call to macro*/
int is_call_macro(char* line, MacroTable* table);

/* print all the lines of the macro to a file*/
void print_macro_to_file(FILE* file, char* macro_name, MacroTable* table);

void free_macros_table(MacroTable table);

#endif