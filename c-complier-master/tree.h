#ifndef TREE_H
#define TREE_H

typedef struct Tree{
    char* content;
    char* name;
    int line;
    int num;
    int headline;
    int nextline;
    char* inner;
    char* code;
    struct Tree** leaves;
    struct Tree* next;
    struct Declator* declator;
}Tree;


Tree* initTree(int num);
Tree* createTree(char* name, int number, ...);

Tree* terminator(char* name, int yylineno);

Tree* binaryOpr(char* name, Tree* t1, Tree* t2, Tree* t3);
Tree* assignOpr(char* name, Tree* t1, Tree* t2, Tree* t3);
Tree* condictOpr(char* name, Tree* t1, Tree* t2, Tree* t3);
Tree* unaryOpr(char* name, Tree* t1, Tree* t2);
Tree* ifOpr(char* name,int headline,int nextline,Tree* op,Tree* stmt);
Tree* ifelseOpr(char* name, int headline, int next1, int next2, Tree* op, Tree* stmt1, Tree* stmt2);
Tree* whileOpr(char* name,int head1, int head2,int nextline,Tree* op,Tree* stmt);
Tree* forOpr(char* name,int head1, int head2, int nextline, Tree* op1, Tree* op2, Tree* op3, Tree* stmt);
Tree* retNull(char* name,Tree* ret);
Tree* retOpr(char* name,Tree* ret,Tree* op);
Tree* unaryFunc(char* name,Tree* func, Tree* op);
// 添加修饰符
Tree* addDeclator(char* name, Tree* t1, Tree* t2);

// name, num_args, init, cond, code
Tree* loop(char* name, int number, ...);

void printTree(Tree* tree);

void freeTree(Tree* tree);

#endif
