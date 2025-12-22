#ifndef INNER_H
#define INNER_H

typedef struct Node{
    char* inner;//
    char* op;//存储算符
    char** var;//存储多个变量名
    int num;//存储变量个数（子节点个数）
    struct Node* next;
}Node;

//获取双目运算中间代码
struct Node* getNodeByDoubleVar(char* op, char* var0, char* var1,int inner_count);

//获取单目运算中间代码
struct Node* getNodeBySingleVar(char* op, char* var,int inner_count);

//拼接多个字符串
char* mergeCode(int num, ...);

//生成行号
char* lineToString(int number);

//int转换为char*
char* toString(int number);

//替换字符串
int swap(char *text, char *a, char *b);

#endif