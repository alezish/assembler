#ifndef DATA_H
#define DATA_H

#include <stdio.h>

/* struct that define row in the data table*/
typedef struct dataNode {
    int data;
    struct dataNode* next;
} dataNode;

/* struct that define the data table*/
typedef struct{
    dataNode* first_node;
} dataTable;

/*insert integer data into the data table*/
void insert_to_data(dataTable* table, int num);

/*print to already opened file*/
void print_all_table_with_index(FILE* file, dataTable* table, int* ic);

/*free all allocated data inside the data table*/
void free_data_table(dataTable* table);

#endif