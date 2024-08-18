#include "../headers/write_bin.h"

void write_bin(char* filename, unsigned char* mem) {
    FILE* bin = fopen(filename, "wb");

    if (!bin) {
        fprintf(stdout, "Bin file couldn't be opened!\n");
        return;
    }

    for (int i = 0; i < MAX_16; i++) {
        fprintf(bin, "%c", mem[i]);
    }
    fclose(bin);
    return;
}