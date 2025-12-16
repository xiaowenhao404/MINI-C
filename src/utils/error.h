/**
 * Mini-C 编译器 - 统一错误处理系统
 * 
 * 文件: error.h
 * 描述: 提供统一的错误和警告报告接口
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 * 
 * 功能:
 * - 格式化的错误和警告输出
 * - 支持文件名、行号、列号定位
 * - 彩色终端输出（可选）
 * - 错误计数和阈值控制
 * - 源代码上下文显示
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdbool.h>

/* ==================== 错误类型定义 ==================== */

/**
 * 错误类型枚举
 */
typedef enum {
    ERROR_LEXICAL,      // 词法错误（非法字符、未闭合字符串等）
    ERROR_SYNTAX,       // 语法错误（解析失败、括号不匹配等）
    ERROR_SEMANTIC,     // 语义错误（类型不匹配、未定义变量等）
    ERROR_INTERNAL      // 内部错误（编译器 bug、内存分配失败等）
} ErrorType;

/**
 * 错误严重级别
 */
typedef enum {
    SEVERITY_NOTE,      // 提示信息
    SEVERITY_WARNING,   // 警告（不影响编译）
    SEVERITY_ERROR,     // 错误（阻止代码生成）
    SEVERITY_FATAL      // 致命错误（立即终止编译）
} ErrorSeverity;

/* ==================== 错误报告器 ==================== */

/**
 * 错误报告器结构
 */
typedef struct ErrorReporter {
    int error_count;        // 错误计数
    int warning_count;      // 警告计数
    int max_errors;         // 最大错误数（超过后停止编译）
    FILE *output;           // 输出流（通常是 stderr）
    bool use_color;         // 是否使用彩色输出
    bool show_source;       // 是否显示源代码上下文
    const char *current_file; // 当前处理的文件名
} ErrorReporter;

/* ==================== 核心函数 ==================== */

/**
 * 创建错误报告器
 * 
 * @param max_errors 最大错误数，0 表示无限制
 * @return 错误报告器指针
 */
ErrorReporter* error_reporter_create(int max_errors);

/**
 * 销毁错误报告器
 * 
 * @param er 错误报告器指针
 */
void error_reporter_destroy(ErrorReporter *er);

/**
 * 设置当前处理的文件
 * 
 * @param er 错误报告器指针
 * @param filename 文件名
 */
void error_reporter_set_file(ErrorReporter *er, const char *filename);

/**
 * 报告错误
 * 
 * @param er 错误报告器
 * @param filename 文件名
 * @param line 行号（从 1 开始）
 * @param column 列号（从 1 开始）
 * @param type 错误类型
 * @param format 格式化字符串（可变参数）
 */
void report_error(ErrorReporter *er, 
                  const char *filename, 
                  int line, 
                  int column,
                  ErrorType type,
                  const char *format, ...);

/**
 * 报告警告
 * 
 * @param er 错误报告器
 * @param filename 文件名
 * @param line 行号
 * @param column 列号
 * @param format 格式化字符串（可变参数）
 */
void report_warning(ErrorReporter *er, 
                    const char *filename, 
                    int line,
                    int column,
                    const char *format, ...);

/**
 * 报告提示信息
 * 
 * @param er 错误报告器
 * @param format 格式化字符串（可变参数）
 */
void report_note(ErrorReporter *er, const char *format, ...);

/**
 * 报告致命错误（立即终止编译）
 * 
 * @param er 错误报告器
 * @param format 格式化字符串（可变参数）
 */
void report_fatal(ErrorReporter *er, const char *format, ...);

/**
 * 检查是否应该停止编译
 * 
 * @param er 错误报告器
 * @return 如果错误数超过阈值返回 true
 */
bool should_stop_compilation(ErrorReporter *er);

/**
 * 打印错误统计信息
 * 
 * @param er 错误报告器
 */
void print_error_summary(ErrorReporter *er);

/* ==================== 辅助函数 ==================== */

/**
 * 获取错误类型的字符串表示
 * 
 * @param type 错误类型
 * @return 类型字符串
 */
const char* error_type_to_string(ErrorType type);

/**
 * 启用/禁用彩色输出
 * 
 * @param er 错误报告器
 * @param enable 是否启用
 */
void error_reporter_set_color(ErrorReporter *er, bool enable);

/**
 * 启用/禁用源代码显示
 * 
 * @param er 错误报告器
 * @param enable 是否启用
 */
void error_reporter_set_show_source(ErrorReporter *er, bool enable);

/* ==================== 彩色输出宏 ==================== */

#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"

/* ==================== 便捷宏 ==================== */

/**
 * 语义错误报告宏（简化调用）
 */
#define SEMANTIC_ERROR(er, node, ...) \
    report_error(er, (er)->current_file, (node)->line, 0, ERROR_SEMANTIC, __VA_ARGS__)

/**
 * 语法错误报告宏
 */
#define SYNTAX_ERROR(er, line, col, ...) \
    report_error(er, (er)->current_file, line, col, ERROR_SYNTAX, __VA_ARGS__)

/**
 * 警告报告宏
 */
#define WARNING(er, node, ...) \
    report_warning(er, (er)->current_file, (node)->line, 0, __VA_ARGS__)

#endif /* ERROR_H */

