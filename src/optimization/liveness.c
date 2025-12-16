/**
 * Mini-C 编译器 - 活性分析实现
 * 
 * 文件: liveness.c
 * 描述: 活性分析算法实现
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#include "liveness.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== USE/DEF集合计算 ==================== */

StringSet* extract_use_from_instruction(IRInstruction *inst) {
    StringSet *use = string_set_create();
    if (!use || !inst) {
        return use;
    }
    
    // 根据指令类型提取使用的变量
    switch (inst->op) {
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
        case IR_MOD:
        case IR_FADD:
        case IR_FSUB:
        case IR_FMUL:
        case IR_FDIV:
        case IR_LT:
        case IR_GT:
        case IR_LE:
        case IR_GE:
        case IR_EQ:
        case IR_NE:
        case IR_AND:
        case IR_OR:
            // 二元运算：使用 arg1 和 arg2
            if (inst->arg1 && inst->arg1[0] != '0' && 
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            if (inst->arg2 && inst->arg2[0] != '0' &&
                (inst->arg2[0] < '0' || inst->arg2[0] > '9')) {
                string_set_add(use, inst->arg2);
            }
            break;
            
        case IR_NOT:
        case IR_I2F:
        case IR_F2I:
        case IR_C2I:
        case IR_LOAD:
            // 一元运算：使用 arg1
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            break;
            
        case IR_IF_FALSE:
        case IR_IF_TRUE:
            // 条件跳转：使用 arg1（条件变量）
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            break;
            
        case IR_RETURN:
            // 返回语句：使用 arg1（返回值）
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            break;
            
        case IR_PARAM:
            // 参数传递：使用 arg1
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            break;
            
        case IR_STORE:
            // 存储指令：使用 arg1（值）和 arg2（地址）
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            if (inst->arg2 && inst->arg2[0] != '0' &&
                (inst->arg2[0] < '0' || inst->arg2[0] > '9')) {
                string_set_add(use, inst->arg2);
            }
            break;
            
        case IR_ARRAY_ADDR:
            // 数组地址计算：使用 arg1（数组名）和 arg2（偏移）
            if (inst->arg1 && inst->arg1[0] != '0' &&
                (inst->arg1[0] < '0' || inst->arg1[0] > '9')) {
                string_set_add(use, inst->arg1);
            }
            if (inst->arg2 && inst->arg2[0] != '0' &&
                (inst->arg2[0] < '0' || inst->arg2[0] > '9')) {
                string_set_add(use, inst->arg2);
            }
            break;
            
        default:
            break;
    }
    
    return use;
}

StringSet* extract_def_from_instruction(IRInstruction *inst) {
    StringSet *def = string_set_create();
    if (!def || !inst) {
        return def;
    }
    
    // 根据指令类型提取定义的变量
    // 定义变量通常是 result 字段
    if (inst->result && inst->result[0] != '0' &&
        (inst->result[0] < '0' || inst->result[0] > '9')) {
        // 排除标签和常量
        if (inst->op != IR_LABEL && inst->op != IR_GOTO) {
            string_set_add(def, inst->result);
        }
    }
    
    // STORE 指令定义的是内存位置（通过地址），这里简化处理
    if (inst->op == IR_STORE && inst->arg2) {
        // 存储指令定义的是 arg2 指向的内存位置
        // 这里不添加到 def，因为这是内存操作
    }
    
    return def;
}

void compute_use_def(BasicBlock *block) {
    if (!block) {
        return;
    }
    
    // 清空现有集合
    string_set_clear(block->use);
    string_set_clear(block->def);
    
    // 从后向前遍历指令（因为USE需要考虑定义顺序）
    StringSet *def_so_far = string_set_create();
    
    for (int i = block->inst_count - 1; i >= 0; i--) {
        IRInstruction *inst = block->instructions[i];
        
        // 提取使用和定义
        StringSet *inst_use = extract_use_from_instruction(inst);
        StringSet *inst_def = extract_def_from_instruction(inst);
        
        // USE[B] = 使用的变量 - 在当前指令之前定义的变量
        for (int j = 0; j < inst_use->count; j++) {
            if (!string_set_contains(def_so_far, inst_use->items[j])) {
                string_set_add(block->use, inst_use->items[j]);
            }
        }
        
        // DEF[B] = 所有定义的变量
        for (int j = 0; j < inst_def->count; j++) {
            string_set_add(block->def, inst_def->items[j]);
            string_set_add(def_so_far, inst_def->items[j]);
        }
        
        string_set_destroy(inst_use);
        string_set_destroy(inst_def);
    }
    
    string_set_destroy(def_so_far);
}

/* ==================== 活性分析算法 ==================== */

bool liveness_converged(CFG *cfg, StringSet **old_live_in, StringSet **old_live_out) {
    if (!cfg || !old_live_in || !old_live_out) {
        return false;
    }
    
    // 检查所有基本块的 live_in 和 live_out 是否都没有变化
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        
        if (!string_set_equal(block->live_in, old_live_in[i]) ||
            !string_set_equal(block->live_out, old_live_out[i])) {
            return false;
        }
    }
    
    return true;
}

bool perform_liveness_analysis(CFG *cfg) {
    if (!cfg) {
        return false;
    }
    
    // 1. 计算所有基本块的 USE 和 DEF 集合
    for (int i = 0; i < cfg->block_count; i++) {
        compute_use_def(cfg->blocks[i]);
    }
    
    // 2. 初始化 live_in 和 live_out 为空集
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        string_set_clear(block->live_in);
        string_set_clear(block->live_out);
    }
    
    // 3. 迭代求解数据流方程
    // 保存旧值用于收敛检查
    StringSet **old_live_in = (StringSet**)malloc(sizeof(StringSet*) * cfg->block_count);
    StringSet **old_live_out = (StringSet**)malloc(sizeof(StringSet*) * cfg->block_count);
    
    for (int i = 0; i < cfg->block_count; i++) {
        old_live_in[i] = string_set_create();
        old_live_out[i] = string_set_create();
    }
    
    int iteration = 0;
    const int MAX_ITERATIONS = 100;  // 防止无限循环
    
    do {
        // 保存当前值
        for (int i = 0; i < cfg->block_count; i++) {
            StringSet *temp_in = string_set_copy(cfg->blocks[i]->live_in);
            StringSet *temp_out = string_set_copy(cfg->blocks[i]->live_out);
            
            string_set_destroy(old_live_in[i]);
            string_set_destroy(old_live_out[i]);
            
            old_live_in[i] = temp_in;
            old_live_out[i] = temp_out;
        }
        
        // 从后向前遍历基本块（逆序迭代）
        for (int i = cfg->block_count - 1; i >= 0; i--) {
            BasicBlock *block = cfg->blocks[i];
            
            // LIVE_OUT[B] = ∪ LIVE_IN[S] (S是B的后继)
            string_set_clear(block->live_out);
            for (int j = 0; j < block->succ_count; j++) {
                BasicBlock *succ = block->successors[j];
                StringSet *union_set = string_set_union(block->live_out, succ->live_in);
                string_set_destroy(block->live_out);
                block->live_out = union_set;
            }
            
            // LIVE_IN[B] = USE[B] ∪ (LIVE_OUT[B] - DEF[B])
            StringSet *diff = string_set_difference(block->live_out, block->def);
            StringSet *new_live_in = string_set_union(block->use, diff);
            
            string_set_destroy(block->live_in);
            string_set_destroy(diff);
            
            block->live_in = new_live_in;
        }
        
        iteration++;
        
        // 检查收敛
        if (liveness_converged(cfg, old_live_in, old_live_out)) {
            break;
        }
        
    } while (iteration < MAX_ITERATIONS);
    
    // 清理
    for (int i = 0; i < cfg->block_count; i++) {
        string_set_destroy(old_live_in[i]);
        string_set_destroy(old_live_out[i]);
    }
    free(old_live_in);
    free(old_live_out);
    
    if (iteration >= MAX_ITERATIONS) {
        fprintf(stderr, "警告: 活性分析在 %d 次迭代后未收敛\n", MAX_ITERATIONS);
        return false;
    }
    
    return true;
}

void print_liveness_analysis(CFG *cfg) {
    if (!cfg) {
        return;
    }
    
    printf("\n========== 活性分析结果 ==========\n");
    
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        printf("--- 基本块 B%d ---\n", block->id);
        
        printf("USE: { ");
        for (int j = 0; j < block->use->count; j++) {
            printf("%s ", block->use->items[j]);
        }
        printf("}\n");
        
        printf("DEF: { ");
        for (int j = 0; j < block->def->count; j++) {
            printf("%s ", block->def->items[j]);
        }
        printf("}\n");
        
        printf("LIVE_IN: { ");
        for (int j = 0; j < block->live_in->count; j++) {
            printf("%s ", block->live_in->items[j]);
        }
        printf("}\n");
        
        printf("LIVE_OUT: { ");
        for (int j = 0; j < block->live_out->count; j++) {
            printf("%s ", block->live_out->items[j]);
        }
        printf("}\n\n");
    }
    
    printf("==================================\n\n");
}

