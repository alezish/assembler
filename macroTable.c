#include "macroTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "common.h"

/* create macro node without data inside*/
MacroNode* create_macro_node(char* macro_name) {
    MacroNode* new_macro = (MacroNode*)handle_malloc(sizeof(MacroNode));
    new_macro->first=NULL;
    new_macro->next_macro=NULL;
    strcpy(new_macro->name, macro_name);
    return new_macro;
}

/* add line to the start of a macro*/
void add_macro_line(MacroNode* macro, char* line) {
    MacroLine* current = macro->first;
    MacroLine* new_line = (MacroLine*) handle_malloc(sizeof(MacroLine));
    strcpy(new_line->line, line);
    new_line->next = current;
    macro->first = new_line;
}

/* add new macro to the table*/
void add_macro_to_table(MacroNode* new_macro, MacroTable* table) {
    MacroNode * current = table->firstMacro;
    new_macro->next_macro=current;
    table->firstMacro=new_macro;
}

/* check if the line contains call to macro*/
int is_call_macro(char* line, MacroTable* table) {
    char * first_word = get_next_word(line, FALSE);
    MacroNode* current_node = table->firstMacro;
    while (current_node) {
        if (strcmp(current_node->name, first_word) == 0) {
            handle_free(first_word);
            return TRUE;
        }
        current_node = current_node->next_macro;
    }
    handle_free(first_word);
    return FALSE;
}

/* print all the lines of the macro to a file*/
void print_macro_to_file(FILE* file, char* macro_name, MacroTable* table) {
    MacroNode* current_node = table->firstMacro;
    MacroLine * curren_line;
    while (current_node) {
        if (strcmp(current_node->name, macro_name) == 0) {
            curren_line = current_node->first;
            while (curren_line) {
                fprintf(file,"%s", curren_line->line);
                curren_line = curren_line->next;
            }
            break;
        }
        current_node = current_node->next_macro;
    }   
}

/* get macro node and free all it's lines memory*/
void free_macro_node_line(MacroNode* node) {
    MacroLine* current, *next;
    current = node->first;
    while (current!=NULL) {
        next = current->next;
        handle_free(current);
        current = next;
    }
}

/* get macro table and free all it's memory*/
void free_macros_table(MacroTable table) {
    MacroNode* next;
    MacroNode* current = table.firstMacro;
    while (current != NULL) {
        next = current->next_macro;
        free_macro_node_line(current);
        handle_free(current);
        current=next;
    }
}