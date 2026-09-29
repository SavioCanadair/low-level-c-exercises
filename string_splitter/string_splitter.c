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

    char substring_being_built = FALSE; // Define if a substring is being built 
    *out_count = 0;
    char all_delimiters = TRUE;

    // First loop to count the substrings
    for (int i = 0; str[i] != '\0'; i++) {
        
        if (str[i] != delimiter) {
            all_delimiters = FALSE;
        }

        if ((str[i] == delimiter || str[i+1] == '\0') && substring_being_built == TRUE) {
            *out_count += 1;
            substring_being_built = FALSE;
        }
        else if (str[i] != delimiter && substring_being_built == FALSE) {
            substring_being_built = TRUE;
        }
    }
    if (substring_being_built == TRUE) {
        *out_count += 1;
    }
    
    if (all_delimiters == TRUE) {
        *out_count = 0;
        return NULL;
    }

    char **string_array = malloc(sizeof(char*) * (*out_count));
    if (string_array == NULL) {
        printf("Memory allocation error!\n");
        return NULL;
    }
    int substring_index = 0;
    
    int start_index = 0;
    int end_index = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        
        if ((str[i] == delimiter || str[i+1] == '\0') && substring_being_built == TRUE) {
            end_index = i;
            if (str[i+1] == '\0' && str[i] != delimiter) {
                end_index += 1;
            }
            // Build the substring
            char *substring = malloc(sizeof(char) * (end_index - start_index + 1));
            if (substring == NULL) {
                printf("Memory allocation error!\n");
                free(string_array);
                return NULL;
            }
            memcpy(substring, str + start_index, end_index - start_index);
            substring[end_index-start_index] = '\0';
            string_array[substring_index] = substring;
            substring_index += 1;
            substring_being_built = FALSE;
        }
        else if (str[i] != delimiter && substring_being_built == FALSE) {
            start_index = i;
            end_index = i; // Set minimum end index
            substring_being_built = TRUE;
        }
    }
    // Handle pending string
    if (substring_being_built == TRUE) {
        end_index += 1;
        // Build the substring
        char *substring = malloc(sizeof(char) * (end_index - start_index + 1));
        if (substring == NULL) {
            printf("Memory allocation error!\n");
            free(string_array);
            return NULL;
        }
        memcpy(substring, str + start_index, end_index - start_index);
        substring[end_index-start_index] = '\0';
        string_array[substring_index] = substring;
        substring_index += 1;
    }

    return string_array;
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
