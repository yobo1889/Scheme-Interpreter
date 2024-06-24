#include <stdbool.h>
#include "object.h"

#ifndef _LINKEDLIST
#define _LINKEDLIST

// Create a new NULL_TYPE value node.
Object *makeNull();

// Create a new CONS_TYPE value node.
Object *cons(Object *newCar, Object *newCdr);

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Object *list);

// Return a new list that is the reverse of the one that is passed in. No stored
// data within the linked list should be duplicated; rather, a new linked list
// of CONS_TYPE nodes should be created, that point to items in the original
// list.
Object *reverse(Object *list);

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Object *car(Object *list);

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Object *cdr(Object *list);

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Object *value);

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Object *value);


#endif
