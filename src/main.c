#include <stdio.h>
#include <stdlib.h>
#include "../headers/FileHandling.h"
#include "../headers/Interpreter.h"

// It has been so long since i've used command line arguments lmao
int main(int argc, char** argv) {
    if (argc == 1) {
        fprintf(stdout, "No File to Compile!\n");
        return 1;
    }

    char* source_file = *(argv + 1);
    FILE* bin = make_VRAM(source_file);

    if (bin == NULL) {
        fprintf(stdout, "INTERNAL ERROR, TRY AGAIN!\n");
        return 1;
    }
    
    

    fclose(bin);
    return 0;
}