/*
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
*/

#include <stdio.h>

void inspect_stack(int depth, int *base_ptr) {
    int local_inspector = 4;
    printf("--- Depth level: %d ---\n", depth);
    printf("Caller variable address: %p\n", base_ptr);
    printf("Local variable address: %p\n", &local_inspector);
    if (base_ptr > &local_inspector) {
        printf("Stack is going down!\n");
    }
    else {
        printf("Stack is going up!\n");
    }
    printf("Return address: %p\n", __builtin_return_address(0));
}

void func_c() {
    int local_c = 3;
    inspect_stack(local_c, &local_c);
}

void func_b() {
    int local_b = 2;
    inspect_stack(local_b, &local_b);
    func_c();
}

void func_a() {
    int local_a = 1;
    inspect_stack(local_a, &local_a);
    func_b();
}

int main() {
    func_a();
    return 0;
}
