#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/parser.h"
#include "../headers/write_bin.h"

int main(int argc, char const *argv[]) {
    char* file_lines[INT16_MAX];
    char* file_words[INT16_MAX];
    d8_token tokens[INT16_MAX];
    unsigned char* machine_code;

    for (int i = 0; i < INT16_MAX; i++) {
        file_lines[i] = calloc(256, 1);
        file_words[i] = calloc(10, 1);
        tokens[i] = 0;
    }
    if (argc != 4) {
        fprintf(stdout, "NOT ENOUGH ARGS!\n");
        return -1;
    }

    switch (read_uasm((char*) argv[1], file_lines)) {
        case 0: {
            break;
        }
        case 1: {
            fprintf(stdout, "No UASM file given\n");
            return 1;
        }
        case 2: {
            fprintf(stdout, "File doesn't exist!\n");
            return 2;
        }
    }

    if (strcmp(argv[2], "-o")) {
        fprintf(stdout, "%s is not a recognized flag\n", argv[2]);
        return -1;
    }

    char* extension = strchr(argv[3], '.');
    if (strcmp(extension, ".bin")) {
        fprintf(stdout, "No binary file given\n");
        return -1;
    }

    line_fixer(file_lines);
    line_splitter(file_lines, file_words);
    tokenizer(file_words, tokens);
    machine_code = token_word_to_mc(tokens, file_words);
    write_bin((char*) argv[3], machine_code);
    
    return 0;
}
