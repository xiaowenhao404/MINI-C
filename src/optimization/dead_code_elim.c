/**
 * Mini-C 编译器 - 死代码消除优化实现
 * 
 * 文件: dead_code_elim.c
 * 描述: 死代码消除优化的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "dead_code_elim.h"
#include "const_folding.h"  // 复用常量判断函数
#include "../../c-complier-master/tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 优化统计 ==================== */

static DCEStats dce_stats = {0, 0, 0, 0};

DCEStats get_dce_stats(void) {
    return dce_stats;
}

void reset_dce_stats(void) {
    dce_stats.eliminated_if_blocks = 0;
    dce_stats.eliminated_else_blocks = 0;
    dce_stats.eliminated_loops = 0;
    dce_stats.total_nodes_removed = 0;
}

void print_dce_stats(void) {
    printf("\n========== 死代码消除优化统计 ==========\n");
    printf("消除的if块: %d\n", dce_stats.eliminated_if_blocks);
    printf("消除的else块: %d\n", dce_stats.eliminated_else_blocks);
    printf("消除的循环: %d\n", dce_stats.eliminated_loops);
    printf("总共移除节点: %d\n", dce_stats.total_nodes_removed);
    printf("======================================\n\n");
}

/* ==================== 常量条件判断 ==================== */

/**
 * 判断是否为常量0
 */
bool is_constant_zero(struct Tree *node) {
    if (!node) {
        return false;
    }
    
    // 使用常量折叠模块的函数
    if (!is_constant_node(node)) {
        return false;
    }
    
    int value = get_int_value(node);
    return value == 0;
}

/**
 * 判断是否为常量非零值
 */
bool is_constant_nonzero(struct Tree *node) {
    if (!node) {
        return false;
    }
    
    // 使用常量折叠模块的函数
    if (!is_constant_node(node)) {
        return false;
    }
    
    int value = get_int_value(node);
    return value != 0;
}

/**
 * 判断是否为编译期常量
 */
bool is_compile_time_constant(struct Tree *node, int *value) {
    if (!node) {
        return false;
    }
    
    if (!is_constant_node(node)) {
        return false;
    }
    
    if (value) {
        *value = get_int_value(node);
    }
    
    return true;
}

/* ==================== 节点类型判断 ==================== */

/**
 * 判断是否为if语句
 */
bool is_if_statement(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    
    return strcmp(node->name, "if_expression") == 0 ||
           strcmp(node->name, "if_else_expression") == 0;
}

/**
 * 判断是否为while语句
 */
bool is_while_statement(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    
    return strcmp(node->name, "while_expression") == 0;
}

/**
 * 判断是否为for语句
 */
bool is_for_statement(struct Tree *node) {
    if (!node || !node->name) {
        return false;
    }
    
    return strcmp(node->name, "for_expression") == 0;
}

/* ==================== 死代码消除核心算法 ==================== */

/**
 * 对AST进行死代码消除优化
 */
struct Tree* eliminate_dead_code(struct Tree *node) {
    if (!node) {
        return NULL;
    }
    
    // if语句优化
    if (is_if_statement(node)) {
        // if_expression 或 if_else_expression
        // 结构: leaves[0]=条件, leaves[1]=then分支, leaves[2]=else分支(可选)
        
        if (node->num >= 1 && node->leaves[0]) {
            Tree *cond = node->leaves[0];
            
            // 先对条件进行常量折叠
            cond = fold_constants(cond);
            node->leaves[0] = cond;
            
            // 检查条件是否为常量0（恒假）
            if (is_constant_zero(cond)) {
                // if(0) { then } else { els }
                dce_stats.eliminated_if_blocks++;
                dce_stats.total_nodes_removed++;
                
                // 如果有else分支，返回else分支
                if (node->num >= 3 && node->leaves[2]) {
                    return eliminate_dead_code(node->leaves[2]);
                }
                
                // 没有else分支，删除整个if语句
                return NULL;
            }
            
            // 检查条件是否为常量非零（恒真）
            if (is_constant_nonzero(cond)) {
                // if(1) { then } else { els }
                
                // 如果有else分支，标记为死代码
                if (node->num >= 3 && node->leaves[2]) {
                    dce_stats.eliminated_else_blocks++;
                    dce_stats.total_nodes_removed++;
                }
                
                // 返回then分支
                if (node->num >= 2 && node->leaves[1]) {
                    return eliminate_dead_code(node->leaves[1]);
                }
            }
        }
        
        // 条件不是常量，递归优化then和else分支
        if (node->num >= 2 && node->leaves[1]) {
            node->leaves[1] = eliminate_dead_code(node->leaves[1]);
        }
        if (node->num >= 3 && node->leaves[2]) {
            node->leaves[2] = eliminate_dead_code(node->leaves[2]);
        }
        
        return node;
    }
    
    // while语句优化
    if (is_while_statement(node)) {
        // while_expression
        // 结构: leaves[0]=条件, leaves[1]=循环体
        
        if (node->num >= 1 && node->leaves[0]) {
            Tree *cond = node->leaves[0];
            
            // 先对条件进行常量折叠
            cond = fold_constants(cond);
            node->leaves[0] = cond;
            
            // 检查条件是否为常量0（恒假）
            if (is_constant_zero(cond)) {
                // while(0) { body }
                // 循环永远不执行，删除整个循环
                dce_stats.eliminated_loops++;
                dce_stats.total_nodes_removed++;
                return NULL;
            }
            
            // 条件为常量非零（恒真循环，保留但可以警告）
            if (is_constant_nonzero(cond)) {
                printf("警告: 检测到无限循环 while(1)\n");
            }
        }
        
        // 递归优化循环体
        if (node->num >= 2 && node->leaves[1]) {
            node->leaves[1] = eliminate_dead_code(node->leaves[1]);
        }
        
        return node;
    }
    
    // for语句优化（简化处理）
    if (is_for_statement(node)) {
        // for语句结构复杂，暂时只递归优化子节点
        for (int i = 0; i < node->num; i++) {
            if (node->leaves && node->leaves[i]) {
                node->leaves[i] = eliminate_dead_code(node->leaves[i]);
            }
        }
        return node;
    }
    
    // 复合语句或句子列表：递归处理所有子节点
    if (node->name && 
        (strcmp(node->name, "sentence") == 0 || 
         strcmp(node->name, "statement") == 0)) {
        for (int i = 0; i < node->num; i++) {
            if (node->leaves && node->leaves[i]) {
                node->leaves[i] = eliminate_dead_code(node->leaves[i]);
            }
        }
        return node;
    }
    
    // 其他节点类型：递归优化子节点
    for (int i = 0; i < node->num; i++) {
        if (node->leaves && node->leaves[i]) {
            node->leaves[i] = eliminate_dead_code(node->leaves[i]);
        }
    }
    
    return node;
}

