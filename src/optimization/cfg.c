/**
 * Mini-C 编译器 - 控制流图(CFG)实现
 * 
 * 文件: cfg.c
 * 描述: 控制流图构建和基本块管理
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#include "cfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 字符串集合实现 ==================== */

StringSet* string_set_create(void) {
    StringSet *set = (StringSet*)malloc(sizeof(StringSet));
    if (!set) {
        return NULL;
    }
    
    set->capacity = 16;
    set->count = 0;
    set->items = (char**)malloc(sizeof(char*) * set->capacity);
    if (!set->items) {
        free(set);
        return NULL;
    }
    
    return set;
}

void string_set_destroy(StringSet *set) {
    if (!set) {
        return;
    }
    
    for (int i = 0; i < set->count; i++) {
        free(set->items[i]);
    }
    free(set->items);
    free(set);
}

void string_set_add(StringSet *set, const char *item) {
    if (!set || !item) {
        return;
    }
    
    // 检查是否已存在
    if (string_set_contains(set, item)) {
        return;
    }
    
    // 扩容
    if (set->count >= set->capacity) {
        set->capacity *= 2;
        set->items = (char**)realloc(set->items, sizeof(char*) * set->capacity);
    }
    
    // 添加元素
    set->items[set->count] = strdup(item);
    set->count++;
}

void string_set_remove(StringSet *set, const char *item) {
    if (!set || !item) {
        return;
    }
    
    for (int i = 0; i < set->count; i++) {
        if (strcmp(set->items[i], item) == 0) {
            free(set->items[i]);
            // 移动后面的元素
            for (int j = i; j < set->count - 1; j++) {
                set->items[j] = set->items[j + 1];
            }
            set->count--;
            return;
        }
    }
}

bool string_set_contains(StringSet *set, const char *item) {
    if (!set || !item) {
        return false;
    }
    
    for (int i = 0; i < set->count; i++) {
        if (strcmp(set->items[i], item) == 0) {
            return true;
        }
    }
    return false;
}

StringSet* string_set_union(StringSet *set1, StringSet *set2) {
    StringSet *result = string_set_create();
    if (!result) {
        return NULL;
    }
    
    // 添加 set1 的所有元素
    for (int i = 0; i < set1->count; i++) {
        string_set_add(result, set1->items[i]);
    }
    
    // 添加 set2 的所有元素
    for (int i = 0; i < set2->count; i++) {
        string_set_add(result, set2->items[i]);
    }
    
    return result;
}

StringSet* string_set_difference(StringSet *set1, StringSet *set2) {
    StringSet *result = string_set_create();
    if (!result) {
        return NULL;
    }
    
    // 添加 set1 中不在 set2 中的元素
    for (int i = 0; i < set1->count; i++) {
        if (!string_set_contains(set2, set1->items[i])) {
            string_set_add(result, set1->items[i]);
        }
    }
    
    return result;
}

StringSet* string_set_intersection(StringSet *set1, StringSet *set2) {
    StringSet *result = string_set_create();
    if (!result) {
        return NULL;
    }
    
    // 添加同时在两个集合中的元素
    for (int i = 0; i < set1->count; i++) {
        if (string_set_contains(set2, set1->items[i])) {
            string_set_add(result, set1->items[i]);
        }
    }
    
    return result;
}

bool string_set_equal(StringSet *set1, StringSet *set2) {
    if (!set1 || !set2) {
        return set1 == set2;
    }
    
    if (set1->count != set2->count) {
        return false;
    }
    
    for (int i = 0; i < set1->count; i++) {
        if (!string_set_contains(set2, set1->items[i])) {
            return false;
        }
    }
    
    return true;
}

void string_set_clear(StringSet *set) {
    if (!set) {
        return;
    }
    
    for (int i = 0; i < set->count; i++) {
        free(set->items[i]);
    }
    set->count = 0;
}

StringSet* string_set_copy(StringSet *set) {
    if (!set) {
        return NULL;
    }
    
    StringSet *copy = string_set_create();
    if (!copy) {
        return NULL;
    }
    
    for (int i = 0; i < set->count; i++) {
        string_set_add(copy, set->items[i]);
    }
    
    return copy;
}

/* ==================== 基本块实现 ==================== */

BasicBlock* basic_block_create(int id) {
    BasicBlock *block = (BasicBlock*)malloc(sizeof(BasicBlock));
    if (!block) {
        return NULL;
    }
    
    block->id = id;
    block->inst_count = 0;
    block->inst_capacity = 16;
    block->instructions = (IRInstruction**)malloc(sizeof(IRInstruction*) * block->inst_capacity);
    
    block->pred_count = 0;
    block->pred_capacity = 8;
    block->predecessors = (BasicBlock**)malloc(sizeof(BasicBlock*) * block->pred_capacity);
    
    block->succ_count = 0;
    block->succ_capacity = 8;
    block->successors = (BasicBlock**)malloc(sizeof(BasicBlock*) * block->succ_capacity);
    
    block->live_in = string_set_create();
    block->live_out = string_set_create();
    block->use = string_set_create();
    block->def = string_set_create();
    
    block->label = NULL;
    
    return block;
}

void basic_block_destroy(BasicBlock *block) {
    if (!block) {
        return;
    }
    
    free(block->instructions);
    free(block->predecessors);
    free(block->successors);
    
    string_set_destroy(block->live_in);
    string_set_destroy(block->live_out);
    string_set_destroy(block->use);
    string_set_destroy(block->def);
    
    if (block->label) {
        free(block->label);
    }
    
    free(block);
}

void basic_block_add_instruction(BasicBlock *block, IRInstruction *inst) {
    if (!block || !inst) {
        return;
    }
    
    // 扩容
    if (block->inst_count >= block->inst_capacity) {
        block->inst_capacity *= 2;
        block->instructions = (IRInstruction**)realloc(
            block->instructions, 
            sizeof(IRInstruction*) * block->inst_capacity
        );
    }
    
    block->instructions[block->inst_count++] = inst;
    
    // 如果是标签指令，记录标签
    if (inst->op == IR_LABEL && inst->arg1) {
        block->label = strdup(inst->arg1);
    }
}

void basic_block_add_predecessor(BasicBlock *block, BasicBlock *pred) {
    if (!block || !pred) {
        return;
    }
    
    // 检查是否已存在
    for (int i = 0; i < block->pred_count; i++) {
        if (block->predecessors[i] == pred) {
            return;
        }
    }
    
    // 扩容
    if (block->pred_count >= block->pred_capacity) {
        block->pred_capacity *= 2;
        block->predecessors = (BasicBlock**)realloc(
            block->predecessors,
            sizeof(BasicBlock*) * block->pred_capacity
        );
    }
    
    block->predecessors[block->pred_count++] = pred;
}

void basic_block_add_successor(BasicBlock *block, BasicBlock *succ) {
    if (!block || !succ) {
        return;
    }
    
    // 检查是否已存在
    for (int i = 0; i < block->succ_count; i++) {
        if (block->successors[i] == succ) {
            return;
        }
    }
    
    // 扩容
    if (block->succ_count >= block->succ_capacity) {
        block->succ_capacity *= 2;
        block->successors = (BasicBlock**)realloc(
            block->successors,
            sizeof(BasicBlock*) * block->succ_capacity
        );
    }
    
    block->successors[block->succ_count++] = succ;
    
    // 同时建立反向关系
    basic_block_add_predecessor(succ, block);
}

/* ==================== 控制流图实现 ==================== */

CFG* cfg_create(void) {
    CFG *cfg = (CFG*)malloc(sizeof(CFG));
    if (!cfg) {
        return NULL;
    }
    
    cfg->block_count = 0;
    cfg->block_capacity = 16;
    cfg->blocks = (BasicBlock**)malloc(sizeof(BasicBlock*) * cfg->block_capacity);
    
    cfg->entry = NULL;
    cfg->exit = NULL;
    
    cfg->label_count = 0;
    cfg->label_names = NULL;
    cfg->label_blocks = NULL;
    
    return cfg;
}

void cfg_destroy(CFG *cfg) {
    if (!cfg) {
        return;
    }
    
    for (int i = 0; i < cfg->block_count; i++) {
        basic_block_destroy(cfg->blocks[i]);
    }
    free(cfg->blocks);
    
    if (cfg->label_names) {
        for (int i = 0; i < cfg->label_count; i++) {
            free(cfg->label_names[i]);
        }
        free(cfg->label_names);
        free(cfg->label_blocks);
    }
    
    free(cfg);
}

void cfg_add_block(CFG *cfg, BasicBlock *block) {
    if (!cfg || !block) {
        return;
    }
    
    // 扩容
    if (cfg->block_count >= cfg->block_capacity) {
        cfg->block_capacity *= 2;
        cfg->blocks = (BasicBlock**)realloc(
            cfg->blocks,
            sizeof(BasicBlock*) * cfg->block_capacity
        );
    }
    
    cfg->blocks[cfg->block_count++] = block;
    
    // 设置入口和出口
    if (!cfg->entry) {
        cfg->entry = block;
    }
    cfg->exit = block;  // 最后添加的块作为出口（会被后续更新）
}

BasicBlock* cfg_find_block_by_label(CFG *cfg, const char *label) {
    if (!cfg || !label) {
        return NULL;
    }
    
    for (int i = 0; i < cfg->label_count; i++) {
        if (cfg->label_names[i] && strcmp(cfg->label_names[i], label) == 0) {
            return cfg->label_blocks[i];
        }
    }
    
    // 如果标签映射中没有，遍历所有基本块查找
    for (int i = 0; i < cfg->block_count; i++) {
        if (cfg->blocks[i]->label && strcmp(cfg->blocks[i]->label, label) == 0) {
            return cfg->blocks[i];
        }
    }
    
    return NULL;
}

void cfg_register_label(CFG *cfg, const char *label, BasicBlock *block) {
    if (!cfg || !label || !block) {
        return;
    }
    
    // 检查是否已存在
    for (int i = 0; i < cfg->label_count; i++) {
        if (cfg->label_names[i] && strcmp(cfg->label_names[i], label) == 0) {
            cfg->label_blocks[i] = block;
            return;
        }
    }
    
    // 扩容
    if (cfg->label_count == 0) {
        cfg->label_names = (char**)malloc(sizeof(char*) * 16);
        cfg->label_blocks = (BasicBlock**)malloc(sizeof(BasicBlock*) * 16);
    } else if (cfg->label_count % 16 == 0) {
        cfg->label_names = (char**)realloc(cfg->label_names, 
            sizeof(char*) * (cfg->label_count + 16));
        cfg->label_blocks = (BasicBlock**)realloc(cfg->label_blocks,
            sizeof(BasicBlock*) * (cfg->label_count + 16));
    }
    
    cfg->label_names[cfg->label_count] = strdup(label);
    cfg->label_blocks[cfg->label_count] = block;
    cfg->label_count++;
}

/* ==================== CFG构建 ==================== */

int find_label_index(IRInstruction **instructions, int count, const char *label) {
    if (!instructions || !label) {
        return -1;
    }
    
    for (int i = 0; i < count; i++) {
        if (instructions[i]->op == IR_LABEL && 
            instructions[i]->arg1 && 
            strcmp(instructions[i]->arg1, label) == 0) {
            return i;
        }
    }
    
    return -1;
}

CFG* build_cfg(IRInstruction **instructions, int count) {
    if (!instructions || count <= 0) {
        return NULL;
    }
    
    // 1. 找到所有 leader（基本块的第一条指令）
    bool *is_leader = (bool*)calloc(count, sizeof(bool));
    if (!is_leader) {
        return NULL;
    }
    
    is_leader[0] = true;  // 第一条指令是 leader
    
    for (int i = 0; i < count; i++) {
        IRInstruction *inst = instructions[i];
        
        // 跳转指令的目标是 leader
        if (inst->op == IR_GOTO || inst->op == IR_IF_FALSE || inst->op == IR_IF_TRUE) {
            if (inst->result) {
                int target_index = find_label_index(instructions, count, inst->result);
                if (target_index >= 0) {
                    is_leader[target_index] = true;
                }
            }
        }
        
        // 跳转指令的下一条是 leader
        if (inst->op == IR_GOTO || inst->op == IR_IF_FALSE || 
            inst->op == IR_IF_TRUE || inst->op == IR_RETURN) {
            if (i + 1 < count) {
                is_leader[i + 1] = true;
            }
        }
        
        // 函数入口是 leader
        if (inst->op == IR_FUNC_BEGIN) {
            is_leader[i] = true;
        }
        
        // 标签指令是 leader
        if (inst->op == IR_LABEL) {
            is_leader[i] = true;
        }
    }
    
    // 2. 根据 leader 划分基本块
    CFG *cfg = cfg_create();
    if (!cfg) {
        free(is_leader);
        return NULL;
    }
    
    BasicBlock *current_block = NULL;
    int block_id = 0;
    
    for (int i = 0; i < count; i++) {
        if (is_leader[i]) {
            // 创建新基本块
            current_block = basic_block_create(block_id++);
            if (!current_block) {
                free(is_leader);
                cfg_destroy(cfg);
                return NULL;
            }
            cfg_add_block(cfg, current_block);
            
            // 如果是标签，注册标签映射
            if (instructions[i]->op == IR_LABEL && instructions[i]->arg1) {
                cfg_register_label(cfg, instructions[i]->arg1, current_block);
            }
        }
        
        // 添加指令到当前基本块
        if (current_block) {
            basic_block_add_instruction(current_block, instructions[i]);
        }
    }
    
    // 3. 建立前驱后继关系
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        if (block->inst_count == 0) {
            continue;
        }
        
        IRInstruction *last_inst = block->instructions[block->inst_count - 1];
        
        if (last_inst->op == IR_GOTO) {
            // 无条件跳转：添加目标基本块为后继
            if (last_inst->result) {
                BasicBlock *target = cfg_find_block_by_label(cfg, last_inst->result);
                if (target) {
                    basic_block_add_successor(block, target);
                }
            }
        } else if (last_inst->op == IR_IF_FALSE || last_inst->op == IR_IF_TRUE) {
            // 条件跳转：添加两个后继（跳转目标和顺序执行）
            if (last_inst->result) {
                BasicBlock *target = cfg_find_block_by_label(cfg, last_inst->result);
                if (target) {
                    basic_block_add_successor(block, target);
                }
            }
            // 顺序执行：添加下一个基本块为后继
            if (i + 1 < cfg->block_count) {
                basic_block_add_successor(block, cfg->blocks[i + 1]);
            }
        } else if (last_inst->op != IR_RETURN && last_inst->op != IR_FUNC_END) {
            // 顺序执行：添加下一个基本块为后继
            if (i + 1 < cfg->block_count) {
                basic_block_add_successor(block, cfg->blocks[i + 1]);
            }
        }
    }
    
    // 更新出口基本块（最后一个非空基本块）
    for (int i = cfg->block_count - 1; i >= 0; i--) {
        if (cfg->blocks[i]->inst_count > 0) {
            cfg->exit = cfg->blocks[i];
            break;
        }
    }
    
    free(is_leader);
    return cfg;
}

void cfg_print(CFG *cfg) {
    if (!cfg) {
        return;
    }
    
    printf("\n========== 控制流图 (CFG) ==========\n");
    printf("基本块数量: %d\n", cfg->block_count);
    printf("入口基本块: B%d\n", cfg->entry ? cfg->entry->id : -1);
    printf("出口基本块: B%d\n", cfg->exit ? cfg->exit->id : -1);
    printf("\n");
    
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        printf("--- 基本块 B%d ---\n", block->id);
        if (block->label) {
            printf("标签: %s\n", block->label);
        }
        printf("指令数量: %d\n", block->inst_count);
        printf("前驱: ");
        for (int j = 0; j < block->pred_count; j++) {
            printf("B%d ", block->predecessors[j]->id);
        }
        printf("\n后继: ");
        for (int j = 0; j < block->succ_count; j++) {
            printf("B%d ", block->successors[j]->id);
        }
        printf("\n\n");
    }
    printf("==================================\n\n");
}

