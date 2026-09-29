/*
### Exercise 3: Safe String Splitter (Heap Allocation & Manual Parsing)
* **Concept:** Manual string manipulation, pointer arithmetic, multi-level pointer arrays (`char **`), and preventing buffer overflows.
* **The Task:** Write a function that mimics a basic `split` utility:
  * `char **split_string(const char *str, char delimiter, int *out_count);`
  * It must scan the input string, dynamically allocate an array of strings (`char **`), allocate memory for each individual substring separated by the delimiter, populate them, and return the array.
  * Write a companion function `void free_split_result(char **arr, int count);` to ensure zero memory leaks.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

char **split_string(const char *str, char delimiter, int *out_count) {
    
    if (str == NULL || *str == '\0') {
        *out_count = 0;
        return NULL;
    }

    const char *p = str;
    *out_count = 0;

    while (*p != '\0') {
        
        while (*p == delimiter) {
            // Jump starting delimiters
            p++;
        }

        if (*p == '\0') {
            break;
        }

        while (*p != delimiter && *p != '\0') {
            // We're in a substring, so we reach the end of it
            p++;
        }

        (*out_count)++;
    }

    if (*out_count == 0) {
        return NULL;
    }
    
    char **substring_array = malloc(sizeof(char*) * (*out_count));
    if (substring_array == NULL) {
        printf("Error in memory allocation!\n");
        return NULL;
    }
    
    int substring_length = 0;
    int substring_counter = 0; // how many substring are we at
    p = str;
    while (*p != '\0') {
        
        while (*p == delimiter) {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        while (*p != delimiter && *p != '\0') {
            // Looping on the substring
            substring_length++;
            p++;
        }

        char *substring = malloc(sizeof(char) * (substring_length + 1));
        if (substring == NULL) {
            printf("Error in memory allocation!\n");
            for (int i = 0; i < substring_counter; i++) {
                free(substring_array[i]);
            }
            free(substring_array);
            return NULL;
        }

        memcpy(substring, p - substring_length, substring_length);
        substring[substring_length] = '\0';
        substring_array[substring_counter] = substring;
        substring_length = 0;
        substring_counter++;
    }

    return substring_array;
}

void free_split_result(char **arr, int count) {

    if (arr == NULL) {
        return;
    }

    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
}

int main() {
    int count = 0;
    char **result = NULL;

    printf("Running tests...\n");

    // --- Test 1: Standard multi-token string ---
    result = split_string("apple,banana,cherry", ',', &count);
    assert(result != NULL);
    assert(count == 3);
    assert(strcmp(result[0], "apple") == 0);
    assert(strcmp(result[1], "banana") == 0);
    assert(strcmp(result[2], "cherry") == 0);
    free_split_result(result, count);

    // --- Test 2: Single token without delimiters ---
    result = split_string("hello", ',', &count);
    assert(result != NULL);
    assert(count == 1);
    assert(strcmp(result[0], "hello") == 0);
    free_split_result(result, count);

    // --- Test 3: Custom delimiter (semicolon) ---
    result = split_string("one;two;three;four", ';', &count);
    assert(result != NULL);
    assert(count == 4);
    assert(strcmp(result[0], "one") == 0);
    assert(strcmp(result[3], "four") == 0);
    free_split_result(result, count);

    // --- Test 4: Empty string edge case ---
    result = split_string("", ',', &count);
    assert(result == NULL);
    assert(count == 0);

    // --- Test 5: String composed entirely of delimiters ---
    result = split_string(",,,,", ',', &count);
    assert(result == NULL);
    assert(count == 0);
    
    // --- Test 6: All strings are single chars --
    result = split_string("a,b,c,d", ',', &count);
    assert(result != NULL);
    assert(count == 4);
    assert(strcmp(result[0], "a") == 0);
    assert(strcmp(result[1], "b") == 0);
    assert(strcmp(result[2], "c") == 0);
    assert(strcmp(result[3], "d") == 0);
    free_split_result(result, count);

    printf("All tests passed successfully!\n");
    return 0;
}
