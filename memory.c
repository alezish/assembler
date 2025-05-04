#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "stringUtils.h"

MallocNode *first_node = {NULL};

/* get size and malloc memory, save the memory address to free in errors, and handles errors if exist*/
void* handle_malloc(size_t size) {
    void* new_address;
    MallocNode* new_node = (MallocNode*)malloc(sizeof(MallocNode));
    if (new_node!=NULL) {
        new_address = malloc(size);
        if (new_address!=NULL) {
            new_node->address = new_address;
            new_node->next = first_node;
            first_node = new_node;
            return new_address; 
        }
    }
    handle_malloc_error(); /* handle the error only if new_node=NULL or new_address=NULL */
    return NULL;
}

/* get memory address and free it's from the memory and memory table*/
void handle_free(void* address) {
    MallocNode *last, *current;
    last = first_node;
    if (last==NULL) { /* no memory saved*/
        free(address);
        return;
    }
    if (address == last->address) {
        /* handle free the first memory in table*/
        first_node = last->next;
        free(address);
        free(last);
    }
    else {
        /* handle free memory in table*/
        current = last->next;
        while (current!=NULL) {
            if (current->address == address) {
                last->next = current->next;
                free(current->address);
                free(current);
                break;
            }
            else {
                last = current;
                current = current->next;
            }
        }    
    }
}

/* after malloc error occur, free all the memory and stop the program, didn't has time to save & close all opened files too */
void handle_malloc_error() {
    MallocNode* tmp = first_node;
    printf(MEMORY_ERROR);
    while (first_node!=NULL) {
        tmp = first_node->next;
        free(first_node->address);
        free(first_node);
        first_node = tmp;
    }
    exit(1);
}
