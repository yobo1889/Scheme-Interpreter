#ifndef _INTERPRETER
#define _INTERPRETER

#include "object.h"

void interpret(Object *tree);
Object *eval(Object *tree, Frame *frame);

#endif

