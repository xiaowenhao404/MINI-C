#include "tree.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "stack.h"
#include "hashMap.h"
#include "inner.h"
#define MAX_LENGTH 3
extern char *yytext;
extern int yylineno;
extern FILE *yyout, *out;
extern struct HashMap* hashMap;
extern int scope, type, preType;
void destoryPartOfHashMap(struct HashMap* hashMap, int scope);

extern struct Tree* root;
extern struct Node* head ;
int inner_count = 1;
extern int line_count;

Tree* createTree(char* name, int number, ...){
    int i;
    va_list valist;
    va_start(valist, number);
    if(number == 1){
        return va_arg(valist, Tree*);
    }
    Tree* tree = initTree(1);
    if(!tree){
        printf("Out of space \n");
        exit(0);
    }
    tree->num = number;
    int len = strlen(name);
    tree->name = (char*)malloc(len + 1);
    memcpy(tree->name, name, len + 1);


    char *str= (char*)malloc(1000);
    tree->leaves = (Tree**)malloc(sizeof(Tree*)*number);
    for(i = 0; i < number; i++){
        tree->leaves[i] = va_arg(valist, Tree*);
        if(tree->leaves[i]->code)
            strcat(str,tree->leaves[i]->code);
    }
    if(strlen(str)>0)
        tree->code = str;

    return tree;
}

//在lex文件里创建终结符节点
Tree* terminator(char* name, int yylineno){
    int i;

    Tree* tree = initTree(1);
    if(!tree){
        printf("Out of space \n");
        exit(0);
    }
    tree->num = 0;
    int len = strlen(name);
    tree->name = (char*)malloc(len + 1);
    memcpy(tree->name, name, len + 1);

    int line = yylineno;
    tree->line = line;
    if(!strcmp(name, "null")){
        len = 1;
    }else{
        len = strlen(yytext);
        fprintf(yyout, "%-15s\t%-15s", name, yytext);
        if(!strcmp(name, "ID")){
            fprintf(yyout, "%p\n", hashMap);
        }else{
            if(type != 0){
                preType = type;
                type = 0;
            }
            if(!strcmp(name, "INT8") || !strcmp(name, "INT10") || !strcmp(name, "INT16")){
                fprintf(yyout, "%s\n", yytext);
            }else{
                fprintf(yyout, "\n");
                if(!strcmp(name, "COMMA")){
                    type = preType;
                }
            }
        }
        if(!strcmp(name, "LCB")){
            scope++;
            preType = 0;
        }else if(!strcmp(name, "RCB")){
            destoryPartOfHashMap(hashMap,scope--); 
        }
    }
    tree->content = (char*)malloc(len + 1);
    memcpy(tree->content, yytext, len + 1);
    tree->inner = (char*)malloc(len + 1);
    memcpy(tree->inner, yytext, len + 1);

    return tree;
}


// op, args ... num is number of args
Tree* op(char* name, int num, ...){
    Tree* t = initTree(1);
    int len = strlen(name);
    t->name = (char*)malloc(len + 1);
    memcpy(t->name, name, len + 1);
    t->num = num;
    t->leaves = (Tree**)malloc(sizeof(Tree*) * num);
    va_list valist;
    va_start(valist, num);
    Tree* temp = va_arg(valist, Tree*);
    len = strlen(temp->content);
    t->content = (char*)malloc(len + 1);
    memcpy(t->content, temp->content, len + 1);
    int i;
    for(i = 0; i < num; i++){
        temp = va_arg(valist, Tree*);
        t->leaves[i] = temp;
    }
    t->declator = NULL;
    return t;
}

//二元运算符创建树
Tree* binaryOpr(char* name, Tree* t1, Tree* t2, Tree* t3){
    //生成树
    Tree* t = op(name, 2, t2, t1, t3);
    //获取中间代码
    Node *node = getNodeByDoubleVar(t->content,t1->inner,t3->inner,inner_count++);
    //赋值
    t->inner = node->inner;
    if(node->op){
        //合并本次操作的中间代码
        t->code = mergeCode(11, t1->code,
            t3->code,
            "#", t->inner, " = ", t1->inner, " " ,t->content, " ", t3->inner, "\n");
        //行号加一
        line_count++;
    }
    return t;
}

//赋值语句创建树
Tree* assignOpr(char* name, Tree* t1, Tree* t2, Tree* t3){
    Tree* t =op(name, 2, t2, t1, t3);
    t->inner = t1->inner;
    t->code = mergeCode(8, t3->code, 
        "#", t1->inner, " " ,t->content, " ", t3->inner, "\n"); 
    line_count++;
    return t;
}

//一元运算符创建树
Tree* unaryOpr(char* name, Tree* t1, Tree* t2){
    Tree* t = op(name, 1, t1, t2);
    //Node *node = getNodeBySingleVar(t->content,t2->inner,inner_count++);
    t->inner=mergeCode(2,"t",toString(inner_count++));
    // if(node->op){
        t->code = mergeCode(8, t2->code, "#", t->inner, " = ",t->content, " ", t2->inner, "\n");
        line_count++;
    // }
    return t;
}

//if创建树
Tree* ifOpr(char* name,int headline,int nextline,Tree* op,Tree* stmt){
    Tree* t = createTree(name, 2, op, stmt);
    t->code = mergeCode(12,op->code,
        "#","if ", op->inner, " goto ",toString(headline+2),"\n",
        "#","goto ", toString(nextline),"\n",
        stmt->code);
    return t;
}

//if else创建树
Tree* ifelseOpr(char* name, int headline, int next1, int next2, Tree* op, Tree* stmt1, Tree* stmt2){
    Tree* t = createTree(name, 3, op, stmt1, stmt2);
    t->code = mergeCode(17,op->code,
        "#", "if ", op->inner, " goto ", toString(headline+2), "\n",
        "#", "goto ", toString(next1+1), "\n", 
        stmt1->code, 
        "#", "goto ", toString(next2), "\n",
        stmt2->code);
    return t;
}

//while创建树
Tree* whileOpr(char* name,int head1, int head2,int nextline,Tree* op,Tree* stmt){
    Tree* t = createTree(name, 2, op, stmt);
    t->code = mergeCode(16,op->code,
        "#", "if ", op->inner, " goto ",toString(head2+2),"\n",
        "#","goto ", toString(nextline+1),"\n",
        stmt->code,
        "#","goto ", toString(head1),"\n"
        );
    return t;
}

//for创建树
Tree* forOpr(char* name,int head1, int head2, int nextline, Tree* op1, Tree* op2, Tree* op3, Tree* stmt){
    Tree* t = createTree(name, 3, op1, op2, op3, stmt);
    t->code = mergeCode(18,op1->code,
        op2->code,
        "#","if ", op2->inner, " goto ",toString(head2+2),"\n",
        "#","goto ", toString(nextline+1),"\n",
        stmt->code,
        op3->code,
        "#","goto ", toString(head1),"\n"
        );
    return t;
}

Tree* retNull(char* name,Tree* ret){
    Tree* t = createTree(name, 1, ret);
    t->code = "#return\n";
    line_count++;
    return t;
}

Tree* retOpr(char* name,Tree* ret,Tree* op){
    Tree* t = createTree(name, 2, ret, op);
    t->code = mergeCode(4,op->code,"#return ",op->inner, "\n");
    line_count++;
    return t;
}

Tree* unaryFunc(char* name,Tree* func, Tree* op){
    Tree* t = createTree(name, 2, func, op);
    t->code = mergeCode(6, op->code,"#arg ", op->inner,"\n#call ",name,"\n");
    return t;
}

Tree* addDeclator(char* name, Tree* t1, Tree* t2){
    Declator* d;
    if(t1->declator){
        for(d = t1->declator; d->next; d = d->next);
        d->next = (Declator*)malloc(sizeof(Declator));
        d = d->next;
    }else{
        t1->declator = (Declator*)malloc(sizeof(Declator));
        d = t1->declator;
    }
    if(!strcmp(name, "Array")){
        d->type = ARRAY;
        t1->declator->length = t2;
    }else{
        d->type = POINTER;
    }
    d->next = NULL;
    return t1;
}



void printTree(Tree* tree){
    int i;
    if(!tree){
        return;
    }
    Stack* s = createStack();
    push(s, tree, 0);
    SNode* temp = NULL;
    while((temp = pop(s))){
        for(i = 0; i < temp->level; i++){
            fprintf(out, "    ");
        }
        fprintf(out, "%s", temp->tree->name);
        if(temp->tree->content){
            fprintf(out, " %s", temp->tree->content);
        }
        // if(temp->tree->inner){
        //     fprintf(out, " inner: %s", temp->tree->inner);
        // }
        // if(temp->tree->code){
        //     fprintf(out, " code: %s", temp->tree->code);
        // }
        fprintf(out, "\n");
        for(i = temp->tree->num - 1; i >= 0; i--){
            push(s, temp->tree->leaves[i], temp->level + 1);
        }
    }
}

void freeTree(Tree* tree){
    if(!tree){
        return;
    }
    return;
}

Tree* initTree(int num){
    Tree* tree = (Tree*)malloc(sizeof(Tree)*num);
    tree->content = NULL;
    tree->name = NULL;
    tree->line =0;
    tree->num = 0;
    tree->headline = 0;
    tree->nextline = 0;
    tree->inner = NULL;
    tree->code = NULL;
    tree->leaves = NULL;
    tree->next = NULL;
    tree->declator = NULL;
    return tree;
}

