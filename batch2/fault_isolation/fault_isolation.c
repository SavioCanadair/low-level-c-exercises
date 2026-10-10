/*
### Exercise 5: Software Fault Isolation (SFI) / Sandboxed Array Access Wrapper
* **Concept:** Bounds checking wrappers, defensive programming, defensive pointer arithmetic, and mitigating out-of-bounds reads/writes.
* **Security Relevance:** Writing hardened systems code, understanding software-based fault isolation, and implementing safe wrappers for low-level memory operations.
* **The Task:** Write a robust, bounds-checked wrapper library for fixed-size buffer operations that prevents any out-of-bounds access.
  * Create a wrapper struct:
    ```c
    typedef struct {
        void *buffer;
        size_t capacity;
        size_t element_size;
    } SafeBuffer;
    ```
  * Implement `int safe_write(SafeBuffer *sb, size_t index, const void *src, size_t size);` and `int safe_read(const SafeBuffer *sb, size_t index, void *dest, size_t size);`.
  * Ensure that any attempt to read or write past `capacity` or trigger integer overflow during index multiplication (`index * element_size`) is intercepted and rejected securely.
* **Detailed Steps to Follow:**
  1. **Multiplication Overflow Protection:** Before performing pointer arithmetic like `buffer + (index * element_size)`, check if `index > SIZE_MAX / element_size` to prevent integer wrap-around vulnerabilities.
  2. **Explicit Assertions & Error Codes:** Return failure codes (`-1`) instead of executing pointer dereferences if indices are out of range.
  3. **Unit Testing Edge Cases:** Write malicious test inputs (e.g., massive index values close to `SIZE_MAX`, negative indices cast to `size_t`) to prove your wrapper safely rejects them without corruption.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct SafeBuffer {
    void *buffer;
    size_t capacity;
    size_t element_size;
} SafeBuffer;

int safe_write(SafeBuffer *sb, size_t index, const void *src, size_t size) {
    
    if (sb == NULL || src == NULL) {
        return -1; // Missing input pointers
    }

    if (index > SIZE_MAX / (sb->element_size)) {
        return -4; // Index too big
    }

    if (index >= sb->capacity) {
        return -2; // Index larger than maximum buffer capacity
    }

    if (size > sb->element_size) {
        return -3; // Size of element to write larger than buffer element size
    }
    
    void *buffer_address = (uint8_t *)sb->buffer + index*(sb->element_size);
    memcpy(buffer_address, src, size);

    return 0;
}

int safe_read(const SafeBuffer *sb, size_t index, void *dst, size_t size) {
    
    if (sb == NULL || dst == NULL) {
        return -1; // Missing input pointers
    }

    if (index > SIZE_MAX / (sb->element_size)) {
        return -4; // Index too big
    }

    if (index >= sb->capacity) {
        return -2; // Reading outside buffer capacity
    }

    if (size > sb->element_size) {
        return -3; // Size of element to copy is bigger than buffer element size
    }

    void *buffer_address = (uint8_t *)sb->buffer + index*(sb->element_size);
    memcpy(dst, buffer_address, size);
    
    return 0;
}

int main() {
    // Setup a SafeBuffer holding 4 elements of 4 bytes each (e.g., an array of 4 ints)
    size_t capacity = 4;
    size_t element_size = sizeof(int);

    // Allocate raw memory for the buffer
    int raw_buffer[4] = { 0 };
    SafeBuffer sb = {
        .buffer = raw_buffer,
        .capacity = capacity,
        .element_size = element_size
    };

    printf("--- Starting SafeBuffer Test Suite ---\n");

    // --- Test 1: Valid Write and Read ---
    int write_val = 42;
    int read_val = 0;

    if (safe_write(&sb, 2, &write_val, sizeof(int)) == 0) {
        printf("[PASS] Test 1a: Successfully wrote to index 2.\n");
    } else {
        printf("[FAIL] Test 1a: Valid write failed.\n");
    }

    if (safe_read(&sb, 2, &read_val, sizeof(int)) == 0 && read_val == 42) {
        printf("[PASS] Test 1b: Successfully read back correct value (42) from index 2.\n");
    } else {
        printf("[FAIL] Test 1b: Valid read failed or returned incorrect data.\n");
    }

    // --- Test 2: Out-of-Bounds Index Check ---
    // Index 4 is out of bounds (valid indices are 0 to 3)
    if (safe_write(&sb, 4, &write_val, sizeof(int)) == -2) {
        printf("[PASS] Test 2: Correctly rejected out-of-bounds index (returned -2).\n");
    } else {
        printf("[FAIL] Test 2: Failed to catch out-of-bounds index.\n");
    }

    // --- Test 3: Size Mismatch Check ---
    // Trying to write too many bytes for a single element slot
    long long oversized_data = 9999;
    if (safe_write(&sb, 0, &oversized_data, sizeof(long long)) == -3) {
        printf("[PASS] Test 3: Correctly rejected oversized data write (returned -3).\n");
    } else {
        printf("[FAIL] Test 3: Failed to catch size mismatch.\n");
    }

    // --- Test 4: Integer Overflow Protection Check ---
    // Pass a massive index near SIZE_MAX to trigger the division check
    size_t malicious_index = SIZE_MAX;
    if (safe_write(&sb, malicious_index, &write_val, sizeof(int)) == -4) {
        printf("[PASS] Test 4: Successfully intercepted multiplication overflow (returned -4).\n");
    } else {
        printf("[FAIL] Test 4: Failed to catch integer overflow.\n");
    }

    return 0;
}
