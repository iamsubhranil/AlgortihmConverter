#include <stdio.h>
#include <stdlib.h>
#include "strutils.h"

typedef enum TokenType{
    KEYWORD, // Any type of keywords : Begin, End, Set, Do, EndDo, While, EndWhile, For, EndFor, If, EndIf, Print
    VARIABLE, // Any valid variable
    CONSTANT, // Any constant
    STRING, // Any string starting and terminating with inverted comma
    OPERATOR, // Any type of operator
    BRACE, // Any type of braces
    NEWLINE // \n or \r\n
} TokenType;

typedef struct Token{
    TokenType type;
    char *val;
    struct Token *next;
} Token;

// Each line must start with a keyword, and each keyword must contain a space in the end
static char * readnextKeyword(FILE *fp){
    char start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(start != ' ' && start != '(' && start != '\n' && start != '\t' && start != '\r'){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    fseek(fp, -1, SEEK_CUR);
    buffer = addToBuffer(buffer, &pointer, '\0');
    //printf("\nKeyword : [%s] Returned : [%c]", buffer, start);
    return buffer;
}

// Reads a constant
static char *readnextConstant(FILE *fp){
    char start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(digit(start)){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    fseek(fp, -1, SEEK_CUR);
    buffer = addToBuffer(buffer, &pointer, '\0');
    return buffer; 
}

// Underscores are not supported
static char *readnextVariable(FILE *fp){
    char start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(aldigit(start)){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    fseek(fp, -1, SEEK_CUR);
    buffer = addToBuffer(buffer, &pointer, '\0');
    return buffer;
}

static int isoperator(char c){
    return c == '=' || c == '>' || c == '<' || c == '-' || c == '+' || c == '*' || c == '/' || c == '!';
}

static char *readnextOperator(FILE *fp){
    char start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(isoperator(start)){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    //printf("\n[operator] next char %c", start);
    fseek(fp, -1, SEEK_CUR);
    buffer = addToBuffer(buffer, &pointer, '\0');
    return buffer; 
}

static int isbrace(char c){
    return c == ')' || c == '(' || c == '{' || c == '}' || c == '[' || c == ']';
}

static char *readnextBrace(FILE *fp){ 
    char start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(isbrace(start)){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    //printf("\n[brace] next char %c", start);
    fseek(fp, -1, SEEK_CUR);
    buffer = addToBuffer(buffer, &pointer, '\0');
    return buffer; 
}

static char *readnextString(FILE *fp){ 
    char start = getc(fp);
    start = getc(fp);
    size_t pointer = 0;
    char *buffer = NULL;
    while(start != '"'){
        buffer = addToBuffer(buffer, &pointer, start);
        start = getc(fp);
    }
    buffer = addToBuffer(buffer, &pointer, '\0');
    return buffer; 
}

Token *tokenize(FILE *stream){
    Token *head = NULL, *prev = NULL;
    int newline = 1;
    char *prevS = NULL;
    while(!feof(stream)){
        char c = getc(stream);
        if(c == -1)
            break;
        //printf("%d ",c);
        fseek(stream, -1, SEEK_CUR);
        Token *t = (Token *)malloc(sizeof(Token));
        t->next = NULL;
        t->val = NULL;
        if(c == ' ' || c == '\t'){
            //while(!feof(stream) && (getc(stream) == ' ' || getc(stream) == '\t'));
            //if(feof(stream))
            //    break; 
            //fseek(stream, -1, SEEK_CUR);
            //free(t);
            getc(stream);
            continue;
        }
        else if(c == '\n' || c == '\r'){
            getc(stream);
            if(c == '\r')
                getc(stream);
            newline = 1;
            t->type = NEWLINE;
            t->val = strdup("[NEWLINE]");
        }
        else if(newline){
            t->type = KEYWORD;
            t->val = readnextKeyword(stream);
            newline = 0;
        }
        else if(digit(c)){
            t->type = CONSTANT;
            t->val = readnextConstant(stream);
        }
        else if(alpha(c)){
            t->type = VARIABLE;
            t->val = readnextVariable(stream);
        }
        else if(isbrace(c)){
            t->type = BRACE;
            t->val = readnextBrace(stream);
        }
        else if(isoperator(c)){
            t->type = OPERATOR;
            t->val = readnextOperator(stream);
        }
        else if(c == '"'){
            t->type = STRING;
            t->val = readnextString(stream);
        }
        else{
            printf("\n[Error] Unknown character : %c", c);
            free(t);
            break;
        }
        if(head == NULL)
            head = t;
        else
            prev->next = t;
        prev = t;
        prevS = t->val;
        //printf("\nToken : [%s]", t->val);
    }
    return head;
}

static void printtokens(Token *head){
    while(head){
        printf(" [");
        switch(head->type){
            case BRACE:
                printf("BRACE:%s]", head->val);
                break;
            case CONSTANT:
                printf("CONSTANT:%s]", head->val);
                break;
            case KEYWORD:
                printf("KEYWORD:%s]", head->val);
                break;
            case OPERATOR:
                printf("OPERATOR:%s]", head->val);
                break;
            case VARIABLE:
                printf("VARIABLE:%s]", head->val);
                break;
            case STRING:
                printf("STRING:%s]", head->val);
                break;
            case NEWLINE:
                printf("NEWLINE]\n");
                break;
        }
        head = head->next;
    }
}

int main(int argc, char **argv){
    FILE *test = fopen(argv[1], "rb");
    Token *head = tokenize(test);
    //printf("\nAnalyzed algo : ");
    printtokens(head);
    fclose(test);
    printf("\n");
    return argc;
}
