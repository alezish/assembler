#ifndef ORDERS_H
#define ORDERS_H 

#include "symbols.h"

#define COMMANDS_LEN 16
#define CODE "code"
#define DIRECTIVES_LEN 4 

int is_directive(char* line);

int is_exact_directive(char* line, char* directive_type);

int get_order_num(char* line);

int get_order_ic_len(char* line);

int is_immediatly_address(char* word);

unsigned int get_order(char* line, int order_num,  int* src_data_word, int* dest_data_word, symbolTable* symbols_table, symbolTable* externals, int ic);

#endif