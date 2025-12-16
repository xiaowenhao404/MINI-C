/**
 * Mini-C 编译器 - 符号表单元测试
 * 
 * 文件: test_symbol_table.c
 * 描述: 测试符号表的各项功能
 * 作者: Mini-C 开发团队
 */

#include "../../src/semantic/symbol_table.h"
#include "../../src/semantic/type_system.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

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

/* ==================== 测试函数 ==================== */

/**
 * 测试符号表创建和销毁
 */
void test_symbol_table_create() {
    SymbolTable *st = symbol_table_create(127);
    
    assert(st != NULL);
    assert(st->size == 127);
    assert(st->scope_level == 0);
    assert(st->next_offset == 0);
    assert(st->buckets != NULL);
    
    symbol_table_destroy(st);
}

/**
 * 测试符号插入和查找
 */
void test_symbol_insert_lookup() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    
    // 插入符号
    Symbol *sym_a = symbol_insert(st, "a", int_type, 1);
    assert(sym_a != NULL);
    assert(strcmp(sym_a->name, "a") == 0);
    assert(sym_a->type == int_type);
    assert(sym_a->scope_level == 0);
    
    // 查找符号
    Symbol *found = symbol_lookup(st, "a");
    assert(found != NULL);
    assert(found == sym_a);
    
    // 查找不存在的符号
    Symbol *not_found = symbol_lookup(st, "b");
    assert(not_found == NULL);
    
    symbol_table_destroy(st);
}

/**
 * 测试重定义检测
 */
void test_symbol_redefinition() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    
    // 插入符号
    Symbol *sym1 = symbol_insert(st, "x", int_type, 1);
    assert(sym1 != NULL);
    
    // 尝试在同一作用域重定义
    Symbol *sym2 = symbol_insert(st, "x", int_type, 2);
    assert(sym2 == NULL);  // 应该失败
    
    symbol_table_destroy(st);
}

/**
 * 测试作用域管理
 */
void test_scope_management() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    
    // 全局作用域
    assert(st->scope_level == 0);
    Symbol *global_a = symbol_insert(st, "a", int_type, 1);
    assert(global_a != NULL);
    assert(global_a->scope_level == 0);
    assert(global_a->is_global == true);
    
    // 进入作用域1
    enter_scope(st);
    assert(st->scope_level == 1);
    Symbol *local_b = symbol_insert(st, "b", int_type, 2);
    assert(local_b != NULL);
    assert(local_b->scope_level == 1);
    assert(local_b->is_global == false);
    
    // 可以在内层作用域声明同名变量（遮蔽外层）
    Symbol *local_a = symbol_insert(st, "a", int_type, 3);
    assert(local_a != NULL);
    assert(local_a->scope_level == 1);
    
    // 查找符号：应该找到内层的a
    Symbol *found_a = symbol_lookup(st, "a");
    assert(found_a == local_a);
    
    // 退出作用域1
    exit_scope(st);
    assert(st->scope_level == 0);
    
    // 现在查找a应该找到全局的a
    found_a = symbol_lookup(st, "a");
    assert(found_a == global_a);
    
    // 查找b应该失败（已被删除）
    Symbol *found_b = symbol_lookup(st, "b");
    assert(found_b == NULL);
    
    symbol_table_destroy(st);
}

/**
 * 测试嵌套作用域
 */
void test_nested_scopes() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 作用域0：全局
    Symbol *global_x = symbol_insert(st, "x", int_type, 1);
    assert(global_x != NULL);
    
    // 作用域1
    enter_scope(st);
    Symbol *s1_y = symbol_insert(st, "y", int_type, 2);
    assert(s1_y != NULL);
    
    // 作用域2
    enter_scope(st);
    Symbol *s2_z = symbol_insert(st, "z", float_type, 3);
    assert(s2_z != NULL);
    
    // 在作用域2可以访问所有变量
    assert(symbol_lookup(st, "x") == global_x);
    assert(symbol_lookup(st, "y") == s1_y);
    assert(symbol_lookup(st, "z") == s2_z);
    
    // 退出作用域2
    exit_scope(st);
    assert(st->scope_level == 1);
    
    // z已被删除
    assert(symbol_lookup(st, "z") == NULL);
    
    // 退出作用域1
    exit_scope(st);
    assert(st->scope_level == 0);
    
    // y已被删除
    assert(symbol_lookup(st, "y") == NULL);
    
    // x仍然存在
    assert(symbol_lookup(st, "x") == global_x);
    
    symbol_table_destroy(st);
}

/**
 * 测试栈偏移量分配
 */
void test_offset_allocation() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    Type *char_type = new_char_type();
    Type *float_type = new_float_type();
    
    // 进入函数作用域
    enter_scope(st);
    
    // 插入变量并检查偏移量
    Symbol *a = symbol_insert(st, "a", int_type, 1);
    assert(a->offset == 0);  // 第一个变量，偏移0
    
    Symbol *b = symbol_insert(st, "b", int_type, 2);
    assert(b->offset == 4);  // int是4字节
    
    Symbol *c = symbol_insert(st, "c", char_type, 3);
    assert(c->offset == 8);  // 对齐到4字节
    
    Symbol *d = symbol_insert(st, "d", float_type, 4);
    assert(d->offset == 12);  // 继续分配
    
    // 检查栈帧大小
    int stack_size = get_stack_size(st);
    assert(stack_size == 16);  // 4 + 4 + 4 + 4 = 16
    
    exit_scope(st);
    symbol_table_destroy(st);
}

/**
 * 测试符号遮蔽（Shadowing）
 */
void test_symbol_shadowing() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 全局作用域定义 x
    Symbol *global_x = symbol_insert(st, "x", int_type, 1);
    assert(global_x != NULL);
    
    // 进入作用域
    enter_scope(st);
    
    // 内层作用域定义同名 x（不同类型）
    Symbol *local_x = symbol_insert(st, "x", float_type, 2);
    assert(local_x != NULL);  // 允许遮蔽
    
    // 查找应该找到内层的 x
    Symbol *found = symbol_lookup(st, "x");
    assert(found == local_x);
    assert(found->type == float_type);
    
    // 退出作用域
    exit_scope(st);
    
    // 现在查找应该找到外层的 x
    found = symbol_lookup(st, "x");
    assert(found == global_x);
    assert(found->type == int_type);
    
    symbol_table_destroy(st);
}

/**
 * 测试多个符号
 */
void test_multiple_symbols() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    
    // 插入多个符号
    symbol_insert(st, "var1", int_type, 1);
    symbol_insert(st, "var2", int_type, 2);
    symbol_insert(st, "var3", int_type, 3);
    symbol_insert(st, "var4", int_type, 4);
    symbol_insert(st, "var5", int_type, 5);
    
    // 检查符号数量
    assert(symbol_table_count(st) == 5);
    assert(symbol_table_count_current_scope(st) == 5);
    
    // 所有符号都能找到
    assert(symbol_lookup(st, "var1") != NULL);
    assert(symbol_lookup(st, "var2") != NULL);
    assert(symbol_lookup(st, "var3") != NULL);
    assert(symbol_lookup(st, "var4") != NULL);
    assert(symbol_lookup(st, "var5") != NULL);
    
    symbol_table_destroy(st);
}

/**
 * 测试符号统计
 */
void test_symbol_count() {
    SymbolTable *st = symbol_table_create(127);
    Type *int_type = new_int_type();
    
    // 全局作用域：2个符号
    symbol_insert(st, "global1", int_type, 1);
    symbol_insert(st, "global2", int_type, 2);
    assert(symbol_table_count(st) == 2);
    assert(symbol_table_count_current_scope(st) == 2);
    
    // 进入作用域1：添加3个符号
    enter_scope(st);
    symbol_insert(st, "local1", int_type, 3);
    symbol_insert(st, "local2", int_type, 4);
    symbol_insert(st, "local3", int_type, 5);
    assert(symbol_table_count(st) == 5);
    assert(symbol_table_count_current_scope(st) == 3);
    
    // 退出作用域1
    exit_scope(st);
    assert(symbol_table_count(st) == 2);
    assert(symbol_table_count_current_scope(st) == 2);
    
    symbol_table_destroy(st);
}

/**
 * 测试不同类型的符号
 */
void test_different_types() {
    SymbolTable *st = symbol_table_create(127);
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // 插入不同类型的变量
    Symbol *int_var = symbol_insert(st, "int_var", int_type, 1);
    Symbol *float_var = symbol_insert(st, "float_var", float_type, 2);
    Symbol *char_var = symbol_insert(st, "char_var", char_type, 3);
    
    assert(int_var != NULL);
    assert(float_var != NULL);
    assert(char_var != NULL);
    
    // 查找并验证类型
    Symbol *found_int = symbol_lookup(st, "int_var");
    assert(found_int->type->kind == TYPE_INT);
    
    Symbol *found_float = symbol_lookup(st, "float_var");
    assert(found_float->type->kind == TYPE_FLOAT);
    
    Symbol *found_char = symbol_lookup(st, "char_var");
    assert(found_char->type->kind == TYPE_CHAR);
    
    symbol_table_destroy(st);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C 符号表单元测试\n");
    printf("========================================\n\n");
    
    // 初始化类型系统
    init_type_system();
    
    // 运行所有测试
    RUN_TEST(test_symbol_table_create);
    RUN_TEST(test_symbol_insert_lookup);
    RUN_TEST(test_symbol_redefinition);
    RUN_TEST(test_scope_management);
    RUN_TEST(test_nested_scopes);
    RUN_TEST(test_offset_allocation);
    RUN_TEST(test_symbol_shadowing);
    RUN_TEST(test_multiple_symbols);
    RUN_TEST(test_symbol_count);
    RUN_TEST(test_different_types);
    
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

