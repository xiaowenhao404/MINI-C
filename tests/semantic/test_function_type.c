/**
 * Mini-C 编译器 - 函数类型测试
 * 
 * 测试 TASK205 子任务 5.1：扩展类型系统支持函数类型
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../../src/semantic/type_system.h"

/**
 * 测试函数类型创建
 */
void test_function_type_creation() {
    printf("测试 1: 函数类型创建...\n");
    
    // 初始化类型系统
    init_type_system();
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 创建函数类型: int func(int, float)
    Type *params[] = {int_type, float_type};
    Type *func_type = new_function_type(int_type, params, 2);
    
    assert(func_type != NULL);
    assert(func_type->kind == TYPE_FUNCTION);
    assert(func_type->return_type == int_type);
    assert(func_type->param_count == 2);
    assert(func_type->param_types[0] == int_type);
    assert(func_type->param_types[1] == float_type);
    
    printf("  ✓ 函数类型创建成功\n");
    printf("  函数签名: %s\n", type_to_string(func_type));
}

/**
 * 测试无参数函数类型
 */
void test_no_param_function() {
    printf("\n测试 2: 无参数函数类型...\n");
    
    Type *void_type = new_void_type();
    
    // 创建函数类型: void func()
    Type *func_type = new_function_type(void_type, NULL, 0);
    
    assert(func_type != NULL);
    assert(func_type->kind == TYPE_FUNCTION);
    assert(func_type->return_type == void_type);
    assert(func_type->param_count == 0);
    assert(func_type->param_types == NULL);
    
    printf("  ✓ 无参数函数类型创建成功\n");
    printf("  函数签名: %s\n", type_to_string(func_type));
}

/**
 * 测试函数类型相等性
 */
void test_function_type_equality() {
    printf("\n测试 3: 函数类型相等性比较...\n");
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 创建两个相同的函数类型
    Type *params1[] = {int_type, float_type};
    Type *func_type1 = new_function_type(int_type, params1, 2);
    
    Type *params2[] = {int_type, float_type};
    Type *func_type2 = new_function_type(int_type, params2, 2);
    
    // 应该相等
    assert(type_equal(func_type1, func_type2));
    printf("  ✓ 相同函数类型判断为相等\n");
    
    // 创建不同返回类型的函数
    Type *func_type3 = new_function_type(float_type, params1, 2);
    assert(!type_equal(func_type1, func_type3));
    printf("  ✓ 不同返回类型的函数类型判断为不相等\n");
    
    // 创建不同参数数量的函数
    Type *params4[] = {int_type};
    Type *func_type4 = new_function_type(int_type, params4, 1);
    assert(!type_equal(func_type1, func_type4));
    printf("  ✓ 不同参数数量的函数类型判断为不相等\n");
    
    // 创建不同参数类型的函数
    Type *params5[] = {float_type, float_type};
    Type *func_type5 = new_function_type(int_type, params5, 2);
    assert(!type_equal(func_type1, func_type5));
    printf("  ✓ 不同参数类型的函数类型判断为不相等\n");
}

/**
 * 测试函数类型字符串表示
 */
void test_function_type_to_string() {
    printf("\n测试 4: 函数类型字符串表示...\n");
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    Type *void_type = new_void_type();
    
    // 测试多参数函数
    Type *params1[] = {int_type, float_type, char_type};
    Type *func_type1 = new_function_type(int_type, params1, 3);
    const char *sig1 = type_to_string(func_type1);
    printf("  函数 1: %s\n", sig1);
    assert(sig1 != NULL);
    
    // 测试无参数函数
    Type *func_type2 = new_function_type(void_type, NULL, 0);
    const char *sig2 = type_to_string(func_type2);
    printf("  函数 2: %s\n", sig2);
    assert(sig2 != NULL);
    
    // 测试单参数函数
    Type *params3[] = {int_type};
    Type *func_type3 = new_function_type(float_type, params3, 1);
    const char *sig3 = type_to_string(func_type3);
    printf("  函数 3: %s\n", sig3);
    assert(sig3 != NULL);
    
    printf("  ✓ 函数类型字符串表示正确\n");
}

/**
 * 测试复杂函数类型
 */
void test_complex_function_types() {
    printf("\n测试 5: 复杂函数类型...\n");
    
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    
    // 创建指针类型作为参数
    Type *int_ptr = new_pointer_type(int_type);
    Type *float_ptr = new_pointer_type(float_type);
    
    // 创建函数类型: float func(int*, float*)
    Type *params[] = {int_ptr, float_ptr};
    Type *func_type = new_function_type(float_type, params, 2);
    
    assert(func_type != NULL);
    assert(func_type->param_count == 2);
    assert(func_type->param_types[0]->kind == TYPE_POINTER);
    assert(func_type->param_types[1]->kind == TYPE_POINTER);
    
    printf("  ✓ 指针参数的函数类型创建成功\n");
    printf("  函数签名: %s\n", type_to_string(func_type));
}

/**
 * 主测试函数
 */
int main() {
    printf("========================================\n");
    printf("Mini-C 编译器 - 函数类型测试\n");
    printf("测试 TASK205 子任务 5.1\n");
    printf("========================================\n\n");
    
    test_function_type_creation();
    test_no_param_function();
    test_function_type_equality();
    test_function_type_to_string();
    test_complex_function_types();
    
    printf("\n========================================\n");
    printf("所有测试通过！✓\n");
    printf("========================================\n");
    
    // 清理类型系统
    cleanup_type_system();
    
    return 0;
}

