#include "inner.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define MAX_LENGTH 5
#define MAX_SIZE 1000
struct Node* head ;

Node* getNodeByDoubleVar(char* op, char* var0, char* var1,int inner_count){
    Node *p = head;
    while (p)
    {
        if (p->num = 2 && !strcmp(p->var[0],var0) && !strcmp(p->var[1],var1) && !strcmp(p->op,op))
        {
            Node *new_node = (Node*)malloc(sizeof(Node));
            new_node->op = NULL;
            new_node->inner = p->inner;
            return new_node;//返回相同的中间代码指针
        }
        p=p->next;
    }
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->var = (char**)malloc(sizeof(char*)*2);//二目运算符
    new_node->inner = (char*)malloc(20);//中间代码最高20位
    new_node->num = 2;
    sprintf(new_node->inner,"t%d",inner_count++);
    
    new_node->var[0] = (char*)malloc(strlen(var0)+1);
    new_node->var[1] = (char*)malloc(strlen(var1)+1);
    new_node->op = (char*)malloc(strlen(op)+1);
    
    memcpy(new_node->var[0], var0, strlen(var0)+1);
    memcpy(new_node->var[1], var1, strlen(var1)+1);
    memcpy(new_node->op, op, strlen(op)+1);

    if(head){
        new_node->next = head;
    }
        
    head = new_node;
    return head;
}



Node* getNodeBySingleVar(char* op, char* var,int inner_count){
    Node *p = head;
    while (p)
    {
        if (p->num = 1 && !strcmp(p->var[0],var) && !strcmp(p->op,op))
        {
            Node *new_node = (Node*)malloc(sizeof(Node));
            new_node->op = NULL;
            new_node->inner = p->inner;
            return new_node;//返回相同的中间代码指针
        }
        p=p->next;
    }
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->var = (char**)malloc(sizeof(char*)*1);//单目运算符
    new_node->inner = (char*)malloc(20);//中间代码最高20位
    new_node->num = 1;
    sprintf(new_node->inner,"t%d",inner_count++);
    
    new_node->var[0] = (char*)malloc(strlen(var)+1);
    new_node->op = (char*)malloc(strlen(op)+1);
    
    memcpy(new_node->var[0], var, strlen(var)+1);
    memcpy(new_node->op, op, strlen(op)+1);

    if(head){
        new_node->next = head;
    }
        
    head = new_node;
    return head;
}


char* mergeCode(int num, ...){
    char *str= (char*)malloc(MAX_SIZE);
    int i=0;
    va_list valist;
    va_start(valist, num);
    for(;i<num;i++){
        char* temp = va_arg(valist, char*);
        if(!temp)
            continue;
        strcat(str,temp);
    }
    if(strlen(str) ==0)
        return NULL;
    return str;
}


char* lineToString(int number){
    char* str = (char*)malloc(MAX_LENGTH);
    sprintf(str, "%d: ",number);
    return str;
}

char* toString(int number){
    char* str = (char*)malloc(MAX_LENGTH);
    sprintf(str, "%d",number);
    return str;
}

int swap(char *text, char *a, char *b)
{
    char *str = text, back[MAX_SIZE];
    if ((str = strstr(str, a)) != NULL)
    {
        strcpy(back, str + strlen(a));
        *str = 0;
        strcat(text, b);
        strcat(text, back);
        str += strlen(b);
        return 1;
    }
    return 0;
}