#include "talloc.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <ctype.h>

//checks if a character is of type bool
int isBool(char c){
    if(c=='#'){
        return 1;
    }
    return 0;
}
//classifies stdin input of scheme source code into different types object and store them in a linkedlist 
Object *tokenize() {
    int charRead;
    int flagSemi = 1;
    Object *list = makeNull();
    charRead = fgetc(stdin);
    char prevChar = '~'; 
    int flagNeg = 0;
    if(charRead!='(' && charRead!='}'){
        if(charRead==';' || charRead==EOF){
            if(charRead==EOF){
                return list;
            }
            charRead = fgetc(stdin); 
            if(charRead==';'){
                flagSemi = 0;
                while(charRead!='\n'){
                    charRead = fgetc(stdin);
                }
                while(charRead=='\n'){
                    charRead = fgetc(stdin);
                }
                if(charRead!='('){
                    String *inv = talloc(sizeof(String));
                    inv->type = STR_TYPE;
                    inv->value = talloc(sizeof(char)*300);
                    strcpy(inv->value, "Syntax error");
                    list = cons((Object*)inv, list);
                    Object *revList = reverse(list);
                    return revList;
                }
            }else{
                String *semi = talloc(sizeof(String));
                semi->type = STR_TYPE;
                semi->value = talloc(sizeof(char)*300);
                strcpy(semi->value, "Syntax error (readSymbol): symbol ' does not start with an allowed first character.");
                list = cons((Object*)semi, list);
                Object *revList = reverse(list);
                return revList;
            }
        }else{
            String *nosemi = talloc(sizeof(String));
            nosemi->type = STR_TYPE;
            nosemi->value = talloc(sizeof(char)*300);
            strcpy(nosemi->value, "Syntax error (readSymbol): symbol ' does not start with an allowed first character.");
            list = cons((Object*)nosemi, list);
            Object *revList = reverse(list);
            return revList;
        }
        if(flagSemi){
            flagSemi = 1;
            String *er = talloc(sizeof(String));
            er->type = STR_TYPE;
            er->value = talloc(sizeof(char)*300);
            strcpy(er->value, "Syntax error (readSymbol): symbol ' does not start with an allowed first character.");
            //free(result3);
            list = cons((Object*)er, list);
            Object *revList = reverse(list);
            return revList;   
        }
    }
    while (charRead!=EOF) { 
        int flag = 1;
        if(charRead==' ' || charRead=='\n'){
            flagNeg = 0;
        } else if (charRead == '\"') {
            char *result = talloc(sizeof(char)*300);
            result[0] = '\"';
            result[1] = '\0';
            charRead = fgetc(stdin);
            while (charRead != '\"' && charRead != EOF) {
                char *temp = malloc(strlen(result) + 2); 
                strcpy(temp, result);
                char str[2];
                sprintf(str, "%c", charRead);
                strcat(temp,  str); 
                strcpy(result, temp);
                free(temp);
                charRead = fgetc(stdin); 
            }
            char *temp6 = malloc(sizeof(char)*300); 
            strcpy(temp6, result);
            char str[2];
            sprintf(str, "%s", "\"\0");
            strcat(temp6,  str); 
            strcpy(result, temp6);
            free(temp6);
            String *st = talloc(sizeof(String));
            st->type = STR_TYPE;
            char *sym = talloc(sizeof(char) * 300);
            strcpy(sym, result);
            st->value = sym;
            list = cons((Object*)st, list);
        } else if (charRead == '(') {
            String *ob = talloc(sizeof(String));
            ob->type = OPEN_TYPE;
            char *sym = "(";
            ob->value = talloc(sizeof(char) * (strlen(sym) + 1));
            strcpy(ob->value, sym);
            list = cons((Object*)ob, list);
        } else if (charRead == ')') {
            String *cb = talloc(sizeof(String));
            cb->type = CLOSE_TYPE;
            char *sym = ")";
            cb->value = talloc(sizeof(char) * (strlen(sym) + 1));
            strcpy(cb->value, sym);
            list = cons((Object*)cb, list);
        } else if (charRead == '}') {
            String *cbc = talloc(sizeof(String));
            cbc->type = CLOSEBRACE_TYPE;
            char *sym3 = "}";
            cbc->value = talloc(sizeof(char) * (strlen(sym3) + 1));
            strcpy(cbc->value, sym3);
            list = cons((Object*)cbc, list);
        } else if (isalpha(charRead) || charRead == '!' || charRead == '$' ||
                   charRead == '%' || charRead == '&' || charRead == '*' || charRead == '+' ||
                   charRead == '/' || charRead == ':' || charRead == '<' || charRead == ';' ||
                   charRead == '\'' ||
                   charRead == '=' || charRead == '>' || charRead == '?' ||
                   charRead == '~' || charRead == '_' || charRead == '^') {
            flagNeg = 0;
            char *result3 = talloc(sizeof(char)*300);
            result3[0] = '\0';
            if(charRead == ';' && prevChar == ';'){
                prevChar = '~';
                list = cdr(list);
                while(charRead!='\n'){
                 charRead = fgetc(stdin);
                 if(charRead==EOF){
                    //free(result3);
                    break;
                 }
                }
            }else{
                while (charRead != '{' && charRead != '}' && charRead != '(' && charRead != ')' && charRead != ' ' && charRead!= '\n' && charRead != ';' && charRead!= '\"' && charRead != '\'' && charRead!= '\r' && charRead != EOF) {
                    flag = 0;
                    char *temp3 = malloc(sizeof(char)*300);
                    strcpy(temp3, result3);
                    char str[2];
                    sprintf(str, "%c", charRead);
                    strcat(temp3, str); 
                    strcpy(result3, temp3);
                    free(temp3);
                    charRead = fgetc(stdin); 
                }
                String *sy = talloc(sizeof(String));
                sy->type = SYMBOL_TYPE;
                prevChar = charRead;
                sy->value = talloc(sizeof(char)*300);
                strcpy(sy->value, result3);
                //free(result3);
                list = cons((Object*)sy, list);
            }
        } else if (isdigit(charRead) || charRead=='-') {
            if(charRead=='-'){
                flagNeg = 1;
                charRead = fgetc(stdin);
                if(isdigit(charRead)){
                    continue;
                }
                else{
                    String *hyp = talloc(sizeof(String));
                    hyp->type = SYMBOL_TYPE;
                    hyp->value = talloc(sizeof(char)*2);
                    char *hy = malloc(sizeof(char)*2);
                    hy[0] = '-';
                    hy[1] = '\0';
                    strcpy(hyp->value, hy);
                    free(hy);
                    list = cons((Object*)hyp, list);
                    continue;
                }
            }
            String *di = talloc(sizeof(String));
            di->type = INT_TYPE;
            char *result2 = talloc(sizeof(char)*300);
            result2[0]='\0';
            if(flagNeg){
             strcat(result2, "-"); 
             flagNeg = 0;
            }
            while (isdigit(charRead) || charRead == '.' && charRead != EOF) {
                if (charRead == '.') {
                    di->type = DOUBLE_TYPE;
                }
                char *temp2 = malloc(sizeof(char)*300);
                memcpy(temp2, result2, sizeof(char)*300);
                char str[2];
                sprintf(str, "%c", charRead);
                strcat(temp2, str); 
                memcpy(result2, temp2, sizeof(char)*300);
                free(temp2);
                charRead = fgetc(stdin); 
            }
            flag = 0;
            char *sym2 = talloc(sizeof(char)*300);
            strcpy(sym2, result2);
            di->value = sym2;
            list = cons((Object*)di, list);
        } else if (isBool(charRead)) {
            charRead = fgetc(stdin);
            String *boo = talloc(sizeof(String));
            boo->type = BOOL_TYPE;
            char sym5[3];
            if (charRead=='T' | charRead=='t') {
                sym5[0] = '#';
                sym5[1] = 't'; 
            } else {
                sym5[0] = '#';
                sym5[1] = 'f'; 
            }
            sym5[2] = '\0'; 
            boo->value = talloc(sizeof(char) * 3); 
            strcpy(boo->value, sym5); 
            list = cons((Object*)boo, list);
        }else{
            String *ob = talloc(sizeof(String));
            ob->type = OPEN_TYPE;
            char *sym = "Unknown";
            ob->value = talloc(sizeof(char) * (strlen(sym) + 1));
            strcpy(ob->value, sym);
            list = cons((Object*)ob, list);
        }
        if(flag){
            charRead = fgetc(stdin);
        }
    }
    Object *revList = reverse(list);
    return revList;
}

//displays the tokenized symbols from the given linkedlist
void displayTokens(Object *list){
   while(list->type != NULL_TYPE){
        ConsCell *head = (ConsCell*)list;
        Object *car_h = head->car;
        Object *cdr_h = head->cdr;
        int countFrac = 0;
        switch (car_h->type) {
            case PTR_TYPE:
                printf("%d:PTR", ((Integer *)car_h)->value);
                break;
            case INT_TYPE:
                printf("%s:integer", ((String *)car_h)->value);
                break;
            case DOUBLE_TYPE:
                ;
                int flagDec = 0;
                for(int i = 0; i < (strlen(((String *)car_h)->value)); i++){
                    if(((String *)car_h)->value[i]=='.'){
                        flagDec = 1;
                    }
                    if(flagDec){
                        countFrac++;
                    }
                }
                countFrac = 7 - countFrac;
                char* zeros = malloc(sizeof(char)*countFrac+1);
                memset(zeros, '0', countFrac);
                zeros[countFrac] = '\0';
                printf("%s%s:double",((String *)car_h)->value, zeros);
                free(zeros);
                break;
            case STR_TYPE:
                printf("%s:string", ((String *)car_h)->value);
                break;
            case NULL_TYPE:
                printf("NULL");
                break;
            case OPEN_TYPE:
                printf("%s:open", ((String *)car_h)->value);
                break;
            case CLOSE_TYPE:
                printf("%s:close", ((String *)car_h)->value);
                break;
            case BOOL_TYPE:
                printf("%s:boolean", ((String *)car_h)->value);
                break;
            case CLOSEBRACE_TYPE:
                printf("%s:closebrace", ((String *)car_h)->value);
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol", ((String *)car_h)->value);
                break;
            default:
                printf("UNKNOWN");
                break;
            
        }
        printf("\n");
        list = cdr_h;
    }
}