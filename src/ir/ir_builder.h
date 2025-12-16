/**
 * Mini-C 编译器 - IR 构建器
 * 
 * 文件: ir_builder.h
 * 描述: 从AST生成中间代码的构建器
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include "ir.h"
#include "../semantic/type_system.h"
#include "../semantic/symbol_table.h"

// 前向声明
struct Tree;

/* ==================== IR 构建器定义 ==================== */

/**
 * IR 构建器结构体
 * 
 * 管理IR生成过程中的状态
 */
typedef struct IRBuilder {
    IRInstruction **instructions;   // IR指令数组
    int count;                       // 当前指令数量
    int capacity;                    // 数组容量
    int temp_count;                  // 临时变量计数
    int label_count;                 // 标签计数
    SymbolTable *symbol_table;       // 符号表（用于查找变量）
} IRBuilder;

/* ==================== IR 构建器创建和销毁 ==================== */

/**
 * 创建IR构建器
 * 
 * @param symbol_table 符号表指针
 * @return IR构建器指针
 */
IRBuilder* ir_builder_create(SymbolTable *symbol_table);

/**
 * 销毁IR构建器
 * 
 * @param builder IR构建器指针
 */
void ir_builder_destroy(IRBuilder *builder);

/* ==================== 临时变量和标签管理 ==================== */

/**
 * 生成新的临时变量名
 * 
 * @param builder IR构建器指针
 * @return 临时变量名（如 "t0", "t1"）
 */
char* new_temp(IRBuilder *builder);

/**
 * 生成新的标签名
 * 
 * @param builder IR构建器指针
 * @return 标签名（如 "L1", "L2"）
 */
char* new_label(IRBuilder *builder);

/* ==================== IR 指令发射 ==================== */

/**
 * 添加IR指令到指令列表
 * 
 * @param builder IR构建器指针
 * @param inst IR指令指针
 */
void emit(IRBuilder *builder, IRInstruction *inst);

/**
 * 发射二元运算指令（辅助函数）
 * 
 * @param builder IR构建器指针
 * @param op 操作码
 * @param arg1 第一操作数
 * @param arg2 第二操作数
 * @param result 结果变量
 */
void emit_binary_op(IRBuilder *builder, IROpcode op, const char *arg1, 
                   const char *arg2, const char *result);

/**
 * 发射赋值指令（辅助函数）
 * 
 * @param builder IR构建器指针
 * @param source 源操作数
 * @param dest 目标变量
 */
void emit_assign(IRBuilder *builder, const char *source, const char *dest);

/**
 * 发射标签指令（辅助函数）
 * 
 * @param builder IR构建器指针
 * @param label 标签名
 */
void emit_label(IRBuilder *builder, const char *label);

/**
 * 发射跳转指令（辅助函数）
 * 
 * @param builder IR构建器指针
 * @param label 目标标签
 */
void emit_goto(IRBuilder *builder, const char *label);

/**
 * 发射条件跳转指令（辅助函数）
 * 
 * @param builder IR构建器指针
 * @param cond 条件变量
 * @param label 目标标签
 */
void emit_if_false(IRBuilder *builder, const char *cond, const char *label);

/* ==================== AST 到 IR 翻译 ==================== */

/**
 * 翻译表达式
 * 
 * @param builder IR构建器指针
 * @param expr 表达式节点
 * @param expr_type 表达式类型（用于判断是否需要类型转换）
 * @return 存储结果的变量名（可能是临时变量）
 */
char* translate_expression(IRBuilder *builder, struct Tree *expr, Type *expr_type);

/**
 * 翻译语句
 * 
 * @param builder IR构建器指针
 * @param stmt 语句节点
 */
void translate_statement(IRBuilder *builder, struct Tree *stmt);

/**
 * 翻译整个程序
 * 
 * @param builder IR构建器指针
 * @param ast AST根节点
 */
void translate_program(IRBuilder *builder, struct Tree *ast);

/* ==================== 函数翻译（2.0版本）==================== */

/**
 * 翻译函数定义
 * 
 * @param builder IR构建器指针
 * @param func_def 函数定义 AST 节点
 */
void translate_function_definition(IRBuilder *builder, struct Tree *func_def);

/**
 * 翻译函数调用表达式
 * 
 * @param builder IR构建器指针
 * @param call_node 函数调用 AST 节点
 * @param expr_type 表达式类型（可选）
 * @return 存储结果的临时变量名
 */
char* translate_function_call(IRBuilder *builder, struct Tree *call_node, Type *expr_type);

/**
 * 翻译 return 语句
 * 
 * @param builder IR构建器指针
 * @param return_stmt return 语句 AST 节点
 */
void translate_return_statement(IRBuilder *builder, struct Tree *return_stmt);

/* ==================== 数组翻译（2.0版本）==================== */

/**
 * 翻译数组访问表达式
 * 
 * @param builder IR构建器指针
 * @param access 数组访问 AST 节点
 * @return 存储元素值的临时变量名
 */
char* translate_array_access(IRBuilder *builder, struct Tree *access);

/* ==================== 输出 IR ==================== */

/**
 * 将IR输出到文件
 * 
 * @param builder IR构建器指针
 * @param filename 输出文件名
 */
void ir_builder_output_to_file(IRBuilder *builder, const char *filename);

/**
 * 打印IR到标准输出（调试用）
 * 
 * @param builder IR构建器指针
 */
void ir_builder_print(IRBuilder *builder);

/**
 * 获取IR指令数量
 * 
 * @param builder IR构建器指针
 * @return 指令数量
 */
int ir_builder_count(IRBuilder *builder);

#endif /* IR_BUILDER_H */

