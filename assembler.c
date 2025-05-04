#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "memory.h"
#include "common.h"

/* global variables for log data, and handle errors*/
int has_errors=0;
int line_counter = 0;
char* filename;

/* main function to handle get assembly file from atrgs, and convert it to binary files*/
int main(int argc, char *argv[]) {
    dataTable* data_table;
    symbolTable* symbol_table;
    argc--;
    while (argc>0) {
        has_errors = FALSE;
        filename = argv[argc];
        handle_pre_assembler_for_file(filename);
        data_table = (dataTable*)handle_malloc(sizeof(dataTable));
        symbol_table = (symbolTable*)handle_malloc(sizeof(symbolTable));
        data_table->first_node=NULL;
        symbol_table->first_node=NULL;
        if (!has_errors)
        {
            first_pass_on_file(filename, data_table, symbol_table);
            second_pass_on_file(filename, data_table, symbol_table);
            handle_free(data_table);
            handle_free(symbol_table); 
        }
        argc--;
    }
    return 1;
}