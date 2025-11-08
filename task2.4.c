#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include "task2.4.h"

int roman_digit_value(char c) {
    switch (c) {
        case 'I': return 1;
        case 'V': return 5;
        case 'X': return 10;
        case 'L': return 50;
        case 'C': return 100;
        case 'D': return 500;
        case 'M': return 1000;
        default: return 0;
    }
}

return_code roman_to_int(const char* str, int* result) {
    if (str == NULL || result == NULL || *str == '\0') {
        return INVALID_INPUT;
    }

    int count = 1;
    for (int i = 1; str[i] != '\0'; i++) {
        if (str[i] == str[i-1]) {
            count++;
            if (count > 3) {
                return INVALID_INPUT;
            }
        } else {
            count = 1;
        }
    }
    
    int res = 0;
    int prev_value = 0;
    int current_value;
    
    for (int i = strlen(str) - 1; i >= 0; i--) {
        current_value = roman_digit_value(str[i]);
        
        if (current_value == 0) {
            return INVALID_INPUT;
        }
        
        if (current_value < prev_value) {
            res -= current_value;
        } else {
            res += current_value;
        }
        
        prev_value = current_value;
    }
    
    if (res < 0 || res > INT_MAX) {
        return ERROR_OVERFLOW;
    }
    
    *result = res;
    return OK;
}

void generate_fibonacci(unsigned int fib[], int* count) {
    fib[0] = 1;
    fib[1] = 2;
    *count = 2;
    
    for (int i = 2; i < 64; i++) {
        unsigned long long next = (unsigned long long)fib[i-1] + fib[i-2];
        if (next > UINT_MAX) break;
        fib[i] = (unsigned int)next;
        (*count)++;
    }
}

return_code zeckendorf_to_uint(const char* str, unsigned int* result) {
    if (str == NULL || result == NULL) {
        return INVALID_INPUT;
    }
    
    int len = strlen(str);
    if (len < 2 || len > 64) {
        return INVALID_INPUT;
    }

    for (int i = 0; i < len; i++) {
        if (str[i] != '0' && str[i] != '1') {
            return INVALID_INPUT;
        }
    }

    if (str[len - 1] != '1') {
        return INVALID_INPUT;
    }

    for (int i = 0; i < len - 2; i++) {
        if (str[i] == '1' && str[i + 1] == '1') {
            return INVALID_INPUT;
        }
    }

    unsigned int fib[64];
    int fib_count;
    generate_fibonacci(fib, &fib_count);

    unsigned int value = 0;
    
    for (int i = 0; i < len - 1; i++) {
        if (str[i] == '1') {
            if (i >= fib_count) {
                return ERROR_OVERFLOW;
            }
            if (value > UINT_MAX - fib[i]) {
                return ERROR_OVERFLOW;
            }
            value += fib[i];
        }
    }
    
    *result = value;
    return OK;
}

return_code str_to_int_base(const char* str, int base, int* result) {
    if (str == NULL || result == NULL) {
        return INVALID_INPUT;
    }

    if (base < 2 || base > 36) {
        base = 10;
    }

    while (isspace(*str)) {
        str++;
    }
    
    if (*str == '\0') {
        return INVALID_INPUT;
    }

    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    long long value = 0;
    int digit;
    
    while (*str != '\0') {
        char c = *str;
        
        if (isdigit(c)) {
            digit = c - '0';
        } else if (islower(c)) {
            digit = c - 'a' + 10;
        } else {
            return INVALID_INPUT;
        }

        if (digit >= base) {
            return INVALID_INPUT;
        }

        if (sign == 1) {
            if (value > (LLONG_MAX - digit) / base) {
                return ERROR_OVERFLOW;
            }
        } 
        else {
            if (-value < (LLONG_MIN + digit) / base) {
                return ERROR_OVERFLOW;
            }
        }
        
        value = value * base + digit;
        str++;
    }
    
    value *= sign;

    if (value > INT_MAX || value < INT_MIN) {
        return ERROR_OVERFLOW;
    }
    
    *result = (int)value;
    return OK;
}
return_code str_to_int_base_upper(const char* str, int base, int* result) {
    if (str == NULL || result == NULL) {
        return INVALID_INPUT;
    }

    if (base < 2 || base > 36) {
        base = 10;
    }

    while (isspace(*str)) {
        str++;
    }
    
    if (*str == '\0') {
        return INVALID_INPUT;
    }

    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    long long value = 0;
    int digit;
    
    while (*str != '\0') {
        char c = *str; 
        
        if (isdigit(c)) {
            digit = c - '0';
        } else if (isupper(c)) {
            digit = c - 'A' + 10;
        } else {
            return INVALID_INPUT;
        }

        if (digit >= base) {
            return INVALID_INPUT;
        }

        if (sign == 1) {
            if (value > (LLONG_MAX - digit) / base) {
                return ERROR_OVERFLOW;
            }
        } 
        else {
            if (-value < (LLONG_MIN + digit) / base) {
                return ERROR_OVERFLOW;
            }
        }
        
        value = value * base + digit;
        str++;
    }
    
    value *= sign;

    if (value > INT_MAX || value < INT_MIN) {
        return ERROR_OVERFLOW;
    }
    
    *result = (int)value;
    return OK;
}
int voversscanf(const char* input_str, const char* format, va_list arg_list) {
    if (input_str == NULL || format == NULL) return -1;

    int assigned = 0;
    const char* input_ptr = input_str;
    const char* format_ptr = format;
    
    while (*format_ptr != '\0') {
        while (isspace((unsigned char)*format_ptr)) format_ptr++;
        if (*format_ptr == '\0') break;
        
        if (*format_ptr != '%') {
            while (isspace((unsigned char)*input_ptr)) input_ptr++;

            if (*input_ptr != *format_ptr) break;
            input_ptr++;
            format_ptr++;
            continue;
        }

        format_ptr++;
        if (*format_ptr == '\0') break;
        
        if (strncmp(format_ptr, "Ro", 2) == 0) {
            format_ptr += 2;

            while (isspace((unsigned char)*input_ptr)) input_ptr++;
            
            const char* roman_start = input_ptr;
            while (roman_digit_value(*input_ptr)) input_ptr++;
            
            if (input_ptr > roman_start) {
                char roman_num[32] = {0};
                int roman_len = input_ptr - roman_start;
                if (roman_len >= 32) roman_len = 31;
                strncpy(roman_num, roman_start, roman_len);
                roman_num[roman_len] = '\0';
                
                int* output_ptr = va_arg(arg_list, int*);
                if (output_ptr != NULL) {
                    int value;
                    if (roman_to_int(roman_num, &value) == OK) {
                        *output_ptr = value;
                        assigned++;
                    }
                }
            }
        }
        else if (strncmp(format_ptr, "Zr", 2) == 0) {
            format_ptr += 2;

            while (isspace((unsigned char)*input_ptr)) input_ptr++;
            
            const char* zeck_start = input_ptr;
            while (*input_ptr == '0' || *input_ptr == '1') input_ptr++;
            
            if (input_ptr > zeck_start) {
                char zeck_str[65] = {0};
                int zeck_len = input_ptr - zeck_start;
                if (zeck_len >= 65) zeck_len = 64;
                strncpy(zeck_str, zeck_start, zeck_len);
                zeck_str[zeck_len] = '\0';
                
                unsigned int* output_ptr = va_arg(arg_list, unsigned int*);
                if (output_ptr != NULL) {
                    unsigned int value;
                    if (zeckendorf_to_uint(zeck_str, &value) == OK) {
                        *output_ptr = value;
                        assigned++;
                    }
                }
            }
        }
        else if (strncmp(format_ptr, "Cv", 2) == 0 || strncmp(format_ptr, "CV", 2) == 0) {
            int uppercase = (format_ptr[1] == 'V');
            format_ptr += 2;

            while (isspace((unsigned char)*input_ptr)) input_ptr++;
            
            const char* base_start = input_ptr;
            if (*input_ptr == '+' || *input_ptr == '-') input_ptr++;
            while (isalnum((unsigned char)*input_ptr)) input_ptr++;

            if (input_ptr > base_start) {
                char base_str[256] = {0};
                int base_len = input_ptr - base_start;
                if (base_len >= 256) base_len = 255;
                strncpy(base_str, base_start, base_len);
                base_str[base_len] = '\0';
                
                int* output_ptr = va_arg(arg_list, int*);
                int base = va_arg(arg_list, int);
                
                if (output_ptr != NULL) {
                    int value;
                    return_code code;
                    if (uppercase) {
                        code = str_to_int_base_upper(base_str, base, &value);
                    } else {
                        code = str_to_int_base(base_str, base, &value);
                    }
                    
                    if (code == OK) {
                        *output_ptr = value;
                        assigned++;
                    }else {
                        fprintf(stderr, "Error: cannot convert '%s' in base %d (%s)\n", 
                        base_str, base, uppercase ? "uppercase" : "lowercase");
                    }
                }
            }
        }
        else {
            const char* spec_start = format_ptr;

            while (*format_ptr && *format_ptr != ' ' && *format_ptr != '%' && *format_ptr != '\0') {
                format_ptr++;
            }
            
            char final_format[256];
            int spec_len = format_ptr - spec_start;
            
            if (spec_len > 0) {
                snprintf(final_format, sizeof(final_format), "%%%.*s%%n", spec_len, spec_start);

                while (isspace((unsigned char)*input_ptr)) input_ptr++;
                
                int chars_read = 0;
                void* arg = va_arg(arg_list, void*);
                
                if (arg) {
                    int result = sscanf(input_ptr, final_format, arg, &chars_read);
                    if (result == 1) {
                        assigned++;
                        input_ptr += chars_read;
                    }
                }
            }
        }
    }
    return assigned;
}

int oversscanf(const char* input_str, const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    int result = voversscanf(input_str, format, arg_list);
    va_end(arg_list);
    return result;
}

int overfscanf(FILE* file, const char* format, ...) {
    if (file == NULL || format == NULL) return -1;

    char line[4096];
    if (fgets(line, sizeof(line), file) == NULL) return -1;

    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
    
    va_list args;
    va_start(args, format);
    int result = voversscanf(line, format, args);
    va_end(args);
    
    return result;
}

int main() {
    printf("Testing oversscanf with Roman numerals:\n");

    const char* test_str1 = "XIV 42 VII 3.14";
    int num1, num2, num3;
    float f;
    
    int result = oversscanf(test_str1, "%Ro %d %Ro %f", &num1, &num2, &num3, &f);
    
    printf("Result: %d assigned values\n", result);
    printf("Roman XIV = %d\n", num1);
    printf("Decimal 42 = %d\n", num2);
    printf("Roman VII = %d\n", num3);
    printf("Float 3.14 = %.2f\n", f);
    
    printf("\nTest 2: Invalid input\n");
    const char* test_str2 = "ABC 123";
    int a, b;
    result = oversscanf(test_str2, "%Ro %d", &a, &b);
    printf("Result: %d assigned values\n", result);
    
    printf("\nTest 3: Zeckendorf representations\n");
    const char* test_str3 = "11 011 1011 101011";
    unsigned int z1, z2, z3, z4;

    result = oversscanf(test_str3, "%Zr %Zr %Zr %Zr", &z1, &z2, &z3, &z4);
    printf("Result: %d assigned values\n", result);
    printf("Zeckendorf 11 = %u\n", z1);
    printf("Zeckendorf 011 = %u\n", z2);
    printf("Zeckendorf 1011 = %u\n", z3);
     printf("Zeckendorf 101011 = %u\n", z4);

    printf("\nTest 4: Mixed specifiers\n");
    const char* test_str4 = "X 1011 25 100011";
    int roman;
    unsigned int zeck1, zeck2;
    int decimal;
    
    result = oversscanf(test_str4, "%Ro %Zr %d %Zr", &roman, &zeck1, &decimal, &zeck2);
    printf("Result: %d assigned values\n", result);
    printf("Roman X = %d\n", roman);
    printf("Zeckendorf 1011 = %u\n", zeck1);
    printf("Decimal 25 = %d\n", decimal);
    printf("Zeckendorf 100011 = %u\n", zeck2);

    printf("\nTest 5: Custom base representations (lowercase)\n");
    const char* test_str5 = "1010 1A -ff 77";
    int bin, hex1, hex2, oct;
    
    result = oversscanf(test_str5, "%Cv %Cv %Cv %Cv", &bin, 2, &hex1, 16, &hex2, 16, &oct, 8);
    printf("Result: %d assigned values\n", result);
    printf("Binary 1010 = %d\n", bin);
    printf("Hex 1A = %d\n", hex1);
    printf("Hex -ff = %d\n", hex2);
    printf("Octal 77 = %d\n", oct);

    printf("\nTest 6: Custom base representations (uppercase)\n");
    const char* test_str6 = "1010 1A -ff 77";
    int bin2, hex3, hex4, oct2;
    
    result = oversscanf(test_str6, "%CV %CV %CV %CV", &bin2, 2, &hex3, 16, &hex4, 16, &oct2, 8);
    printf("Result: %d assigned values\n", result);
    printf("Binary 1010 = %d\n", bin2);
    printf("Hex 1A = %d\n", hex3);
    printf("Hex -ff = %d\n", hex4);
    printf("Octal 77 = %d\n", oct2);

    printf("\nTest 7: Testing overfscanf\n");
    FILE* test_file = fopen("test.txt", "w+");
    if (test_file) {
        fprintf(test_file, "XIV 1011 1a\n");
        fflush(test_file);
        rewind(test_file);
        
        int r1, r3;
        unsigned int r2;

        result = overfscanf(test_file, "%Ro %Zr %Cv", &r1, &r2, &r3, 16);
        
        printf("overfscanf result: %d assigned values\n", result);
        printf("Roman XIV = %d\n", r1);
        printf("Zeckendorf 1011 = %u\n", r2);
        printf("Hex 1a = %d\n", r3);
        
        fclose(test_file);
    } else {
        printf("ERROR: Could not create test.txt file!\n");
    }
    
    printf("\nTest 8: Testing standard specifiers with modifiers\n");
    const char* test_str8 = "123 -45 3000000000 12.34 A hello";
    short sval;
    long lval;
    long long llval;
    double dval;
    char cval;
    char strval[20];

    result = oversscanf(test_str8, "%hd %ld %lld %lf %c %5s", &sval, &lval, &llval, &dval, &cval, strval);
    printf("Result: %d assigned values\n", result);
    printf("short: %hd\n", sval);
    printf("long: %ld\n", lval);
    printf("long long: %lld\n", llval);
    printf("double: %.2f\n", dval);
    printf("char: %c\n", cval);
    printf("string: %s\n", strval);

    printf("\nTest 9: Testing suppression\n");
    const char* test_str9 = "100 200 300";
    int x, z;
    result = oversscanf(test_str9, "%d %*d %d", &x, &z);
    printf("Result: %d assigned values\n", result);
    printf("First: %d, Third: %d (second skipped)\n", x, z);

    printf("\nTest 10: Testing field width\n");
    const char* test_str10 = "123456789";
    int width_num1, width_num2;
    result = oversscanf(test_str10, "%3d%3d", &width_num1, &width_num2);
    printf("Result: %d assigned values\n", result);
    printf("First 3 digits: %d, Next 3 digits: %d\n", width_num1, width_num2);

    return 0;
}