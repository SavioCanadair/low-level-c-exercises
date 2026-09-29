# Low-Level C Practice Exercises

Building on your mastery of dynamic memory and linked lists, these five exercises scale up in complexity to help you transition into systems programming and security concepts. 

---

### Exercise 1: The Doubly-Linked Process List (Bidirectional Traversal)
* **Concept:** Pointers to pointers, backward traversal, robust pointer rewiring.
* **The Task:** Modify your `Process` struct to add a `struct Process *prev` pointer. Then, rewrite or create new versions of:
  * `Process *add_process_doubly(Process *head, int pid, const char *name);`
  * `void print_processes_reverse(Process *tail);` (Hint: you'll need a helper function or logic to find the tail node first).
  * `Process *remove_process_doubly(Process *head, int pid);` (Ensuring both `prev` and `next` pointers are properly updated for neighbors).

---

### Exercise 2: Implementing a Fixed-Capacity Ring Buffer (Circular Queue)
* **Concept:** Raw memory blocks, modular arithmetic indices, fixed-size constraints (crucial for OS kernel queues and network drivers).
* **The Task:** Create a `RingBuffer` struct that manages a heap-allocated array of integers of a fixed capacity $N$. Implement:
  * `RingBuffer *create_buffer(int capacity);`
  * `int write_buffer(RingBuffer *buf, int value);` (Returns 0 on success, -1 if full).
  * `int read_buffer(RingBuffer *buf, int *out_value);` (Returns 0 on success, -1 if empty).
  * `void free_buffer(RingBuffer *buf);`

---

### Exercise 3: Safe String Splitter (Heap Allocation & Manual Parsing)
* **Concept:** Manual string manipulation, pointer arithmetic, multi-level pointer arrays (`char **`), and preventing buffer overflows.
* **The Task:** Write a function that mimics a basic `split` utility:
  * `char **split_string(const char *str, char delimiter, int *out_count);`
  * It must scan the input string, dynamically allocate an array of strings (`char **`), allocate memory for each individual substring separated by the delimiter, populate them, and return the array.
  * Write a companion function `void free_split_result(char **arr, int count);` to ensure zero memory leaks.

#### REIMPLEMENT THIS IN A CLEANER WAY
1. Switch from Indices to Pointers
Instead of managing i, startindex, and endindex, you can use a pointer (e.g., const char *p = str) to walk through the string.
A pointer is an address in memory, so you can measure distances directly by subtracting pointers (end_ptr - start_ptr), which eliminates a lot of index math.

2. Simplify the Two-Pass Structure
If you look at how a cleaner two-pass approach is structured conceptually:
- Pass 1 (Counting): You can walk through the string with a pointer. Every time you hit a delimiter (and making sure you ignore consecutive delimiters or handle leading/trailing cases cleanly), you increment your count.
- Pass 2 (Extracting): Instead of complex boolean state flags inside a loop, you can look for chunks. You find where a token starts, use a pointer or a function like strchr to find where it ends (either the next delimiter or \0), calculate its exact length instantly (end - start), allocate memory, and copy it.

By using pointers to jump from token to token rather than looking at every single character through a state machine, the logic shrinks down significantly and becomes much easier to read at a glance.

---

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

---

### Exercise 5: Binary File Parser (Binary Serialization)
* **Concept:** Struct padding, binary I/O (`fread`/`fwrite`), reading structured data directly from disk files.
* **The Task:** Write a program that takes your linked list of processes and serializes (saves) them directly into a binary file on disk.
  * `void save_to_disk(const Process *head, const char *filename);`
  * `Process *load_from_disk(const char *filename);`
  * *Security Bonus consideration:* Be mindful of endianness and struct padding when writing raw binary structures to disk!

