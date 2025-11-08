#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

typedef enum return_code {
    OK,
    INVALID_INPUT,
    ERROR_OVERFLOW,
    ERROR_MEMORY
} return_code;

int roman_digit_value(char c);
return_code roman_to_int(const char* str, int* result);
void generate_fibonacci(unsigned int fib[], int* count);
return_code zeckendorf_to_uint(const char* str, unsigned int* result);
return_code str_to_int_base(const char* str, int base, int* result);
return_code str_to_int_base_upper(const char* str, int base, int* result);
int voversscanf(const char* input_str, const char* format, va_list arg_list);
int oversscanf(const char* str, const char* format, ...);
int overfscanf(FILE* stream, const char* format, ...);