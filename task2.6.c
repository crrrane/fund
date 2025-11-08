#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "task2.6.h"

#ifdef _WIN32
    #include <windows.h>
#endif

int is_id_unique(const StudentArray* array, unsigned int id) {
    if (array == NULL) return 1;
    
    for (int i = 0; i < array->count; i++) {
        if (array->students[i].id == id) {
            return 0;
        }
    }
    return 1;
}

int is_latin_string(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    for (const char* p = str; *p; p++) {
        if (!isalpha((unsigned char)*p)) {
            return 0;
        }
    }
    return 1;
}

return_code add_student_to_array(StudentArray* array, const Student* student) {
    if (array == NULL || student == NULL) {
        return INVALID_INPUT;
    }
    
    if (!is_id_unique(array, student->id)) {
        return INVALID_INPUT;
    }
    
    if (array->count >= array->capacity) {
        int new_capacity = (array->capacity == 0) ? 10 : array->capacity * 2;
        Student* new_students = realloc(array->students, new_capacity * sizeof(Student));
        if (new_students == NULL) {
            return ERROR_MEMORY;
        }
        array->students = new_students;
        array->capacity = new_capacity;
    }
    
    Student* dest = &array->students[array->count];

    dest->id = student->id;
    dest->grades_count = student->grades_count;
    dest->average_grade = student->average_grade;

    dest->first_name = malloc(strlen(student->first_name) + 1);
    dest->last_name = malloc(strlen(student->last_name) + 1);
    dest->group = malloc(strlen(student->group) + 1);
    
    if (dest->first_name == NULL || dest->last_name == NULL || dest->group == NULL) {
        free(dest->first_name);
        free(dest->last_name);
        free(dest->group);
        return ERROR_MEMORY;
    }
    
    strcpy(dest->first_name, student->first_name);
    strcpy(dest->last_name, student->last_name);
    strcpy(dest->group, student->group);

    if (student->grades_count > 0) {
        dest->grades = malloc(student->grades_count * sizeof(unsigned char));
        if (dest->grades == NULL) {
            free(dest->first_name);
            free(dest->last_name);
            free(dest->group);
            return ERROR_MEMORY;
        }
        memcpy(dest->grades, student->grades, student->grades_count * sizeof(unsigned char));
    } else {
        dest->grades = NULL;
    }
    
    array->count++;
    return OK;
}

static void trim_whitespace_inplace(char* s) {
    if (s == NULL) return;
    char* start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

return_code read_students_from_file(const char* filename, StudentArray* array) {
    if (filename == NULL || array == NULL) return INVALID_INPUT;
    FILE* file = fopen(filename, "r");
    if (file == NULL) return ERROR_OPEN_FILE;

    array->students = NULL;
    array->count = 0;
    array->capacity = 0;

    char line[1024];
    int line_number = 0;
    char incomplete_line[2048] = "";

    while (fgets(line, sizeof(line), file) != NULL) {
        line_number++;
        if (strlen(line) == 0 || (strlen(line) == 1 && line[0] == '\n')) continue;

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[--len] = '\0';
        if (len > 0 && line[len - 1] == '\r') line[--len] = '\0';

        char full_line[4096];
        if (strlen(incomplete_line) > 0) {
            snprintf(full_line, sizeof(full_line), "%s%s", incomplete_line, line);
            incomplete_line[0] = '\0';
        } else {
            strcpy(full_line, line);
        }

        int field_count = 0;
        char test_line[4096];
        strcpy(test_line, full_line);
        char* test_token = strtok(test_line, ";");
        while (test_token != NULL) {
            field_count++;
            test_token = strtok(NULL, ";");
        }

        if (field_count < 5) {
            size_t cur_len = strlen(incomplete_line);
            size_t add_len = strlen(full_line);
            if (cur_len + add_len + 2 < sizeof(incomplete_line)) {
                strcat(incomplete_line, full_line);
                if (cur_len > 0 && incomplete_line[cur_len - 1] != ';')
                    strcat(incomplete_line, ";");
            } else {
                fprintf(stderr, "Too long incomplete record at line %d, skipping.\n", line_number);
                incomplete_line[0] = '\0';
            }
            continue;
        }

        Student student = {0};
        char line_copy[4096];
        strcpy(line_copy, full_line);

        char* token = strtok(line_copy, ";");
        int actual_field_count = 0;
        int valid_record = 1;

        while (token != NULL && valid_record) {
            trim_whitespace_inplace(token);
            switch (actual_field_count) {
                case 0: {
                    char* endptr;
                    unsigned long id = strtoul(token, &endptr, 10);
                    if (endptr == token || *endptr != '\0' || id > UINT_MAX || !is_id_unique(array, (unsigned int)id)) {
                        fprintf(stderr, "Error parsing ID at line %d: '%s' (must be unique positive integer)\n", line_number, token);
                        valid_record = 0;
                    } else student.id = (unsigned int)id;
                } break;

                case 1:
                    if (!strlen(token) || !is_latin_string(token)) {
                        fprintf(stderr, "Invalid first name at line %d: '%s'\n", line_number, token);
                        valid_record = 0;
                    } else {
                        student.first_name = malloc(strlen(token) + 1);
                        strcpy(student.first_name, token);
                    }
                    break;

                case 2:
                    if (!strlen(token) || !is_latin_string(token)) {
                        fprintf(stderr, "Invalid last name at line %d: '%s'\n", line_number, token);
                        valid_record = 0;
                        free(student.first_name);
                        student.first_name = NULL;
                    } else {
                        student.last_name = malloc(strlen(token) + 1);
                        strcpy(student.last_name, token);
                    }
                    break;

                case 3:
                    if (!strlen(token)) {
                        fprintf(stderr, "Empty group at line %d\n", line_number);
                        valid_record = 0;
                        free(student.first_name);
                        free(student.last_name);
                    } else {
                        student.group = malloc(strlen(token) + 1);
                        strcpy(student.group, token);
                    }
                    break;

                case 4: {
                    char grades_copy[256];
                    strcpy(grades_copy, token);
                    char* grade_token = strtok(grades_copy, ",");
                    int grade_count = 0;
                    int grades_valid = 1;
                    student.grades = malloc(5 * sizeof(unsigned char));
                    while (grade_token != NULL && grades_valid && grade_count < 5) {
                        trim_whitespace_inplace(grade_token);
                        char* endptr;
                        unsigned long grade = strtoul(grade_token, &endptr, 10);
                        if (endptr == grade_token || *endptr != '\0' || grade > 255) {
                            fprintf(stderr, "Invalid grade at line %d: '%s'\n", line_number, grade_token);
                            grades_valid = 0;
                        } else student.grades[grade_count++] = (unsigned char)grade;
                        grade_token = strtok(NULL, ",");
                    }
                    if (!grades_valid || grade_count != 5) {
                        fprintf(stderr, "Invalid number of grades at line %d: expected 5, got %d\n", line_number, grade_count);
                        free(student.grades);
                        student.grades = NULL;
                        valid_record = 0;
                    } else {
                        student.grades_count = 5;
                        double sum = 0;
                        for (int i = 0; i < 5; i++) sum += student.grades[i];
                        student.average_grade = sum / 5.0;
                    }
                } break;
            }
            if (valid_record) {
                actual_field_count++;
                token = strtok(NULL, ";");
            }
        }

        if (valid_record && actual_field_count == 5) {
            if (add_student_to_array(array, &student) != OK) {
                fprintf(stderr, "Error adding student at line %d\n", line_number);
                free(student.first_name);
                free(student.last_name);
                free(student.group);
                free(student.grades);
            }
        } else {
            free(student.first_name);
            free(student.last_name);
            free(student.group);
            free(student.grades);
        }
    }

    if (strlen(incomplete_line) > 0)
        fprintf(stderr, "Incomplete record at end of file: '%s'\n", incomplete_line);

    fclose(file);
    return OK;
}


return_code free_student_array(StudentArray* array) {
    if (array == NULL) {
        return INVALID_INPUT;
    }
    
    for (int i = 0; i < array->count; i++) {
        free(array->students[i].first_name);
        free(array->students[i].last_name);
        free(array->students[i].group);
        free(array->students[i].grades);
    }
    
    free(array->students);
    array->students = NULL;
    array->count = 0;
    array->capacity = 0;
    
    return OK;
}

typedef int (*StudentPredicate)(const Student*, const void*);

return_code find_students(const StudentArray* array, StudentArray* result, 
                         StudentPredicate predicate, const void* criteria) {
    if (array == NULL || result == NULL || predicate == NULL) {
        return INVALID_INPUT;
    }
    
    result->students = NULL;
    result->count = 0;
    result->capacity = 0;
    
    for (int i = 0; i < array->count; i++) {
        if (predicate(&array->students[i], criteria)) {
            return_code code = add_student_to_array(result, &array->students[i]);
            if (code != OK) {
                free_student_array(result);
                return code;
            }
        }
    }
    
    return (result->count > 0) ? OK : ERROR_NOT_FOUND;
}

int predicate_by_id(const Student* student, const void* criteria) {
    unsigned int id = *(const unsigned int*)criteria;
    return student->id == id;
}

int predicate_by_last_name(const Student* student, const void* criteria) {
    const char* last_name = (const char*)criteria;
    return strcmp(student->last_name, last_name) == 0;
}

int predicate_by_first_name(const Student* student, const void* criteria) {
    const char* first_name = (const char*)criteria;
    return strcmp(student->first_name, first_name) == 0;
}

int predicate_by_group(const Student* student, const void* criteria) {
    const char* group = (const char*)criteria;
    return strcmp(student->group, group) == 0;
}

return_code find_students_by_id(const StudentArray* array, unsigned int id, StudentArray* result) {
    return find_students(array, result, predicate_by_id, &id);
}

return_code find_students_by_last_name(const StudentArray* array, const char* last_name, StudentArray* result) {
    return find_students(array, result, predicate_by_last_name, last_name);
}

return_code find_students_by_first_name(const StudentArray* array, const char* first_name, StudentArray* result) {
    return find_students(array, result, predicate_by_first_name, first_name);
}

return_code find_students_by_group(const StudentArray* array, const char* group, StudentArray* result) {
    return find_students(array, result, predicate_by_group, group);
}

int compare_by_id(const void* a, const void* b) {
    const Student* student_a = (const Student*)a;
    const Student* student_b = (const Student*)b;
    return (student_a->id > student_b->id) - (student_a->id < student_b->id);
}

int compare_by_string_field(const void* a, const void* b, const char* (*get_field)(const Student*)) {
    const Student* student_a = (const Student*)a;
    const Student* student_b = (const Student*)b;
    const char* field_a = get_field(student_a);
    const char* field_b = get_field(student_b);
    return strcmp(field_a, field_b);
}

const char* get_last_name(const Student* student) {
    return student->last_name;
}

const char* get_first_name(const Student* student) {
    return student->first_name;
}

const char* get_group(const Student* student) {
    return student->group;
}

int compare_by_last_name(const void* a, const void* b) {
    return compare_by_string_field(a, b, get_last_name);
}

int compare_by_first_name(const void* a, const void* b) {
    return compare_by_string_field(a, b, get_first_name);
}

int compare_by_group(const void* a, const void* b) {
    return compare_by_string_field(a, b, get_group);
}

double calculate_overall_average(const StudentArray* array) {
    if (array == NULL || array->count == 0) {
        return 0.0;
    }
    
    double total_sum = 0.0;
    int total_grades = 0;
    
    for (int i = 0; i < array->count; i++) {
        total_sum += array->students[i].average_grade * array->students[i].grades_count;
        total_grades += array->students[i].grades_count;
    }
    
    return (total_grades > 0) ? total_sum / total_grades : 0.0;
}

return_code print_student_by_id_to_file(const StudentArray* array, unsigned int id, const char* filename) {
    if (array == NULL || filename == NULL) {
        return INVALID_INPUT;
    }
    
    StudentArray result;
    return_code code = find_students_by_id(array, id, &result);
    if (code != OK) {
        return code;
    }
    
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        free_student_array(&result);
        return ERROR_OPEN_FILE;
    }
    
    for (int i = 0; i < result.count; i++) {
        fprintf(file, "Student found by ID %u:\n", id);
        fprintf(file, "Name: %s %s\n", result.students[i].first_name, result.students[i].last_name);
        fprintf(file, "Group: %s\n", result.students[i].group);
        fprintf(file, "Average grade: %.2f\n", result.students[i].average_grade);
        fprintf(file, "Grades: ");
        for (int j = 0; j < result.students[i].grades_count; j++) {
            fprintf(file, "%u", result.students[i].grades[j]);
            if (j < result.students[i].grades_count - 1) {
                fprintf(file, ", ");
            }
        }
        fprintf(file, "\n\n");
    }
    
    fclose(file);
    free_student_array(&result);
    return OK;
}

return_code print_students_above_average_to_file(const StudentArray* array, const char* filename) {
    if (array == NULL || filename == NULL) {
        return INVALID_INPUT;
    }
    
    double overall_average = calculate_overall_average(array);
    
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        return ERROR_OPEN_FILE;
    }
    
    fprintf(file, "Students with average grade above overall average (%.2f):\n", overall_average);
    
    int found = 0;
    for (int i = 0; i < array->count; i++) {
        if (array->students[i].average_grade > overall_average) {
            fprintf(file, "%s %s (Average: %.2f)\n", 
                   array->students[i].last_name, 
                   array->students[i].first_name,
                   array->students[i].average_grade);
            found = 1;
        }
    }
    
    if (!found) {
        fprintf(file, "No students found with average grade above overall average.\n");
    }
    
    fprintf(file, "\n");
    fclose(file);
    return OK;
}

return_code check_different_files(const char* file1, const char* file2) {
    if (file1 == NULL || file2 == NULL) {
        return INVALID_INPUT;
    }
    
    char abs_file1[PATH_MAX];
    char abs_file2[PATH_MAX];

    #ifdef _WIN32
        if (_fullpath(abs_file1, file1, PATH_MAX) == NULL) { 
            return OK;
        }
        if (_fullpath(abs_file2, file2, PATH_MAX) == NULL){
            return OK;
        }
    #else
        if (realpath(file1, abs_file1) == NULL){
            return OK;
        } 
        if (realpath(file2, abs_file2) == NULL) {
            return OK;
        }
    #endif
    
    if (strcmp(abs_file1, abs_file2) == 0) {
        return INVALID_INPUT;
    }
    return OK;
}

void print_menu() {
    printf("\n=== Student Management System ===\n");
    printf("1. Find students by ID\n");
    printf("2. Find students by last name\n");
    printf("3. Find students by first name\n");
    printf("4. Find students by group\n");
    printf("5. Sort students by ID\n");
    printf("6. Sort students by last name\n");
    printf("7. Sort students by first name\n");
    printf("8. Sort students by group\n");
    printf("9. Print student by ID to trace file\n");
    printf("10. Print students above average to trace file\n");
    printf("11. Display all students\n");
    printf("0. Exit\n");
    printf("Choose an option: ");
}

void display_students(const StudentArray* array) {
    if (array == NULL || array->count == 0) {
        printf("No students to display.\n");
        return;
    }
    
    printf("\n%-5s %-15s %-15s %-10s %-8s %s\n", 
           "ID", "Last Name", "First Name", "Group", "Avg", "Grades");
    printf("----------------------------------------------------------------\n");
    
    for (int i = 0; i < array->count; i++) {
        printf("%-5u %-15s %-15s %-10s %-8.2f ", 
               array->students[i].id,
               array->students[i].last_name,
               array->students[i].first_name,
               array->students[i].group,
               array->students[i].average_grade);
        
        for (int j = 0; j < array->students[i].grades_count; j++) {
            printf("%u", array->students[i].grades[j]);
            if (j < array->students[i].grades_count - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <students_file> [trace_file]\n", argv[0]);
        return INVALID_INPUT;
    }
    
    const char* students_file = argv[1];
    const char* trace_file = (argc >= 3) ? argv[2] : "trace.txt";

    return_code file_check = check_different_files(students_file, trace_file);
    if (file_check != OK) {
        printf("Error: Students file and trace file cannot be the same!\n");
        return INVALID_INPUT;
    }
    
    StudentArray students;
    return_code code = read_students_from_file(students_file, &students);
    
    if (code != OK) {
        printf("Error reading students file: %s\n", students_file);
        return code;
    }
    
    printf("Successfully loaded %d students from %s\n", students.count, students_file);
    printf("Trace file: %s\n", trace_file);
    
    int choice;
    char buffer[256];
    StudentArray search_result;
    
    do {
        print_menu();
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        char* endptr;
        choice = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\n') {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        switch (choice) {
            case 1: {
                unsigned int id;
                printf("Enter student ID: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    char* endptr;
                    id = strtoul(buffer, &endptr, 10);
                    if (endptr == buffer || *endptr != '\n') {
                        printf("Invalid ID format. Please enter a positive integer.\n");
                    } else {
                        code = find_students_by_id(&students, id, &search_result);
                        if (code == OK) {
                            display_students(&search_result);
                            free_student_array(&search_result);
                        } else {
                            printf("Student with ID %u not found.\n", id);
                        }
                    }
                }
                break;
            }
            
            case 2: {
                char last_name[100];
                printf("Enter last name: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    size_t len = strlen(buffer);
                    if (len > 0 && buffer[len - 1] == '\n') {
                        buffer[len - 1] = '\0';
                    }
                    if (strlen(buffer) > 0) {
                        strcpy(last_name, buffer);
                        code = find_students_by_last_name(&students, last_name, &search_result);
                        if (code == OK) {
                            display_students(&search_result);
                            free_student_array(&search_result);
                        } else {
                            printf("Students with last name '%s' not found.\n", last_name);
                        }
                    } else {
                        printf("Empty last name.\n");
                    }
                }
                break;
            }
            
            case 3: {
                char first_name[100];
                printf("Enter first name: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    size_t len = strlen(buffer);
                    if (len > 0 && buffer[len - 1] == '\n') {
                        buffer[len - 1] = '\0';
                    }
                    if (strlen(buffer) > 0) {
                        strcpy(first_name, buffer);
                        code = find_students_by_first_name(&students, first_name, &search_result);
                        if (code == OK) {
                            display_students(&search_result);
                            free_student_array(&search_result);
                        } else {
                            printf("Students with first name '%s' not found.\n", first_name);
                        }
                    } else {
                        printf("Empty first name.\n");
                    }
                }
                break;
            }
            
            case 4: {
                char group[100];
                printf("Enter group: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    size_t len = strlen(buffer);
                    if (len > 0 && buffer[len - 1] == '\n') {
                        buffer[len - 1] = '\0';
                    }
                    if (strlen(buffer) > 0) {
                        strcpy(group, buffer);
                        code = find_students_by_group(&students, group, &search_result);
                        if (code == OK) {
                            display_students(&search_result);
                            free_student_array(&search_result);
                        } else {
                            printf("Students in group '%s' not found.\n", group);
                        }
                    } else {
                        printf("Empty group.\n");
                    }
                }
                break;
            }
            
            case 5:
                qsort(students.students, students.count, sizeof(Student), compare_by_id);
                printf("Students sorted by ID.\n");
                display_students(&students);
                break;
                
            case 6:
                qsort(students.students, students.count, sizeof(Student), compare_by_last_name);
                printf("Students sorted by last name.\n");
                display_students(&students);
                break;
                
            case 7:
                qsort(students.students, students.count, sizeof(Student), compare_by_first_name);
                printf("Students sorted by first name.\n");
                display_students(&students);
                break;
                
            case 8:
                qsort(students.students, students.count, sizeof(Student), compare_by_group);
                printf("Students sorted by group.\n");
                display_students(&students);
                break;
                
            case 9: {
                unsigned int id;
                printf("Enter student ID to trace: ");
                if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                    char* endptr;
                    id = strtoul(buffer, &endptr, 10);
                    if (endptr == buffer || *endptr != '\n') {
                        printf("Invalid ID format. Please enter a positive integer.\n");
                    } else {
                        code = print_student_by_id_to_file(&students, id, trace_file);
                        if (code == OK) {
                            printf("Student data written to trace file.\n");
                        } else {
                            printf("Student with ID %u not found.\n", id);
                        }
                    }
                }
                break;
            }
            
            case 10:
                code = print_students_above_average_to_file(&students, trace_file);
                if (code == OK) {
                    printf("Students above average written to trace file.\n");
                } else {
                    printf("Error writing to trace file.\n");
                }
                break;
                
            case 11:
                display_students(&students);
                break;
                
            case 0:
                printf("Exiting...\n");
                break;
                
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
        
    } while (choice != 0);
    
    free_student_array(&students);
    return OK;
}