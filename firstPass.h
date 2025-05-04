#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "common.h"
#include "data.h"
#include "orders.h"
#include "symbols.h"
#include <stdio.h>

/*handle first pass on the file*/
void first_pass_on_file(char* file_name, dataTable* data_table, symbolTable* symbols_table);
#endif