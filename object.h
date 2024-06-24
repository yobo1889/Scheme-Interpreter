#ifndef _VALUE
#define _VALUE

typedef enum {
    INT_TYPE, DOUBLE_TYPE, STR_TYPE, CONS_TYPE, NULL_TYPE, PTR_TYPE,
    OPEN_TYPE, CLOSE_TYPE, BOOL_TYPE, SYMBOL_TYPE, CLOSEBRACE_TYPE,
    UNSPECIFIED_TYPE, VOID_TYPE, CLOSURE_TYPE, PRIMITIVE_TYPE

} objectType;

typedef struct Object {
    objectType type;
} Object;

typedef struct Integer {
    objectType type;
    int value;
} Integer;

typedef struct Double {
    objectType type;
    double value;
} Double;

typedef struct String {
    objectType type;
    char *value;
} String;

typedef struct Pointer {
    objectType type;
    void *value;
} Pointer;

typedef struct ConsCell {
    objectType type;
    Object *car;
    Object *cdr;
} ConsCell;

// Store a 1 for a true, a 0 for a false
typedef struct Boolean {
    objectType type;
    int value;
} Boolean;

typedef struct Symbol {
    objectType type;
    char *value;
} Symbol;

// A frame is a linked list of bindings, and a pointer to another frame.  A
// binding is a variable name (represented as a string), and a pointer to the
// Object it is bound to. Specifically how you implement the list of bindings
// is up to you.
typedef struct Frame {
    Object *bindings;
    struct Frame *parent;
} Frame;

// For purposes of this project a closure is just another type of value,
// containing everything needed to execute a user-defined function:
// (1) a list of formal parameter names
// (2) a pointer to the function body
// (3) a pointer to the environment frame in which the function was created.
typedef struct Closure {
    objectType type;
    Object *paramNames;
    Object *functionCode;
    Frame *frame;
} Closure;


// A primitive style function; just a pointer to it, with the right
// signature (pf = primitive function)
typedef struct Primitive {
    objectType type;
    Object *(*pf)(Object *);
} Primitive;

#endif
