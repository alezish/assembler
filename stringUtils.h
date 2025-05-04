#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#define FIRST_PASS_LOG "start first pass on file: %s"
#define START_PRE_ASSEMBLER_LOG "start pre assembler for file: %s"
#define START_SEC_PASS_LOG "start second pass on file: %s"
#define FINISH_PRE_ASSEMBLER_LOG "finish pre assembler analyze"
#define MEMORY_ERROR "memory error, try to free all memory and exit the assembler immediatly"
#define DATA_OUT_OF_RANGE_ERROR "num is too big to be inserted to data"
#define RELATIEV_ADDRES_OOT "too big relative address in symbols table, that can't insert to memory, %s"
#define NO_STRING_IN_LINE "no string started in line: %s"
#define STRING_END_WITHOUT_QUOTES "string line didnt end with quotes, entire line: %s"
#define LABEL_WITHOUT_DIRECTIVE_OR_ORDER "synthax error, the line contains only label without directive or order"
#define ORDER_AND_DIRECTIVE_IN_ONE_LINE "synthax error, the line contatin directive and order"
#define EXTERNAL_OR_ENTRY_WITH_COMMA "synthax error, external or entry directive shouldn't contains comma"
#define COMMAS_DONT_MATCH_DATA "synthax error, count of comma's dont match count of data in line"
#define WORDS_UNMATCH_ORDER "synthax error, num of words in line dont match the order type"
#define WORDS_UNMATCH_COMMAS "syntax error, num of words in line don't match the num of comma's"
#define WRONG_NUMBER_IN_DATA "wrong number in data command line:  %s"
#define FINISH_FIRST_PASS "finish first pass on file, dc start in address: %d"
#define PRE_ASSEMBLY_FILE_ERR "Error reading or writing to pre assembly file"
#define MACRO_LINE_SYNTHAX_ERR "macro start line synthax error %s"
#define MACROEND_LINE_SYNTHAX_ERR "synthax error, macro end line contains another words in line"
#define CANT_OPEN_FINAL_DEST_FILE "can't open file for write"
#define ENTRY_LINE_WITHOUT_ENTRY "entry line without good symbol, entryName: %s"
#define START_WRITE_BIN_FILES "start write to binary files"
#define FINISH_WRITE_BIN_FILES "finish to analyze and convert the assembly file"
#define FINISH_ANALYZE_WITH_ERRORS "finish analyze file: '%s', found errors, no binary file create"
#define LABEL_ALREADY_EXIST "label with the name: %s already exist"

char* read_next_word(char ** read_ptr, int skip_spaces);

char* get_next_word(char * read_ptr, int skip_spaces);

int str_to_int(char* word);

int is_empty_or_comment(char* line);

int is_valid_num(char* word);

char get_register(char* word);

void log_data(const char *format, ...);

void print_error(const char *format, ...);

#endif