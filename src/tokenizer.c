#include "../headers/tokenizer.h"

// Takes in an array of strings
// Removes the new lines and any empty lines
void line_fixer(char **lines) {
    for (int i = 0; *lines[i]; i++) {
        lines[i][strlen(lines[i]) - 1] = '\0';
    }

    for (int i = 0; lines[i]; i++) {
        if (lines[i][0] == '\0') {
            for (int j = i; lines[j]; j++) {
                if ((!lines[j+1] && !lines[j+1]) || (!lines[j+1][0] && !lines[j+2][0])) {
                    lines[j] = NULL;
                    break;
                }
                strcpy(lines[j], lines[j + 1]);
            }
        }
    }
}

// Takes in 2 arrays of strings
// Fills the words array with words extracted from the lines array
void line_splitter(char** lines, char** words) {
    char* word = calloc(10, 1);
    char* copy = calloc(256, 1);
    char* tester;
    int total = 0;

    for (int i = 0; lines[i]; i++) {
        strcpy(copy, lines[i]);

        strcpy(word, strtok(copy, " "));
        while (1) {
            strcpy(words[total++], word);
            if (word[0] == '/') {
                break;
            }

            tester = strtok(NULL, " ");

            if (!tester) {
                break;
            }
            strcpy(word, tester);
        }
    }
}

// Takes an array of an array of strings (words) and an array of tokens
// The function fills the array of tokens
// Each element is a token converted from each word.
void tokenizer(char** words, d8_token* tokens) {
    for(int i = 0; *words[i]; i++) {
        switch (words[i][0]) {
            case '/': {
                tokens[i] = COMMENT;
                break;
            }
            case '.': {
                tokens[i] = DIRECTIVE;
                break;
            }
            case '$': {
                tokens[i] = IMMEDIATE;
                break;
            }
            case '%': {
                tokens[i] = REGISTER;
                break;
            }
            case '@': {
                tokens[i] = ADDRESS;
                break;
            }
            case '_': {
                tokens[i] = LABEL;
                break;
            }
            default: {
                tokens[i] = OPCODE;
                break;
            }
        }
    }
}
