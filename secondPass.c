#include "secondPass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "stringUtils.h"
#include "common.h"
#include "orders.h"

char POST_MACROS_EXT[] = ".am";
const char ASSEMBLY_EXT[] = ".ob";
const char ENTRY_EXT[] = ".ent";
const char EXTENAL_EXT[] = ".ext";

/* write the macing commands to file*/
void write_machine_file(char* file_name, dataTable* assembly, dataTable* data_table) {
    int ic;
    FILE *file;
    char* assmebler_file_name;
    assmebler_file_name = (char*)handle_malloc((strlen(file_name)+strlen(ASSEMBLY_EXT)+1)*sizeof(char));
    sprintf(assmebler_file_name, "%s%s", file_name, ASSEMBLY_EXT);
    file = fopen(assmebler_file_name, "w");
     if (file == NULL) {
        print_error(CANT_OPEN_FINAL_DEST_FILE);
        handle_malloc_error();
    }
    else {
        ic = START_IC;
        print_all_table_with_index(file, assembly, &ic);
        print_all_table_with_index(file, data_table, &ic);
        fclose(file);    
    }
}

/* write the entries data to file*/
void write_entries_file(char* file_name, symbolTable* symbols_table) {
    char* entries_file_name = (char*)handle_malloc((strlen(file_name)+strlen(ENTRY_EXT)+1)*sizeof(char));
    FILE *file;
    sprintf(entries_file_name, "%s%s", file_name, ENTRY_EXT);
    file = fopen(entries_file_name, "w");
    if (file == NULL) {
        print_error(CANT_OPEN_FINAL_DEST_FILE);
        handle_malloc_error();
    }
    else {
        print_entries_to_file(file, symbols_table);
        fclose(file);    
    }
}

/* write the externals data to file*/
void write_externals_file(char* file_name, symbolTable* externals) {
    char* externals_file_name = (char*)handle_malloc((strlen(file_name)+strlen(EXTENAL_EXT)+1)*sizeof(char));
    FILE *file;
    sprintf(externals_file_name,"%s%s", file_name, EXTENAL_EXT);
    file = fopen(externals_file_name, "w");
    if (file == NULL) {
        print_error(CANT_OPEN_FINAL_DEST_FILE);
        handle_malloc_error();
    }
    else {
        print_externals_to_file(file, externals);
        fclose(file);    
    }
}

/* handle the second pass on file, with the data and symbols table from the first pass*/
void second_pass_on_file(char* file_name, dataTable* data_table, symbolTable* symbols_table) {
    dataTable assembly ={NULL};
    symbolTable externals = {NULL};
    symbolNode* entry_node;
    FILE* file;
    char *tmp, *word, *post_macros_filename, *line, *line_data;
    int ic, order_num;
    int src_data_word, dest_data_word;
    src_data_word = dest_data_word =0; 
    post_macros_filename = (char*)handle_malloc((strlen(file_name)+strlen(POST_MACROS_EXT)+1)*sizeof(char));
    line = (char*)handle_malloc(LINE_MAX_LEN*sizeof(char));
    line_counter=0;
    sprintf(post_macros_filename,"%s%s", file_name, POST_MACROS_EXT);
    file = fopen(post_macros_filename, "r");
    handle_free(post_macros_filename);
    ic=100;
    log_data(START_SEC_PASS_LOG, file_name);
    while (fgets(line, LINE_MAX_LEN, file)) {
        line_counter++;
        line_data = line; /* for updating the pointer inside line without override the line loc, for the while term don't cause overflow*/
        if (is_label_line(line_data)) {
            tmp = read_next_word(&line_data, TRUE); /* for continue read the line after the label*/
            handle_free(tmp);
        }
        word = read_next_word(&line_data, TRUE);
        order_num = get_order_num(word);
        if (order_num>=0) {
            /* handle write order word and it's data words*/
            insert_to_data(&assembly, get_order(line_data, order_num, &src_data_word, &dest_data_word, symbols_table, &externals, ic));
            if (src_data_word) {
                insert_to_data(&assembly, src_data_word);
            }
            if(dest_data_word) {
                insert_to_data(&assembly, dest_data_word);
            }
        }
        else {
            /* handle entries*/
            if (strcmp(word, ENTRY)==0) {
                handle_free(word);
                word = read_next_word(&line_data, TRUE); /* continue read the word after the entry label*/
                entry_node = get_symbol(symbols_table, word);
                if (entry_node) {
                    strcpy(entry_node->property, ENTRY);
                    entry_node=NULL;
                }
                else {
                    print_error(ENTRY_LINE_WITHOUT_ENTRY, word);
                }
            }
        }
        handle_free(word);
        src_data_word = dest_data_word =0; 
        ic++;
    }
    if (!has_errors) {
        log_data(START_WRITE_BIN_FILES);
        line_counter=0;
        write_machine_file(file_name, &assembly, data_table);
        write_entries_file(file_name, symbols_table);
        write_externals_file(file_name, &externals);
        log_data(FINISH_WRITE_BIN_FILES);
    }
    else {
        log_data(FINISH_ANALYZE_WITH_ERRORS, file_name);
    }
    /* free all memory located for analyze this file*/
    handle_free(line);
    free_symbol_table(symbols_table);
    free_symbol_table(&externals);
    free_data_table(data_table);
    free_data_table(&assembly);
}