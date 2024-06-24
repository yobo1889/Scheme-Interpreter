#include "object.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <assert.h>

//converts curl braces to closed parenthessis based on the number of open parenthesis
Object* curlToParent(Object* tree){
    Object *result = makeNull();
    Object *current = tree;
    assert(current != NULL && "current is null");
    int open = 0;
    while (current->type != NULL_TYPE) {
        if(car(current)->type==OPEN_TYPE){
            result = cons(car(current), result);
            open++;
        }else if(car(current)->type==CLOSEBRACE_TYPE){
            for(int i = 0; i < open; i++) {
                Object* newClose = talloc(sizeof(Object));
                newClose->type = CLOSE_TYPE;
                result = cons(newClose, result);
            }
            open = 0;
        }else if(car(current)->type==CLOSE_TYPE){
            result = cons(car(current), result);
            open--;
        }else{
            result = cons(car(current), result);
        }

        current = cdr(current);
    }
    result = reverse(result);
    return result;
}

// a sub parse function that returns a list everyhting there is an open parenthesis
Object *addToParseTree(Object *tree, int *depth, Object *token) {
    if (token->type == OPEN_TYPE) {
        tree = cons(token, tree);
        (*depth)++;
    } else {
        tree = cons(token, tree);
    }
    return tree;
}

//error handling function

void syntaxError(int err) {
    if (err == 1) {
        printf("Syntax error: more parent\n");
        texit(1);
    } else if (err == 2) {
        printf("Syntax error: less parent\n");
        texit(1);
    }
}


//main parse functions that returns a parsed tree of a tokenized scheme
Object *parse(Object *tokens) {
    tokens = curlToParent(tokens);
    Object *tree = makeNull();
    int depth = 0;
    Object *current = tokens;
    assert(current != NULL && "current is null");

    while (current->type != NULL_TYPE) {
        if (car(current)->type != CLOSE_TYPE) {
            Object *token = car(current);
            tree = addToParseTree(tree, &depth, token);
            current = cdr(current);
        } else {
            Object *sub_list = makeNull();
            Object *sub_car = tree;
            while (sub_car->type != NULL_TYPE && car(sub_car)->type != OPEN_TYPE) {
                Object *token = car(sub_car);
                sub_list = cons(token, sub_list);
                sub_car = cdr(sub_car);
            }
            if (sub_car->type == NULL_TYPE) {
                syntaxError(1);
            }
            tree = cdr(sub_car);
            tree = cons(sub_list, tree);
            depth--;
            current = cdr(current);
        }
    }

    if (depth != 0) {
        syntaxError(2);
    }
    return reverse(tree);
}

//prints the content of a parsed scheme tree
void printTree(Object *tree){
    while(!isNull(tree)){
        if(car(tree)->type == CONS_TYPE){
            printf("( ");
            printTree(car(tree));
            printf(") ");
        }else if(car(tree)->type == INT_TYPE){
            printf("%i ", ((Integer*)car(tree))->value);
        }else if(car(tree)->type == DOUBLE_TYPE){
            printf("%f ", ((Double*)car(tree))->value);
        } else if(car(tree)->type == NULL_TYPE){
            printf("() ");
        } else if(car(tree)->type == SYMBOL_TYPE){
            printf("%s ", ((String*)car(tree))->value);
        } else if(car(tree)->type == STR_TYPE){
            printf("\"%s\" ", ((String*)car(tree))->value);
        } else if(car(tree)->type == BOOL_TYPE){
            if(((Boolean *)car(tree))->value==1){
                    printf("#t ");
                }else{
                    printf("#f ");
                }
        }
        tree = cdr(tree);
    }
    printf("\n");
    return;
}