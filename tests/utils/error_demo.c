/**
 * Mini-C 编译器 - 错误处理系统演示程序
 * 
 * 展示错误处理系统的各种功能
 */

#include "../../src/utils/error.h"
#include <stdio.h>

int main() {
    printf("========================================\n");
    printf("  Mini-C 错误处理系统功能演示\n");
    printf("========================================\n\n");
    
    // 创建错误报告器
    ErrorReporter *er = error_reporter_create(10);
    error_reporter_set_file(er, "example.c");
    
    // 演示1：语义错误
    printf("【演示1】语义错误 - 类型不匹配\n");
    printf("--------------------------------------\n");
    report_error(er, "example.c", 5, 10, ERROR_SEMANTIC,
                 "类型不匹配: 无法将 'float' 赋值给 'int'");
    printf("\n");
    
    // 演示2：语法错误
    printf("【演示2】语法错误 - 缺少分号\n");
    printf("--------------------------------------\n");
    report_error(er, "example.c", 12, 25, ERROR_SYNTAX,
                 "期望 ';' 但遇到 '}'");
    printf("\n");
    
    // 演示3：警告
    printf("【演示3】警告 - 未使用的变量\n");
    printf("--------------------------------------\n");
    report_warning(er, "example.c", 8, 9,
                   "变量 'unused_var' 已声明但从未使用");
    printf("\n");
    
    // 演示4：格式化输出
    printf("【演示4】格式化错误信息\n");
    printf("--------------------------------------\n");
    const char *expected_type = "int";
    const char *actual_type = "float";
    report_error(er, "example.c", 20, 15, ERROR_SEMANTIC,
                 "函数 'foo' 期望参数类型为 '%s'，但传入了 '%s'",
                 expected_type, actual_type);
    printf("\n");
    
    // 演示5：提示信息
    printf("【演示5】提示信息\n");
    printf("--------------------------------------\n");
    report_note(er, "建议: 使用显式类型转换，例如 (int)3.14");
    printf("\n");
    
    // 演示6：词法错误
    printf("【演示6】词法错误 - 非法字符\n");
    printf("--------------------------------------\n");
    report_error(er, "example.c", 3, 5, ERROR_LEXICAL,
                 "非法字符 '@' 在标识符中");
    printf("\n");
    
    // 演示7：多个连续警告
    printf("【演示7】多个警告\n");
    printf("--------------------------------------\n");
    report_warning(er, "example.c", 15, 5, "隐式类型转换: float → int");
    report_warning(er, "example.c", 16, 5, "比较浮点数相等可能不精确");
    report_warning(er, "example.c", 18, 10, "函数 'bar' 可能返回未初始化的值");
    printf("\n");
    
    // 演示8：错误统计
    printf("【演示8】编译统计\n");
    printf("--------------------------------------\n");
    print_error_summary(er);
    printf("\n");
    
    // 演示9：禁用彩色输出
    printf("【演示9】无彩色输出模式\n");
    printf("--------------------------------------\n");
    error_reporter_set_color(er, false);
    report_error(er, "example.c", 25, 10, ERROR_SEMANTIC,
                 "未定义的变量 'undefined_var'");
    report_warning(er, "example.c", 26, 5,
                   "条件表达式始终为真");
    printf("\n");
    
    // 重新启用彩色
    error_reporter_set_color(er, true);
    
    // 最终统计
    printf("【最终统计】\n");
    printf("--------------------------------------\n");
    print_error_summary(er);
    
    // 清理
    error_reporter_destroy(er);
    
    printf("\n演示完成！\n");
    return 0;
}

