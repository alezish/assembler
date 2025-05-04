
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "common.h"
#include "orders.h"
#include "memory.h"

const char* DIRECTIVES[DIRECTIVES_LEN] = {DATA, STRING, ENTRY, EXTERNAL}; /* all the directives*/
const char* COMMANDS[COMMANDS_LEN] = {"mov","cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"}; /* all the commands*/

const char OP_CODES[COMMANDS_LEN] = {0,1,2,2,4,5,5,5,5,9,9,9,12,13,14,15}; /* op codes ordered by commands indexes*/
const char FUNCTS[COMMANDS_LEN] = {0,0,1,2,0,1,2,3,4,1,2,3,0,0,0,0}; /* functs ordered by commands indexes*/

/* check if line contains  a directive*/
int is_directive(char* line) {
    int i;
    for (i=0;i<DIRECTIVES_LEN;i++) {
        if (strstr(line, DIRECTIVES[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

/* check if line is a specific directive */
int is_exact_directive(char* line, char* directive_type) {
    char* first_word = get_next_word(line, TRUE);
    if (strcmp(first_word, directive_type)==0) {
        handle_free(first_word);
        return TRUE;
    }
    handle_free(first_word);
    return FALSE;
}

/* get the index of orders in the const arrays by the order name*/
int get_order_num(char* word) {
    int i;
    for (i=0;i<COMMANDS_LEN;i++) {
        if (strcmp(word, COMMANDS[i])==0) {
            return i;
        }
    }
    return -1;
}

/* analyze the num of words in memory the order need, for the ic*/
int get_order_ic_len(char* line) {
    int num;
    char* word = read_next_word(&line, TRUE);
    if (get_order_num(word) == -1) {
        /* the first word isn't order, so it dont take lines in memory*/
        return 0;
    }
    num = 1; /* the orders binary word itself*/
    handle_free(word); 
    word = read_next_word(&line,TRUE); /* read the next word in line*/ 
    while (*word !='\0' && *word != '\n') { 
        if (get_register(word)==-1) {  /* every operand that isn't register, need a data word */
            num++;
        }
        handle_free(word);
        word = read_next_word(&line, TRUE);
    }
    return num;
}

/* get op code by order name*/
unsigned char get_op_code(char* order) {
    int i;
    for (i=0;i<COMMANDS_LEN; i++) {
        if (strcmp(COMMANDS[i], order)==0) {
            return (OP_CODES[i] & 0xF);
        }
    }
    return COMMANDS_LEN;
}

/* get funct by order name*/
unsigned char get_funct(char* order) {
    int i;
    for (i=0;i<COMMANDS_LEN; i++) {
        if (strcmp(COMMANDS[i], order)==0) {
            return (FUNCTS[i] & 0x7);
        }
    }
    return COMMANDS_LEN;
}

/* check if "shitat miun"  is direct address by analyze the operand and symbol*/
int is_direct_address(symbolTable* table, char* word) {
    if (has_symbol(table, word)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/* check if "shitat miun"  is immediatly address by analyze the operand*/
int is_immediatly_address(char* word) {
    return (*word =='#' && is_valid_num(word+1));
}

/* check if "shitat miun"  is realtive address by analyze the operand and symbol*/
int is_relative_address(symbolTable* symbols_table,char* word) {
    return (*word =='&' && has_symbol(symbols_table, word+1));
}

/* get the direct data word for operand*/
unsigned int get_direct_data_word(symbolTable* symbols_table, symbolTable* externals, char* word, int ic) {
    unsigned int value;
    symbolNode *symbol = get_symbol(symbols_table, word);
    if (strcmp(symbol->property, EXTERNAL) ==0) {
        add_symbol(externals, symbol->name, NULL, ic); /* add externals symbols to the externals table*/
    }
    value = (symbol->value << 3);
    value &= 0xFFFFF8;  /* */
    value |= 0x02; /* add ARE*/
    return value;
}

/* get the immediatly data word for operand*/
 int get_immediatly_data_word(char* word) {
    int num = str_to_int(word+1);
    num = num << 3;
    num &= 0xFFFFF8;
    num |= 0x04;
    return num; 
}

/* get the relative data word for operand*/
int get_relative_data_word(symbolTable* symbols_table,char* word, int ic) {
    int diff = get_label_value(symbols_table, word+1)-ic;
    /* if number is under 0, but in range, the 23 bit should be on too, so unsigned int can be unsigned 24 bits*/
    if (diff < 0-pow(2, ADDRESS_MAX_LEN-1)) {
        print_error(RELATIEV_ADDRES_OOT, word);
    }
    diff = diff <<3;
    diff &= 0xFFFFF8;
    diff |= 0x04;
    return diff;
}

/* merge all the data of the order to a single word*/
unsigned int combine_order_bits(unsigned char op_code, unsigned char funct, unsigned char src_reg, unsigned char dest_reg, unsigned char src_address, unsigned char dest_address) {
    unsigned int word = 0;
    op_code &= 0x3F;
    src_address &= 0x03;
    src_reg &= 0x07;
    dest_address &= 0x03;
    dest_reg &= 0x07;
    funct &= 0x1F;
    word |= (op_code << 18);
    word |= (src_address << 16);
    word |= (src_reg << 13);
    word |= (dest_address << 11);
    word |= (dest_reg << 8);
    word |= (funct << 3);
    return word;
}

/* create order from line, get all it's params and combine the bits and the data words. the pointers is the quickest way to handle 3 variables in single function*/
unsigned int get_order(char* line, int order_num,  int* src_data_word, int* dest_data_word, symbolTable* symbols_table, symbolTable* externals, int ic) {
    char *word1, *dest_word;
    char src_reg, dest_reg;
    unsigned char op_code, funct, src_address, dest_address, are;
    op_code=funct=src_reg=dest_reg=src_address=dest_address=are=0;
    op_code = OP_CODES[order_num];
    funct = OP_CODES[order_num];
    /* handle stop command that not contains any register or another data*/
    if (order_num==15) { 
        return combine_order_bits(op_code, funct, 0, 0, 0, 0);
    }
    word1 = read_next_word(&line, TRUE);
    dest_word = read_next_word(&line, TRUE); /*if there is 1 operand it is the dest word, so dest word will be eq to word1*/
    if (*dest_word!='\n' && *dest_word!=' ' && *dest_word!= '\0') { /* assign data to src word and data only if there dest operand, so the first operand is the src*/
        src_reg = get_register(word1);
        if (src_reg >= 0) {
            src_address=3;
            *src_data_word=0;
            src_reg=0; /* no register so the bits should be zero*/
        }
        else {
            src_reg = 0;
            if (is_immediatly_address(word1)) {
                src_address=0;
                *src_data_word = get_immediatly_data_word(word1);
            }
            else if (is_direct_address(symbols_table, word1)) {
                src_address=1;
                *src_data_word = get_direct_data_word(symbols_table, externals, word1, ic);
            }
            else if (is_relative_address(symbols_table, word1)) {
                src_address=2;
                *src_data_word = get_relative_data_word(symbols_table, word1, ic);
            }
            else {
                src_address = 0;
                *src_data_word = 0;
            }
        }
        handle_free(word1);    
    }
    else { /* no second operand so the dest operand is the word that was saved as word1*/
        src_reg = 0;
        src_address = 0;
        handle_free(dest_word);
        dest_word = word1;
    }
    dest_reg = get_register(dest_word);
    if (dest_reg >= 0) {
        dest_address=3;
        *dest_data_word=0;
        dest_reg=0; /* no register so the bits should be zero*/
    }
    else {
        dest_reg = 0;
        if (is_immediatly_address(dest_word)) {
            dest_address=0;
            *dest_data_word = get_immediatly_data_word(dest_word);
        }
        else if (is_direct_address(symbols_table, dest_word)) {
            dest_address=1;
            *dest_data_word = get_direct_data_word(symbols_table, externals, dest_word, ic);
        }
        else if (is_relative_address(symbols_table, dest_word)) {
            dest_address=2;
            *dest_data_word = get_relative_data_word(symbols_table, dest_word, ic);
        }
        else {
            /* it is no register too, checked before*/
            dest_address = 0;
            *dest_data_word = 0;
            print_error("address for dest operand dont exist or dont match the order, line: %s", line);
        }
    }
    if (dest_word!=NULL) {
        handle_free(dest_word);
    }
    return combine_order_bits(op_code, funct, src_reg, dest_reg, src_address, dest_address);
}