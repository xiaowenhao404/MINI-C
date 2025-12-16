/**
 * Mini-C 编译器 - 函数符号测试
 * 
 * 测试 TASK205 子任务 5.2：扩展符号表支持函数符号
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../src/semantic/symbol_table.h"
#include "../../src/semantic/type_system.h"

/**
 * 测试函数符号插入
 */
void test_function_symbol_insert() {
    printf("测试 1: 函数符号插入...\n");
    
    // 初始化类型系统和符号表
    init_type_system();
    SymbolTable *st = symbol_table_create(127);
    
    // 创建函数类型: int add(int, int)
    Type *int_type = new_int_type();
    Type *params[] = {int_type, int_type};
    Type *func_type = new_function_type(int_type, params, 2);
    
    // 插入函数符号
    Symbol *add_sym = symbol_insert(st, "add", func_type, 1);
    
    assert(add_sym != NULL);
    assert(add_sym->kind == SYM_FUNCTION);
    assert(strcmp(add_sym->name, "add") == 0);
    assert(add_sym->type == func_type);
    assert(add_sym->offset == 0);  // 函数不需要栈偏移
    
    printf("  ✓ 函数符号插入成功\n");
    printf("  函数名: %s\n", add_sym->name);
    printf("  函数类型: %s\n", type_to_string(add_sym->type));
    
    symbol_table_destroy(st);
}

/**
 * 测试函数符号重定义检测
 */
void test_function_redefinition() {
    printf("\n测试 2: 函数重定义检测...\n");
    
    SymbolTable *st = symbol_table_create(127);
    
    // 创建函数类型
    Type *int_type = new_int_type();
    Type *params[] = {int_type, int_type};
    Type *func_type = new_function_type(int_type, params, 2);
    
    // 插入第一个函数
    Symbol *first = symbol_insert(st, "calculate", func_type, 1);
    assert(first != NULL);
    printf("  ✓ 第一次插入成功\n");
    
    // 尝试重定义同名函数（应该失败）
    Symbol *duplicate = symbol_insert(st, "calculate", func_type, 2);
    assert(duplicate == NULL);
    printf("  ✓ 重定义检测成功（返回 NULL）\n");
    
    symbol_table_destroy(st);
}

/**
 * 测试函数符号查找
 */
void test_function_symbol_lookup() {
    printf("\n测试 3: 函数符号查找...\n");
    
    SymbolTable *st = symbol_table_create(127);
    
    // 插入多个函数
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *void_type = new_void_type();
    
    // 函数1: int add(int, int)
    Type *params1[] = {int_type, int_type};
    Type *func_type1 = new_function_type(int_type, params1, 2);
    Symbol *add_sym = symbol_insert(st, "add", func_type1, 1);
    
    // 函数2: float calculate(float, float)
    Type *params2[] = {float_type, float_type};
    Type *func_type2 = new_function_type(float_type, params2, 2);
    Symbol *calc_sym = symbol_insert(st, "calculate", func_type2, 2);
    
    // 函数3: void print()
    Type *func_type3 = new_function_type(void_type, NULL, 0);
    Symbol *print_sym = symbol_insert(st, "print", func_type3, 3);
    
    // 查找函数
    Symbol *found_add = symbol_lookup(st, "add");
    assert(found_add == add_sym);
    assert(symbol_is_function(found_add));
    
    Symbol *found_calc = symbol_lookup(st, "calculate");
    assert(found_calc == calc_sym);
    
    Symbol *found_print = symbol_lookup(st, "print");
    assert(found_print == print_sym);
    
    // 查找不存在的函数
    Symbol *not_found = symbol_lookup(st, "nonexistent");
    assert(not_found == NULL);
    
    printf("  ✓ 函数符号查找正确\n");
    printf("  已插入函数: add, calculate, print\n");
    
    symbol_table_destroy(st);
}

/**
 * 测试函数辅助函数
 */
void test_function_helper_functions() {
    printf("\n测试 4: 函数辅助函数...\n");
    
    SymbolTable *st = symbol_table_create(127);
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // 创建函数类型: int process(int, float, char)
    Type *params[] = {int_type, float_type, char_type};
    Type *func_type = new_function_type(int_type, params, 3);
    
    Symbol *func_sym = symbol_insert(st, "process", func_type, 1);
    
    // 测试 symbol_is_function
    assert(symbol_is_function(func_sym));
    printf("  ✓ symbol_is_function() 正确\n");
    
    // 测试 symbol_get_param_count
    int param_count = symbol_get_param_count(func_sym);
    assert(param_count == 3);
    printf("  ✓ symbol_get_param_count() = %d\n", param_count);
    
    // 测试 symbol_get_param_type
    Type *param0 = symbol_get_param_type(func_sym, 0);
    assert(param0 == int_type);
    printf("  ✓ 参数 0 类型: %s\n", type_to_string(param0));
    
    Type *param1 = symbol_get_param_type(func_sym, 1);
    assert(param1 == float_type);
    printf("  ✓ 参数 1 类型: %s\n", type_to_string(param1));
    
    Type *param2 = symbol_get_param_type(func_sym, 2);
    assert(param2 == char_type);
    printf("  ✓ 参数 2 类型: %s\n", type_to_string(param2));
    
    // 测试 symbol_get_return_type
    Type *ret_type = symbol_get_return_type(func_sym);
    assert(ret_type == int_type);
    printf("  ✓ 返回类型: %s\n", type_to_string(ret_type));
    
    // 测试边界情况
    Type *invalid_param = symbol_get_param_type(func_sym, 10);
    assert(invalid_param == NULL);
    printf("  ✓ 无效参数索引返回 NULL\n");
    
    symbol_table_destroy(st);
}

/**
 * 测试函数与变量共存
 */
void test_function_and_variable_coexist() {
    printf("\n测试 5: 函数与变量符号共存...\n");
    
    SymbolTable *st = symbol_table_create(127);
    
    Type *int_type = new_int_type();
    
    // 插入变量符号
    Symbol *var_a = symbol_insert(st, "a", int_type, 1);
    assert(var_a != NULL);
    assert(var_a->kind == SYM_VARIABLE);
    printf("  ✓ 插入变量 'a'\n");
    
    // 插入函数符号
    Type *params[] = {int_type};
    Type *func_type = new_function_type(int_type, params, 1);
    Symbol *func_b = symbol_insert(st, "b", func_type, 2);
    assert(func_b != NULL);
    assert(func_b->kind == SYM_FUNCTION);
    printf("  ✓ 插入函数 'b'\n");
    
    // 再插入一个变量
    Symbol *var_c = symbol_insert(st, "c", int_type, 3);
    assert(var_c != NULL);
    assert(var_c->kind == SYM_VARIABLE);
    printf("  ✓ 插入变量 'c'\n");
    
    // 查找验证
    assert(symbol_lookup(st, "a") == var_a);
    assert(symbol_lookup(st, "b") == func_b);
    assert(symbol_lookup(st, "c") == var_c);
    
    assert(!symbol_is_function(var_a));
    assert(symbol_is_function(func_b));
    assert(!symbol_is_function(var_c));
    
    printf("  ✓ 函数和变量符号可正确共存\n");
    
    symbol_table_destroy(st);
}

/**
 * 测试不同作用域的函数
 */
void test_function_in_different_scopes() {
    printf("\n测试 6: 不同作用域的函数符号...\n");
    
    SymbolTable *st = symbol_table_create(127);
    
    Type *int_type = new_int_type();
    Type *params[] = {int_type};
    Type *func_type = new_function_type(int_type, params, 1);
    
    // 全局作用域插入函数
    Symbol *global_func = symbol_insert(st, "global_function", func_type, 1);
    assert(global_func != NULL);
    assert(global_func->scope_level == 0);
    assert(global_func->is_global == true);
    printf("  ✓ 全局函数插入成功（作用域级别 0）\n");
    
    // 函数通常在全局作用域定义，但测试作用域管理
    enter_scope(st);
    
    // 尝试在局部作用域插入变量（正常）
    Symbol *local_var = symbol_insert(st, "local_x", int_type, 2);
    assert(local_var != NULL);
    assert(local_var->scope_level == 1);
    assert(local_var->is_global == false);
    printf("  ✓ 局部变量插入成功（作用域级别 1）\n");
    
    // 退出作用域
    exit_scope(st);
    
    // 验证全局函数仍可访问
    Symbol *found = symbol_lookup(st, "global_function");
    assert(found == global_func);
    printf("  ✓ 退出作用域后全局函数仍可访问\n");
    
    // 局部变量应已被删除
    Symbol *deleted = symbol_lookup(st, "local_x");
    assert(deleted == NULL);
    printf("  ✓ 局部变量已正确删除\n");
    
    symbol_table_destroy(st);
}

/**
 * 主测试函数
 */
int main() {
    printf("========================================\n");
    printf("Mini-C 编译器 - 函数符号测试\n");
    printf("测试 TASK205 子任务 5.2\n");
    printf("========================================\n\n");
    
    test_function_symbol_insert();
    test_function_redefinition();
    test_function_symbol_lookup();
    test_function_helper_functions();
    test_function_and_variable_coexist();
    test_function_in_different_scopes();
    
    printf("\n========================================\n");
    printf("所有测试通过！✓\n");
    printf("========================================\n");
    
    cleanup_type_system();
    
    return 0;
}

