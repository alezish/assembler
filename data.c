#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "memory.h"
#include "math.h"

/*insert integer data into the data table*/
void insert_to_data(dataTable* table, int num) {
    dataNode *new_node, *current;
    new_node = (dataNode*)handle_malloc(sizeof(dataNode));
    new_node->data = num;
    new_node->next = NULL;
    /* handle insertning the first node to table without try to use NULL->next*/
    if (table->first_node==NULL) {
        table->first_node = new_node;
    } 
    else {
        current = table->first_node;
        while (current->next!=NULL) {
            /* stop the loop in the last node*/
            current = current->next;
        }
        current->next=new_node;
    }
}

/*print all the the table to already opened file pointer
use the ic for lines count, and update it for the next tables print (orders and data)
*/
void print_all_table_with_index(FILE* file, dataTable* table, int* ic) {
    dataNode* current = table->first_node;
    while (current!=NULL)
    {
        fprintf(file, "%06d \t 0x%06X\n", *ic, current->data & 0x00FFFFFF);
        current = current->next;
        (*ic)++;
    }
}

/*free all allocated data inside the data table*/
void free_data_table(dataTable* table) {
    dataNode* next;
    dataNode* current = table->first_node;
    while (current) {
        next = current->next;
        handle_free(current);
        current = next;
    }
}