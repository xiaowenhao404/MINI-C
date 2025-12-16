/**
 * Mini-C 编译器 - 语义分析器单元测试
 * 
 * 文件: test_semantic_analyzer.c
 * 描述: 测试语义分析器的各项功能
 * 作者: Mini-C 开发团队
 */

#include "../../src/semantic/semantic_analyzer.h"
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

/* ==================== 辅助函数：手动构建简单AST ==================== */

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
 * 创建简单的节点（带子节点）
 */
Tree* create_node(const char *name, int num_children, Tree **children) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    node->name = strdup(name);
    node->content = NULL;
    node->line = 1;
    node->num = num_children;
    node->leaves = children;
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    return node;
}

/**
 * 释放简单的树节点
 */
void free_simple_tree(Tree *node) {
    if (!node) return;
    
    if (node->name) free(node->name);
    if (node->content) free(node->content);
    
    if (node->leaves) {
        for (int i = 0; i < node->num; i++) {
            free_simple_tree(node->leaves[i]);
        }
        free(node->leaves);
    }
    
    free(node);
}

/* ==================== 测试函数 ==================== */

/**
 * 测试语义分析器创建和销毁
 */
void test_analyzer_create() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    
    assert(sa != NULL);
    assert(sa->symbol_table != NULL);
    assert(sa->error_count == 0);
    assert(sa->warning_count == 0);
    
    semantic_analyzer_destroy(sa);
}

/**
 * 测试错误报告
 */
void test_error_reporting() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    
    // 测试错误报告
    semantic_error(sa, 10, "这是一个测试错误");
    assert(sa->error_count == 1);
    assert(has_errors(sa) == true);
    
    // 测试警告报告
    semantic_warning(sa, 15, "这是一个测试警告");
    assert(sa->warning_count == 1);
    
    semantic_analyzer_destroy(sa);
}

/**
 * 测试类型查询
 */
void test_type_lookup() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    Type *int_type = new_int_type();
    
    // 插入一个变量
    symbol_insert(sa->symbol_table, "x", int_type, 1);
    
    // 查找变量类型
    Type *found_type = lookup_variable_type(sa, "x", 1);
    assert(found_type != NULL);
    assert(type_equal(found_type, int_type));
    
    // 查找不存在的变量（应产生错误）
    Type *not_found = lookup_variable_type(sa, "y", 2);
    assert(not_found == NULL);
    assert(sa->error_count == 1);
    
    semantic_analyzer_destroy(sa);
}

/**
 * 测试节点类型判断
 */
void test_node_predicates() {
    // 创建测试节点
    Tree *decl_node = create_leaf_node("declare_expression", NULL, 1);
    Tree *expr_node = create_leaf_node("additive_expression", NULL, 1);
    Tree *id_node = create_leaf_node("ID", "x", 1);
    
    // 测试判断函数
    assert(is_declaration_node(decl_node) == true);
    assert(is_declaration_node(expr_node) == false);
    
    assert(is_expression_node(expr_node) == true);
    assert(is_expression_node(decl_node) == false);
    
    assert(is_lvalue(id_node) == true);
    assert(is_lvalue(expr_node) == false);
    
    // 清理
    free_simple_tree(decl_node);
    free_simple_tree(expr_node);
    free_simple_tree(id_node);
}

/**
 * 测试获取节点类型
 */
void test_get_node_type() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    
    // 创建常量节点
    Tree *int_node = create_leaf_node("INT10", "123", 1);
    Type *int_type = get_node_type(sa, int_node);
    assert(int_type != NULL);
    assert(int_type->kind == TYPE_INT);
    
    // 创建ID节点并插入符号表
    Type *float_type = new_float_type();
    symbol_insert(sa->symbol_table, "x", float_type, 1);
    
    Tree *id_node = create_leaf_node("ID", "x", 1);
    Type *found_type = get_node_type(sa, id_node);
    assert(found_type != NULL);
    assert(found_type->kind == TYPE_FLOAT);
    
    // 清理
    free_simple_tree(int_node);
    free_simple_tree(id_node);
    semantic_analyzer_destroy(sa);
}

/**
 * 测试简单表达式分析
 */
void test_simple_expression() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    
    // 创建简单常量表达式
    Tree *num_node = create_leaf_node("INT10", "42", 1);
    Type *type = analyze_expression(sa, num_node);
    
    assert(type != NULL);
    assert(type->kind == TYPE_INT);
    assert(sa->error_count == 0);
    
    // 清理
    free_simple_tree(num_node);
    semantic_analyzer_destroy(sa);
}

/**
 * 测试变量查找分析
 */
void test_variable_lookup_analysis() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    Type *int_type = new_int_type();
    
    // 先插入变量到符号表
    symbol_insert(sa->symbol_table, "a", int_type, 1);
    
    // 创建ID节点并分析
    Tree *id_node = create_leaf_node("ID", "a", 2);
    Type *type = analyze_expression(sa, id_node);
    
    assert(type != NULL);
    assert(type->kind == TYPE_INT);
    assert(sa->error_count == 0);
    
    // 测试未定义变量
    Tree *undefined_node = create_leaf_node("ID", "undefined_var", 3);
    Type *undef_type = analyze_expression(sa, undefined_node);
    
    assert(undef_type == NULL);
    assert(sa->error_count == 1);  // 应该有一个错误
    
    // 清理
    free_simple_tree(id_node);
    free_simple_tree(undefined_node);
    semantic_analyzer_destroy(sa);
}

/**
 * 测试作用域管理（集成测试）
 */
void test_scope_in_analysis() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    Type *int_type = new_int_type();
    
    // 全局作用域
    Symbol *global_x = symbol_insert(sa->symbol_table, "x", int_type, 1);
    assert(global_x != NULL);
    assert(sa->symbol_table->scope_level == 0);
    
    // 进入作用域（模拟进入函数或块）
    enter_scope(sa->symbol_table);
    assert(sa->symbol_table->scope_level == 1);
    
    // 在内层作用域插入变量
    Symbol *local_y = symbol_insert(sa->symbol_table, "y", int_type, 2);
    assert(local_y != NULL);
    
    // 退出作用域
    exit_scope(sa->symbol_table);
    assert(sa->symbol_table->scope_level == 0);
    
    // y应该已被删除
    Symbol *found_y = symbol_lookup(sa->symbol_table, "y");
    assert(found_y == NULL);
    
    // x应该还在
    Symbol *found_x = symbol_lookup(sa->symbol_table, "x");
    assert(found_x == global_x);
    
    semantic_analyzer_destroy(sa);
}

/**
 * 测试类型检查工具函数
 */
void test_type_check_utils() {
    SemanticAnalyzer *sa = semantic_analyzer_create("test.c");
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 插入不同类型的变量
    symbol_insert(sa->symbol_table, "int_var", int_type, 1);
    symbol_insert(sa->symbol_table, "float_var", float_type, 2);
    
    // 查找并验证类型
    Type *found_int = lookup_variable_type(sa, "int_var", 1);
    assert(found_int->kind == TYPE_INT);
    
    Type *found_float = lookup_variable_type(sa, "float_var", 2);
    assert(found_float->kind == TYPE_FLOAT);
    
    assert(sa->error_count == 0);
    
    semantic_analyzer_destroy(sa);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C 语义分析器单元测试\n");
    printf("========================================\n\n");
    
    // 运行所有测试
    RUN_TEST(test_analyzer_create);
    RUN_TEST(test_error_reporting);
    RUN_TEST(test_type_lookup);
    RUN_TEST(test_node_predicates);
    RUN_TEST(test_get_node_type);
    RUN_TEST(test_simple_expression);
    RUN_TEST(test_variable_lookup_analysis);
    RUN_TEST(test_scope_in_analysis);
    RUN_TEST(test_type_check_utils);
    
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

