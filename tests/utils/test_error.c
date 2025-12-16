/**
 * Mini-C 编译器 - 错误处理系统单元测试
 * 
 * 文件: test_error.c
 * 描述: 测试错误报告器的各项功能
 * 作者: Mini-C 开发团队
 */

#include "../../src/utils/error.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

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

/* ==================== 测试函数 ==================== */

/**
 * 测试错误报告器创建和销毁
 */
void test_error_reporter_lifecycle() {
    ErrorReporter *er = error_reporter_create(10);
    
    assert(er != NULL);
    assert(er->error_count == 0);
    assert(er->warning_count == 0);
    assert(er->max_errors == 10);
    assert(er->output == stderr);
    
    error_reporter_destroy(er);
}

/**
 * 测试错误计数
 */
void test_error_counting() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 报告几个错误
    report_error(er, "test.c", 1, 1, ERROR_SEMANTIC, "测试错误1");
    assert(er->error_count == 1);
    
    report_error(er, "test.c", 2, 5, ERROR_SYNTAX, "测试错误2");
    assert(er->error_count == 2);
    
    report_error(er, "test.c", 3, 10, ERROR_LEXICAL, "测试错误3");
    assert(er->error_count == 3);
    
    error_reporter_destroy(er);
}

/**
 * 测试警告计数
 */
void test_warning_counting() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 报告几个警告
    report_warning(er, "test.c", 1, 1, "测试警告1");
    assert(er->warning_count == 1);
    
    report_warning(er, "test.c", 2, 5, "测试警告2");
    assert(er->warning_count == 2);
    
    // 警告不应该影响错误计数
    assert(er->error_count == 0);
    
    error_reporter_destroy(er);
}

/**
 * 测试错误和警告混合
 */
void test_mixed_errors_and_warnings() {
    ErrorReporter *er = error_reporter_create(100);
    
    report_error(er, "test.c", 1, 1, ERROR_SEMANTIC, "错误");
    report_warning(er, "test.c", 2, 1, "警告");
    report_error(er, "test.c", 3, 1, ERROR_SYNTAX, "错误");
    report_warning(er, "test.c", 4, 1, "警告");
    
    assert(er->error_count == 2);
    assert(er->warning_count == 2);
    
    error_reporter_destroy(er);
}

/**
 * 测试错误阈值
 */
void test_error_threshold() {
    ErrorReporter *er = error_reporter_create(3); // 最多3个错误
    
    report_error(er, "test.c", 1, 1, ERROR_SEMANTIC, "错误1");
    assert(!should_stop_compilation(er));
    
    report_error(er, "test.c", 2, 1, ERROR_SEMANTIC, "错误2");
    assert(!should_stop_compilation(er));
    
    report_error(er, "test.c", 3, 1, ERROR_SEMANTIC, "错误3");
    assert(should_stop_compilation(er)); // 应该停止
    
    error_reporter_destroy(er);
}

/**
 * 测试错误类型字符串
 */
void test_error_type_strings() {
    assert(strcmp(error_type_to_string(ERROR_LEXICAL), "词法错误") == 0);
    assert(strcmp(error_type_to_string(ERROR_SYNTAX), "语法错误") == 0);
    assert(strcmp(error_type_to_string(ERROR_SEMANTIC), "语义错误") == 0);
    assert(strcmp(error_type_to_string(ERROR_INTERNAL), "内部错误") == 0);
}

/**
 * 测试彩色输出开关
 */
void test_color_toggle() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 默认启用
    assert(er->use_color == true);
    
    // 禁用
    error_reporter_set_color(er, false);
    assert(er->use_color == false);
    
    // 启用
    error_reporter_set_color(er, true);
    assert(er->use_color == true);
    
    error_reporter_destroy(er);
}

/**
 * 测试源代码显示开关
 */
void test_show_source_toggle() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 默认启用
    assert(er->show_source == true);
    
    // 禁用
    error_reporter_set_show_source(er, false);
    assert(er->show_source == false);
    
    // 启用
    error_reporter_set_show_source(er, true);
    assert(er->show_source == true);
    
    error_reporter_destroy(er);
}

/**
 * 测试当前文件设置
 */
void test_current_file() {
    ErrorReporter *er = error_reporter_create(100);
    
    error_reporter_set_file(er, "main.c");
    assert(strcmp(er->current_file, "main.c") == 0);
    
    error_reporter_set_file(er, "utils.c");
    assert(strcmp(er->current_file, "utils.c") == 0);
    
    error_reporter_destroy(er);
}

/**
 * 测试格式化输出
 */
void test_formatted_output() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 测试可变参数
    report_error(er, "test.c", 10, 5, ERROR_SEMANTIC, 
                 "类型不匹配: 期望 '%s' 但得到 '%s'", "int", "float");
    assert(er->error_count == 1);
    
    report_warning(er, "test.c", 20, 3, 
                   "未使用的变量 '%s'", "unused_var");
    assert(er->warning_count == 1);
    
    error_reporter_destroy(er);
}

/**
 * 测试提示信息
 */
void test_note_reporting() {
    ErrorReporter *er = error_reporter_create(100);
    
    // 提示信息不应增加错误或警告计数
    report_note(er, "这是一条提示信息");
    assert(er->error_count == 0);
    assert(er->warning_count == 0);
    
    error_reporter_destroy(er);
}

/**
 * 测试错误统计输出
 */
void test_error_summary() {
    ErrorReporter *er = error_reporter_create(100);
    
    printf("\n--- 测试错误统计输出（无错误无警告）---\n");
    print_error_summary(er);
    
    printf("\n--- 测试错误统计输出（有错误有警告）---\n");
    report_error(er, "test.c", 1, 1, ERROR_SEMANTIC, "测试错误");
    report_warning(er, "test.c", 2, 1, "测试警告");
    print_error_summary(er);
    
    error_reporter_destroy(er);
}

/* ==================== 主测试函数 ==================== */

int main() {
    printf("========================================\n");
    printf("  Mini-C 错误处理系统单元测试\n");
    printf("========================================\n\n");
    
    // 运行所有测试
    RUN_TEST(test_error_reporter_lifecycle);
    RUN_TEST(test_error_counting);
    RUN_TEST(test_warning_counting);
    RUN_TEST(test_mixed_errors_and_warnings);
    RUN_TEST(test_error_threshold);
    RUN_TEST(test_error_type_strings);
    RUN_TEST(test_color_toggle);
    RUN_TEST(test_show_source_toggle);
    RUN_TEST(test_current_file);
    RUN_TEST(test_formatted_output);
    RUN_TEST(test_note_reporting);
    RUN_TEST(test_error_summary);
    
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

