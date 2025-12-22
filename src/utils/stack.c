#include "stack.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

Stack* createStack(){
    Stack* s = (Stack*)malloc(sizeof(Stack));
    s->top = NULL;
    s->button = NULL;
    return s;
}

void freeStack(Stack* s){
    SNode* n;
    while((n = s->top)){
        s->top = n->next;
        free(n);
    }
}

void push(Stack* s, struct Tree* t, int level){
    if(!s){
        s = createStack();
    }
    SNode* n = (SNode*)malloc(sizeof(SNode));
    n->tree = t;
    n->level = level;
    n->next = NULL;
    if(!s->button){
        s->button = n;
        s->top = n;
    }else{
        n->next = s->top;
        s->top = n;
    }
}

SNode* pop(Stack* s){
    if(!s || !s->button){
        return NULL;
    }
    SNode* n = s->top;
    if(s->top == s->button){
        s->top = s->button = NULL;
    }else{
        s->top = n->next;
    }
    return n;
}
