/*
### Exercise 4: Simple Heap Block Inspector (Metadata Simulation)
* **Concept:** How heap allocators work under the hood, metadata headers, pointer casting.
* **The Task:** Simulate a mini heap block header. Define a struct:
  ```c
  typedef struct BlockHeader {
      size_t size;
      int is_free;
      struct BlockHeader *next;
  } BlockHeader;
  ```
  Write a program that requests a large chunk of memory using `malloc` to act as your "arena", and implement a function `void *my_malloc(size_t size)` that searches your arena using a first-fit or best-fit algorithm, splits a block if it's too large, marks it as allocated, and returns a pointer to the payload *right after* the header.
*/

#include <stdlib.h>
#include <stdio.h>

typedef struct BlockHeader {
    size_t size;
    int is_free;
    struct BlockHeader *next;
} BlockHeader;

static BlockHeader *master = NULL;

void *initialize_arena(size_t size) {
    
    void *arena = malloc(size);
    master = arena;
    master->size = size - (sizeof(BlockHeader));
    master->is_free = 1;
    master->next = NULL;

    return arena;
}

void *my_alloc(size_t size) {
    
    BlockHeader *current_block = master;
    size_t remaining_size;
    char minimum_size = 1;

    while (current_block != NULL) {
        
        if (current_block->is_free == 1 && current_block->size >= (size + sizeof(BlockHeader) + minimum_size)) {
            
            remaining_size = current_block->size - size;
            BlockHeader *new_empty_block = (void *)current_block + size + sizeof(BlockHeader);
            new_empty_block->is_free = 1;
            new_empty_block->size = remaining_size - (sizeof(BlockHeader));
            new_empty_block->next = current_block->next;

            current_block->is_free = 0;
            current_block->size = size;
            current_block->next = new_empty_block;

            return (void *)current_block + sizeof(BlockHeader);
        }
        else if (current_block->is_free == 1 && current_block->size == size) {
            current_block->is_free = 0;
            return (void *)current_block + sizeof(BlockHeader);
        }

        current_block = current_block->next;
   }

   printf("Not enough memory in the arena!\n");
   return NULL;
}

void print_arena() {
    
    BlockHeader* current_block = master;
    int counter = 0;
    while (current_block != NULL) {
        printf("--- NODE %d ---\n", counter);
        printf("Size: %ld, Free: %d\n", current_block->size, current_block->is_free);
        current_block = current_block->next;
        counter++;
    }
}

int main() {

    void *arena = initialize_arena(1000);
    
    // Test allocating a couple of blocks
    void *block1 = my_alloc(200);
    void *block2 = my_alloc(300);

    if (block1 != NULL && block2 != NULL) {
        printf("Successfully allocated block1 and block2!\n");
    }

    print_arena();

    // Try an allocation that is too large for the remaining space
    void *block3 = my_alloc(600); // This should trigger "Not enough memory"

    free(arena);

    return 0;
}
