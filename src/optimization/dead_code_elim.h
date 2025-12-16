/**
 * Mini-C 编译器 - 死代码消除优化
 * 
 * 文件: dead_code_elim.h
 * 描述: 死代码消除优化的接口定义
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 * 
 * 功能:
 * - 识别并移除永远不会执行的代码
 * - 优化恒假/恒真条件分支
 * - 删除恒假循环
 * - 减少生成的IR和汇编代码
 */

#ifndef DEAD_CODE_ELIM_H
#define DEAD_CODE_ELIM_H

#include <stdbool.h>

// 前向声明
struct Tree;

/* ==================== 死代码消除主函数 ==================== */

/**
 * 对AST进行死代码消除优化
 * 
 * 识别并移除永远不会执行的代码：
 * - if(0) {...} → 删除或保留else分支
 * - if(1) {A} else {B} → 只保留A
 * - while(0) {...} → 删除整个循环
 * 
 * @param node AST节点指针
 * @return 优化后的AST节点指针（可能是NULL）
 */
struct Tree* eliminate_dead_code(struct Tree *node);

/* ==================== 常量条件判断 ==================== */

/**
 * 判断节点是否为常量0
 * 
 * @param node AST节点指针
 * @return 是常量0返回true，否则返回false
 */
bool is_constant_zero(struct Tree *node);

/**
 * 判断节点是否为常量非零值（真值）
 * 
 * @param node AST节点指针
 * @return 是非零常量返回true，否则返回false
 */
bool is_constant_nonzero(struct Tree *node);

/**
 * 判断节点是否为编译期可确定的常量
 * 
 * @param node AST节点指针
 * @param value 如果是常量，存储其值
 * @return 是常量返回true，否则返回false
 */
bool is_compile_time_constant(struct Tree *node, int *value);

/* ==================== 节点类型判断 ==================== */

/**
 * 判断节点是否为if语句
 * 
 * @param node AST节点指针
 * @return 是if语句返回true，否则返回false
 */
bool is_if_statement(struct Tree *node);

/**
 * 判断节点是否为while语句
 * 
 * @param node AST节点指针
 * @return 是while语句返回true，否则返回false
 */
bool is_while_statement(struct Tree *node);

/**
 * 判断节点是否为for语句
 * 
 * @param node AST节点指针
 * @return 是for语句返回true，否则返回false
 */
bool is_for_statement(struct Tree *node);

/* ==================== 优化统计 ==================== */

/**
 * 死代码消除统计信息
 */
typedef struct DCEStats {
    int eliminated_if_blocks;       // 消除的if块数量
    int eliminated_else_blocks;     // 消除的else块数量
    int eliminated_loops;           // 消除的循环数量
    int total_nodes_removed;        // 总共移除的节点数量
} DCEStats;

/**
 * 获取死代码消除统计信息
 * 
 * @return 统计信息结构
 */
DCEStats get_dce_stats(void);

/**
 * 重置死代码消除统计信息
 */
void reset_dce_stats(void);

/**
 * 打印死代码消除统计信息
 */
void print_dce_stats(void);

#endif /* DEAD_CODE_ELIM_H */

