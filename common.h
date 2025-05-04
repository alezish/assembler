#ifndef COMMON_H
#define COMMON_H
#define TRUE  1
#define FALSE 0
#define LINE_MAX_LEN 82
#define DATA ".data"
#define STRING ".string"
#define ENTRY ".entry"
#define EXTERNAL ".extern"
#define ADDRESS_MAX_LEN 21
#define START_IC 100
#include "stringUtils.h"
extern int has_errors;
extern int line_counter;
extern char* filename;

#endif
