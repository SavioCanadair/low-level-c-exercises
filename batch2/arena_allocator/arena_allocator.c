/*
### Exercise 1: Custom Arena Allocator with Boundary Tags and Coalescing
* **Concept:** Custom memory management, heap chunk metadata manipulation, merging adjacent free blocks (coalescing), and preventing fragmentation.
* **Security Relevance:** Understanding how custom allocators manage metadata headers and free lists is vital for analyzing heap exploitation techniques (e.g., dlmalloc, ptmalloc internals).
* **The Task:** Build upon your previous heap block inspector by implementing block coalescing.
  * Define a header struct containing chunk size, a flags bitfield (e.g., `is_free`), and pointers for a doubly-linked free list (`prev`/`next` free blocks).
  * Implement `void *arena_malloc(Arena *arena, size_t size);` using a first-fit strategy.
  * Implement `void arena_free(Arena *arena, void *ptr);` that not only marks the block as free but checks adjacent physical memory addresses to **coalesce** adjacent free blocks into a single larger chunk.
* **Detailed Steps to Follow:**
  1. **Arena Initialization:** Allocate a large contiguous block of memory via `malloc` (e.g., 64KB) to act as your heap arena. Initialize the first header to span the entire payload area, marking it as free.
  2. **Payload-to-Header Arithmetic:** Ensure that when `arena_malloc` returns a pointer, it points to the address immediately *following* the `BlockHeader`. Implement pointer arithmetic to recover the header pointer given a payload pointer (i.e., `(BlockHeader *)ptr - 1`).
  3. **The Coalescing Logic:** In `arena_free`, use the current block's size and address to compute the address of the next physical block in memory. If the adjacent block is also free, unlink it from your free list and merge its size into the current block. Repeat or check the previous block if tracking backward pointers or footer tags.
  4. **Testing:** Write a test harness that allocates multiple blocks, frees them out of order, and asserts that adjacent free chunks successfully merge into larger contiguous blocks.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct BlockHeader {
    size_t size;
    int is_free;
    struct BlockHeader *next;
    struct BlockHeader *prev;
} BlockHeader;

static BlockHeader *master = NULL;

void *initialize_arena(size_t size) {
    
    void *arena = malloc(size);
    master = arena;
    master->size = size - (sizeof(BlockHeader));
    master->is_free = 1;
    master->next = NULL;
    master->prev = NULL;

    return arena;
}

void *my_alloc(size_t size) {
    
    BlockHeader *current_block = master;
    BlockHeader *previous_block = master;
    size_t remaining_size;
    char minimum_size = 1;

    while (current_block != NULL) {
        
        if (current_block->is_free == 1 && current_block->size >= (size + sizeof(BlockHeader) + minimum_size)) {
            
            remaining_size = current_block->size - size;
            BlockHeader *new_empty_block = (void *)current_block + size + sizeof(BlockHeader);
            new_empty_block->is_free = 1;
            new_empty_block->size = remaining_size - (sizeof(BlockHeader));
            new_empty_block->next = current_block->next;
            new_empty_block->prev = current_block;

            current_block->is_free = 0;
            current_block->size = size;
            if (current_block->next != NULL) {
                current_block->next->prev = new_empty_block;
            }
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

void my_free(void* ptr) {
    
    if (ptr == NULL) {
        return;
    }

    //Convert ptr to block header ptr
    BlockHeader *block_to_remove = (BlockHeader*)ptr - 1;
    
    if (block_to_remove->is_free == 1) {
        // block is already free
        return;
    }

    block_to_remove->is_free = 1;
    if (block_to_remove->next != NULL && block_to_remove->next->is_free == 1) {
        // coalesce with next block
        block_to_remove->size += block_to_remove->next->size + sizeof(BlockHeader);
        block_to_remove->next = block_to_remove->next->next;
        if (block_to_remove->next != NULL) {
            block_to_remove->next->prev = block_to_remove;
        }
    }
    if (block_to_remove->prev != NULL && block_to_remove->prev->is_free == 1) {
        // coalesce with prev block
        block_to_remove->prev->size += block_to_remove->size + sizeof(BlockHeader);
        block_to_remove->prev->next = block_to_remove->next;
        if (block_to_remove->next != NULL) {
            block_to_remove->next->prev = block_to_remove->prev;
        }
    }

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
    size_t arena_size = 1000;
    void *arena = initialize_arena(arena_size);
    assert(arena != NULL);

    printf("=== Test 1: Basic Allocation & OOM Check ===\n");
    void *b1 = my_alloc(200);
    void *b2 = my_alloc(300);
    assert(b1 != NULL);
    assert(b2 != NULL);

    // Try allocating more than what's left
    void *b_too_large = my_alloc(700);
    assert(b_too_large == NULL);
    printf("OOM check passed: large allocation correctly returned NULL.\n\n");

    printf("=== Test 2: Sequential Free & Full Coalescing ===\n");
    my_free(b1);
    my_free(b2);

    // After freeing all blocks, master should coalesce back to original single free block
    assert(master->is_free == 1);
    assert(master->next == NULL);
    assert(master->prev == NULL);
    printf("Full coalescence passed: Arena successfully returned to a single master chunk.\n\n");

    printf("=== Test 3: Out-of-Order Freeing & Intermediate Coalescing ===\n");
    void *x1 = my_alloc(100);
    void *x2 = my_alloc(100);
    void *x3 = my_alloc(100);

    // Free x1 and x3 first (leaving x2 allocated in the middle)
    my_free(x1);
    my_free(x3);
    
    // Now free x2, which should trigger both forward and backward coalescing
    my_free(x2);

    // Verify ultimate full merge
    assert(master->is_free == 1);
    assert(master->next == NULL);
    printf("Out-of-order coalescing test passed successfully!\n");

    free(arena);
    printf("\nAll tests passed successfully!\n");

    return 0;
}
