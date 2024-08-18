#include "../headers/parser.h"

unsigned short str_to_addr(char* addr_str) {
    unsigned short addr_hex = 0x0000;
    if (strlen(addr_str) != 4) {
        fprintf(stdout, "IMPROPER ADDRESS LENGTH: %s\n", addr_str);
        return 0;
    }
    for (int i = 3; i >= 0; i--) {
        switch (addr_str[i]) {
            case '0': {
                addr_hex += (0 << (4*(3-i)));
                break;
            }
            case '1': {
                addr_hex += (1 << (4*(3-i)));
                break;
            }
            case '2': {
                addr_hex += (2 << (4*(3-i)));
                break;
            }
            case '3': {
                addr_hex += (3 << (4*(3-i)));
                break;
            }
            case '4': {
                addr_hex += (4 << (4*(3-i)));
                break;
            }
            case '5': {
                addr_hex += (5 << (4*(3-i)));
                break;
            }
            case '6': {
                addr_hex += (6 << (4*(3-i)));
                break;
            }
            case '7': {
                addr_hex += (7 << (4*(3-i)));
                break;
            }
            case '8': {
                addr_hex += (8 << (4*(3-i)));
                break;
            }
            case '9': {
                addr_hex += (9 << (4*(3-i)));
                break;
            }
            case 'A': {
                addr_hex += (0xA << (4*(3-i)));
                break;
            }
            case 'B': {
                addr_hex += (0xB << (4*(3-i)));
                break;
            }
            case 'C': {
                addr_hex += (0xC << (4*(3-i)));
                break;
            }
            case 'D': {
                addr_hex += (0xD << (4*(3-i)));
                break;
            }
            case 'E': {
                addr_hex += (0xE << (4*(3-i)));
                break;
            }
            case 'F': {
                addr_hex += (0xF << (4*(3-i)));
                break;
            }
            
            default: {
                fprintf(stdout, "NOT A HEX DIGIT: %c\n", addr_str[i]);
                return 0;
                break;
            }
        }
    }

    return addr_hex;
}

unsigned char str_to_byte(char* byte_str) {
    unsigned char byte_hex = 0x00;
    if (strlen(byte_str) != 2) {
        fprintf(stdout, "IMPROPER BYTE  LENGTH: %s\n", byte_str);
        return 0;
    }
    for (int i = 1; i >= 0; i--) {
        switch (byte_str[i]) {
            case '0': {
                byte_hex += (0 << (4*(1-i)));
                break;
            }
            case '1': {
                byte_hex += (1 << (4*(1-i)));
                break;
            }
            case '2': {
                byte_hex += (2 << (4*(1-i)));
                break;
            }
            case '3': {
                byte_hex += (3 << (4*(1-i)));
                break;
            }
            case '4': {
                byte_hex += (4 << (4*(1-i)));
                break;
            }
            case '5': {
                byte_hex += (5 << (4*(1-i)));
                break;
            }
            case '6': {
                byte_hex += (6 << (4*(1-i)));
                break;
            }
            case '7': {
                byte_hex += (7 << (4*(1-i)));
                break;
            }
            case '8': {
                byte_hex += (8 << (4*(1-i)));
                break;
            }
            case '9': {
                byte_hex += (9 << (4*(1-i)));
                break;
            }
            case 'A': {
                byte_hex += (0xA << (4*(1-i)));
                break;
            }
            case 'B': {
                byte_hex += (0xB << (4*(1-i)));
                break;
            }
            case 'C': {
                byte_hex += (0xC << (4*(1-i)));
                break;
            }
            case 'D': {
                byte_hex += (0xD << (4*(1-i)));
                break;
            }
            case 'E': {
                byte_hex += (0xE << (4*(1-i)));
                break;
            }
            case 'F': {
                byte_hex += (0xF << (4*(1-i)));
                break;
            }
            
            default: {
                fprintf(stdout, "NOT A HEX DIGIT: %c\n", byte_str[i]);
                return 0;
                break;
            }
        }
    }

    return byte_hex;
}

unsigned char* token_word_to_mc(d8_token* tokens, char** words) {
    d8_token token;
    char* word;
    char** label_names = calloc(256, 1);
    unsigned short* label_addresses = calloc(256, 1);
    unsigned char* RAM = calloc(MAX_16, 1);
    unsigned short addr = 0x0000;
    int label_index = 0;

    for (int i = 0; tokens[i]; i++) {
        token = tokens[i];
        word = words[i];
        switch (token) {
            case DIRECTIVE: {
                if (!strcmp(word, ".offset")) {
                    if (tokens[++i] != ADDRESS) {
                        fprintf(stdout, "OFFSET DIRECTIVE NEEDS AN ADDRESS\n");
                        return NULL;
                    }
                    addr += str_to_addr((words[i] + 1));

                } else if (!strcmp(word, ".start")) {
                    if (tokens[++i] != ADDRESS) {
                        fprintf(stdout, "START DIRECTIVE NEEDS AN ADDRESS\n");
                        return NULL;
                    }
                    unsigned short start_addr = str_to_addr((words[i] + 1));
                    unsigned char MSB = start_addr >> 8;
                    unsigned char LSB = start_addr;
                    RAM[0xFFFE] = LSB;
                    RAM[0xFFFF] = MSB;

                } else {
                    fprintf(stdout, "INVALID DIRECTIVE: %s\n", word);
                    return NULL;
                }
                break;
            }

            case COMMENT: {
                continue;
            }

            case OPCODE: {
                // Time for a shit-ton of if-else!
                if (!strcmp(word, "LBL")) { // label checking
                    if (tokens[++i] != LABEL) {
                        fprintf(stdout, "LABEL REQUIRES A PROPER LABEL\n");
                        return NULL;
                    }
                    char* label = calloc(10, 1);
                    strcpy(label, (words[i]));
                    label_names[label_index] = label;
                    label_addresses[label_index++] = addr;
                    break;
                } else if (!strcmp(word, "NOP")) { // NOP
                    RAM[addr++] = 0x00;
                } else if (!strcmp(word, "MOVA")) { // MOV
                    RAM[addr++] = 0x01;
                    // Correct value?
                    // If not, stop.
                    if (tokens[++i] != IMMEDIATE) {
                        fprintf(stdout, "MOVA NEEDS AN IMMEDIATE VALUE\n");
                        return NULL;
                    }
                    // If so, keep going.
                    RAM[addr++] = str_to_byte(words[i] + 1);
                } else if (!strcmp(word, "MOVB")) {
                    RAM[addr++] = 0x02;
                    // Correct value?
                    // If not, stop.
                    if (tokens[++i] != IMMEDIATE) {
                        fprintf(stdout, "MOVB NEEDS AN IMMEDIATE VALUE\n");
                        return NULL;
                    }
                    // If so, keep going.
                    RAM[addr++] = str_to_byte(words[i] + 1);
                } else if (!strcmp(word, "MOVC")) {
                    RAM[addr++] = 0x03;
                    // Correct value?
                    // If not, stop.
                    if (tokens[++i] != IMMEDIATE) {
                        fprintf(stdout, "MOVC NEEDS AN IMMEDIATE VALUE\n");
                        return NULL;
                    }
                    // If so, keep going.
                    RAM[addr++] = str_to_byte(words[i] + 1);
                } else if (!strcmp(word, "MOVD")) {
                    RAM[addr++] = 0x04;
                    // Correct value?
                    // If not, stop.
                    if (tokens[++i] != IMMEDIATE) {
                        fprintf(stdout, "MOVD NEEDS AN IMMEDIATE VALUE\n");
                        return NULL;
                    }
                    // If so, keep going.
                    RAM[addr++] = str_to_byte(words[i] + 1);
                } else if (!strcmp(word, "CPY")) {
                    RAM[addr++] = 0x05;
                    // Check (and store) the next two arguments.
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "CPY NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "CPY NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "STR")) { // MEMORY
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "STR NEEDS A REGISTER TO COPY FROM\n");
                        return NULL;
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }

                    if (tokens[++i] != ADDRESS) {
                        fprintf(stdout, "STR NEEDS AN ADDRESS TO COPY TO\n");
                        return NULL;
                    }

                    unsigned short address = str_to_addr((words[i] + 1));
                    unsigned char MSB = address >> 8;
                    unsigned char LSB = address;
                    RAM[addr++] = LSB;
                    RAM[addr++] = MSB;
                } else if (!strcmp(word, "LDR")) {
                    if (tokens[++i] != ADDRESS) {
                        fprintf(stdout, "STR NEEDS AN ADDRESS TO COPY TO\n");
                        return NULL;
                    }

                    unsigned short address = str_to_addr((words[i] + 1));
                    unsigned char MSB = address >> 8;
                    unsigned char LSB = address;
                    RAM[addr++] = LSB;
                    RAM[addr++] = MSB;

                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "STR NEEDS A REGISTER TO COPY FROM\n");
                        return NULL;
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "ADD")) { // MATH
                    RAM[addr++] = 0x08;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "ADD NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "ADD NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "SUB")) {
                    RAM[addr++] = 0x09;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "SUB NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "SUB NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "INC")) {
                    RAM[addr++] = 0x0A;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "INC NEEDS A REGISTER\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "DEC")) {
                    RAM[addr++] = 0x0B;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "DEC NEEDS A REGISTER\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "ADC")) {
                    RAM[addr++] = 0x0C;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "ADC NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "ADC NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "SBB")) {
                    RAM[addr++] = 0x0D;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "SBB NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "SBB NEEDS TWO REGISTERS\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else if (!strcmp(word, "NEG")) {
                    RAM[addr++] = 0x0E;
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "NEG NEEDS A REGISTER\n");
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } /* ... */ else if(!strcmp(word, "HLT")) {
                    RAM[addr++] = 0x18;
                } /* ... */ else if (!strcmp(word, "JL")) {
                    RAM[addr++] = 0x2C;
                    if ((tokens[++i] != ADDRESS) && (tokens[i] != LABEL)) {
                        fprintf(stdout, "JL NEEDS AN ADDRESS TO JUMP TO\n");
                        return NULL;
                    }
                    if (tokens[i] == LABEL) {
                        int len = sizeof(label_names)/sizeof(label_names[0]);
                        int present = 0;
                        int j;
                        for (j = 0; j < len; j++) {
                            if(!strcmp(label_names[j], words[i])) {
                                present = 1;
                                break;
                            }
                        }
                        if (!present) {
                            fprintf(stdout, "JL NEEDS A VALID LABEL\n");
                            return NULL;
                        }

                        unsigned short address = label_addresses[j];
                        unsigned char MSB = address >> 8;
                        unsigned char LSB = address;
                        RAM[addr++] = LSB;
                        RAM[addr++] = MSB;
                        
                    } else {
                        unsigned short address = str_to_addr((words[i] + 1));
                        unsigned char MSB = address >> 8;
                        unsigned char LSB = address;
                        RAM[addr++] = LSB;
                        RAM[addr++] = MSB;
                    }

                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "JL NEEDS TWO REGISTERS TO COMPARE\n");
                        return NULL;
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                    if (tokens[++i] != REGISTER) {
                        fprintf(stdout, "JL NEEDS TWO REGISTERS TO COMPARE\n");
                        return NULL;
                    }
                    switch (words[i][1]) {
                        case 'A': {
                            RAM[addr++] = 0x00;
                            break;
                        }
                        case 'B': {
                            RAM[addr++] = 0x01;
                            break;
                        }
                        case 'C': {
                            RAM[addr++] = 0x02;
                            break;
                        }
                        case 'D': {
                            RAM[addr++] = 0x03;
                            break;
                        }

                        default: {
                            fprintf(stdout, "%d is not a valid register\n", word[i]);
                            return NULL;
                        }
                    }
                } else {
                    fprintf(stdout, "INVALID OPCODE: %s\n", word);
                }
                break;
            }

            default: {
                fprintf(stdout, "INVALID STATEMENT BEGINNER: %s\n", word);
                // return NULL;
            }
        }
    }
    return RAM;
}