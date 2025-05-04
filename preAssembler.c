#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "common.h"
#include "macroTable.h"
#include "orders.h"
#include "preAssembler.h"

const char SOURCE_EXT[] = ".as"; /* extension for source file*/
const char PRE_ASSEM_EXT[] = ".am"; /* extension for after pre-assembler dest file*/

/* check if line is an end of macro*/
int is_end_of_macro (char* line) {
    char * word = get_next_word(line, TRUE);
    if (strcmp(word, MACRO_END_WORD)==0) {
        handle_free(word);
        return TRUE;
    } 
    else {
        handle_free(word);
        return FALSE;
    }
}

/* check if line is a start of macro*/
int is_macro_start(char* line) {
    char * word = get_next_word(line, FALSE);
    if (strcmp(word, MACRO_START_WORD)==0) {
        handle_free(word);
        return TRUE;
    }
    else {
        handle_free(word);
        return FALSE;
    }
}

/* check if the macro name isn't directive, order, or if there another words inside line*/
int is_valid_macro(char* macro_name, char* line) {
    /* check if macro name is order*/
    if (get_order_num(macro_name)>=0) {
        return FALSE;
    }
    if (is_directive(line)) {
        return FALSE;
    }
    macro_name = read_next_word(&line, TRUE); /*continue inside line after the macro name to verify it is empty*/
    handle_free(macro_name);
    if (!is_empty_or_comment(line)) {
        return FALSE;
    }
    return TRUE;
}

/* verify if macro end is valid, by verify it is alone word in line */
int is_valid_end_macro(char* line) {
    char *tmp = read_next_word(&line, TRUE);
    handle_free(tmp);
    if (is_empty_or_comment(line)) {
        return TRUE;
    }
    return FALSE;
}


/*handle the pre assmebler to fetch the macro to a new file*/
void handle_pre_assembler_for_file(char * file_name) {
    MacroTable table;
    MacroNode *current;
    char* src_file_name, *dest_file_name, *line, *line_data, *macro_name;
    FILE * src_file, *dest_file;
    int inside_macro = FALSE;
    log_data(START_PRE_ASSEMBLER_LOG, file_name);
    line = (char*)handle_malloc(LINE_MAX_LEN*sizeof(char));
    /* get src and dest filenames*/
    src_file_name = (char*)handle_malloc((strlen(file_name)+strlen(SOURCE_EXT)+1)*sizeof(char));
    dest_file_name = (char*)handle_malloc((strlen(file_name)+strlen(PRE_ASSEM_EXT)+1)*sizeof(char));
    sprintf(src_file_name, "%s%s", file_name, SOURCE_EXT);
    sprintf(dest_file_name, "%s%s", file_name, PRE_ASSEM_EXT);
    src_file = fopen(src_file_name, "r");
    dest_file = fopen(dest_file_name, "w");
    table.firstMacro=NULL;
    if (src_file == NULL || dest_file == NULL) {
        print_error(PRE_ASSEMBLY_FILE_ERR);
    }
    table.firstMacro=NULL;
    line_counter=0;
    while (fgets(line, LINE_MAX_LEN, src_file)) {
        line_data = line; /* for updating the pointer inside line without override the line loc, for the while term don't cause overflow*/
        line_counter++;
        if (is_macro_start(line_data)) {
            inside_macro = TRUE;
            macro_name = read_next_word(&line_data, FALSE); /* continue over the mcro command to the macro name*/
            handle_free(macro_name); 
            macro_name = get_next_word(line_data, TRUE);
            if (!is_valid_macro(macro_name, line_data)) {
                print_error(MACRO_LINE_SYNTHAX_ERR, line_data);
            }
            current = create_macro_node(macro_name); /* create the macro node, the lines will add to it later*/
            handle_free(macro_name);
            continue;
        }
        if (inside_macro) {
            if (!is_end_of_macro(line_data)) {
                add_macro_line(current, line_data); /* add command line to the macro*/
            }
            else {
                inside_macro = FALSE;
                if (!is_valid_end_macro(line_data)) {
                    print_error(MACROEND_LINE_SYNTHAX_ERR);
                    break;
                }
                add_macro_to_table(current, &table); /* add all the macro node with it's line to the macros table*/
            }
        }
        else {
            if (is_call_macro(line_data, &table)) {
                macro_name = get_next_word(line_data, FALSE);
                print_macro_to_file(dest_file, macro_name, &table); /* print all the macro data to the dest file*/
                handle_free(macro_name);
            }
            else {
                if (strcmp("\n", line)) { /* remove the empty lines from print it to the am file*/
                    fprintf(dest_file, "%s", line);
                }
            }
        }
        handle_free(line_data);
    }
    /* free memory and close files*/
    free_macros_table(table);
    log_data(FINISH_PRE_ASSEMBLER_LOG);
    fclose(dest_file);
    fclose(src_file);
}