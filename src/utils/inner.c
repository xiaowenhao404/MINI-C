#include "inner.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define MAX_LENGTH 20
#define MAX_SIZE 100000
struct Node *head;

Node *getNodeByDoubleVar(char *op, char *var0, char *var1, int inner_count)
{
    Node *p = head;
    while (p)
    {
        if (p->num == 2 && !strcmp(p->var[0], var0) && !strcmp(p->var[1], var1) && !strcmp(p->op, op))
        {
            Node *new_node = (Node *)malloc(sizeof(Node));
            new_node->op = NULL;
            new_node->inner = p->inner;
            return new_node; // 返回相同的中间代码指针
        }
        p = p->next;
    }
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->var = (char **)malloc(sizeof(char *) * 2); // 二目运算符
    new_node->inner = (char *)malloc(20);                // 中间代码最高20位
    new_node->num = 2;
    sprintf(new_node->inner, "t%d", inner_count++);

    new_node->var[0] = (char *)malloc(strlen(var0) + 1);
    new_node->var[1] = (char *)malloc(strlen(var1) + 1);
    new_node->op = (char *)malloc(strlen(op) + 1);

    memcpy(new_node->var[0], var0, strlen(var0) + 1);
    memcpy(new_node->var[1], var1, strlen(var1) + 1);
    memcpy(new_node->op, op, strlen(op) + 1);

    if (head)
    {
        new_node->next = head;
    }

    head = new_node;
    return head;
}

Node *getNodeBySingleVar(char *op, char *var, int inner_count)
{
    Node *p = head;
    while (p)
    {
        if (p->num == 1 && !strcmp(p->var[0], var) && !strcmp(p->op, op))
        {
            Node *new_node = (Node *)malloc(sizeof(Node));
            new_node->op = NULL;
            new_node->inner = p->inner;
            return new_node; // 返回相同的中间代码指针
        }
        p = p->next;
    }
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->var = (char **)malloc(sizeof(char *) * 1); // 单目运算符
    new_node->inner = (char *)malloc(20);                // 中间代码最高20位
    new_node->num = 1;
    sprintf(new_node->inner, "t%d", inner_count++);

    new_node->var[0] = (char *)malloc(strlen(var) + 1);
    new_node->op = (char *)malloc(strlen(op) + 1);

    memcpy(new_node->var[0], var, strlen(var) + 1);
    memcpy(new_node->op, op, strlen(op) + 1);

    if (head)
    {
        new_node->next = head;
    }

    head = new_node;
    return head;
}

char *mergeCode(int num, ...)
{
    char *str = (char *)malloc(MAX_SIZE);
    str[0] = '\0'; // 初始化为空字符串，修复缓冲区溢出问题
    int i = 0;
    va_list valist;
    va_start(valist, num);
    for (; i < num; i++)
    {
        char *temp = va_arg(valist, char *);
        if (!temp)
            continue;
        strcat(str, temp);
    }
    va_end(valist); // 正确结束 va_list
    if (strlen(str) == 0)
        return NULL;
    return str;
}

char *lineToString(int number)
{
    char *str = (char *)malloc(MAX_LENGTH);
    sprintf(str, "%d: ", number);
    return str;
}

char *toString(int number)
{
    char *str = (char *)malloc(MAX_LENGTH);
    sprintf(str, "%d", number);
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

// 标签映射结构
typedef struct LabelMap
{
    char label[64]; // 标签名
    int line;       // 对应行号
} LabelMap;

// 替换标签为实际行号
// 新格式：标签定义 @LABEL: 不以行号开头，标签对应它后面紧随的第一个带行号的行
// 步骤1：先统计最大行号
// 步骤2：扫描代码，找到 @LABEL: 定义，记录后续的行号（末尾标签用最大行号+1）
// 步骤3：将所有 goto @LABEL 替换为 goto 行号
// 步骤4：删除标签定义
void replaceLabels(char *code)
{
    if (!code)
        return;

    LabelMap labels[256]; // 最多256个标签
    int label_count = 0;

    // 先统计最大行号
    int max_line = 0;
    char *p = code;
    while (*p)
    {
        if (p == code || *(p - 1) == '\n')
        {
            int line_num = 0;
            char *num_start = p;
            while (*p >= '0' && *p <= '9')
            {
                line_num = line_num * 10 + (*p - '0');
                p++;
            }
            if (*p == ':' && *(p + 1) == ' ' && p > num_start)
            {
                if (line_num > max_line)
                    max_line = line_num;
            }
        }
        p++;
    }

    // 第一遍：扫描所有标签定义，记录其后第一个行号
    // 标签格式: "@LABEL:" 后面紧跟 "行号: " 或在下一行有 "行号: "
    p = code;
    while (*p)
    {
        // 查找 @LABEL: 模式（不以行号开头）
        if (*p == '@')
        {
            char *label_start = p + 1; // 跳过 @
            char *label_end = label_start;
            // 找到标签名结束位置
            while (*label_end && *label_end != ':' && *label_end != '\n' &&
                   *label_end != ' ' && *label_end != '@')
            {
                label_end++;
            }
            if (*label_end == ':' && label_end > label_start)
            {
                // 检查这是否是标签定义（不是 goto @LABEL）
                // 往回检查是否有 "goto " 前缀
                int is_goto = 0;
                if (p >= code + 5)
                {
                    if (strncmp(p - 5, "goto ", 5) == 0)
                    {
                        is_goto = 1;
                    }
                }

                if (!is_goto)
                {
                    // 这是标签定义，找到后面的第一个行号
                    int label_len = label_end - label_start;
                    char *search = label_end + 1; // 跳过 ':'
                    int found_line = 0;
                    int target_line = max_line + 1; // 默认使用最大行号+1

                    // 寻找下一个行号（格式：行首的数字后跟 ": "）
                    while (*search && !found_line)
                    {
                        // 检查是否在行首或紧跟标签
                        if (search == label_end + 1 || *(search - 1) == '\n')
                        {
                            int line_num = 0;
                            char *num_start = search;
                            while (*search >= '0' && *search <= '9')
                            {
                                line_num = line_num * 10 + (*search - '0');
                                search++;
                            }
                            if (*search == ':' && *(search + 1) == ' ' && search > num_start)
                            {
                                // 找到行号
                                target_line = line_num;
                                found_line = 1;
                            }
                        }
                        if (!found_line)
                            search++;
                    }

                    // 记录标签（无论是否找到后续行号）
                    if (label_len < 63 && label_count < 256)
                    {
                        strncpy(labels[label_count].label, label_start, label_len);
                        labels[label_count].label[label_len] = '\0';
                        labels[label_count].line = target_line;
                        label_count++;
                    }
                }
            }
        }
        p++;
    }

    // 第二遍：替换所有 goto @LABEL 为 goto 行号
    for (int i = 0; i < label_count; i++)
    {
        char search_pattern[256];
        char replace_str[32];
        snprintf(search_pattern, sizeof(search_pattern), "goto @%s", labels[i].label);
        snprintf(replace_str, sizeof(replace_str), "goto %d", labels[i].line);

        // 替换所有出现
        while (swap(code, search_pattern, replace_str))
            ;
    }

    // 第三遍：删除标签定义（将 "@LABEL:" 替换为空）
    for (int i = 0; i < label_count; i++)
    {
        char search_pattern[256];
        snprintf(search_pattern, sizeof(search_pattern), "@%s:", labels[i].label);

        // 替换为空字符串
        while (swap(code, search_pattern, ""))
            ;
    }
}