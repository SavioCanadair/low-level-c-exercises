/*
### Exercise 2: Implementing a Function Pointer Dispatch Table (Virtual Method Table Simulation)
* **Concept:** Function pointers, code pointers, struct-of-function-pointers patterns, and indirect calls.
* **Security Relevance:** Control Flow Integrity (CFI) research, understanding function pointer overwrite mitigations, and analyzing virtual table (vtable) hijacking vulnerabilities in C++ or custom C object models.
* **The Task:** Create a simple plugin or command execution engine using a struct containing function pointers.
  * Define a `CommandOps` struct containing function pointers for `init`, `execute`, and `cleanup`.
  * Implement two distinct modules (e.g., `NetworkModule` and `FileModule`) that populate a `CommandOps` instance with their own specialized functions.
  * Implement a dispatcher function that takes a pointer to `CommandOps` and an argument buffer, invoking the routine via indirect calls.
* **Detailed Steps to Follow:**
  1. **Define the Interface:**
     ```c
     typedef struct CommandOps {
         int (*init)(void **state);
         int (*execute)(void *state, const char *input);
         void (*cleanup)(void *state);
     } CommandOps;
     ```
  2. **Implement Modules:** Write concrete functions for `network_init`, `network_execute`, `network_cleanup`, and equivalent file-handling functions.
  3. **Dispatch & Polymorphism:** Write a function `int run_command(const CommandOps *ops, const char *input);` that abstracts away the underlying implementation.
  4. **Exploration & Reflection:** Print the actual memory addresses of the functions stored inside your struct using `%p` format specifiers. Observe how they point into the text (code) segment of your binary.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct CommandOps {
    int (*init)(void **state);
    int (*execute)(void *state, const char *input);
    void (*cleanup)(void *state);
} CommandOps;

typedef struct State {
    char type; // N for network, F for file
    char init_flag; // 0, 1 if in init status
    char exec_flag; // 0, 1 if in execute status
} State;

// Network module
int network_init(void **state) {
    
    State *network_state = malloc(sizeof(State));
    if (network_state == NULL) {
        printf("Error in memory allocation!\n");
        return 1;
    }

    network_state->type = 'N';
    network_state->init_flag = 1;
    network_state->exec_flag = 0;

    *state = network_state;
    return 0;
}

int network_execute(void *state, const char *input) {
    
    if (state == NULL || input == NULL) {
        printf("Missing inputs to execute function!\n");
        return 1;
    }

    ((State *)state)->init_flag = 0;
    ((State *)state)->exec_flag = 1;
    
    printf("Executing network command: %s\n", input);
    return 0;
}

void network_cleanup(void *state) {   
    free(state);
}

// File module
int file_init(void **state) {
    
    State *file_state = malloc(sizeof(State));
    if (file_state == NULL) {
        printf("Error in memory allocation!\n");
        return 1;
    }

    file_state->type = 'N';
    file_state->init_flag = 1;
    file_state->exec_flag = 0;

    *state = file_state;
    return 0;
}

int file_execute(void *state, const char *input) {
    
    if (state == NULL || input == NULL) {
        printf("Missing inputs to execute function!\n");
        return 1;
    }

    ((State *)state)->init_flag = 0;
    ((State *)state)->exec_flag = 1;

    printf("Executing file command: %s\n", input);
    return 0;
}

void file_cleanup(void *state) {
    free(state);
}

// Abstract function
int run_command(const CommandOps *ops, const char *input) {
       
    void *state = NULL;

    // To run a command I init, then execute and then cleanup
    printf("Running init function at address: %p\n", (void *)ops->init);
    int res = ops->init(&state);
    if (res == 1) {
        printf("Error in init\n");
        return 1;
    }
    printf("Running exec function at address: %p\n", (void *)ops->execute);
    res = ops->execute(state, input); // if execute fails I still need cleanup
    printf("Running clean function at address: %p\n", (void *)ops->cleanup);
    ops->cleanup(state);
    if (res == 1) {
        printf("Error in execute\n");
        return 1;
    }

    return 0;
}

int main() {

    // 1. Instantiate the Network Module vtable
    CommandOps network_ops = {
        .init = network_init,
        .execute = network_execute,
        .cleanup = network_cleanup
    };

    // 2. Instantiate the File Module vtable
    CommandOps file_ops = {
        .init = file_init,
        .execute = file_execute,
        .cleanup = file_cleanup
    };

    printf("=== Testing Network Module Dispatcher ===\n");
    run_command(&network_ops, "GET /index.html HTTP/1.1");

    printf("\n=== Testing File Module Dispatcher ===\n");
    run_command(&file_ops, "/etc/passwd");

    return 0;
}
