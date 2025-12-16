/**
 * Mini-C 编译器 - 活性分析
 * 
 * 文件: liveness.h
 * 描述: 活性分析算法实现
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#ifndef LIVENESS_H
#define LIVENESS_H

#include "cfg.h"

/* ==================== USE/DEF集合计算 ==================== */

/**
 * 计算基本块的 USE 和 DEF 集合
 * 
 * USE[B]: 在B中使用但在使用前未定义的变量
 * DEF[B]: 在B中定义的变量
 * 
 * @param block 基本块
 */
void compute_use_def(BasicBlock *block);

/**
 * 从指令中提取使用的变量
 */
StringSet* extract_use_from_instruction(IRInstruction *inst);

/**
 * 从指令中提取定义的变量
 */
StringSet* extract_def_from_instruction(IRInstruction *inst);

/* ==================== 活性分析算法 ==================== */

/**
 * 执行活性分析（数据流方程迭代求解）
 * 
 * 数据流方程：
 * LIVE_OUT[B] = ∪ LIVE_IN[S] (S是B的后继)
 * LIVE_IN[B] = USE[B] ∪ (LIVE_OUT[B] - DEF[B])
 * 
 * @param cfg 控制流图
 * @return true 如果分析成功
 */
bool perform_liveness_analysis(CFG *cfg);

/**
 * 检查活性分析是否收敛
 */
bool liveness_converged(CFG *cfg, StringSet **old_live_in, StringSet **old_live_out);

/**
 * 打印活性分析结果（用于调试）
 */
void print_liveness_analysis(CFG *cfg);

#endif /* LIVENESS_H */

