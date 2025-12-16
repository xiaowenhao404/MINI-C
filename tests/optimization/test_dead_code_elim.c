/**
 * Mini-C 编译器 - 死代码消除优化单元测试
 * 
 * 文件: test_dead_code_elim.c
 * 描述: 测试死代码消除优化功能
 * 作者: Mini-C 开发团队
 */

#include "../../src/optimization/dead_code_elim.h"
#include "../../src/optimization/const_folding.h"
#include "../../c-complier-master/tree.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// 测试计数器
static int test_count = 0;
static int pass_count = 0;

// 辅助宏
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
 * 创建常量节点
 */
Tree* create_const_node(const char *name, const char *value) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    node->name = strdup(name);
    node->content = strdup(value);
    node->line = 1;
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
 * 创建if语句节点
 */
Tree* create_if_node(Tree *cond, Tree *then_branch, Tree *else_branch) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    node->name = strdup(else_branch ? "if_else_expression" : "if_expression");
    node->content = NULL;
    node->line = 1;
    node->num = else_branch ? 3 : 2;
    node->leaves = (Tree**)malloc(sizeof(Tree*) * node->num);
    node->leaves[0] = cond;
    node->leaves[1] = then_branch;
    if (else_branch) {
        node->leaves[2] = else_branch;
    }
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    return node;
}

/**
 * 创建while语句节点
 */
Tree* create_while_node(Tree *cond, Tree *body) {
    Tree *node = (Tree*)malloc(sizeof(Tree));
    node->name = strdup("while_expression");
    node->content = NULL;
    node->line = 1;
    node->num = 2;
    node->leaves = (Tree**)malloc(sizeof(Tree*) * 2);
    node->leaves[0] = cond;
    node->leaves[1] = body;
    node->next = NULL;
    node->headline = 0;
    node->nextline = 0;
    node->inner = NULL;
    node->code = NULL;
    node->declator = NULL;
    return node;
}

/* ==================== 测试函数 ==================== */

/**
 * 测试常量零判断
 */
void test_is_constant_zero() {
    Tree *zero = create_const_node("INT10", "0");
    Tree *one = create_const_node("INT10", "1");
    Tree *nonconst = create_const_node("ID", "x");
    
    assert(is_constant_zero(zero) == true);
    assert(is_constant_zero(one) == false);
    assert(is_constant_zero(nonconst) == false);
    
    free(zero->name); free(zero->content); free(zero);
    free(one->name); free(one->content); free(one);
    free(nonconst->name); free(nonconst->content); free(nonconst);
}

/**
 * 测试常量非零判断
 */
void test_is_constant_nonzero() {
    Tree *zero = create_const_node("INT10", "0");
    Tree *one = create_const_node("INT10", "1");
    Tree *fortytwo = create_const_node("INT10", "42");
    
    assert(is_constant_nonzero(zero) == false);
    assert(is_constant_nonzero(one) == true);
    assert(is_constant_nonzero(fortytwo) == true);
    
    free(zero->name); free(zero->content); free(zero);
    free(one->name); free(one->content); free(one);
    free(fortytwo->name); free(fortytwo->content); free(fortytwo);
}

/**
 * 测试节点类型判断
 */
void test_node_type_predicates() {
    Tree *if_node = create_const_node("if_expression", NULL);
    Tree *while_node = create_const_node("while_expression", NULL);
    Tree *for_node = create_const_node("for_expression", NULL);
    Tree *other = create_const_node("assignment_expression", NULL);
    
    assert(is_if_statement(if_node) == true);
    assert(is_if_statement(while_node) == false);
    
    assert(is_while_statement(while_node) == true);
    assert(is_while_statement(if_node) == false);
    
    assert(is_for_statement(for_node) == true);
    assert(is_for_statement(other) == false);
    
    free(if_node->name); free(if_node);
    free(while_node->name); free(while_node);
    free(for_node->name); free(for_node);
    free(other->name); free(other);
}

/**
 * 测试消除if(0)
 */
void test_eliminate_if_zero() {
    reset_dce_stats();
    
    // 创建: if (0) { then_stmt }
    Tree *cond = create_const_node("INT10", "0");
    Tree *then_stmt = create_const_node("statement", NULL);
    Tree *if_node = create_if_node(cond, then_stmt, NULL);
    
    // 优化
    Tree *result = eliminate_dead_code(if_node);
    
    // if(0)应该被删除
    assert(result == NULL);
    DCEStats stats = get_dce_stats();
    assert(stats.eliminated_if_blocks == 1);
    
    printf("(优化: if(0){...} → 删除) ");
}

/**
 * 测试消除if(0)但保留else
 */
void test_eliminate_if_zero_keep_else() {
    reset_dce_stats();
    
    // 创建: if (0) { then_stmt } else { else_stmt }
    Tree *cond = create_const_node("INT10", "0");
    Tree *then_stmt = create_const_node("statement", "then");
    Tree *else_stmt = create_const_node("statement", "else");
    Tree *if_node = create_if_node(cond, then_stmt, else_stmt);
    
    // 优化
    Tree *result = eliminate_dead_code(if_node);
    
    // 应该只保留else分支
    assert(result != NULL);
    assert(result == else_stmt);
    DCEStats stats = get_dce_stats();
    assert(stats.eliminated_if_blocks == 1);
    
    printf("(优化: if(0){A}else{B} → B) ");
}

/**
 * 测试保留if(1)删除else
 */
void test_eliminate_if_one() {
    reset_dce_stats();
    
    // 创建: if (1) { then_stmt } else { else_stmt }
    Tree *cond = create_const_node("INT10", "1");
    Tree *then_stmt = create_const_node("statement", "then");
    Tree *else_stmt = create_const_node("statement", "else");
    Tree *if_node = create_if_node(cond, then_stmt, else_stmt);
    
    // 优化
    Tree *result = eliminate_dead_code(if_node);
    
    // 应该只保留then分支
    assert(result != NULL);
    assert(result == then_stmt);
    DCEStats stats = get_dce_stats();
    assert(stats.eliminated_else_blocks == 1);
    
    printf("(优化: if(1){A}else{B} → A) ");
}

/**
 * 测试消除while(0)
 */
void test_eliminate_while_zero() {
    reset_dce_stats();
    
    // 创建: while (0) { body }
    Tree *cond = create_const_node("INT10", "0");
    Tree *body = create_const_node("statement", NULL);
    Tree *while_node = create_while_node(cond, body);
    
    // 优化
    Tree *result = eliminate_dead_code(while_node);
    
    // while(0)应该被删除
    assert(result == NULL);
    DCEStats stats = get_dce_stats();
    assert(stats.eliminated_loops == 1);
    
    printf("(优化: while(0){...} → 删除) ");
}

/**
 * 测试保留while(1)
 */
void test_keep_while_one() {
    reset_dce_stats();
    
    // 创建: while (1) { body }
    Tree *cond = create_const_node("INT10", "1");
    Tree *body = create_const_node("statement", NULL);
    Tree *while_node = create_while_node(cond, body);
    
    // 优化（应该保留，但有无限循环警告）
    Tree *result = eliminate_dead_code(while_node);
    
    // while(1)应该保留
    assert(result != NULL);
    assert(result == while_node);
    
    printf("(保留: while(1){...} 有警告) ");
}

/**
 * 测试优化统计
 */
void test_dce_stats() {
    reset_dce_stats();
    
    // 创建多个可优化的结构
    Tree *cond0 = create_const_node("INT10", "0");
    Tree *then1 = create_const_node("statement", NULL);
    Tree *if1 = create_if_node(cond0, then1, NULL);
    
    Tree *cond2 = create_const_node("INT10", "0");
    Tree *body2 = create_const_node("statement", NULL);
    Tree *while1 = create_while_node(cond2, body2);
    
    // 优化
    eliminate_dead_code(if1);
    eliminate_dead_code(while1);
    
    // 检查统计
    DCEStats stats = get_dce_stats();
    assert(stats.eliminated_if_blocks == 1);
    assert(stats.eliminated_loops == 1);
    assert(stats.total_nodes_removed == 2);
    
    printf("(统计: 消除%d个if块, %d个循环) ", 
           stats.eliminated_if_blocks, stats.eliminated_loops);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C 死代码消除优化单元测试\n");
    printf("========================================\n\n");
    
    // 运行所有测试
    RUN_TEST(test_is_constant_zero);
    RUN_TEST(test_is_constant_nonzero);
    RUN_TEST(test_node_type_predicates);
    RUN_TEST(test_eliminate_if_zero);
    RUN_TEST(test_eliminate_if_zero_keep_else);
    RUN_TEST(test_eliminate_if_one);
    RUN_TEST(test_eliminate_while_zero);
    RUN_TEST(test_keep_while_one);
    RUN_TEST(test_dce_stats);
    
    // 打印优化统计
    print_dce_stats();
    
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

