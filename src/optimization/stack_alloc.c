/**
 * Mini-C 编译器 - 栈槽复用优化实现
 * 
 * 文件: stack_alloc.c
 * 描述: 基于活性分析的栈槽复用优化
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#include "stack_alloc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 干涉图实现 ==================== */

InterferenceGraph* interference_graph_create(void) {
    InterferenceGraph *ig = (InterferenceGraph*)malloc(sizeof(InterferenceGraph));
    if (!ig) {
        return NULL;
    }
    
    ig->node_count = 0;
    ig->node_capacity = 16;
    ig->nodes = (InterferenceNode**)malloc(sizeof(InterferenceNode*) * ig->node_capacity);
    
    return ig;
}

void interference_graph_destroy(InterferenceGraph *ig) {
    if (!ig) {
        return;
    }
    
    for (int i = 0; i < ig->node_count; i++) {
        InterferenceNode *node = ig->nodes[i];
        if (node) {
            free(node->var_name);
            free(node->neighbors);
            free(node);
        }
    }
    free(ig->nodes);
    free(ig);
}

InterferenceNode* interference_graph_add_node(InterferenceGraph *ig, const char *var_name) {
    if (!ig || !var_name) {
        return NULL;
    }
    
    // 检查是否已存在
    InterferenceNode *existing = interference_graph_find_node(ig, var_name);
    if (existing) {
        return existing;
    }
    
    // 扩容
    if (ig->node_count >= ig->node_capacity) {
        ig->node_capacity *= 2;
        ig->nodes = (InterferenceNode**)realloc(
            ig->nodes,
            sizeof(InterferenceNode*) * ig->node_capacity
        );
    }
    
    // 创建新节点
    InterferenceNode *node = (InterferenceNode*)malloc(sizeof(InterferenceNode));
    node->var_name = strdup(var_name);
    node->color = -1;  // 未分配
    node->degree = 0;
    node->neighbor_count = 0;
    node->neighbor_capacity = 8;
    node->neighbors = (InterferenceNode**)malloc(sizeof(InterferenceNode*) * node->neighbor_capacity);
    node->is_spilled = false;
    
    ig->nodes[ig->node_count++] = node;
    
    return node;
}

InterferenceNode* interference_graph_find_node(InterferenceGraph *ig, const char *var_name) {
    if (!ig || !var_name) {
        return NULL;
    }
    
    for (int i = 0; i < ig->node_count; i++) {
        if (ig->nodes[i] && strcmp(ig->nodes[i]->var_name, var_name) == 0) {
            return ig->nodes[i];
        }
    }
    
    return NULL;
}

void interference_graph_add_edge(InterferenceGraph *ig, const char *var1, const char *var2) {
    if (!ig || !var1 || !var2 || strcmp(var1, var2) == 0) {
        return;
    }
    
    InterferenceNode *node1 = interference_graph_find_node(ig, var1);
    InterferenceNode *node2 = interference_graph_find_node(ig, var2);
    
    if (!node1) {
        node1 = interference_graph_add_node(ig, var1);
    }
    if (!node2) {
        node2 = interference_graph_add_node(ig, var2);
    }
    
    // 检查边是否已存在
    for (int i = 0; i < node1->neighbor_count; i++) {
        if (node1->neighbors[i] == node2) {
            return;  // 边已存在
        }
    }
    
    // 添加边（双向）
    // node1 -> node2
    if (node1->neighbor_count >= node1->neighbor_capacity) {
        node1->neighbor_capacity *= 2;
        node1->neighbors = (InterferenceNode**)realloc(
            node1->neighbors,
            sizeof(InterferenceNode*) * node1->neighbor_capacity
        );
    }
    node1->neighbors[node1->neighbor_count++] = node2;
    node1->degree++;
    
    // node2 -> node1
    if (node2->neighbor_count >= node2->neighbor_capacity) {
        node2->neighbor_capacity *= 2;
        node2->neighbors = (InterferenceNode**)realloc(
            node2->neighbors,
            sizeof(InterferenceNode*) * node2->neighbor_capacity
        );
    }
    node2->neighbors[node2->neighbor_count++] = node1;
    node2->degree++;
}

/* ==================== 栈槽分配 ==================== */

InterferenceGraph* build_interference_graph(CFG *cfg) {
    if (!cfg) {
        return NULL;
    }
    
    InterferenceGraph *ig = interference_graph_create();
    if (!ig) {
        return NULL;
    }
    
    // 遍历所有基本块
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        
        // 对于 LIVE_OUT 中的每对变量，添加干涉边
        StringSet *live_out = block->live_out;
        
        for (int j = 0; j < live_out->count; j++) {
            for (int k = j + 1; k < live_out->count; k++) {
                // 两个变量在同一个 LIVE_OUT 中，它们干涉
                interference_graph_add_edge(ig, live_out->items[j], live_out->items[k]);
            }
        }
    }
    
    return ig;
}

int allocate_stack_slots(InterferenceGraph *ig, int max_colors) {
    if (!ig || max_colors <= 0) {
        return -1;
    }
    
    // 贪心着色算法
    // 按度数从高到低排序（简化：直接遍历）
    
    int max_color_used = -1;
    
    for (int i = 0; i < ig->node_count; i++) {
        InterferenceNode *node = ig->nodes[i];
        
        if (node->color >= 0) {
            continue;  // 已着色
        }
        
        // 找出邻居使用的颜色
        bool *used_colors = (bool*)calloc(max_colors, sizeof(bool));
        
        for (int j = 0; j < node->neighbor_count; j++) {
            int neighbor_color = node->neighbors[j]->color;
            if (neighbor_color >= 0 && neighbor_color < max_colors) {
                used_colors[neighbor_color] = true;
            }
        }
        
        // 找一个未使用的颜色
        int color = -1;
        for (int c = 0; c < max_colors; c++) {
            if (!used_colors[c]) {
                color = c;
                break;
            }
        }
        
        free(used_colors);
        
        if (color >= 0) {
            node->color = color;
            if (color > max_color_used) {
                max_color_used = color;
            }
        } else {
            // 无法着色，标记为溢出
            node->is_spilled = true;
            fprintf(stderr, "警告: 变量 '%s' 无法分配栈槽（溢出）\n", node->var_name);
        }
    }
    
    return max_color_used + 1;  // 返回使用的栈槽数量
}

int get_variable_slot(InterferenceGraph *ig, const char *var_name) {
    if (!ig || !var_name) {
        return -1;
    }
    
    InterferenceNode *node = interference_graph_find_node(ig, var_name);
    if (!node) {
        return -1;
    }
    
    return node->color;
}

void print_interference_graph(InterferenceGraph *ig) {
    if (!ig) {
        return;
    }
    
    printf("\n========== 干涉图 ==========\n");
    printf("变量数量: %d\n\n", ig->node_count);
    
    for (int i = 0; i < ig->node_count; i++) {
        InterferenceNode *node = ig->nodes[i];
        printf("变量: %s (度数: %d)\n", node->var_name, node->degree);
        printf("  邻居: ");
        for (int j = 0; j < node->neighbor_count; j++) {
            printf("%s ", node->neighbors[j]->var_name);
        }
        printf("\n");
    }
    
    printf("==========================\n\n");
}

void print_stack_allocation(InterferenceGraph *ig) {
    if (!ig) {
        return;
    }
    
    printf("\n========== 栈槽分配结果 ==========\n");
    
    int allocated_count = 0;
    int spilled_count = 0;
    
    for (int i = 0; i < ig->node_count; i++) {
        InterferenceNode *node = ig->nodes[i];
        if (node->is_spilled) {
            printf("变量 %s: 溢出（无法分配）\n", node->var_name);
            spilled_count++;
        } else {
            printf("变量 %s: 栈槽 %d\n", node->var_name, node->color);
            allocated_count++;
        }
    }
    
    printf("\n统计:\n");
    printf("  已分配: %d 个变量\n", allocated_count);
    printf("  溢出: %d 个变量\n", spilled_count);
    printf("  总变量: %d 个\n", ig->node_count);
    
    printf("==============================\n\n");
}

