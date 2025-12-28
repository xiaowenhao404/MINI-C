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
extern struct HashMap *hashMap;
extern int scope, type, preType;
void destoryPartOfHashMap(struct HashMap *hashMap, int scope);

extern struct Tree *root;
extern struct Node *head;
int inner_count = 1;
extern int line_count;

Tree *createTree(char *name, int number, ...)
{
    int i;
    va_list valist;
    va_start(valist, number);
    if (number == 1)
    {
        return va_arg(valist, Tree *);
    }
    Tree *tree = initTree(1);
    if (!tree)
    {
        printf("Out of space \n");
        exit(0);
    }
    tree->num = number;
    int len = strlen(name);
    tree->name = (char *)malloc(len + 1);
    memcpy(tree->name, name, len + 1);

    char *str = (char *)malloc(100000); // 增加缓冲区大小
    str[0] = '\0';                      // 初始化为空字符串
    tree->leaves = (Tree **)malloc(sizeof(Tree *) * number);
    for (i = 0; i < number; i++)
    {
        tree->leaves[i] = va_arg(valist, Tree *);
        if (tree->leaves[i]->code)
        {
            strcat(str, tree->leaves[i]->code);
        }
    }
    if (strlen(str) > 0)
        tree->code = str;

    return tree;
}

// 在lex文件里创建终结符节点
Tree *terminator(char *name, int yylineno)
{
    Tree *tree = initTree(1);
    if (!tree)
    {
        printf("Out of space \n");
        exit(0);
    }
    tree->num = 0;
    int len = strlen(name);
    tree->name = (char *)malloc(len + 1);
    memcpy(tree->name, name, len + 1);

    int line = yylineno;
    tree->line = line;
    if (!strcmp(name, "null"))
    {
        len = 1;
    }
    else
    {
        len = strlen(yytext);
        fprintf(yyout, "%-15s\t%-15s", name, yytext);
        if (!strcmp(name, "ID"))
        {
            fprintf(yyout, "%p\n", hashMap);
        }
        else
        {
            if (type != 0)
            {
                preType = type;
                type = 0;
            }
            if (!strcmp(name, "INT8") || !strcmp(name, "INT10") || !strcmp(name, "INT16"))
            {
                fprintf(yyout, "%s\n", yytext);
            }
            else
            {
                fprintf(yyout, "\n");
                if (!strcmp(name, "COMMA"))
                {
                    type = preType;
                }
            }
        }
        if (!strcmp(name, "LCB"))
        {
            scope++;
            preType = 0;
        }
        else if (!strcmp(name, "RCB"))
        {
            destoryPartOfHashMap(hashMap, scope--);
        }
    }
    tree->content = (char *)malloc(len + 1);
    memcpy(tree->content, yytext, len + 1);
    tree->inner = (char *)malloc(len + 1);
    memcpy(tree->inner, yytext, len + 1);

    return tree;
}

// op, args ... num is number of args
Tree *op(char *name, int num, ...)
{
    Tree *t = initTree(1);
    int len = strlen(name);
    t->name = (char *)malloc(len + 1);
    memcpy(t->name, name, len + 1);
    t->num = num;
    t->leaves = (Tree **)malloc(sizeof(Tree *) * num);
    va_list valist;
    va_start(valist, num);
    Tree *temp = va_arg(valist, Tree *);
    len = strlen(temp->content);
    t->content = (char *)malloc(len + 1);
    memcpy(t->content, temp->content, len + 1);
    int i;
    for (i = 0; i < num; i++)
    {
        temp = va_arg(valist, Tree *);
        t->leaves[i] = temp;
    }
    t->declator = NULL;
    return t;
}

// 二元运算符创建树
Tree *binaryOpr(char *name, Tree *t1, Tree *t2, Tree *t3)
{
    // 生成树
    Tree *t = op(name, 2, t2, t1, t3);
    // 获取中间代码
    Node *node = getNodeByDoubleVar(t->content, t1->inner, t3->inner, inner_count++);
    // 赋值
    t->inner = node->inner;
    if (node->op)
    {
        // 合并本次操作的中间代码
        t->code = mergeCode(11, t1->code,
            t3->code,
                            "#", t->inner, " = ", t1->inner, " ", t->content, " ", t3->inner, "\n");
        // 行号加一
        line_count++;
    }
    return t;
}

// 赋值语句创建树
Tree *assignOpr(char *name, Tree *t1, Tree *t2, Tree *t3)
{
    Tree *t = op(name, 2, t2, t1, t3);
    t->inner = t1->inner;
    t->code = mergeCode(8, t3->code, 
                        "#", t1->inner, " ", t->content, " ", t3->inner, "\n");
    line_count++;
    return t;
}

// 一元运算符创建树
Tree *unaryOpr(char *name, Tree *t1, Tree *t2)
{
    Tree *t = op(name, 1, t1, t2);
    // Node *node = getNodeBySingleVar(t->content,t2->inner,inner_count++);
    t->inner = mergeCode(2, "t", toString(inner_count++));
    // if(node->op){
    t->code = mergeCode(8, t2->code, "#", t->inner, " = ", t->content, " ", t2->inner, "\n");
        line_count++;
    // }
    return t;
}

// if创建树 - 使用标签系统
Tree *ifOpr(char *name, char *label_true, char *label_end, Tree *op, Tree *stmt)
{
    Tree *t = createTree(name, 2, op, stmt);
    // 生成：if condition goto @label_true
    //       goto @label_end
    //       @label_true: ... stmt ... (标签不占用行号)
    //       @label_end: (结束标签)
    // 标签使用 @LABEL: 格式，不使用 # 前缀，不占用行号
    // 参数数量: 1 + 6 + 4 + 3 + 1 + 3 = 18
    t->code = mergeCode(18, op->code,
                        "#", "if ", op->inner, " goto @", label_true, "\n",
                        "#", "goto @", label_end, "\n",
                        "@", label_true, ":", stmt->code,
                        "@", label_end, ":");
    return t;
}

// if else创建树 - 使用标签系统
Tree *ifelseOpr(char *name, char *label_true, char *label_else, char *label_end, Tree *op, Tree *stmt1, Tree *stmt2)
{
    Tree *t = createTree(name, 3, op, stmt1, stmt2);
    // 生成：if condition goto @label_true
    //       goto @label_else
    //       @label_true: ... stmt1 ... (标签不占用行号)
    //       goto @label_end
    //       @label_else: ... stmt2 ... (标签不占用行号)
    //       @label_end:
    // 参数数量: 1 + 6 + 4 + 3 + 1 + 4 + 3 + 1 + 3 = 26
    t->code = mergeCode(26, op->code,
                        "#", "if ", op->inner, " goto @", label_true, "\n",
                        "#", "goto @", label_else, "\n",
                        "@", label_true, ":", stmt1->code,
                        "#", "goto @", label_end, "\n",
                        "@", label_else, ":", stmt2->code,
                        "@", label_end, ":");
    return t;
}

// while创建树 - 使用标签系统
Tree *whileOpr(char *name, char *label_start, char *label_body, char *label_end, Tree *op, Tree *stmt)
{
    Tree *t = createTree(name, 2, op, stmt);
    // 生成：@label_start: ... condition ... (标签不占用行号)
    //       if condition goto @label_body
    //       goto @label_end
    //       @label_body: ... stmt ... (标签不占用行号)
    //       goto @label_start
    //       @label_end:
    // 参数数量: 3 + 1 + 6 + 4 + 3 + 1 + 4 + 3 = 25
    t->code = mergeCode(25, "@", label_start, ":", op->code,
                        "#", "if ", op->inner, " goto @", label_body, "\n",
                        "#", "goto @", label_end, "\n",
                        "@", label_body, ":", stmt->code,
                        "#", "goto @", label_start, "\n",
                        "@", label_end, ":");
    return t;
}

// for创建树 - 使用标签系统
Tree *forOpr(char *name, char *label_cond, char *label_body, char *label_end, Tree *op1, Tree *op2, Tree *op3, Tree *stmt)
{
    Tree *t = createTree(name, 3, op1, op2, op3, stmt);
    // 生成：... init (op1) ...
    //       @label_cond: ... condition (op2) ... (标签不占用行号)
    //       if condition goto @label_body
    //       goto @label_end
    //       @label_body: ... stmt ... (标签不占用行号)
    //       ... update (op3) ...
    //       goto @label_cond
    //       @label_end:
    // 参数数量: 1 + 3 + 1 + 6 + 4 + 3 + 1 + 1 + 4 + 3 = 27
    t->code = mergeCode(27, op1->code,
                        "@", label_cond, ":", op2->code,
                        "#", "if ", op2->inner, " goto @", label_body, "\n",
                        "#", "goto @", label_end, "\n",
                        "@", label_body, ":", stmt->code,
                        op3->code,
                        "#", "goto @", label_cond, "\n",
                        "@", label_end, ":");
    return t;
}

Tree *retNull(char *name, Tree *ret)
{
    // 不使用 createTree，因为它在 number==1 时会直接返回 ret
    // 我们需要创建一个新的节点来包装 ret
    Tree *t = initTree(1);
    if (!t)
    {
        printf("Out of space \n");
        exit(0);
    }
    t->num = 1;
    int len = strlen(name);
    t->name = (char *)malloc(len + 1);
    memcpy(t->name, name, len + 1);
    t->leaves = (Tree **)malloc(sizeof(Tree *) * 1);
    t->leaves[0] = ret;
    t->code = "#return\n";
    line_count++;
    return t;
}

Tree *retOpr(char *name, Tree *ret, Tree *op)
{
    Tree *t = createTree(name, 2, ret, op);
    t->code = mergeCode(4, op->code, "#return ", op->inner, "\n");
    line_count++;
    return t;
}

Tree *unaryFunc(char *name, Tree *func, Tree *op)
{
    Tree *t = createTree(name, 2, func, op);
    t->code = mergeCode(6, op->code, "#arg ", op->inner, "\n#call ", name, "\n");
    return t;
}

Tree *addDeclator(char *name, Tree *t1, Tree *t2)
{
    Declator *d;
    if (t1->declator)
    {
        for (d = t1->declator; d->next; d = d->next)
            ;
        d->next = (Declator *)malloc(sizeof(Declator));
        d = d->next;
    }
    else
    {
        t1->declator = (Declator *)malloc(sizeof(Declator));
        d = t1->declator;
    }
    if (!strcmp(name, "Array"))
    {
        d->type = ARRAY;
        t1->declator->length = t2;
    }
    else
    {
        d->type = POINTER;
    }
    d->next = NULL;
    return t1;
}

// ============ 函数定义和调用 ============

// 函数定义中间代码生成
Tree *funcDefOpr(char *func_name, Tree *params, Tree *body)
{
    Tree *t = initTree(1);
    t->name = "FUNC_DEF";
    
    // 生成函数定义的中间代码
    // 格式: FUNC_BEGIN func_name
    //       param p1
    //       param p2
    //       ... (函数体代码)
    //       FUNC_END func_name
    if (params && params->code)
    {
        t->code = mergeCode(8, 
            "#FUNC_BEGIN ", func_name, "\n",
            params->code,
            body ? body->code : "",
            "#FUNC_END ", func_name, "\n");
    }
    else
    {
        t->code = mergeCode(6,
            "#FUNC_BEGIN ", func_name, "\n",
            body ? body->code : "",
            "#FUNC_END ", func_name, "\n");
    }
    return t;
}

// 多参数函数调用中间代码生成
Tree *funcCallOpr(char *func_name, Tree *args)
{
    Tree *t = initTree(1);
    t->name = "FUNC_CALL";
    t->inner = mergeCode(2, "t", toString(inner_count++));
    
    // 生成函数调用的中间代码
    // 格式: arg arg1
    //       arg arg2
    //       result = call func_name
    if (args && args->code)
    {
        // 6个字符串参数
        t->code = mergeCode(6,
            args->code,
            "#", t->inner, " = call ", func_name, "\n");
    }
    else
    {
        // 5个字符串参数
        t->code = mergeCode(5,
            "#", t->inner, " = call ", func_name, "\n");
    }
    line_count++;
    return t;
}

// ============ 数组操作 ============

// 数组声明
Tree *arrayDeclOpr(char *arr_name, int size)
{
    Tree *t = initTree(1);
    t->name = "ARRAY_DECL";
    t->inner = arr_name;
    // 格式: arr = alloc size*4 (每个int 4字节)
    t->code = mergeCode(5, "#", arr_name, " = alloc ", toString(size * 4), "\n");
    line_count++;
    return t;
}

// 数组访问 arr[index]
Tree *arrayAccessOpr(Tree *arr, Tree *index)
{
    Tree *t = initTree(1);
    t->name = "ARRAY_ACCESS";
    char *temp_offset = mergeCode(2, "t", toString(inner_count++));
    char *temp_addr = mergeCode(2, "t", toString(inner_count++));
    t->inner = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = index * 4
    //       t2 = arr + t1
    //       t3 = load t2
    t->code = mergeCode(18,
        index->code,
        "#", temp_offset, " = ", index->inner, " * 4\n",
        "#", temp_addr, " = ", arr->inner, " + ", temp_offset, "\n",
        "#", t->inner, " = load ", temp_addr, "\n");
    line_count += 3;
    return t;
}

// 数组赋值 arr[index] = value
Tree *arrayAssignOpr(Tree *arr, Tree *index, Tree *value)
{
    Tree *t = initTree(1);
    t->name = "ARRAY_ASSIGN";
    char *temp_offset = mergeCode(2, "t", toString(inner_count++));
    char *temp_addr = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = index * 4
    //       t2 = arr + t1
    //       store value t2
    // 19个字符串参数
    t->code = mergeCode(19,
        index->code,
        value->code,
        "#", temp_offset, " = ", index->inner, " * 4\n",
        "#", temp_addr, " = ", arr->inner, " + ", temp_offset, "\n",
        "#store ", value->inner, " ", temp_addr, "\n");
    line_count += 3;
    return t;
}

// ============ 指针操作 ============

// 取地址 &var
Tree *addrOfOpr(Tree *var)
{
    Tree *t = initTree(1);
    t->name = "ADDR_OF";
    t->inner = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = addr var
    t->code = mergeCode(6,
        var->code ? var->code : "",
        "#", t->inner, " = addr ", var->inner, "\n");
    line_count++;
    return t;
}

// 解引用 *ptr
Tree *derefOpr(Tree *ptr)
{
    Tree *t = initTree(1);
    t->name = "DEREF";
    t->inner = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = load ptr
    t->code = mergeCode(6,
        ptr->code ? ptr->code : "",
        "#", t->inner, " = load ", ptr->inner, "\n");
    line_count++;
    return t;
}

// 指针赋值 *ptr = value
Tree *ptrAssignOpr(Tree *ptr, Tree *value)
{
    Tree *t = initTree(1);
    t->name = "PTR_ASSIGN";
    
    // 格式: store value ptr
    // 7个字符串参数
    t->code = mergeCode(7,
        ptr->code ? ptr->code : "",
        value->code ? value->code : "",
        "#store ", value->inner, " ", ptr->inner, "\n");
    line_count++;
    return t;
}

// ============ 结构体操作 ============

// 结构体成员访问 struct_var.member
Tree *structAccessOpr(Tree *struct_var, char *member, int offset)
{
    Tree *t = initTree(1);
    t->name = "STRUCT_ACCESS";
    char *temp_addr = mergeCode(2, "t", toString(inner_count++));
    t->inner = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = struct_var + offset
    //       t2 = load t1
    // 13个字符串参数
    t->code = mergeCode(13,
        struct_var->code ? struct_var->code : "",
        "#", temp_addr, " = ", struct_var->inner, " + ", toString(offset), "\n",
        "#", t->inner, " = load ", temp_addr, "\n");
    line_count += 2;
    return t;
}

// 结构体成员赋值 struct_var.member = value
Tree *structAssignOpr(Tree *struct_var, char *member, int offset, Tree *value)
{
    Tree *t = initTree(1);
    t->name = "STRUCT_ASSIGN";
    char *temp_addr = mergeCode(2, "t", toString(inner_count++));
    
    // 格式: t1 = struct_var + offset
    //       store value t1
    // 13个字符串参数
    t->code = mergeCode(13,
        struct_var->code ? struct_var->code : "",
        value->code ? value->code : "",
        "#", temp_addr, " = ", struct_var->inner, " + ", toString(offset), "\n",
        "#store ", value->inner, " ", temp_addr, "\n");
    line_count += 2;
    return t;
}

void printTree(Tree *tree)
{
    int i;
    if (!tree)
    {
        return;
    }
    Stack *s = createStack();
    push(s, tree, 0);
    SNode *temp = NULL;
    while ((temp = pop(s)))
    {
        for (i = 0; i < temp->level; i++)
        {
            fprintf(out, "    ");
        }
        fprintf(out, "%s", temp->tree->name);
        if (temp->tree->content)
        {
            fprintf(out, " %s", temp->tree->content);
        }
        // if(temp->tree->inner){
        //     fprintf(out, " inner: %s", temp->tree->inner);
        // }
        // if(temp->tree->code){
        //     fprintf(out, " code: %s", temp->tree->code);
        // }
        fprintf(out, "\n");
        for (i = temp->tree->num - 1; i >= 0; i--)
        {
            push(s, temp->tree->leaves[i], temp->level + 1);
        }
    }
}

void freeTree(Tree *tree)
{
    if (!tree)
    {
        return;
    }
    return;
}

Tree *initTree(int num)
{
    Tree *tree = (Tree *)malloc(sizeof(Tree) * num);
    tree->content = NULL;
    tree->name = NULL;
    tree->line = 0;
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
