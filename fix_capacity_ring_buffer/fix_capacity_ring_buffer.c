/*
### Exercise 2: Implementing a Fixed-Capacity Ring Buffer (Circular Queue)
* **Concept:** Raw memory blocks, modular arithmetic indices, fixed-size constraints (crucial for OS kernel queues and network drivers).
* **The Task:** Create a `RingBuffer` struct that manages a heap-allocated array of integers of a fixed capacity $N$. Implement:
  * `RingBuffer *create_buffer(int capacity);`
  * `int write_buffer(RingBuffer *buf, char value);` (Returns 0 on success, -1 if full).
  * `int read_buffer(RingBuffer *buf, char *out_value);` (Returns 0 on success, -1 if empty).
  * `void free_buffer(RingBuffer *buf);`
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct RingBuffer {
    char *data; // Pointer to data storage
    int write_position; // Index tracking where to write new data
    int read_position; // Index tracking where to read next item
    int capacity; // Maximum size
    int count; // Current number of elements
} RingBuffer;

RingBuffer *create_buffer(int capacity) {
    
    RingBuffer *buffer = malloc(sizeof(RingBuffer));
    if (buffer == NULL) { // Check failed allocation
        printf("Memory allocation failed!\n");
        return NULL;
    }

    buffer->data = malloc(sizeof(char) * capacity);
    if (buffer->data == NULL) { // Check failed allocation
        printf("Memory allocation failed!\n");
        free(buffer);
        return NULL;
    }
    buffer->capacity = capacity;
    buffer->write_position = 0;
    buffer->read_position = 0;
    buffer->count = 0;

    return buffer;
}

int write_buffer(RingBuffer *buf, char value) {
    
    if (buf == NULL) {
        return -1;
    }

    if (buf->count >= buf->capacity) {
        return -1;
    }
    
    buf->data[buf->write_position] = value;
    buf->count += 1;
    
    if (buf->write_position >= buf->capacity - 1) {
        buf->write_position = 0;
    }
    else {
        buf->write_position += 1;
    }

    return 0;
}

int read_buffer(RingBuffer *buf, char *out_value) {
    
    if (buf == NULL || out_value == NULL) {
        return -1;
    }

    if (buf->count == 0) {
        return -1;
    }
    
    *out_value = buf->data[buf->read_position];
    buf->count -= 1;
    
    if (buf->read_position >= buf->capacity -1) {
        buf->read_position = 0;
    }
    else {
        buf->read_position += 1;
    }
    
    return 0;
}

void free_buffer(RingBuffer *buf) {   
    
    if (buf == NULL) {
        return;
    }

    free(buf->data);
    free(buf);
}

int main() {
    
    printf("Starting Ring Buffer test suite...\n");

    // Test 1: Initialize a small buffer with capacity 3
    RingBuffer *buf = create_buffer(3);
    assert(buf != NULL);
    assert(buf->capacity == 3);
    assert(buf->count == 0);

    // Test 2: Write items up to full capacity ('A', 'B', 'C')
    assert(write_buffer(buf, 'A') == 0);
    assert(write_buffer(buf, 'B') == 0);
    assert(write_buffer(buf, 'C') == 0);
    assert(buf->count == 3);

    // Test 3: Try writing to a full buffer (should fail and return -1)
    assert(write_buffer(buf, 'D') == -1);

    // Test 4: Read items back and verify FIFO order ('A', then 'B')
    char val;
    assert(read_buffer(buf, &val) == 0);
    assert(val == 'A');
    assert(buf->count == 2);

    assert(read_buffer(buf, &val) == 0);
    assert(val == 'B');
    assert(buf->count == 1);

    // Test 5: Test wrap-around! 
    // The buffer currently has space because we read 2 items. 
    // Writing 'D' should force the write position to wrap around to the beginning of the array.
    assert(write_buffer(buf, 'D') == 0);
    assert(buf->count == 2);

    // Test 6: Read remaining items to verify wrap-around order ('C', then 'D')
    assert(read_buffer(buf, &val) == 0);
    assert(val == 'C');

    assert(read_buffer(buf, &val) == 0);
    assert(val == 'D');
    assert(buf->count == 0); // Should be empty now

    // Test 7: Try reading from an empty buffer (should fail and return -1)
    assert(read_buffer(buf, &val) == -1);

    // Test 8: Clean up memory safely
    free_buffer(buf);

    printf("SUCCESS: All ring buffer assertions passed!\n");
    return 0;
}
