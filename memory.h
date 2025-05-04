#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

typedef struct MallocNode {
    void* address;
    struct MallocNode* next;
} MallocNode;

void* handle_malloc(size_t size);
void handle_free(void* address);
void handle_malloc_error();

#endif