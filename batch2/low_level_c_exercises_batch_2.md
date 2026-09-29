# Advanced Low-Level C Practice Exercises for Security Research

Building upon your mastery of manual heap allocators, pointers, and binary serialization, these next five exercises step further into systems-level programming, memory safety considerations, and primitives commonly analyzed in vulnerability research and binary exploitation.

---

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

---

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

---

### Exercise 3: Stack Frame Inspector and Return Address Exploration
* **Concept:** Calling conventions (`cdecl` or `sysv`), the layout of stack frames, stack growth direction, and frame pointers (`rbp`/`ebp`).
* **Security Relevance:** Essential foundation for understanding stack-based buffer overflows, return-oriented programming (ROP), and stack unwinding.
* **The Task:** Write a program that safely inspects its own stack layout and local variables across nested function calls without causing undefined behavior.
  * Implement a deeply nested function call chain (e.g., `func_a` calls `func_b` which calls `func_c`).
  * Inside the deepest function, accept an argument or local variable and use pointer arithmetic/casting to print the memory addresses of local variables and local stack parameters.
* **Detailed Steps to Follow:**
  1. **Address Inspection:** In a function `void inspect_stack(int depth, int *base_ptr) { ... }`, declare local automatic variables and print their addresses (`&local_var`). Observe whether stack addresses increase or decrease as you move down stack frames.
  2. **Compiler Intrinsic Exploration:** Research and use GCC/Clang builtins like `__builtin_frame_address(0)` or `__builtin_return_address(0)` to inspect the saved frame pointer and return address on x86_64 architectures.
  3. **Safety Notice:** *Strictly read-only!* Do not attempt to modify return addresses in this exercise; focus entirely on mapping the physical layout of stack frames on your target architecture.

---

### Exercise 4: Custom Bitwise Protocol Parser with Type-Length-Value (TLV) Encoding
* **Concept:** Bitwise operations, shifting, masking, endianness handling, packing data, and parsing untrusted byte streams.
* **Security Relevance:** Network security research, protocol reverse engineering, and finding integer overflow or buffer overflow bugs in custom network packet parsers.
* **The Task:** Implement a secure parser for a custom binary network protocol using a Type-Length-Value (TLV) format.
  * Define a binary packet format where each message consists of a 1-byte Type, a 2-byte Length (Network Byte Order / Big Endian), and a variable-length Value payload.
  * Write a function `int parse_tlv_packet(const uint8_t *stream, size_t stream_len, TLVPacket *out_packet);` that validates bounds safely before extracting fields.
* **Detailed Steps to Follow:**
  1. **Endianness Conversion:** Implement manual conversion functions or use standard macros (`ntohs`, `htons`) to handle 16-bit and 32-bit integers correctly regardless of host machine architecture (Little Endian vs. Big Endian).
  2. **Bounds Checking (Crucial for Security):** Before reading the Length field or copying the Value payload into a buffer, verify that `stream_len` is large enough to contain the header and the specified length. Explicitly check for integer overflows if lengths are added together (e.g., `header_len + payload_len`).
  3. **Robust Error Handling:** Return specific negative error codes for truncated packets, unknown types, or oversized payloads rather than crashing or reading out-of-bounds memory.

---

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

---

### Recommended Workflow for Security Research
* Compile all code with strict security flags to catch issues early: `-Wall -Wextra -Werror -Wpedantic -O2 -fno-omit-frame-pointer`.
* Use tools like Valgrind (`valgrind --leak-check=full`) or AddressSanitizer (`-fsanitize=address,undefined`) during development to verify that your manual pointer arithmetic and custom allocators are free of memory leaks or out-of-bounds bugs.