/**
 * Mini-C 编译器 - 控制流图(CFG)定义
 * 
 * 文件: cfg.h
 * 描述: 控制流图数据结构和函数声明
 * 作者: Mini-C 开发团队
 * 版本: 2.0
 */

#ifndef CFG_H
#define CFG_H

#include "../ir/ir.h"
#include <stdbool.h>

/* ==================== 集合数据结构（用于活性分析）==================== */

/**
 * 字符串集合（用于存储变量名）
 */
typedef struct StringSet {
    char **items;      // 字符串数组
    int count;         // 元素数量
    int capacity;      // 容量
} StringSet;

/**
 * 创建字符串集合
 */
StringSet* string_set_create(void);

/**
 * 销毁字符串集合
 */
void string_set_destroy(StringSet *set);

/**
 * 向集合添加元素
 */
void string_set_add(StringSet *set, const char *item);

/**
 * 从集合移除元素
 */
void string_set_remove(StringSet *set, const char *item);

/**
 * 检查集合是否包含元素
 */
bool string_set_contains(StringSet *set, const char *item);

/**
 * 集合并集：result = set1 ∪ set2
 */
StringSet* string_set_union(StringSet *set1, StringSet *set2);

/**
 * 集合差集：result = set1 - set2
 */
StringSet* string_set_difference(StringSet *set1, StringSet *set2);

/**
 * 集合交集：result = set1 ∩ set2
 */
StringSet* string_set_intersection(StringSet *set1, StringSet *set2);

/**
 * 检查两个集合是否相等
 */
bool string_set_equal(StringSet *set1, StringSet *set2);

/**
 * 清空集合
 */
void string_set_clear(StringSet *set);

/**
 * 复制集合
 */
StringSet* string_set_copy(StringSet *set);

/* ==================== 基本块 ==================== */

/**
 * 基本块结构
 */
typedef struct BasicBlock {
    int id;                         // 基本块编号
    IRInstruction **instructions;   // 指令列表
    int inst_count;                 // 指令数量
    int inst_capacity;              // 指令容量
    
    struct BasicBlock **predecessors;  // 前驱基本块数组
    int pred_count;                    // 前驱数量
    int pred_capacity;                 // 前驱容量
    
    struct BasicBlock **successors;    // 后继基本块数组
    int succ_count;                    // 后继数量
    int succ_capacity;                 // 后继容量
    
    // 活性分析使用
    StringSet *live_in;    // 块入口活跃变量集合
    StringSet *live_out;   // 块出口活跃变量集合
    StringSet *use;        // 使用的变量集合
    StringSet *def;        // 定义的变量集合
    
    // 标签信息（用于跳转目标）
    char *label;           // 基本块标签（如果有）
} BasicBlock;

/**
 * 创建基本块
 */
BasicBlock* basic_block_create(int id);

/**
 * 销毁基本块
 */
void basic_block_destroy(BasicBlock *block);

/**
 * 向基本块添加指令
 */
void basic_block_add_instruction(BasicBlock *block, IRInstruction *inst);

/**
 * 添加前驱基本块
 */
void basic_block_add_predecessor(BasicBlock *block, BasicBlock *pred);

/**
 * 添加后继基本块
 */
void basic_block_add_successor(BasicBlock *block, BasicBlock *succ);

/* ==================== 控制流图 ==================== */

/**
 * 控制流图结构
 */
typedef struct CFG {
    BasicBlock **blocks;    // 基本块数组
    int block_count;        // 基本块数量
    int block_capacity;     // 基本块容量
    
    BasicBlock *entry;      // 入口基本块
    BasicBlock *exit;       // 出口基本块
    
    // 标签到基本块的映射（用于快速查找）
    char **label_names;     // 标签名数组
    BasicBlock **label_blocks;  // 对应的基本块数组
    int label_count;        // 标签数量
} CFG;

/**
 * 创建控制流图
 */
CFG* cfg_create(void);

/**
 * 销毁控制流图
 */
void cfg_destroy(CFG *cfg);

/**
 * 向CFG添加基本块
 */
void cfg_add_block(CFG *cfg, BasicBlock *block);

/**
 * 根据标签查找基本块
 */
BasicBlock* cfg_find_block_by_label(CFG *cfg, const char *label);

/**
 * 注册标签到基本块的映射
 */
void cfg_register_label(CFG *cfg, const char *label, BasicBlock *block);

/* ==================== CFG构建 ==================== */

/**
 * 从IR指令序列构建控制流图
 * 
 * @param instructions IR指令数组
 * @param count 指令数量
 * @return 构建好的CFG
 */
CFG* build_cfg(IRInstruction **instructions, int count);

/**
 * 查找标签在指令序列中的索引
 */
int find_label_index(IRInstruction **instructions, int count, const char *label);

/**
 * 打印CFG（用于调试）
 */
void cfg_print(CFG *cfg);

#endif /* CFG_H */

