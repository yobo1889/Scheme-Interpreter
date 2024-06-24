#include <stdlib.h>
#include "talloc.h"
#include "linkedlist.h"
#include "object.h"
#include <assert.h>



// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
Object *head = NULL;


void *talloc(size_t size){
    if(head==NULL){
        head = makeNull();
    }
    void *mal = malloc(size);
    Pointer *val4 = malloc(sizeof(Pointer));
    val4->type = PTR_TYPE;
    val4->value = mal;
    head = cons((Object*)val4,head);
    return mal;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree(){
  while (head != NULL && head->type == CONS_TYPE) {
        ConsCell *currentCell = (ConsCell *)head;
        Pointer* val = (Pointer*) currentCell->car;
        Object *nextObject = currentCell->cdr;
        free(val->value);
        free(val);
        free(currentCell);
        head = nextObject;
   }
   if(head==NULL){
    free(head);
   }    
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status){
    tfree();
    exit(status);
}


