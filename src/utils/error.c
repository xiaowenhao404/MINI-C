/**
 * Mini-C 编译器 - 统一错误处理系统实现
 * 
 * 文件: error.c
 * 描述: 错误和警告报告的具体实现
 * 作者: Mini-C 开发团队
 * 版本: 1.0
 */

#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ==================== 错误报告器管理 ==================== */

/**
 * 创建错误报告器
 */
ErrorReporter* error_reporter_create(int max_errors) {
    ErrorReporter *er = (ErrorReporter*)malloc(sizeof(ErrorReporter));
    if (!er) {
        fprintf(stderr, "Fatal: 无法分配内存给错误报告器\n");
        exit(1);
    }
    
    er->error_count = 0;
    er->warning_count = 0;
    er->max_errors = max_errors > 0 ? max_errors : 100; // 默认最大100个错误
    er->output = stderr;
    er->use_color = true;  // 默认启用彩色输出
    er->show_source = true; // 默认显示源代码
    er->current_file = NULL;
    
    return er;
}

/**
 * 销毁错误报告器
 */
void error_reporter_destroy(ErrorReporter *er) {
    if (er) {
        free(er);
    }
}

/**
 * 设置当前文件
 */
void error_reporter_set_file(ErrorReporter *er, const char *filename) {
    if (er) {
        er->current_file = filename;
    }
}

/**
 * 启用/禁用彩色输出
 */
void error_reporter_set_color(ErrorReporter *er, bool enable) {
    if (er) {
        er->use_color = enable;
    }
}

/**
 * 启用/禁用源代码显示
 */
void error_reporter_set_show_source(ErrorReporter *er, bool enable) {
    if (er) {
        er->show_source = enable;
    }
}

/* ==================== 辅助函数 ==================== */

/**
 * 获取错误类型的字符串
 */
const char* error_type_to_string(ErrorType type) {
    switch (type) {
        case ERROR_LEXICAL:  return "词法错误";
        case ERROR_SYNTAX:   return "语法错误";
        case ERROR_SEMANTIC: return "语义错误";
        case ERROR_INTERNAL: return "内部错误";
        default:             return "未知错误";
    }
}

/**
 * 获取彩色前缀（如果启用）
 */
static const char* get_color_prefix(ErrorReporter *er, ErrorSeverity severity) {
    if (!er->use_color) {
        return "";
    }
    
    switch (severity) {
        case SEVERITY_NOTE:    return COLOR_CYAN;
        case SEVERITY_WARNING: return COLOR_YELLOW;
        case SEVERITY_ERROR:   return COLOR_RED;
        case SEVERITY_FATAL:   return COLOR_BOLD COLOR_RED;
        default:               return "";
    }
}

/**
 * 获取严重级别的字符串
 */
static const char* severity_to_string(ErrorSeverity severity) {
    switch (severity) {
        case SEVERITY_NOTE:    return "提示";
        case SEVERITY_WARNING: return "警告";
        case SEVERITY_ERROR:   return "错误";
        case SEVERITY_FATAL:   return "致命错误";
        default:               return "消息";
    }
}

/* ==================== 核心报告函数 ==================== */

/**
 * 通用错误报告函数
 */
static void report_message(ErrorReporter *er,
                          const char *filename,
                          int line,
                          int column,
                          ErrorSeverity severity,
                          const char *format,
                          va_list args) {
    if (!er) {
        return;
    }
    
    // 获取彩色前缀
    const char *color_prefix = get_color_prefix(er, severity);
    const char *color_reset = er->use_color ? COLOR_RESET : "";
    
    // 输出位置信息：filename:line:column:
    if (filename && line > 0) {
        if (column > 0) {
            fprintf(er->output, "%s%s:%d:%d: %s%s%s: ",
                    er->use_color ? COLOR_BOLD : "",
                    filename, line, column,
                    color_prefix,
                    severity_to_string(severity),
                    color_reset);
        } else {
            fprintf(er->output, "%s%s:%d: %s%s%s: ",
                    er->use_color ? COLOR_BOLD : "",
                    filename, line,
                    color_prefix,
                    severity_to_string(severity),
                    color_reset);
        }
    } else {
        fprintf(er->output, "%s%s%s: ",
                color_prefix,
                severity_to_string(severity),
                color_reset);
    }
    
    // 输出错误消息（可变参数）
    vfprintf(er->output, format, args);
    fprintf(er->output, "\n");
    
    // TODO: 显示源代码上下文（如果启用）
    // if (er->show_source && filename && line > 0) {
    //     show_source_context(er, filename, line, column);
    // }
}

/**
 * 报告错误
 */
void report_error(ErrorReporter *er,
                  const char *filename,
                  int line,
                  int column,
                  ErrorType type,
                  const char *format, ...) {
    if (!er) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    report_message(er, filename, line, column, SEVERITY_ERROR, format, args);
    va_end(args);
    
    er->error_count++;
    
    // 如果错误数超过阈值，输出提示
    if (er->error_count >= er->max_errors) {
        fprintf(er->output, "%s注意: 错误数过多，停止编译%s\n",
                er->use_color ? COLOR_YELLOW : "",
                er->use_color ? COLOR_RESET : "");
    }
}

/**
 * 报告警告
 */
void report_warning(ErrorReporter *er,
                    const char *filename,
                    int line,
                    int column,
                    const char *format, ...) {
    if (!er) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    report_message(er, filename, line, column, SEVERITY_WARNING, format, args);
    va_end(args);
    
    er->warning_count++;
}

/**
 * 报告提示信息
 */
void report_note(ErrorReporter *er, const char *format, ...) {
    if (!er) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    report_message(er, NULL, 0, 0, SEVERITY_NOTE, format, args);
    va_end(args);
}

/**
 * 报告致命错误
 */
void report_fatal(ErrorReporter *er, const char *format, ...) {
    if (!er) {
        er = error_reporter_create(0);
    }
    
    va_list args;
    va_start(args, format);
    report_message(er, NULL, 0, 0, SEVERITY_FATAL, format, args);
    va_end(args);
    
    // 致命错误，立即退出
    exit(1);
}

/**
 * 检查是否应该停止编译
 */
bool should_stop_compilation(ErrorReporter *er) {
    if (!er) {
        return false;
    }
    
    return er->error_count >= er->max_errors;
}

/**
 * 打印错误统计信息
 */
void print_error_summary(ErrorReporter *er) {
    if (!er) {
        return;
    }
    
    // 如果没有错误和警告，不输出
    if (er->error_count == 0 && er->warning_count == 0) {
        fprintf(er->output, "%s编译成功，无错误无警告%s\n",
                er->use_color ? COLOR_GREEN : "",
                er->use_color ? COLOR_RESET : "");
        return;
    }
    
    fprintf(er->output, "\n");
    fprintf(er->output, "========== 编译统计 ==========\n");
    
    // 输出错误数
    if (er->error_count > 0) {
        fprintf(er->output, "%s错误: %d%s\n",
                er->use_color ? COLOR_RED : "",
                er->error_count,
                er->use_color ? COLOR_RESET : "");
    } else {
        fprintf(er->output, "错误: 0\n");
    }
    
    // 输出警告数
    if (er->warning_count > 0) {
        fprintf(er->output, "%s警告: %d%s\n",
                er->use_color ? COLOR_YELLOW : "",
                er->warning_count,
                er->use_color ? COLOR_RESET : "");
    } else {
        fprintf(er->output, "警告: 0\n");
    }
    
    fprintf(er->output, "==============================\n");
    
    // 最终结果
    if (er->error_count > 0) {
        fprintf(er->output, "%s编译失败%s\n",
                er->use_color ? COLOR_RED : "",
                er->use_color ? COLOR_RESET : "");
    } else if (er->warning_count > 0) {
        fprintf(er->output, "%s编译成功（有警告）%s\n",
                er->use_color ? COLOR_YELLOW : "",
                er->use_color ? COLOR_RESET : "");
    } else {
        fprintf(er->output, "%s编译成功%s\n",
                er->use_color ? COLOR_GREEN : "",
                er->use_color ? COLOR_RESET : "");
    }
}

