/**
 * Mini-C 编译器 - 常量折叠优化
 * 
 * 文件: const_folding.h
 * 描述: 常量折叠优化的接口定义
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 * 
 * 功能:
 * - 在AST层面识别常量表达式
 * - 编译期计算常量运算
 * - 减少生成的IR指令数量
 * - 提高程序运行效率
 */

#ifndef CONST_FOLDING_H
#define CONST_FOLDING_H

#include <stdbool.h>

// 前向声明
struct Tree;

/* ==================== 常量折叠主函数 ==================== */

/**
 * 对AST进行常量折叠优化
 * 
 * 递归遍历AST，识别并计算常量表达式，将其替换为结果常量节点
 * 
 * @param node AST节点指针
 * @return 优化后的AST节点指针（可能是新节点）
 */
struct Tree* fold_constants(struct Tree *node);

/* ==================== 辅助判断函数 ==================== */

/**
 * 判断节点是否为常量节点
 * 
 * 常量节点包括：INT10, INT8, INT16等数字字面量
 * 
 * @param node AST节点指针
 * @return 是常量返回true，否则返回false
 */
bool is_constant_node(struct Tree *node);

/**
 * 判断节点是否为整数常量
 * 
 * @param node AST节点指针
 * @return 是整数常量返回true，否则返回false
 */
bool is_int_constant(struct Tree *node);

/**
 * 判断节点是否为浮点常量
 * 
 * @param node AST节点指针
 * @return 是浮点常量返回true，否则返回false
 */
bool is_float_constant(struct Tree *node);

/* ==================== 常量值获取 ==================== */

/**
 * 获取整数常量的值
 * 
 * @param node 常量节点
 * @return 整数值
 */
int get_int_value(struct Tree *node);

/**
 * 获取浮点常量的值
 * 
 * @param node 常量节点
 * @return 浮点值
 */
float get_float_value(struct Tree *node);

/* ==================== 常量运算计算 ==================== */

/**
 * 计算整数二元运算
 * 
 * @param op 运算符（'+', '-', '*', '/', '%'）
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @param result 存储计算结果的指针
 * @return 成功返回true，失败（如除零）返回false
 */
bool eval_int_binop(char op, int lhs, int rhs, int *result);

/**
 * 计算浮点二元运算
 * 
 * @param op 运算符（'+', '-', '*', '/'）
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @param result 存储计算结果的指针
 * @return 成功返回true，失败返回false
 */
bool eval_float_binop(char op, float lhs, float rhs, float *result);

/**
 * 计算关系运算（返回0或1）
 * 
 * @param op 运算符（'<', '>', "<=", ">="等）
 * @param lhs 左操作数（整数）
 * @param rhs 右操作数（整数）
 * @param result 存储计算结果（0或1）
 * @return 成功返回true
 */
bool eval_relational_op(const char *op, int lhs, int rhs, int *result);

/**
 * 计算逻辑运算
 * 
 * @param op 运算符（"&&", "||"）
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @param result 存储计算结果（0或1）
 * @return 成功返回true
 */
bool eval_logical_op(const char *op, int lhs, int rhs, int *result);

/* ==================== 常量节点创建 ==================== */

/**
 * 创建整数常量节点
 * 
 * @param value 整数值
 * @param line 行号
 * @return 新的常量节点
 */
struct Tree* create_int_constant(int value, int line);

/**
 * 创建浮点常量节点
 * 
 * @param value 浮点值
 * @param line 行号
 * @return 新的常量节点
 */
struct Tree* create_float_constant(float value, int line);

/* ==================== 优化统计 ==================== */

/**
 * 获取优化统计信息结构
 */
typedef struct OptimizationStats {
    int folded_expressions;     // 折叠的表达式数量
    int nodes_removed;          // 移除的节点数量
    int nodes_created;          // 创建的新节点数量
} OptimizationStats;

/**
 * 获取优化统计信息
 * 
 * @return 优化统计结构
 */
OptimizationStats get_optimization_stats(void);

/**
 * 重置优化统计信息
 */
void reset_optimization_stats(void);

/**
 * 打印优化统计信息
 */
void print_optimization_stats(void);

#endif /* CONST_FOLDING_H */

