#include "linkedlist.h"
#include "object.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "talloc.h"

// Creates and returns an empty list object with NULL_TYPE type.

Object *nullObject = NULL;

Object *makeNull(){
    nullObject = talloc(sizeof(Object));
    nullObject->type = NULL_TYPE;
    return nullObject;
}

// Constructs a new cons cell with given car and cdr, returning it as an Object pointer.
Object *cons(Object *newCar, Object *newCdr){
    ConsCell *consObject = (ConsCell*) talloc(sizeof(ConsCell));
    consObject->car = newCar;
    consObject->cdr = newCdr;
    consObject->type = CONS_TYPE;
    return (Object*) consObject;
}

// Iterates through the list and prints each element based on its type.
void display(Object *list) {
    while(list->type != NULL_TYPE){
        ConsCell *head = (ConsCell*)list;
        Object *car_h = head->car;
        Object *cdr_h = head->cdr;
        switch (car_h->type) {
            case PTR_TYPE:
                printf("%d ", ((Integer *)car_h)->value);
                break;
            case INT_TYPE:
                printf("%d ", ((Integer *)car_h)->value);
                break;
            case DOUBLE_TYPE:
                printf("%f ", ((Double *)car_h)->value);
                break;
            case STR_TYPE:
                printf("%s ", ((String *)car_h)->value);
                break;
            case CONS_TYPE:
                printf("CONS ");
                break;
            case NULL_TYPE:
                printf("NULLS ");
                break;
        }
        printf("\n");
        list = cdr_h;
    }
}

// Reverses the list by deep copying each element and constructing a new reversed list.
Object *reverse(Object *list) {
   
    Object *reversedList = talloc(sizeof(Object));
    reversedList->type = NULL_TYPE;
    while (list->type != NULL_TYPE) {
        ConsCell *currentCell = (ConsCell *)list;
        Object *deepCopiedObject = NULL;

        switch (currentCell->car->type) {
            case INT_TYPE: {
                Integer *originalInt = (Integer *)currentCell->car;
                deepCopiedObject = (Object *) originalInt;
                break;
            }
            case DOUBLE_TYPE: {
                Double *originalInt = (Double *)currentCell->car;
                deepCopiedObject = (Object *) originalInt;
                break;
            }
            case STR_TYPE: {
                String *originalInt = (String *)currentCell->car;
                deepCopiedObject = (Object *)originalInt;
                break;
            }
            case CONS_TYPE:
                break;
            case PTR_TYPE:
                break;
            default:
                break;
        }

        if (deepCopiedObject != NULL) {
            reversedList = cons(deepCopiedObject, reversedList);
        }

        list = currentCell->cdr;
    }
    return reversedList;
}

// Cleans up all memory used by the list, including nested structures.
void cleanup(Object *list) {
    while (list != NULL && list->type == CONS_TYPE) {
        ConsCell *currentCell = (ConsCell *)list;
        Object *nextObject = currentCell->cdr;

        switch (currentCell->car->type) {
            case INT_TYPE:
            case PTR_TYPE:
                free(currentCell->car);
                break;
            case DOUBLE_TYPE:
                free(currentCell->car);
                break;
            case STR_TYPE:
                free(((String *)currentCell->car)->value);
                free(currentCell->car);
                break;
            case CONS_TYPE:
                cleanup(currentCell->car);
                break;
            default:
                break;
        }

        free(currentCell);
        list = nextObject;
    }
    free(list);
}

// Returns the 'car' part of a cons cell.
Object *car(Object *list){
    ConsCell *head = (ConsCell*) list;
    return head->car;
}

// Returns the 'cdr' part of a cons cell.
Object *cdr(Object *list){
    ConsCell *head = (ConsCell*)list;
    return head->cdr;
}

// Checks if the given object is of NULL_TYPE.
bool isNull(Object *value){
    return value->type == NULL_TYPE;
}

// Calculates and returns the length of the list.
int length(Object *value){
    int length = 0;
    while(value->type != NULL_TYPE){
        length++;
        value = ((ConsCell*)value)->cdr;
    }
    return length;
}
