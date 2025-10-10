#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #define REALPATH(path, resolved) _fullpath(resolved, path, _MAX_PATH)
#else
    #include <stdlib.h>
    #define REALPATH(path, resolved) realpath(path, resolved)
#endif


typedef enum return_code {
    OK,
    INVALID_INPUT,
    ERROR_OVERFLOW,
    ERROR_MEMORY,
    ERROR_OPEN_FILE
} return_code;

return_code parse_flag(const char* flag, char* op, int* has_out);
char* gen_out_name(const char* input);

return_code d(const char* input, const char* output);
return_code i(const char* input, const char* output);
return_code s(const char* input, const char* output);
return_code a(const char* input, const char* output);