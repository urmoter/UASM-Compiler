#ifndef T_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/read_uasm.h"

typedef enum {
    OPCODE = 1,
    COMMENT,
    DIRECTIVE,
    IMMEDIATE,
    REGISTER,
    ADDRESS,
    LABEL,
} d8_token;

void line_fixer(char** lines);
void line_splitter(char** lines, char** words);
void tokenizer(char** words, d8_token* tokens);


#endif // !T_h