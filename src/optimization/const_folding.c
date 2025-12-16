/**
 * Mini-C 编译器 - 常量折叠优化实现
 * 
 * 文件: const_folding.c
 * 描述: 常量折叠优化的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "const_folding.h"
#include "../../c-complier-master/tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* ==================== 优化统计 ==================== */

static OptimizationStats stats = {0, 0, 0};

OptimizationStats get_optimization_stats(void) {
    return stats;
}

void reset_optimization_stats(void) {
    stats.folded_expressions = 0;
    stats.nodes_removed = 0;
    stats.nodes_created = 0;
}

void print_optimization_stats(void) {
    printf("\n========== 常量折叠优化统计 ==========\n");
    printf("折叠的表达式: %d\n", stats.folded_expressions);
    printf("移除的节点: %d\n", stats.nodes_removed);
    printf("创建的节点: %d\n", stats.nodes_created);
    printf("====================================\n\n");
}

/* ==================== 常量节点判断 ==================== */

/**
 * 判断是否为常量节点
 */
bool is_constant_node(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    
    // INT10, INT8, INT16 都是整数常量
    return strcmp(node->name, "INT10") == 0 ||
           strcmp(node->name, "INT8") == 0 ||
           strcmp(node->name, "INT16") == 0;
    // TODO: 添加浮点常量 FLOAT_LIT 支持
}

/**
 * 判断是否为整数常量
 */
bool is_int_constant(struct Tree *node) {
    return is_constant_node(node);
}

/**
 * 判断是否为浮点常量
 */
bool is_float_constant(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    // TODO: 添加浮点字面量识别
    return false;
}

/* ==================== 常量值获取 ==================== */

/**
 * 获取整数常量的值
 */
int get_int_value(struct Tree *node) {
    if (!node || !node->content) {
        return 0;
    }
    
    // 根据不同进制解析
    if (strcmp(node->name, "INT8") == 0) {
        // 八进制
        return (int)strtol(node->content, NULL, 8);
    } else if (strcmp(node->name, "INT16") == 0) {
        // 十六进制
        return (int)strtol(node->content, NULL, 16);
    } else {
        // 十进制
        return atoi(node->content);
    }
}

/**
 * 获取浮点常量的值
 */
float get_float_value(struct Tree *node) {
    if (!node || !node->content) {
        return 0.0f;
    }
    
    return atof(node->content);
}

/* ==================== 常量运算计算 ==================== */

/**
 * 计算整数二元运算
 */
bool eval_int_binop(char op, int lhs, int rhs, int *result) {
    if (!result) {
        return false;
    }
    
    switch (op) {
        case '+':
            *result = lhs + rhs;
            return true;
        case '-':
            *result = lhs - rhs;
            return true;
        case '*':
            *result = lhs * rhs;
            return true;
        case '/':
            // 检查除零
            if (rhs == 0) {
                fprintf(stderr, "警告: 常量折叠遇到除零运算\n");
                return false;
            }
            *result = lhs / rhs;
            return true;
        case '%':
            // 检查除零
            if (rhs == 0) {
                fprintf(stderr, "警告: 常量折叠遇到取模零运算\n");
                return false;
            }
            *result = lhs % rhs;
            return true;
        default:
            return false;
    }
}

/**
 * 计算浮点二元运算
 */
bool eval_float_binop(char op, float lhs, float rhs, float *result) {
    if (!result) {
        return false;
    }
    
    switch (op) {
        case '+':
            *result = lhs + rhs;
            return true;
        case '-':
            *result = lhs - rhs;
            return true;
        case '*':
            *result = lhs * rhs;
            return true;
        case '/':
            // 检查除零
            if (fabs(rhs) < 1e-10) {
                fprintf(stderr, "警告: 常量折叠遇到浮点除零运算\n");
                return false;
            }
            *result = lhs / rhs;
            return true;
        default:
            return false;
    }
}

/**
 * 计算关系运算
 */
bool eval_relational_op(const char *op, int lhs, int rhs, int *result) {
    if (!op || !result) {
        return false;
    }
    
    if (strcmp(op, "<") == 0) {
        *result = (lhs < rhs) ? 1 : 0;
    } else if (strcmp(op, ">") == 0) {
        *result = (lhs > rhs) ? 1 : 0;
    } else if (strcmp(op, "<=") == 0) {
        *result = (lhs <= rhs) ? 1 : 0;
    } else if (strcmp(op, ">=") == 0) {
        *result = (lhs >= rhs) ? 1 : 0;
    } else if (strcmp(op, "==") == 0) {
        *result = (lhs == rhs) ? 1 : 0;
    } else if (strcmp(op, "!=") == 0) {
        *result = (lhs != rhs) ? 1 : 0;
    } else {
        return false;
    }
    
    return true;
}

/**
 * 计算逻辑运算
 */
bool eval_logical_op(const char *op, int lhs, int rhs, int *result) {
    if (!op || !result) {
        return false;
    }
    
    if (strcmp(op, "&&") == 0) {
        *result = (lhs && rhs) ? 1 : 0;
    } else if (strcmp(op, "||") == 0) {
        *result = (lhs || rhs) ? 1 : 0;
    } else {
        return false;
    }
    
    return true;
}

/* ==================== 常量节点创建 ==================== */

/**
 * 创建整数常量节点
 */
struct Tree* create_int_constant(int value, int line) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    if (!node) {
        fprintf(stderr, "错误: 常量节点内存分配失败\n");
        return NULL;
    }
    
    // 设置节点属性
    node->name = strdup("INT10");
    
    // 将整数转换为字符串
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    node->content = strdup(buffer);
    
    node->line = line;
    node->num = 0;
    node->leaves = NULL;
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    
    stats.nodes_created++;
    
    return node;
}

/**
 * 创建浮点常量节点
 */
struct Tree* create_float_constant(float value, int line) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    if (!node) {
        fprintf(stderr, "错误: 常量节点内存分配失败\n");
        return NULL;
    }
    
    // 设置节点属性
    node->name = strdup("FLOAT_LIT");  // 假设浮点字面量节点名
    
    // 将浮点转换为字符串
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%f", value);
    node->content = strdup(buffer);
    
    node->line = line;
    node->num = 0;
    node->leaves = NULL;
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    
    stats.nodes_created++;
    
    return node;
}

/* ==================== 常量折叠核心算法 ==================== */

/**
 * 对AST进行常量折叠优化
 */
struct Tree* fold_constants(struct Tree *node) {
    if (!node) {
        return NULL;
    }
    
    // 叶子节点（常量或变量）直接返回
    if (node->num == 0) {
        return node;
    }
    
    // 递归优化所有子节点
    for (int i = 0; i < node->num; i++) {
        if (node->leaves && node->leaves[i]) {
            node->leaves[i] = fold_constants(node->leaves[i]);
        }
    }
    
    // 检查是否为可折叠的二元运算表达式
    if (!node->name) {
        return node;
    }
    
    bool is_binary_expr = 
        strcmp(node->name, "additive_expression") == 0 ||
        strcmp(node->name, "multiplicative_expression") == 0 ||
        strcmp(node->name, "relational_expression") == 0 ||
        strcmp(node->name, "equality_expression") == 0 ||
        strcmp(node->name, "logical_and_expression") == 0 ||
        strcmp(node->name, "logical_or_expression") == 0;
    
    if (!is_binary_expr || node->num < 3) {
        return node;
    }
    
    // 二元运算表达式结构: leaves[0]=左操作数, leaves[1]=运算符, leaves[2]=右操作数
    Tree *left = node->leaves[0];
    Tree *right = node->leaves[2];
    Tree *op_node = node->leaves[1];
    
    if (!left || !right || !op_node || !op_node->content) {
        return node;
    }
    
    // 检查左右是否都是常量
    if (!is_constant_node(left) || !is_constant_node(right)) {
        return node;
    }
    
    // 获取运算符
    const char *op_str = op_node->content;
    char op = op_str[0];  // 大多数运算符是单字符
    
    // 获取常量值
    int left_val = get_int_value(left);
    int right_val = get_int_value(right);
    int result_val;
    
    // 尝试计算
    bool success = false;
    
    // 算术运算
    if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%') {
        success = eval_int_binop(op, left_val, right_val, &result_val);
    }
    // 关系运算
    else if (strcmp(op_str, "<") == 0 || strcmp(op_str, ">") == 0 ||
             strcmp(op_str, "<=") == 0 || strcmp(op_str, ">=") == 0 ||
             strcmp(op_str, "==") == 0 || strcmp(op_str, "!=") == 0) {
        success = eval_relational_op(op_str, left_val, right_val, &result_val);
    }
    // 逻辑运算
    else if (strcmp(op_str, "&&") == 0 || strcmp(op_str, "||") == 0) {
        success = eval_logical_op(op_str, left_val, right_val, &result_val);
    }
    
    if (success) {
        // 创建结果常量节点
        Tree *result_node = create_int_constant(result_val, node->line);
        
        // 更新统计信息
        stats.folded_expressions++;
        stats.nodes_removed += 2;  // 左右操作数将被释放
        
        // 注意：这里简化实现，不释放旧节点
        // 完整实现应该释放 node, left, right, op_node
        // 但需要小心处理，避免重复释放
        
        return result_node;
    }
    
    return node;
}

