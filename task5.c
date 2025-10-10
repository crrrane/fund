#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "task5.h"

return_code parse_flag(const char* flag, char* op, int* has_out) {
    if (flag == NULL || strlen(flag) < 2 || (flag[0] != '-' && flag[0] != '/')) {
        return INVALID_INPUT;
    }
    
    *has_out = 0;
    if (strlen(flag) == 3 && flag[1] == 'n') {
        *has_out = 1;
        *op = flag[2];
    } else if (strlen(flag) == 2) {
        *op = flag[1];
    } else {
        return INVALID_INPUT;
    }

    if (*op != 'd' && *op != 'i' && *op != 's' && *op != 'a') {
        return INVALID_INPUT;
    }
    
    return OK;
}

char* gen_out_name(const char* input) {
    const char* name;

    if (input == NULL) {
        return NULL;
    }
    
    const char* slash = strrchr(input, '/');
    if (slash == NULL) {
        slash = strrchr(input, '\\');
    }
    if (slash != NULL) {
        name = slash + 1;
    } else {
        name = input;
    }
    
    char* res = (char*)malloc(strlen(name) + 5);
    if (res == NULL) {
        return NULL;
    }
    strcpy(res, "out_");
    strcat(res, name);
    
    return res;
}

return_code check_same_file(const char* input, const char* output) {
    if (input == NULL || output == NULL) {
        return INVALID_INPUT;
    }

    char abs_input[4096];
    char abs_output[4096];

    #ifdef _WIN32
        if (_fullpath(abs_input, input, 4096) == NULL) { 
            return OK;
        }
        if (_fullpath(abs_output, output, 4096) == NULL){
            return OK;
        }
    #else
        if (realpath(input, abs_input) == NULL){
            return OK;
        } 
        if (realpath(output, abs_output) == NULL) {
            return OK;
        }
    #endif
    
    if (strcmp(abs_input, abs_output) == 0) {
        return INVALID_INPUT;
    }

    return OK;
}

return_code d(const char* input, const char* output) {
    FILE* in = fopen(input, "r");
    if (in == NULL) {
        return ERROR_OPEN_FILE;
    }
    
    FILE* out = fopen(output, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_OPEN_FILE;
    }
    
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c < '0' || c > '9') {
            fputc(c, out);
        }
    }
    
    fclose(in);
    fclose(out);
    return OK;
}

return_code i(const char* input, const char* output) {
    FILE* in = fopen(input, "r");
    if (in == NULL) {
        return ERROR_OPEN_FILE;
    }
    
    FILE* out = fopen(output, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_OPEN_FILE;
    }
    
    int c;
    int letter_count = 0;
    int has_chars_in_line = 0;
    
    while ((c = fgetc(in)) != EOF) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            letter_count++;
            has_chars_in_line = 1;
        }
        
        if (c == '\n') {
            fprintf(out, "%d\n", letter_count);
            letter_count = 0;
            has_chars_in_line = 0;
        }
    }
    if (has_chars_in_line || letter_count > 0) {
        fprintf(out, "%d\n", letter_count);
    }
    
    fclose(in);
    fclose(out);
    return OK;
}

return_code s(const char* input, const char* output) {
    FILE* in = fopen(input, "r");
    if (in == NULL) {
        return ERROR_OPEN_FILE;
    }
    
    FILE* out = fopen(output, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_OPEN_FILE;
    }
    
    int c;
    int special_count = 0;
    int has_chars_in_line = 0;
    
    while ((c = fgetc(in)) != EOF) {
        unsigned char uc = (unsigned char)c;

        if (!((uc >= 'A' && uc <= 'Z') ||  (uc >= 'a' && uc <= 'z') ||  (uc >= '0' && uc <= '9') || uc == ' ')) {
            special_count++;
            has_chars_in_line = 1;
        }
        
        if (c == '\n') {
            fprintf(out, "%d\n", special_count);
            special_count = 0;
            has_chars_in_line = 0;
        }
    }

    if (has_chars_in_line || special_count > 0) {
        fprintf(out, "%d\n", special_count);
    }
    
    fclose(in);
    fclose(out);
    return OK;
}

return_code a(const char* input, const char* output) {
    FILE* in = fopen(input, "r");
    if (in == NULL) {
        return ERROR_OPEN_FILE;
    }
    
    FILE* out = fopen(output, "w");
    if (out == NULL) {
        fclose(in);
        return ERROR_OPEN_FILE;
    }
    
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c >= '0' && c <= '9') {
            fputc(c, out);
        } else {
            fprintf(out, "%02X", (unsigned char)c);
        }
    }
    
    fclose(in);
    fclose(out);
    return OK;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Invalid input\n");
        return INVALID_INPUT;
    }
    
    const char* flag = argv[1];
    const char* input = argv[2];

    char op;
    int has_out;
    return_code code = parse_flag(flag, &op, &has_out);
    if (code != OK) {
        printf("Invalid input\n");
        return INVALID_INPUT;
    }

    if (has_out && argc < 4) {
        printf("Invalid input\n");
        return INVALID_INPUT;
    }

    const char* output;
    char* gen_name = NULL;
    
    if (has_out) {
        output = argv[3];

        code = check_same_file(input, output);
        if (code != OK) {
            printf("Error: Input and output files cannot be the same\n");
            return INVALID_INPUT;
        }
    } else {
        gen_name = gen_out_name(input);
        if (gen_name == NULL) {
            printf("Memory error\n");
            return ERROR_MEMORY;
        }
        output = gen_name;
        code = check_same_file(input, output);
        if (code != OK) {
            printf("Error: Generated output name conflicts with input file\n");
            free(gen_name);
            return INVALID_INPUT;
        }
    }

    switch (op) {
        case 'd':
            code = d(input, output);
            break;
        case 'i':
            code = i(input, output);
            break;
        case 's':
            code = s(input, output);
            break;
        case 'a':
            code = a(input, output);
            break;
        default:
            code = INVALID_INPUT;
            break;
    }
    
    if (gen_name != NULL) {
        free(gen_name);
    }
    
    if (code != OK) {
        printf("Error\n");
        return code;
    }
    
    return OK;
}