#include <stdio.h>
#include "tokenizer.h"
#include "object.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "interpreter.h"

int main() {

    Object *list = tokenize();
    Object *tree = parse(list);
    interpret(tree);

    tfree();
    return 0;
}
