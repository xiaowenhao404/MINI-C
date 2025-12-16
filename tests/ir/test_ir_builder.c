/**
 * Mini-C 编译器 - IR构建器单元测试
 * 
 * 文件: test_ir_builder.c
 * 描述: 测试IR构建器的各项功能
 * 作者: Mini-C 开发团队
 */

#include "../../src/ir/ir_builder.h"
#include "../../src/ir/ir.h"
#include "../../src/semantic/type_system.h"
#include "../../src/semantic/symbol_table.h"
#include "../../c-complier-master/tree.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// 测试计数器
static int test_count = 0;
static int pass_count = 0;

// 辅助宏：运行测试
#define RUN_TEST(test_func) \
    do { \
        printf("运行测试: %s ... ", #test_func); \
        test_count++; \
        test_func(); \
        pass_count++; \
        printf("通过\n"); \
    } while(0)

/* ==================== 辅助函数 ==================== */

/**
 * 创建简单的叶子节点
 */
Tree* create_leaf_node(const char *name, const char *content, int line) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    node->name = strdup(name);
    node->content = content ? strdup(content) : NULL;
    node->line = line;
    node->num = 0;
    node->leaves = NULL;
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    return node;
}

/**
 * 释放树节点
 */
void free_tree_node(Tree *node) {
    if (!node) return;
    if (node->name) free(node->name);
    if (node->content) free(node->content);
    if (node->leaves) {
        for (int i = 0; i < node->num; i++) {
            free_tree_node(node->leaves[i]);
        }
        free(node->leaves);
    }
    free(node);
}

/* ==================== 测试函数 ==================== */

/**
 * 测试IR构建器创建和销毁
 */
void test_ir_builder_create() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    assert(builder != NULL);
    assert(builder->count == 0);
    assert(builder->temp_count == 0);
    assert(builder->label_count == 0);
    assert(builder->symbol_table == st);
    
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试临时变量生成
 */
void test_temp_generation() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    char *t0 = new_temp(builder);
    char *t1 = new_temp(builder);
    char *t2 = new_temp(builder);
    
    assert(strcmp(t0, "t0") == 0);
    assert(strcmp(t1, "t1") == 0);
    assert(strcmp(t2, "t2") == 0);
    
    free(t0);
    free(t1);
    free(t2);
    
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试标签生成
 */
void test_label_generation() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    char *l0 = new_label(builder);
    char *l1 = new_label(builder);
    char *l2 = new_label(builder);
    
    assert(strcmp(l0, "L0") == 0);
    assert(strcmp(l1, "L1") == 0);
    assert(strcmp(l2, "L2") == 0);
    
    free(l0);
    free(l1);
    free(l2);
    
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试IR指令创建
 */
void test_ir_instruction_create() {
    IRInstruction *inst = ir_instruction_create(IR_ADD, "a", "b", "t0");
    
    assert(inst != NULL);
    assert(inst->op == IR_ADD);
    assert(strcmp(inst->arg1, "a") == 0);
    assert(strcmp(inst->arg2, "b") == 0);
    assert(strcmp(inst->result, "t0") == 0);
    
    ir_instruction_free(inst);
}

/**
 * 测试IR指令发射
 */
void test_emit_instruction() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    // 发射几条指令
    emit_binary_op(builder, IR_ADD, "a", "b", "t0");
    emit_assign(builder, "t0", "c");
    
    assert(builder->count == 2);
    assert(builder->instructions[0]->op == IR_ADD);
    assert(builder->instructions[1]->op == IR_ASSIGN);
    
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试操作码转字符串
 */
void test_opcode_to_string() {
    assert(strcmp(opcode_to_string(IR_ADD), "+") == 0);
    assert(strcmp(opcode_to_string(IR_SUB), "-") == 0);
    assert(strcmp(opcode_to_string(IR_MUL), "*") == 0);
    assert(strcmp(opcode_to_string(IR_DIV), "/") == 0);
    assert(strcmp(opcode_to_string(IR_ASSIGN), "=") == 0);
    assert(strcmp(opcode_to_string(IR_GOTO), "goto") == 0);
    assert(strcmp(opcode_to_string(IR_LABEL), "LABEL") == 0);
}

/**
 * 测试简单表达式翻译
 */
void test_translate_constant() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    // 创建常量节点
    Tree *num_node = create_leaf_node("INT10", "42", 1);
    
    // 翻译常量
    char *result = translate_expression(builder, num_node, NULL);
    
    assert(result != NULL);
    assert(strcmp(result, "42") == 0);
    
    free(result);
    free_tree_node(num_node);
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试变量引用翻译
 */
void test_translate_variable() {
    SymbolTable *st = symbol_table_create(127);
    IRBuilder *builder = ir_builder_create(st);
    
    // 创建ID节点
    Tree *id_node = create_leaf_node("ID", "x", 1);
    
    // 翻译变量
    char *result = translate_expression(builder, id_node, NULL);
    
    assert(result != NULL);
    assert(strcmp(result, "x") == 0);
    
    free(result);
    free_tree_node(id_node);
    ir_builder_destroy(builder);
    symbol_table_destroy(st);
}

/**
 * 测试IR指令输出格式
 */
void test_ir_instruction_to_string() {
    char buffer[256];
    
    // 测试二元运算
    IRInstruction *add = ir_instruction_create(IR_ADD, "a", "b", "t0");
    ir_instruction_to_string(add, buffer, sizeof(buffer));
    assert(strstr(buffer, "t0") != NULL);
    assert(strstr(buffer, "a") != NULL);
    assert(strstr(buffer, "b") != NULL);
    ir_instruction_free(add);
    
    // 测试标签
    IRInstruction *label = ir_instruction_create(IR_LABEL, NULL, NULL, "L1");
    ir_instruction_to_string(label, buffer, sizeof(buffer));
    assert(strstr(buffer, "L1:") != NULL);
    ir_instruction_free(label);
    
    // 测试跳转
    IRInstruction *goto_inst = ir_instruction_create(IR_GOTO, NULL, NULL, "L2");
    ir_instruction_to_string(goto_inst, buffer, sizeof(buffer));
    assert(strstr(buffer, "goto") != NULL);
    assert(strstr(buffer, "L2") != NULL);
    ir_instruction_free(goto_inst);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C IR构建器单元测试\n");
    printf("========================================\n\n");
    
    // 初始化类型系统
    init_type_system();
    
    // 运行所有测试
    RUN_TEST(test_ir_builder_create);
    RUN_TEST(test_temp_generation);
    RUN_TEST(test_label_generation);
    RUN_TEST(test_ir_instruction_create);
    RUN_TEST(test_emit_instruction);
    RUN_TEST(test_opcode_to_string);
    RUN_TEST(test_translate_constant);
    RUN_TEST(test_translate_variable);
    RUN_TEST(test_ir_instruction_to_string);
    
    // 清理类型系统
    cleanup_type_system();
    
    // 输出测试结果
    printf("\n========================================\n");
    printf("  测试结果统计\n");
    printf("========================================\n");
    printf("总测试数: %d\n", test_count);
    printf("通过数: %d\n", pass_count);
    printf("失败数: %d\n", test_count - pass_count);
    
    if (pass_count == test_count) {
        printf("\n✓ 所有测试通过！\n");
        return 0;
    } else {
        printf("\n✗ 有测试失败！\n");
        return 1;
    }
}

