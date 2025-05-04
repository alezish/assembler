#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "orders.h"
#include "common.h"
#include "memory.h"
#include "firstPass.h"

/* extension for post macros*/
char POST_MACROS_EX[] = ".am";
const int COMMANDS_WORDS[COMMANDS_LEN] = {3,3,3,3,3,2,2,2,2,2,2,2,2,2,1,1}; /*num of words foreach command*/

/* add .string directive data to the data table*/
void add_string_data(dataTable* table, char* line, int* dc) {
    char *start_line, *tmp;
    start_line= line;
    tmp = read_next_word(&line, TRUE); /* continue inside line to be where the data is and skip the .string label*/
    handle_free(tmp);
    while (*line == ' ' || *line == '\t') { /* continue to the start of the next word*/
        line++;
    }    

    if(*line!='\"') {
        print_error(NO_STRING_IN_LINE, start_line);
    }
    line++;
    while(*(line+1) !='\0' && *(line+1)!='\n') {
        insert_to_data(table, *line); /* didnt has time to handle  non string chars if should be*/
        (*dc)++;
        line++;
    }
    if (*(line)!='\"') {
        print_error(STRING_END_WITHOUT_QUOTES, line);
    }
    insert_to_data(table, 0);
    (*dc)++;
}

/* this function get line after the label, so it contains only order/directive and params, and verify if it is well synthax*/
void verify_synthax(char* line) {
    int comma_count,word_count, quotes_count, order_num, word_end;
    char* line_data = line;
    comma_count = word_count = quotes_count = 0;
    word_end = TRUE;
    order_num = get_order_num(line);
    while (*line != '\n' && *line!='\0') {
        if (word_end && *line !=' ' && *line !=',' && *line != '\t') { /* analyze that line started*/
            word_count++;
            word_end=FALSE;
        }
        if (*line ==',') {
            comma_count++;
            word_end=TRUE;
        }
        else if (*line =='\"') {
            quotes_count++;
        }
        else if (!word_end && (*line =='\t' || *line ==' ')) {
            word_end=TRUE;  
        }
        line++;
    }
    if (is_empty_or_comment(line_data)) { /* shouldnt be empty line after a label, if there wasn't label we shouldn't be inside the function because in the previous function it checked that the line isn't empty*/
        print_error (LABEL_WITHOUT_DIRECTIVE_OR_ORDER);
    }
    if (is_directive(line_data) && order_num>=0) {
        print_error(ORDER_AND_DIRECTIVE_IN_ONE_LINE);
    }
    if (is_exact_directive(line_data, EXTERNAL) || is_exact_directive(line_data, ENTRY)) {
        if (comma_count>0) {
            print_error(EXTERNAL_OR_ENTRY_WITH_COMMA);
        }
    }
    else if (is_exact_directive(line_data, DATA)) {
        if (comma_count!= word_count-2) {
            print_error(COMMAS_DONT_MATCH_DATA);
        }
    }
    else if (is_exact_directive(line_data, STRING)) {
        /* didnt has time to handle read commas and words outside string which shouldnt be */
    }
    if (order_num>0) {
        if (COMMANDS_WORDS[order_num]!= word_count) {
            print_error (WORDS_UNMATCH_ORDER);
        }
        if (comma_count!= word_count-2) {
            print_error(WORDS_UNMATCH_COMMAS);
        }
    }
}

/* add .data directive from line to the data table with it's dc for add to memory later*/
void add_ints_data(dataTable* table, char* line, int* dc) {
    int num;
    char* word = read_next_word(&line, TRUE);
    handle_free(word); /* for continue read the data after the .data label*/
    word = read_next_word(&line, TRUE);
    while (*word!='\0') {
        if (is_valid_num(word)) {
            num = str_to_int(word);
            if (num > (pow (2, ADDRESS_MAX_LEN-1)-1) || num < (0-pow(2, ADDRESS_MAX_LEN-1))) {
                print_error(DATA_OUT_OF_RANGE_ERROR);
                print_error("num: %d", num); 
                num = 0; /* multi defend for avoiding overriding bits and memory*/
            }
            insert_to_data(table, num);
            (*dc)++;
        }
        else {
            print_error(WRONG_NUMBER_IN_DATA, word);
        }
        handle_free(word);
        word = read_next_word(&line, TRUE);
    }
    handle_free(word);
}

/*
handle first pass on the file, the function get the  filename, and pointers to empty symbols and data table, 
the function analyze the synthax of line, the labels  the string and data, and the externals. and calculate the ic and dc len of the final binary code
 */
void first_pass_on_file(char* file_name, dataTable* data_table, symbolTable* symbols_table) {
    FILE *file;
    char* label, *tmp, *post_macros_filename, *line, *line_data;
    int ic, dc, is_label;
    post_macros_filename = (char*)handle_malloc((strlen(file_name)+strlen(POST_MACROS_EX)+1)*sizeof(char));
    line = (char*)handle_malloc(LINE_MAX_LEN*sizeof(char));
    sprintf(post_macros_filename,"%s%s", file_name, POST_MACROS_EX);
    file = fopen(post_macros_filename, "r");
    ic= START_IC;
    dc=0;
    line_counter=0;
    log_data(FIRST_PASS_LOG, post_macros_filename);
    while (fgets(line, LINE_MAX_LEN, file)) {
        line_data=line;
        line_counter++;
        is_label=FALSE;
        if (!is_empty_or_comment(line_data)) { /* for skipping on empty lines*/
            if(is_label_line(line_data)) {
                /* understand that we need to save the order / directive as a label or symbol*/
                is_label = TRUE;
                label=get_label(line_data); 
                tmp = read_next_word(&line_data, FALSE); /* for continue analyze the line, starting after the label*/
                handle_free(tmp);
            }
            /* handle synthax errors*/
            verify_synthax(line_data); 
            if (is_exact_directive(line_data, STRING)) {
                if (is_label) {
                    add_symbol(symbols_table, label, STRING, dc); /* add label to the string data*/
                }
                add_string_data(data_table, line_data, &dc); /* add the string to data table and update dc length*/
            }
            else if (is_exact_directive(line_data, DATA)) {
                if (is_label) {
                    add_symbol(symbols_table, label, DATA, dc); /* add label to the data*/
                }
                add_ints_data(data_table, line_data, &dc); /* add the data to the data table and update dc length*/
            }
            else if (is_exact_directive(line_data, ENTRY)) { /* entries handled in the second pass*/
                continue;
            }
            else if (is_exact_directive(line_data, EXTERNAL)) {
                label = read_next_word(&line_data, FALSE); /* continue reading line after the external label*/
                handle_free(label);
                label = read_next_word(&line_data, TRUE);
                add_symbol(symbols_table, label, EXTERNAL, 0);
            }
            else { /* it is order line*/
                if (is_label) {
                    add_symbol(symbols_table, label, CODE, ic); /* add label to the code with the ic*/
                }
                ic+=get_order_ic_len(line_data); /* analyze the order len in the memory and update the ic*/
            }
            if (is_label) {
                handle_free(label);
                is_label=FALSE;
            }
        }
    }
    log_data(FINISH_FIRST_PASS, ic);
    add_ic_to_dcf(symbols_table, ic); /* add all the ic count to the dc in the symbols table to get their real address in memory*/
}