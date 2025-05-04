#ifndef SYMBOLS_H
#define SYMBOLS_H
#include <stdio.h>

/* define row of symbol in table*/
typedef struct symbolNode
{
    char name[31];
    char property[10];
    unsigned int value;
    struct  symbolNode* next;
}symbolNode;

/* define the whole symbols table*/
typedef struct symbolTable {
    symbolNode* first_node;
}symbolTable;

int has_symbol(symbolTable* table, char* name);

unsigned int get_label_value(symbolTable* table, char* name);

symbolNode* get_symbol(symbolTable* table, char* name);

void add_symbol(symbolTable* table, char* name, char* property, unsigned int value);

void add_ic_to_dcf(symbolTable* table, int ic);

unsigned int is_label_line(char* line);

char* get_label(char* line);

void print_entries_to_file(FILE* file, symbolTable* table);

void print_externals_to_file(FILE* file, symbolTable* table);

void free_symbol_table(symbolTable* table);

#endif