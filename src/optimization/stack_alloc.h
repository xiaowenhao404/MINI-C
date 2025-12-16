/**
 * Mini-C 编译器 - 栈槽复用优化
 * 
 * 文件: stack_alloc.h
 * 描述: 基于活性分析的栈槽复用优化
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#ifndef STACK_ALLOC_H
#define STACK_ALLOC_H

#include "cfg.h"
#include "liveness.h"

/* ==================== 干涉图 ==================== */

/**
 * 干涉图节点（变量）
 */
typedef struct InterferenceNode {
    char *var_name;              // 变量名
    int color;                   // 分配的颜色（栈槽编号），-1表示未分配
    int degree;                  // 度数（邻居数量）
    
    struct InterferenceNode **neighbors;  // 邻居节点数组
    int neighbor_count;                  // 邻居数量
    int neighbor_capacity;               // 邻居容量
    
    bool is_spilled;             // 是否溢出（无法着色）
} InterferenceNode;

/**
 * 干涉图
 */
typedef struct InterferenceGraph {
    InterferenceNode **nodes;    // 节点数组
    int node_count;              // 节点数量
    int node_capacity;           // 节点容量
} InterferenceGraph;

/**
 * 创建干涉图
 */
InterferenceGraph* interference_graph_create(void);

/**
 * 销毁干涉图
 */
void interference_graph_destroy(InterferenceGraph *ig);

/**
 * 添加变量节点
 */
InterferenceNode* interference_graph_add_node(InterferenceGraph *ig, const char *var_name);

/**
 * 查找变量节点
 */
InterferenceNode* interference_graph_find_node(InterferenceGraph *ig, const char *var_name);

/**
 * 添加干涉边（两个变量不能共享栈槽）
 */
void interference_graph_add_edge(InterferenceGraph *ig, const char *var1, const char *var2);

/* ==================== 栈槽分配 ==================== */

/**
 * 从CFG构建干涉图
 * 
 * 规则：如果两个变量在某个基本块的 LIVE_OUT 中同时存在，则它们干涉
 */
InterferenceGraph* build_interference_graph(CFG *cfg);

/**
 * 使用贪心着色算法分配栈槽
 * 
 * @param ig 干涉图
 * @param max_colors 最大可用颜色数（栈槽数）
 * @return 使用的最大颜色编号
 */
int allocate_stack_slots(InterferenceGraph *ig, int max_colors);

/**
 * 获取变量的栈槽编号
 */
int get_variable_slot(InterferenceGraph *ig, const char *var_name);

/**
 * 打印干涉图（用于调试）
 */
void print_interference_graph(InterferenceGraph *ig);

/**
 * 打印栈槽分配结果
 */
void print_stack_allocation(InterferenceGraph *ig);

#endif /* STACK_ALLOC_H */

