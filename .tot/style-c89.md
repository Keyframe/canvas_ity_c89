# C89 Coding Standards - CRITICAL

## **Preamble**

These are non-negotiable rules for writing C89 code. The goal is to produce code that is both extremely performant and safe. All code must adhere to these standards without exception. If you believe a deviation is necessary, you must state the rule you wish to override, provide a justification, and ask for explicit permission before proceeding.

## **Compiler Flags**

All C89 code must be compiled with the following flags. No exceptions.

```bash
-std=c89 \
-Wall \
-Wextra \
-Wpedantic \
-Werror \
-O3 \
-D_FORTIFY_SOURCE=2 \
-fstack-protector-strong \
-fPIC
```

## **C89 Constraints**

The following C99/C11 features are **not available** in C89 and must not be used:

*   **`//` single-line comments:** Use `/* */` block comments exclusively.
*   **`<stdint.h>` and fixed-width types:** `int32_t`, `uint64_t`, etc. are not available. Use `long`, `unsigned long`, `short`, `unsigned short`, `unsigned char`, or define project-specific typedefs. Use `<limits.h>` to verify sizes at compile time with `#if` checks.
*   **`<stdbool.h>` and `_Bool`:** Not available. Define `typedef int bool_t;` or use `int` with 0/1.
*   **`inline` keyword:** Not part of C89. Use `static` functions in headers or macros for inline expansion.
*   **`restrict` keyword:** Not part of C89. Omit entirely.
*   **Designated initializers:** `{.field = value}` syntax is not available. Use positional initialization.
*   **Compound literals:** `(type){...}` syntax is not available.
*   **Mixed declarations and code:** All variable declarations must appear at the beginning of a block, before any statements.
*   **`for` loop initializer declarations:** `for (int i = 0; ...)` is not allowed. Declare the variable before the loop.
*   **Variadic macros:** `__VA_ARGS__` is not available. Use variadic functions with `<stdarg.h>` if needed.
*   **`long long`:** Not guaranteed in C89. Use `long` or, where 64-bit is required, document the platform dependency.
*   **`__func__`:** Not available. Use string literals or macros for function name logging.
*   **`snprintf`:** Not part of C89 standard library. Use `sprintf` with manual buffer size discipline, or provide a portability shim.
*   **Flexible array members:** `struct { int n; char data[]; }` is not available. Use `char data[1]` with over-allocation.

## **Prohibited Practices**

The following are strictly forbidden:

*   **Undefined Behavior:** Any operation that results in undefined behavior is forbidden.
*   **Unchecked Memory Operations:** All pointer and array access must be explicitly bounds-checked.
*   **Unchecked Allocations:** The return value of `malloc`, `calloc`, and `realloc` must be checked for `NULL`.
*   **`goto`:** The `goto` statement is forbidden.
*   **Variable-Length Arrays (VLAs):** VLAs are forbidden (and not part of C89 regardless).
*   **Implicit Function Declarations:** All functions must have a prototype before they are called.
*   **Bitfields:** Bitfields are forbidden due to their implementation-defined nature. Use bitmasks and shifts instead.
*   **Magic Numbers:** Use named constants (`#define` or `const`) instead of magic numbers.

## **Variables and Naming Conventions**

*   **Naming Conventions:** Use `snake_case` for variable and function names. Use `UPPER_SNAKE_CASE` for macros and constants.
*   **Variable Declarations:** Declare variables at the beginning of a block. This is mandatory in C89.
*   **Single assignments:** Assign to a variable only once, and not change it afterward, except in loops for iterations. Use `const` to enforce immutability where applicable.

## **Memory Management**

*   **Ownership and Lifetime:** Memory ownership and object lifetimes must be clearly documented and managed.
*   **`malloc`/`free`:** Every `malloc`, `calloc`, or `realloc` must have a corresponding `free`.
*   **`calloc`:** Use `calloc` when you need zero-initialized memory.
*   **Stack Allocation:** Prefer stack allocation for small, fixed-size data.
*   **Cast `malloc` return:** In C89, `malloc` returns `void *` which requires explicit cast when assigned to a typed pointer. Always cast.

## **Types and Conversions**

*   **Explicit Conversions:** All type conversions must be explicit via casts.
*   **Platform-Checked Types:** Since `<stdint.h>` is unavailable, use `<limits.h>` compile-time checks to verify type sizes match expectations. Define project typedefs (e.g., `typedef unsigned char ci_uint8;`) and validate with `#if UCHAR_MAX != 255` guards.
*   **`size_t`:** Use `size_t` (from `<stddef.h>`) for all sizes, counts, and array indices.

## **Concurrency**

*   If concurrency is required, use `pthreads`.
*   All shared data must be protected by mutexes.
*   Strictly follow lock acquisition and release protocols to avoid deadlocks.

## **Build System**

*   **MANDATORY**: The `make` utility must be used for building the project. No other build systems (e.g., CMake, Bazel) are permitted.

## **Testing Requirements - CRITICAL**

*   **MANDATORY**: All code must be accompanied by unit tests.
*   **MANDATORY**: All tests must pass before any code is committed.
*   **MANDATORY**: No code changes are to be made without corresponding test validation.

## **Formatting and Style**

*   **MANDATORY**: The Google C Style Guide must be followed, with the following exceptions:
    * Indentation must be 4 spaces (not 2).
    * Line length must not exceed 100 characters.
    * Opening braces for functions and control structures must be on the same line.
    * Use `snake_case` for variable and function names instead of `camelCase`.
    * All comments must use `/* */` block comment syntax, never `//`.
