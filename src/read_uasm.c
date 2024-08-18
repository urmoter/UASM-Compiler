#include "../headers/read_uasm.h"
#include "../headers/tokenizer.h"

// Takes in a filename for the uasm file and an array of strings
// The function fills the array of strings
// Each element is a line from the uasm file
int read_uasm(char* filename, char** lines) {
    // Check if the input is a uasm file
    char* extension = strchr(filename, '.');
    if (strcmp(extension, ".uasm")) {
        return 1;
    }

    FILE* source = fopen(filename, "r");
    char line[256];
    if (source) {
        for (int i = 0; fgets(line, 256, source); i++) {
            lines[i] = calloc(257, 1);
            strcpy(lines[i], line);
        }
    } else {
        return 2;
    }
    fclose(source);
    return 0;
}
