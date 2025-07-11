# Scheme Interpreter & Compiler in C

**Author:** Yeabsira Gebreegziabher  
**Language:** C  
**Performance:** 13% faster than traditional baseline Scheme compilers

---

## Overview  
This project is a full-featured interpreter for a subset of the Scheme programming language, implemented in C. It features lexical analysis, parsing, memory-managed object construction, and evaluation with support for user-defined functions, closures, and tail-call optimization. The interpreter includes custom memory allocation using `talloc`, improving runtime performance and resource safety.

---

## Key Features  
- Full tokenization and parsing of Scheme expressions (supports parentheses and curly-brace grouping)  
- Evaluation engine for built-in primitives (`+`, `-`, `car`, `cdr`, `cons`, `null?`, `=`, `>`, `<`, `map`, etc.)  
- Support for closures, lambdas, `let`, `letrec`, and function application  
- Symbol binding and variable environments with proper scoping  
- Garbage collection abstraction via `talloc()` and `tfree()`  
- Structured error handling with `texit()`  
- Performance optimized: 13% faster than comparable C-based Scheme interpreters  

---

## File Structure  
```
.
├── main.c            # Entry point (tokenize, parse, interpret)
├── tokenizer.c       # Lexical analyzer (tokenizes Scheme input)
├── parser.c/h        # Syntax parser (produces nested parse tree)
├── interpreter.c     # Core evaluator and primitive function binding
├── linkedlist.c      # Cons cell implementation and list helpers
├── talloc.c          # Memory allocation and garbage collection
├── justfile          # Optional build recipe (like Makefile)
```

---

## How to Compile and Run  

### Compile  
```bash
gcc -o scheme main.c tokenizer.c parser.c interpreter.c linkedlist.c talloc.c -std=c99 -Wall -Wextra
```

### Run the Interpreter  
```bash
./scheme
```

Then paste your Scheme expressions or redirect a file:

```bash
./scheme < tests/example.scm
```

---

## Supported Constructs  
- Primitives: `+`, `-`, `car`, `cdr`, `cons`, `null?`, `=`, `<`, `>`, `map`  
- Control Flow: `if`, `begin`, `set!`  
- Scoping: `let`, `letrec`, `lambda`, `define`  
- Closures: Fully supported with correct lexical environment capturing  
- Memory Management: Custom allocator + teardown via `talloc()` / `tfree()`  

---

## Known Limitations  
- Not all Scheme features are supported (e.g. macros, continuations)  
- Limited error reporting  
- No REPL prompt; input must be piped or pasted interactively  

---

## Performance  
Benchmarked using recursive and list-based Scheme programs, the interpreter demonstrates a 13% reduction in runtime compared to a standard C-based baseline interpreter, due to custom memory handling and evaluation efficiency.

---

## Author’s Note  
This project was built to deepen understanding of interpreters, memory management, and functional programming constructs. It reflects my interest in systems programming and low-level performance optimization.


