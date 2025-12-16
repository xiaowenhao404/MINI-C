/**
 * Mini-C 编译器 - 类型系统单元测试
 * 
 * 文件: test_type_system.c
 * 描述: 测试类型系统的各项功能
 * 作者: Mini-C 开发团队
 */

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
 * 测试基本类型创建
 */
void test_basic_type_creation() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    Type *void_type = new_void_type();
    
    assert(int_type != NULL);
    assert(float_type != NULL);
    assert(char_type != NULL);
    assert(void_type != NULL);
    
    assert(int_type->kind == TYPE_INT);
    assert(float_type->kind == TYPE_FLOAT);
    assert(char_type->kind == TYPE_CHAR);
    assert(void_type->kind == TYPE_VOID);
}

/**
 * 测试类型大小计算
 */
void test_type_size() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    Type *void_type = new_void_type();
    
    assert(type_size(int_type) == 4);
    assert(type_size(float_type) == 4);
    assert(type_size(char_type) == 1);
    assert(type_size(void_type) == 0);
}

/**
 * 测试类型比较
 */
void test_type_equal() {
    Type *int_type1 = new_int_type();
    Type *int_type2 = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // 相同类型应该相等
    assert(type_equal(int_type1, int_type2) == true);
    
    // 不同类型应该不相等
    assert(type_equal(int_type1, float_type) == false);
    assert(type_equal(int_type1, char_type) == false);
    assert(type_equal(float_type, char_type) == false);
}

/**
 * 测试类型兼容性
 */
void test_type_compatible() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // int 和 float 兼容
    assert(type_compatible(int_type, float_type) == true);
    assert(type_compatible(float_type, int_type) == true);
    
    // char 和 int 兼容
    assert(type_compatible(char_type, int_type) == true);
    assert(type_compatible(int_type, char_type) == true);
    
    // char 和 float 兼容
    assert(type_compatible(char_type, float_type) == true);
    assert(type_compatible(float_type, char_type) == true);
    
    // 相同类型兼容
    assert(type_compatible(int_type, int_type) == true);
    assert(type_compatible(float_type, float_type) == true);
    assert(type_compatible(char_type, char_type) == true);
}

/**
 * 测试类型提升
 */
void test_promote_type() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    Type *result;
    
    // int + int → int
    result = promote_type(int_type, int_type);
    assert(result != NULL);
    assert(result->kind == TYPE_INT);
    
    // float + float → float
    result = promote_type(float_type, float_type);
    assert(result != NULL);
    assert(result->kind == TYPE_FLOAT);
    
    // int + float → float
    result = promote_type(int_type, float_type);
    assert(result != NULL);
    assert(result->kind == TYPE_FLOAT);
    
    result = promote_type(float_type, int_type);
    assert(result != NULL);
    assert(result->kind == TYPE_FLOAT);
    
    // char + int → int
    result = promote_type(char_type, int_type);
    assert(result != NULL);
    assert(result->kind == TYPE_INT);
    
    // char + float → float
    result = promote_type(char_type, float_type);
    assert(result != NULL);
    assert(result->kind == TYPE_FLOAT);
    
    // char + char → int
    result = promote_type(char_type, char_type);
    assert(result != NULL);
    assert(result->kind == TYPE_INT);
}

/**
 * 测试类型判断函数
 */
void test_type_predicates() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // is_integer_type
    assert(is_integer_type(int_type) == true);
    assert(is_integer_type(char_type) == true);
    assert(is_integer_type(float_type) == false);
    
    // is_float_type
    assert(is_float_type(float_type) == true);
    assert(is_float_type(int_type) == false);
    assert(is_float_type(char_type) == false);
    
    // is_numeric_type
    assert(is_numeric_type(int_type) == true);
    assert(is_numeric_type(float_type) == true);
    assert(is_numeric_type(char_type) == true);
}

/**
 * 测试类型字符串表示
 */
void test_type_to_string() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    Type *void_type = new_void_type();
    
    assert(strcmp(type_to_string(int_type), "int") == 0);
    assert(strcmp(type_to_string(float_type), "float") == 0);
    assert(strcmp(type_to_string(char_type), "char") == 0);
    assert(strcmp(type_to_string(void_type), "void") == 0);
}

/**
 * 测试类型转换判断
 */
void test_can_cast() {
    Type *int_type = new_int_type();
    Type *float_type = new_float_type();
    Type *char_type = new_char_type();
    
    // 数值类型之间可以互相转换
    assert(can_cast(int_type, float_type) == true);
    assert(can_cast(float_type, int_type) == true);
    assert(can_cast(char_type, int_type) == true);
    assert(can_cast(int_type, char_type) == true);
    assert(can_cast(char_type, float_type) == true);
    assert(can_cast(float_type, char_type) == true);
}

/**
 * 测试数组类型（预留）
 */
void test_array_type() {
    Type *int_type = new_int_type();
    Type *array_type = new_array_type(int_type, 10);
    
    assert(array_type != NULL);
    assert(array_type->kind == TYPE_ARRAY);
    assert(array_type->array_len == 10);
    assert(array_type->size == 40);  // 10 * 4
    assert(type_equal(array_type->base, int_type) == true);
    
    type_free(array_type);
}

/**
 * 测试指针类型（预留）
 */
void test_pointer_type() {
    Type *int_type = new_int_type();
    Type *pointer_type = new_pointer_type(int_type);
    
    assert(pointer_type != NULL);
    assert(pointer_type->kind == TYPE_POINTER);
    assert(pointer_type->size == 8);  // x86-64指针是8字节
    assert(type_equal(pointer_type->base, int_type) == true);
    
    type_free(pointer_type);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C 类型系统单元测试\n");
    printf("========================================\n\n");
    
    // 初始化类型系统
    init_type_system();
    
    // 运行所有测试
    RUN_TEST(test_basic_type_creation);
    RUN_TEST(test_type_size);
    RUN_TEST(test_type_equal);
    RUN_TEST(test_type_compatible);
    RUN_TEST(test_promote_type);
    RUN_TEST(test_type_predicates);
    RUN_TEST(test_type_to_string);
    RUN_TEST(test_can_cast);
    RUN_TEST(test_array_type);
    RUN_TEST(test_pointer_type);
    
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

