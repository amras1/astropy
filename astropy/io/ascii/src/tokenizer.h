// Licensed under a 3-clause BSD style license - see LICENSE.rst

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <ctype.h>

typedef enum
{
    START_LINE = 0,
    START_FIELD,
    START_QUOTED_FIELD,
    FIELD,
    QUOTED_FIELD,
    QUOTED_FIELD_NEWLINE,
    COMMENT,
    CARRIAGE_RETURN
} tokenizer_state;

typedef enum
{
    NO_ERROR,
    INVALID_LINE,
    TOO_MANY_COLS,
    NOT_ENOUGH_COLS,
    CONVERSION_ERROR,
    OVERFLOW_ERROR
} err_code;

typedef struct
{
    char *source;          // single string containing all of the input
    int source_len;        // length of the input
    int source_pos;        // current index in source for tokenization
    char delimiter;        // delimiter character
    char comment;          // comment character
    char quotechar;        // quote character
    char *output;          // single null-delimited string containing output
    char **line_ptrs;      // array of pointers to the beginning of each line
    char **read_ptrs;      // array of pointers to current field in each line
    int output_pos;        // current index in output
    int line_ptrs_len;     // length of line_ptrs in memory
    int num_cols;          // number of table columns
    int num_rows;          // number of table rows
    int fill_extra_cols;   // represents whether or not to fill rows with too few values
    tokenizer_state state; // current state of the tokenizer
    err_code code;         // represents the latest error that has occurred
    int iter_col;          // index of the column being iterated over
    char *curr_pos;        // current iteration position
    char *buf;             // buffer for empty data
    int strip_whitespace_lines;  // whether to strip whitespace at the beginning and end of lines
    int strip_whitespace_fields; // whether to strip whitespace at the beginning and end of fields
    int use_fast_converter;      // whether to use the fast converter for floats
} tokenizer_t;

/*
Example input/output
--------------------

source: "A,B,C\n10,5.,6\n1,2,3"
output: "A\x00B\x00C\x0010\x005.\x006\x001\x002\x003"
line_ptrs: [output, output + 5, output + 13]
*/

#define INITIAL_NUM_LINES 50

tokenizer_t *create_tokenizer(char delimiter, char comment, char quotechar, int fill_extra_cols,
                              int strip_whitespace_lines, int strip_whitespace_fields,
                              int use_fast_converter);
void delete_tokenizer(tokenizer_t *tokenizer);
void delete_data(tokenizer_t *tokenizer);
void resize_col(tokenizer_t *self, int index);
int skip_lines(tokenizer_t *self, int offset, int header);
int tokenize(tokenizer_t *self, int end, int header, int num_cols);
long str_to_long(tokenizer_t *self, char *str, int row);
double str_to_double(tokenizer_t *self, char *str, int row);
double xstrtod(const char *str, char **endptr, char decimal,
               char sci, char tsep, int skip_trailing);
char *next_field(tokenizer_t *self, int row, int *field_len);
void set_read_ptrs(tokenizer_t *self);
void reset_read_ptrs(tokenizer_t *self);
void update_line_ptrs(tokenizer_t *self);
void advance_read_ptrs(tokenizer_t *self);
void rewind_line_ptrs(tokenizer_t *self);
char *get_line(char *ptr, int *len, int map_len);

#endif
