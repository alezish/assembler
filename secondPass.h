#ifndef SECOND_PASS_H
#define SECOND_PASS_H


#include "symbols.h"
#include "data.h"


/* write the machine commands to file*/
void write_machine_file(char* file_name, dataTable* assembly, dataTable* data_table);

/* write the entries data to file*/
void write_entries_file(char* file_name, symbolTable* symbols_table);

/* write the externals data to file*/
void write_externals_file(char* file_name, symbolTable* externals);

/* handle the second pass on file*/
void second_pass_on_file(char* file_name, dataTable* data_table, symbolTable* symbols_table);

#endif