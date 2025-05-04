#include "symbols.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "memory.h"
#include "common.h"
#include <string.h>

/* check if symbol exist in table*/
int has_symbol(symbolTable* table, char* name) {
    symbolNode* current;
    current = table->first_node;
    while (current != NULL) {
        if (strcmp(name, current->name)==0) {
            return TRUE;
        }
        current = current->next;
    }
    return FALSE;
}


/* get the value of a symbol in the table*/
unsigned int get_label_value(symbolTable* table, char* name) {
    symbolNode* current = table->first_node;
    while (current != NULL) {
        if (strcmp(name, current->name)==0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;
}

/* get the whole data of a symbol*/
symbolNode* get_symbol(symbolTable* table, char* name) {
    symbolNode* current = table->first_node;
    while (current != NULL) {
        if (strcmp(name, current->name)==0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
} 

/*insert new symbol to the table, get it's name property and value, and create a symbol*/
void add_symbol(symbolTable* table, char* name, char* property, unsigned int value) {
    symbolNode *tmp, *new_node;
    if(has_symbol(table, name) && property!=NULL) { /* if property is null, it is external label which can be twice with same name*/
        print_error(LABEL_ALREADY_EXIST, name);
    }
    new_node = (symbolNode*)handle_malloc(sizeof(symbolNode));
    strcpy(new_node->name, name);
    if (property) {
        strcpy(new_node->property, property);
    }
    new_node->value=value;
    tmp = table->first_node;
    new_node->next = tmp;
    table->first_node = new_node;
}

/*increase the dc to be in the right place of data in memory after all the orders*/
void add_ic_to_dcf(symbolTable* table, int ic) {
    symbolNode* current = table->first_node;
    while (current!=NULL) {
        if (strcmp(current->property, DATA) ==0) {
            current->value+=ic;
        }
        current=current->next;
    }
}

/* check if the line is a decleration of a label*/
unsigned int is_label_line(char* line) {
    char* first_word;
    int first_word_len;
    int i;
    first_word = get_next_word(line, TRUE);
    first_word_len = strlen(first_word);
    if (first_word[first_word_len-1]!=':') {
        handle_free (first_word);
        return FALSE;
    }
    else {
        for (i=0; i<first_word_len-1;i++) {
            if (!isalpha(first_word[i]) && !isdigit(first_word[i]) ) {
                return FALSE; /* if i had time, it was better to handle this error as known label with bad name, now i handle it only by not detect it as a label or as a order*/
            }
        }
    }
    return TRUE;
}

/* get the label name from the line, without the ':' in the end*/
char* get_label(char* line) {
    char* first_word = get_next_word(line, TRUE);
    first_word[strlen(first_word)-1]='\0';
    return first_word;
}

/* print all the entries to the file, get pointer to the entires table and to the opened file*/
void print_entries_to_file(FILE* file, symbolTable* table) {
    symbolNode* current = table->first_node;
    while (current)
    {
        if(strcmp(current->property, ENTRY)==0) {
            fprintf(file, "%s \t 0x%06X\n", current->name, current->value);
        }
        current = current->next;
    }
}
/* print all the externals data to a file, get pointer to the externals table and to the opened file*/
void print_externals_to_file(FILE* file, symbolTable* table) {
    symbolNode* current = table->first_node;
    while (current)
    {
        fprintf(file, "%s \t 0x%06X\n", current->name, current->value);
        current = current->next;
    }
}

/* free all the data in a symbol table*/
void free_symbol_table(symbolTable* table) {
    symbolNode* next;
    symbolNode* current = table->first_node;
    while (current != NULL) {
        next = current->next;
        handle_free(current);
        current=next;
    }
}