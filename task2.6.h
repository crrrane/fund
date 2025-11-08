#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum return_code {
    OK,
    INVALID_INPUT,
    ERROR_OVERFLOW,
    ERROR_MEMORY,
    ERROR_OPEN_FILE,
    ERROR_NOT_FOUND
} return_code;

typedef struct {
    unsigned int id;
    char* first_name;
    char* last_name;
    char* group;
    unsigned char* grades;
    int grades_count;
    double average_grade;
} Student;

typedef struct {
    Student* students;
    int count;
    int capacity;
} StudentArray;

return_code read_students_from_file(const char* filename, StudentArray* array);
return_code free_student_array(StudentArray* array);
return_code find_students_by_id(const StudentArray* array, unsigned int id, StudentArray* result);
return_code find_students_by_last_name(const StudentArray* array, const char* last_name, StudentArray* result);
return_code find_students_by_first_name(const StudentArray* array, const char* first_name, StudentArray* result);
return_code find_students_by_group(const StudentArray* array, const char* group, StudentArray* result);

int compare_by_id(const void* a, const void* b);
int compare_by_last_name(const void* a, const void* b);
int compare_by_first_name(const void* a, const void* b);
int compare_by_group(const void* a, const void* b);

return_code print_student_by_id_to_file(const StudentArray* array, unsigned int id, const char* filename);
return_code print_students_above_average_to_file(const StudentArray* array, const char* filename);

double calculate_overall_average(const StudentArray* array);
int is_latin_string(const char* str);
return_code add_student_to_array(StudentArray* array, const Student* student);