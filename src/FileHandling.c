#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* make_VRAM(char* source) {
    int file_name_length = strlen(source);
    if (file_name_length < 5) {
        fprintf(stdout, "File doesn't have a name!\n");
        return NULL;
    }
    char* extension = strrchr(source, '.');

    if (!(extension && !strcmp(extension, ".uasm"))) {
        fprintf(stdout, "Wrong file type, use '.uasm'!\n");
        return NULL;
    }

    char* name = malloc(sizeof(char) * 256);
    strncpy(name, source, (file_name_length - 5));
    char* type = ".bin";
    char* RAM_name = strcat(name, type);
    FILE* VRAM = fopen(RAM_name, "w");

    return VRAM;
}
