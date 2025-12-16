#ifndef STACK_H
#define STACK_H

typedef struct SNode{
    struct Tree* tree;
    int level;
    struct SNode* next;
}SNode;

typedef struct Stack{
    struct SNode* top;
    struct SNode* button;
}Stack;

Stack* createStack();

void freeStack(Stack* s);

// add Tree from top
void push(Stack* s, struct Tree* t, int level);

// pop Tree from top
SNode* pop(Stack* s);

#endif