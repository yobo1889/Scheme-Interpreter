#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "linkedlist.h"
#include "talloc.h"
#include "interpreter.h"
#include "parser.h"
#include "tokenizer.h"
#include <assert.h>


//creates a new frame
Frame* newFrame(){
    Frame *frame = talloc(sizeof(Frame));
    frame->bindings = makeNull();
    frame->parent = NULL;
    return frame;
}
Object *defFrame;

//gets a symbol from  a given frame
Object* getSymbol(Object* sym, Frame* frame){
    Frame *currentFrame = frame;
    Object *foundBinding = NULL;
    char *targetSymbol = ((Symbol*)sym)->value;
    int matchCount = 0;
    while (currentFrame) {
        for (Object *binding = currentFrame->bindings; binding->type != NULL_TYPE; binding = cdr(binding)) {
            if (!strcmp(((Symbol*)car(car(binding)))->value, targetSymbol)) {
                foundBinding = binding;
                matchCount++;
            }
        }
        if (matchCount == 1) break;
        if (matchCount > 1) {
            printf("Evaluation Error: getSymbol \n");
            texit(1);
        }
        currentFrame = currentFrame->parent;
    }
    
    if (matchCount == 1){
        Object* match = car(cdr(car(foundBinding)));
        if(match->type==PRIMITIVE_TYPE){
            return match;
        }
        if(match->type == CONS_TYPE){
            return match;
        }
        if(match->type == NULL_TYPE){
            return makeNull();
        }
        return eval(car(cdr(car(foundBinding))), frame);
    } 
    return NULL;

    printf("Evaluation Error: getSymbol \n");
    texit(1);
}

//applies a function/procedure 
Object *apply(Object *function, Object *args) {
    assert(function->type == CLOSURE_TYPE || function->type == PRIMITIVE_TYPE);

    Frame *frame = talloc(sizeof(Frame));
    Object *tempBind = makeNull();
    Object *current = args;
    Object *params;
    Object *cur_param;
    Closure* closure;
    if (function->type == CLOSURE_TYPE) {
        closure = (Closure*) function;
        frame->parent = closure->frame;
        params = closure->paramNames;
        cur_param = params;
    } else {
        frame->parent = NULL;
        params = NULL;
        cur_param = NULL;
    }
    if (function->type == PRIMITIVE_TYPE) {
        return ((Primitive*)function)->pf(args);
    }
    while (current->type != NULL_TYPE) {
        if (cur_param->type == NULL_TYPE) {
            printf("Evaluation error: apply - too many arguments provided.\n");
            texit(1);
        }
        Object *list = makeNull();
        list = cons(car(current), list);
        list = cons(car(cur_param), list);
        tempBind = cons(list, tempBind);
        current = cdr(current);
        cur_param = cdr(cur_param);
    }
    if (cur_param->type != NULL_TYPE) {
        printf("Evaluation error: apply - too few arguments provided.\n");
        texit(1);
    }
    frame->bindings = tempBind;
    Object *body = closure->functionCode;
    Object *result = NULL;
    while (body->type != NULL_TYPE) {
        result = eval(car(body), frame);
        body = cdr(body);
    }
    return result;
}

//evaluates each of the parameter in args
Object *evalEach(Object *args, Frame *frame) {
    Object *current = args;
    Object *evaled_args = makeNull();
    
    while (current->type != NULL_TYPE) {
        Object *arg = car(current);
        Object *evaled_arg = eval(arg, frame);
        
        evaled_args = cons(evaled_arg, evaled_args);
        current = cdr(current);
    }
    return reverse(evaled_args);
}


//general eval function that handles s expression
Object *eval(Object *tree, Frame *frame) {
    switch (tree->type) {
        case INT_TYPE: 
            return tree;
            break;
        case DOUBLE_TYPE:
            return tree;
            break;
        case STR_TYPE:
            return tree;
            break;
        case BOOL_TYPE:
            return tree;
            break;
        case UNSPECIFIED_TYPE:
            return tree; 
            break;
        case PRIMITIVE_TYPE:
            return tree;
            break;
        case SYMBOL_TYPE: {
            Object* symbol = getSymbol(tree, frame);
            if(symbol!=NULL){
              return symbol;
            }
            break;
        }
        Object* first;
        case CONS_TYPE: 
                first = car(tree);
                Object *args = cdr(tree); 
                Object* clo = makeNull();
                if (first != NULL && first->type == SYMBOL_TYPE) {
                    if (strcmp(((String *)first)->value, "if") == 0) {
                            if (length(args) < 2){
                                printf("Evaluation error\n");
                                texit(1); 
                            }
                            Object *expr = car(args);
                            Object *consequent = car(cdr(args));
                            if (length(args) == 2) {
                                if (expr->type == BOOL_TYPE && ((Boolean*)expr)->value == 0){
                                    Object *obj = makeNull(); 
                                    obj->type = UNSPECIFIED_TYPE; 
                                    return obj;  
                                }
                                else{
                                    return eval(consequent, frame); 
                                }
                            }
                            Object *alternative = car(cdr(cdr(args))); 
                            Object *evalResult = eval(expr, frame);
                            if (((Boolean *)evalResult)->value == 0) { 
                                return eval(alternative, frame);
                            } else { 
                                return eval(consequent, frame);
                            }
                    } else if (strcmp(((String *)first)->value, "lambda") == 0) {
                        if (args->type != CONS_TYPE) {
                            printf("Evaluation Error: Lambda expects arguments.\n");
                            texit(1);
                        }

                        Object *params = car(args);
                        Object *body = cdr(args);

                        if(isNull(body)){
                            printf("Evaluation Error: Lambda expects arguments.\n");
                            texit(1);
                        }
                        Object *param1 = params;
                        while (param1->type != NULL_TYPE) {
                            if(car(param1)->type!=SYMBOL_TYPE){
                                printf("Evaluation Error: Non symbol param\n");
                                texit(0);
                            }
                            Object *param2 = cdr(param1);
                            while (param2->type != NULL_TYPE) {
                                if (strcmp(((String *)car(param1))->value, ((String *)car(param2))->value) == 0) {
                                    printf("Evaluation Error: Duplicate parameter '%s' in lambda definition.\n", ((String *)car(param1))->value);
                                    texit(1);
                                }
                                param2 = cdr(param2);
                            }
                            param1 = cdr(param1);
                        }
                        Closure* closure = talloc(sizeof(Closure));
                        closure->type = CLOSURE_TYPE;
                        closure->paramNames = params;
                        closure->functionCode = body;
                        closure->frame = frame;
                        return (Object*) closure;
                    }else if (strcmp(((String *)first)->value, "define") == 0) { 
                        Object *var = car(args);
                        Object *expr = car(cdr(args));
                        Frame* localF = newFrame();
                        Object *eval_expr = eval(expr, frame);
                        if(getSymbol(var, localF)!=NULL){
                            printf("Evaluation error\n");
                            texit(0);
                        }
                        Object *tempBind = makeNull();
                        tempBind = cons(eval_expr, tempBind);
                        tempBind = cons(var, tempBind);
                        frame->bindings = cons(tempBind, frame->bindings);
                        localF->bindings = cons(tempBind, localF->bindings);
                        Object *void_val = talloc(sizeof(Object));
                        void_val->type = VOID_TYPE;
                        return void_val;
                    } else if (strcmp(((String *)first)->value, "begin") == 0) {
                        Object *expr = evalEach(args, frame);
                        while (expr->type != NULL_TYPE) {
                            if (cdr(expr)->type == NULL_TYPE) {
                                return car(expr);
                            }
                            else {
                                expr = cdr(expr);
                            }
                        }
                        Object *voidd = talloc(sizeof(Object));
                        voidd->type = VOID_TYPE;
                        return voidd;
                    } else if (strcmp(((String *)first)->value, "let") == 0) {
                        Object *bindings = car(args);
                        Object *body = cdr(args);
                        if (bindings->type == NULL_TYPE && length(body) == 0) {
                            Object *obj = makeNull();
                            obj->type = UNSPECIFIED_TYPE;
                            return obj;
                        }
                        if (bindings->type != CONS_TYPE) {
                            printf("Evaluation error\n");
                            texit(1);
                        }
                        Frame *newF = newFrame();
                        newF->parent = frame;
                        while (bindings->type != NULL_TYPE) {
                            Object *bindingDetail = car(bindings);
                            if (bindingDetail->type != CONS_TYPE || length(bindingDetail) != 2 || car(bindingDetail)->type != SYMBOL_TYPE) {
                                printf("Evaluation error\n");
                                texit(1);
                            }
                            Object *variable = car(bindingDetail);
                            Object *value = eval(car(cdr(bindingDetail)), frame);   
                            Object *binding = cons(variable, cons(value, makeNull()));
                            newF->bindings = cons(binding, newF->bindings);
                            bindings = cdr(bindings);
                        }
                        Object *result = makeNull();
                        while (body->type != NULL_TYPE) {
                            result = eval(car(body), newF);
                            body = cdr(body);
                        }
                        return result;
                    }else if (strcmp(((String *)first)->value, "quote") == 0) {
                        return(car(args));
                    }else if(strcmp(((String *)first)->value, "letrec") == 0){
                        Object *current = car(args);
                        if (car(current)->type != CONS_TYPE) {
                             printf("Evaluation error\n");
                             texit(1); 
                        }
                        Frame *newF = newFrame();
                        newF->parent = frame;
                        Object *tempBind = makeNull();
                        while (current->type != NULL_TYPE) {
                            Object *value = car(cdr(car(current)));
                            Object *final_value = eval(value, newF);
                            Object *binding = cons(final_value,makeNull());
                            Object *symbol = car(car(current));
                            if (symbol->type != SYMBOL_TYPE) {
                                printf("Evaluation error\n");
                                texit(1); 
                            }
                            binding = cons(symbol, binding);
                            tempBind = cons(binding, tempBind);
                            current = cdr(current);
                        }
                        newF->bindings = tempBind;
                        return eval(car(cdr(args)), newF);
                    }else if(strcmp(((String *)first)->value, "set!") == 0){
                        Object *targetSymbol = car(args);
                        if (targetSymbol->type != SYMBOL_TYPE) {
                            printf("Evaluation error\n");
                            texit(1); 
                        }
                        Object *newValue = eval(car(cdr(args)), frame);
                        bool isSymbolFound = false;
                        Frame *searchFrame = frame;

                        while (searchFrame && !isSymbolFound) {
                            for (Object *currentBinding = searchFrame->bindings; currentBinding->type != NULL_TYPE; currentBinding = cdr(currentBinding)) {
                                Object *pair = car(currentBinding);
                                if (pair->type == CONS_TYPE && car(pair)->type == SYMBOL_TYPE && strcmp(((Symbol *)car(pair))->value, ((Symbol *)targetSymbol)->value) == 0) {
                                    ((ConsCell *)cdr(pair))->car = newValue;
                                    isSymbolFound = true;
                                    break;
                                }
                            }
                            searchFrame = isSymbolFound ? NULL : searchFrame->parent;
                        }

                        if (!isSymbolFound) {
                            printf("Evaluation error\n");
                            texit(1); 
                        }
                        Object *result = talloc(sizeof(Object));
                        result->type = VOID_TYPE;
                        return result;
                    }else{
                        Object *evaledOperator = eval(first, frame);
                        Object *evaledArgs = evalEach(args, frame);
                        return apply(evaledOperator, evaledArgs);
                    } 
                }else{
                    Object *evaledOperator = eval(first, frame);
                    Object *evaledArgs = evalEach(args, frame);
                    return apply(evaledOperator,evaledArgs);
                }
            break;
        case NULL_TYPE:
            return tree;
            break;
        case CLOSURE_TYPE:
            return tree;
            break;
        default:
            printf("Evaluation Error: default\n");
            texit(1); 
    }
    printf("Evaluation Error: Main\n");
    texit(1); 
    return NULL; 
}

//evaluates cons primitive
Object *primCons(Object *args) {
    if (args->type == NULL_TYPE) {
        printf("Evaluation Error\n");
        texit(1); 
    }
    if (car(args)->type == NULL_TYPE) {
        printf("Evaluation Error\n");
        texit(1); 
    }
    if (cdr(args)->type == NULL_TYPE) {
        printf("Evaluation Error\n");
        texit(1); 
    }
    if (cdr(args)->type == CONS_TYPE && cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    Object *conCar = car(args);
    Object *conCdr; 
    if (cdr(args)->type == CONS_TYPE) {
        conCdr = car(cdr(args));
    }
    else {
        conCdr = cdr(args);
    }
    Object *sum = cons(conCar, conCdr);
    return sum;
}
//evaluates car primitive
Object *primCar(Object *args) {
    if (args->type == NULL_TYPE || cdr(args)->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    Object *list = car(args);
    if (list->type != CONS_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    return car(list);
}

//evaluates cdr primitive
Object *primCdr(Object *args) {
    if (args->type == NULL_TYPE){
        printf("Evaluation error\n");
        texit(1); 
    }
    if(args->type != CONS_TYPE){
        printf("Evaluation error\n");
        texit(1); 
    }
    if (cdr(args)->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    Object *list = car(args);
    if (list->type != CONS_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    return cdr(list);
}
//evaluates null? primitive
Object *primNull(Object *args) {
    if (args->type == NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    if(args->type != CONS_TYPE){
        printf("Evaluation error\n");
        texit(1); 
    }
    if (cdr(args)->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1); 
    }
    if (car(args)->type == NULL_TYPE) {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 1;
        return bo;
    }
    else {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 0;
        return bo;
    }
}
//evaluates addition(+) primitive
Object *primAdd(Object *args) {
    float result = 0;
    Object *current = NULL;
    while (args->type != NULL_TYPE) {
        current = car(args);
        if (current->type != INT_TYPE) {
            if (current->type != DOUBLE_TYPE) {
                printf("Evaluation error\n");
                texit(1);
            }
            result = result + ((Double *)current)->value;
            args = cdr(args);
        }
        else {
            result = result + ((Integer*)current)->value;
            args = cdr(args);
        }
    }

    Object *sum = talloc(sizeof(Integer));
    sum->type = INT_TYPE;
    ((Integer*)sum)->value = 0;
    if(current!=NULL){
        if (current->type == DOUBLE_TYPE){
            sum = talloc(sizeof(Double));
            sum->type = DOUBLE_TYPE;
            ((Double *)sum)->value = result;
        }else{
            sum = talloc(sizeof(Integer));
            sum->type = INT_TYPE;
            ((Integer*)sum)->value = (int)result;
        }
    }
    return sum;
}
//evaluates map primitive
Object *primMap(Object *args) {
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE || cdr(cdr(args))->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    Object *function = car(args);
    if (function->type != CLOSURE_TYPE && function->type != PRIMITIVE_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    Object *list = car(cdr(args));
    if (list->type != CONS_TYPE && list->type != NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    Object *resultList = makeNull();
    while (list->type != NULL_TYPE) {
        Object *element = car(list);
        Object *appliedResult = apply(function, cons(element, makeNull()));
        resultList = cons(appliedResult, resultList);
        list = cdr(list);
    }
    return reverse(resultList);
}
//evaluates equal primitive
Object *primEqual(Object *args) {
    if (args->type == NULL_TYPE || car(args)->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    float num1;
    float num2;
    if(car(args)->type == INT_TYPE) {
        num1 = (float)((Integer* )car(args))->value;
    }
    else if(car(args)->type == DOUBLE_TYPE) {
        num1 = ((Double* )car(args))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if(car(cdr(args))->type == INT_TYPE) {
        num2 = (float)((Integer* )car(cdr(args)))->value;
    }
    else if(car(cdr(args))->type == DOUBLE_TYPE) {
        num2 = ((Double* )car(cdr(args)))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if (num1 == num2) {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 1;
        return bo;
    }
    else {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 0;
        return bo;
    }
}
//evaluates > primitive
Object *primGreater(Object *args) {
    if (args->type == NULL_TYPE || car(args)->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    float num1;
    float num2;
    if(car(args)->type == INT_TYPE) {
        num1 = (float)((Integer* )car(args))->value;
    }
    else if(car(args)->type == DOUBLE_TYPE) {
        num1 = ((Double* )car(args))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if(car(cdr(args))->type == INT_TYPE) {
        num2 = (float)((Integer* )car(cdr(args)))->value;
    }
    else if(car(cdr(args))->type == DOUBLE_TYPE) {
        num2 = ((Double* )car(cdr(args)))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if (num1 > num2) {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 1;
        return bo;
    }
    else {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 0;
        return bo;
    }
}
//evalutes < primitive
Object *primLess(Object *args) {
    if (args->type == NULL_TYPE || car(args)->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error\n");
        texit(1);
    }
    float num1;
    float num2;
    if(car(args)->type == INT_TYPE) {
        num1 = (float)((Integer* )car(args))->value;
    }
    else if(car(args)->type == DOUBLE_TYPE) {
        num1 = ((Double* )car(args))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if(car(cdr(args))->type == INT_TYPE) {
        num2 = (float)((Integer* )car(cdr(args)))->value;
    }
    else if(car(cdr(args))->type == DOUBLE_TYPE) {
        num2 = ((Double* )car(cdr(args)))->value;
    }
    else{
        printf("Evaluation error\n");
        texit(1);
    }
    if (num1 < num2) {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 1;
        return bo;
    }
    else {
        Object* bo = talloc(sizeof(Boolean));
        bo->type = BOOL_TYPE;
        ((Boolean*)bo)->value = 0;
        return bo;
    }
}
//evaluates subtract primitive
Object *primSubtract(Object *args) {
    double diff = 0.0;
    bool flag = true;

    while (args->type != NULL_TYPE) {
        Object *arg = car(args);

        if (arg->type == INT_TYPE || arg->type == DOUBLE_TYPE) {
            double value = (arg->type == INT_TYPE) ? (double)((Integer*)arg)->value : ((Double*)arg)->value;

            if (flag) {
                diff += value;
                flag = false;
            } else {
                diff -= value;
            }
        } else {
            fprintf(stderr, "Evaluation error\n");
            texit(EXIT_FAILURE);
        }

        args = cdr(args);
    }

    Object *finalResult = talloc(sizeof(Double));
    finalResult->type = DOUBLE_TYPE;
    ((Double*)finalResult)->value = diff;
    return finalResult;

}

//assings primitives to their respective symbol
void bind(char *name, Object *(*function)(struct Object *), Frame *frame) {
   
    Object *proc = talloc(sizeof(Primitive));
    proc->type = PRIMITIVE_TYPE;
    ((Primitive*)proc)->pf = function;
    Object *symbol = talloc(sizeof(Symbol));
    symbol->type = SYMBOL_TYPE;
    ((Symbol *)symbol)->value = name;   
    Object *newBinding = makeNull();
    newBinding = cons(proc, newBinding);
    newBinding = cons(symbol, newBinding);
    frame->bindings = cons(newBinding, frame->bindings);
}

//prints out a linkeled list tree
void printCons(Object *tree) {
    if (tree == NULL) {
        printf("()");
        return;
    }
    switch (tree->type) {
        case INT_TYPE:
            printf("%d", ((Integer *)tree)->value);
            break;
        case DOUBLE_TYPE:
            printf("%f", ((Double *)tree)->value);
            break;
        case STR_TYPE:
            printf("\"%s\"", ((String *)tree)->value);
            break;
        case BOOL_TYPE:
            printf("%s", ((Boolean *)tree)->value ? "#t" : "#f");
            break;
        case SYMBOL_TYPE:
            printf("%s", ((Symbol *)tree)->value);
            break;
        case CONS_TYPE:
            printf("(");
            printCons(car(tree));
            Object *cdrPart = cdr(tree);
            while (cdrPart && cdrPart->type == CONS_TYPE) {
                printf(" ");
                printCons(car(cdrPart));
                cdrPart = cdr(cdrPart);
            }
            if (cdrPart != NULL && cdrPart->type != NULL_TYPE) {
                printf(" . ");
                printCons(cdrPart);
            }
            printf(")");
            break;
        case NULL_TYPE:
            printf("()");
            break;
        case PRIMITIVE_TYPE:
            printf("#<primitive>");
            break;
        case CLOSURE_TYPE:
            printf("#<closure>");
            break;
        case VOID_TYPE:
            break;
        default:
            printf("Unknown type");
            break;
    }
}

//iterates to a tokenized sceheme to evaluate expressions
void interpret(Object *tree) {
    Frame *frame = newFrame();
    defFrame = makeNull();
    bind("+", primAdd, frame);
    bind("null?", primNull, frame);
    bind("car", primCar, frame);
    bind("cdr", primCdr, frame);
    bind("cons", primCons, frame);
    bind("map", primMap, frame);
    bind("-", primSubtract, frame);
    bind("<", primLess, frame);
    bind(">", primGreater, frame);
    bind("=", primEqual, frame);
    

    while (tree->type != NULL_TYPE) {
        if (car(tree)->type == CONS_TYPE) {
           
            if (strcmp(((String *)car(car(tree)))->value, "define") == 0) {
                if(isNull(cdr(car(tree)))){
                    printf("Evaluation error\n");
                    texit(0);
                }
                Object *variableToDefine = car(cdr(car(tree)));
                Object *defFrameIterator = defFrame;
                if(variableToDefine->type!=SYMBOL_TYPE || isNull(cdr(cdr(car(tree))))){
                    printf("Evaluation error\n");
                    texit(0);
                }
                while (defFrameIterator->type != NULL_TYPE) {
                    if (strcmp(((String *)car(car(defFrameIterator)))->value, ((String *)variableToDefine)->value) == 0) {
                        printf("Evaluation error: double define\n");
                        texit(1);
                    }
                    defFrameIterator = cdr(defFrameIterator);
                }
                defFrame = cons(cons(variableToDefine, makeNull()), defFrame);
            }
        }
        Object *result = eval(car(tree), frame);
        switch (result->type) {
            case INT_TYPE:
                printf("%d ", ((Integer *)result)->value);
                break;
            case DOUBLE_TYPE:
                printf("%f> ", ((Double *)result)->value);
                break;
            case STR_TYPE:
                printf("\"%s\" ", ((String *)result)->value);
                break;
            case SYMBOL_TYPE:
                printf("%s ", ((String *)result)->value);
                break;
            case BOOL_TYPE:
                printf("%s ", ((Boolean *)result)->value ? "#t" : "#f");
                break; 
            case OPEN_TYPE:
                printf(" (");
                break;
            case CLOSE_TYPE:
                printf(") ");
                break;
            case PTR_TYPE:
                printf("%d ", ((Integer *)result)->value);
                break;
            case UNSPECIFIED_TYPE:
                printf("#<unspecified>");
                break;
            case CONS_TYPE:
                printCons((Object *)result);
                break;
            case CLOSEBRACE_TYPE:
                printf("}");
                break;
            case PRIMITIVE_TYPE:
                printf("#<procedure>\n");
                break;
            case NULL_TYPE:
                printf("()");   
            case VOID_TYPE:
                break;
            case CLOSURE_TYPE:
                printf("#<procedure>\n");
                break;   
        }
        printf("\n");
        tree = cdr(tree);
    }
}

