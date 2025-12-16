# Mini-C 错误处理系统使用指南

## 概述

`src/utils/error.h` 和 `error.c` 提供了统一的错误和警告报告接口，帮助编译器各模块输出格式化、易读的错误信息。

## 核心特性

### 1. 统一的错误格式
```
filename:line:column: error: message
```

示例：
```
test.c:5:10: 错误: 类型不匹配: 无法将 'float' 赋值给 'int'
```

### 2. 错误分类
- **词法错误**（`ERROR_LEXICAL`）：非法字符、未闭合字符串等
- **语法错误**（`ERROR_SYNTAX`）：解析失败、括号不匹配等
- **语义错误**（`ERROR_SEMANTIC`）：类型不匹配、未定义变量等
- **内部错误**（`ERROR_INTERNAL`）：编译器 bug、内存分配失败等

### 3. 严重级别
- **提示**（`SEVERITY_NOTE`）：额外信息
- **警告**（`SEVERITY_WARNING`）：潜在问题（不阻止编译）
- **错误**（`SEVERITY_ERROR`）：必须修复的问题
- **致命错误**（`SEVERITY_FATAL`）：立即终止编译

### 4. 彩色终端输出
- 错误：红色
- 警告：黄色
- 提示：青色
- 成功：绿色

### 5. 错误计数和阈值控制
- 自动统计错误和警告数量
- 超过阈值后停止编译
- 输出详细的统计信息

## 快速开始

### 创建错误报告器

```c
#include "utils/error.h"

// 创建错误报告器（最多 100 个错误）
ErrorReporter *er = error_reporter_create(100);

// 设置当前文件
error_reporter_set_file(er, "main.c");
```

### 报告错误

```c
// 基本错误报告
report_error(er, "main.c", 10, 5, ERROR_SEMANTIC,
             "未定义的变量 'x'");

// 格式化错误消息
report_error(er, "main.c", 15, 20, ERROR_SEMANTIC,
             "类型不匹配: 期望 '%s' 但得到 '%s'",
             "int", "float");
```

### 报告警告

```c
report_warning(er, "main.c", 8, 10,
               "变量 'unused' 已声明但从未使用");
```

### 检查是否应停止编译

```c
if (should_stop_compilation(er)) {
    fprintf(stderr, "错误过多，停止编译\n");
    return -1;
}
```

### 输出统计信息

```c
// 打印错误和警告统计
print_error_summary(er);

// 清理
error_reporter_destroy(er);
```

## 详细 API 文档

### 错误报告器管理

#### `error_reporter_create`
```c
ErrorReporter* error_reporter_create(int max_errors);
```
创建错误报告器。
- **参数**：`max_errors` - 最大错误数，0 表示无限制
- **返回**：错误报告器指针

#### `error_reporter_destroy`
```c
void error_reporter_destroy(ErrorReporter *er);
```
销毁错误报告器，释放内存。

#### `error_reporter_set_file`
```c
void error_reporter_set_file(ErrorReporter *er, const char *filename);
```
设置当前处理的文件名。

### 错误报告函数

#### `report_error`
```c
void report_error(ErrorReporter *er,
                  const char *filename,
                  int line,
                  int column,
                  ErrorType type,
                  const char *format, ...);
```
报告错误。
- **filename**：文件名
- **line**：行号（从 1 开始）
- **column**：列号（从 1 开始，0 表示不显示列号）
- **type**：错误类型
- **format**：格式化字符串（类似 printf）

#### `report_warning`
```c
void report_warning(ErrorReporter *er,
                    const char *filename,
                    int line,
                    int column,
                    const char *format, ...);
```
报告警告。

#### `report_note`
```c
void report_note(ErrorReporter *er, const char *format, ...);
```
报告提示信息。

#### `report_fatal`
```c
void report_fatal(ErrorReporter *er, const char *format, ...);
```
报告致命错误并立即终止程序。

### 辅助函数

#### `should_stop_compilation`
```c
bool should_stop_compilation(ErrorReporter *er);
```
检查错误数是否超过阈值。

#### `print_error_summary`
```c
void print_error_summary(ErrorReporter *er);
```
打印错误和警告统计信息。

#### `error_reporter_set_color`
```c
void error_reporter_set_color(ErrorReporter *er, bool enable);
```
启用/禁用彩色输出。

#### `error_reporter_set_show_source`
```c
void error_reporter_set_show_source(ErrorReporter *er, bool enable);
```
启用/禁用源代码上下文显示（预留功能）。

## 便捷宏

为了简化错误报告，提供了几个便捷宏：

### `SEMANTIC_ERROR`
```c
#define SEMANTIC_ERROR(er, node, ...) \
    report_error(er, (er)->current_file, (node)->line, 0, ERROR_SEMANTIC, __VA_ARGS__)
```

**使用示例**：
```c
if (!type_compatible(lhs, rhs)) {
    SEMANTIC_ERROR(er, node, "类型不兼容: '%s' 和 '%s'",
                   type_to_string(lhs), type_to_string(rhs));
}
```

### `SYNTAX_ERROR`
```c
#define SYNTAX_ERROR(er, line, col, ...) \
    report_error(er, (er)->current_file, line, col, ERROR_SYNTAX, __VA_ARGS__)
```

### `WARNING`
```c
#define WARNING(er, node, ...) \
    report_warning(er, (er)->current_file, (node)->line, 0, __VA_ARGS__)
```

## 集成到模块

### 语义分析器集成

```c
// 在 semantic_analyzer.h 中
typedef struct SemanticAnalyzer {
    SymbolTable *symbol_table;
    ErrorReporter *error_reporter;  // 添加错误报告器
    // ...
} SemanticAnalyzer;

// 在 semantic_analyzer.c 中
SemanticAnalyzer* semantic_analyzer_create() {
    SemanticAnalyzer *sa = malloc(sizeof(SemanticAnalyzer));
    sa->symbol_table = symbol_table_create();
    sa->error_reporter = error_reporter_create(100);  // 创建
    error_reporter_set_file(sa->error_reporter, "input.c");
    return sa;
}

void analyze_expression(SemanticAnalyzer *sa, Tree *node) {
    // 检查类型
    if (!type_compatible(lhs_type, rhs_type)) {
        SEMANTIC_ERROR(sa->error_reporter, node,
                      "类型不匹配: 无法将 '%s' 赋值给 '%s'",
                      type_to_string(rhs_type),
                      type_to_string(lhs_type));
        return;
    }
    
    // 检查是否应停止
    if (should_stop_compilation(sa->error_reporter)) {
        return;
    }
}

void semantic_analyzer_destroy(SemanticAnalyzer *sa) {
    // 输出统计
    print_error_summary(sa->error_reporter);
    
    // 清理
    error_reporter_destroy(sa->error_reporter);
    symbol_table_destroy(sa->symbol_table);
    free(sa);
}
```

### IR 生成器集成

```c
typedef struct IRBuilder {
    IRInstruction **instructions;
    ErrorReporter *error_reporter;  // 添加
    // ...
} IRBuilder;

IRBuilder* ir_builder_create() {
    IRBuilder *builder = malloc(sizeof(IRBuilder));
    builder->error_reporter = error_reporter_create(50);
    return builder;
}

void generate_ir(IRBuilder *builder, Tree *node) {
    if (node == NULL) {
        report_error(builder->error_reporter, NULL, 0, 0,
                    ERROR_INTERNAL, "遇到空节点");
        return;
    }
    // ...
}
```

## 输出示例

### 成功编译（无错误）
```
编译成功，无错误无警告
```

### 有警告的编译
```
main.c:10:5: 警告: 变量 'x' 已声明但从未使用
main.c:15:10: 警告: 隐式类型转换: float → int

========== 编译统计 ==========
错误: 0
警告: 2
==============================
编译成功（有警告）
```

### 编译失败
```
main.c:5:10: 错误: 未定义的变量 'foo'
main.c:8:15: 错误: 类型不匹配: 期望 'int' 但得到 'float'
main.c:12:1: 错误: 缺少返回语句

========== 编译统计 ==========
错误: 3
警告: 0
==============================
编译失败
```

### 错误过多
```
test.c:1:1: 错误: 语法错误
test.c:2:1: 错误: 语法错误
...
test.c:100:1: 错误: 语法错误
注意: 错误数过多，停止编译
```

## 彩色输出控制

### 检测终端支持
```c
#include <unistd.h>

bool is_terminal() {
    return isatty(fileno(stderr));
}

// 只在终端中启用彩色
ErrorReporter *er = error_reporter_create(100);
error_reporter_set_color(er, is_terminal());
```

### 环境变量控制
```c
#include <stdlib.h>

bool should_use_color() {
    const char *no_color = getenv("NO_COLOR");
    if (no_color && *no_color) {
        return false;  // NO_COLOR 环境变量已设置
    }
    return isatty(fileno(stderr));
}
```

## 最佳实践

### 1. 错误消息要清晰
❌ 不好：
```c
report_error(er, file, line, col, ERROR_SEMANTIC, "错误");
```

✅ 好：
```c
report_error(er, file, line, col, ERROR_SEMANTIC,
             "类型不匹配: 无法将 '%s' 赋值给 '%s'",
             type_to_string(src_type),
             type_to_string(dst_type));
```

### 2. 提供上下文信息
```c
report_error(er, file, line, col, ERROR_SEMANTIC,
             "函数 '%s' 未定义", func_name);
report_note(er, "提示: 是否忘记包含头文件？");
```

### 3. 合理设置错误阈值
```c
// 对于交互式编译，使用较小的阈值
ErrorReporter *er = error_reporter_create(10);

// 对于批处理，使用较大的阈值
ErrorReporter *er = error_reporter_create(100);
```

### 4. 及时检查是否应停止
```c
void analyze_statements(SemanticAnalyzer *sa, Tree *stmts) {
    for (int i = 0; i < stmt_count; i++) {
        analyze_statement(sa, stmts[i]);
        
        // 每分析一条语句后检查
        if (should_stop_compilation(sa->error_reporter)) {
            return;
        }
    }
}
```

### 5. 始终输出统计信息
```c
int main() {
    ErrorReporter *er = error_reporter_create(100);
    
    // ... 编译过程 ...
    
    // 最后输出统计
    print_error_summary(er);
    
    // 根据错误数决定退出码
    int exit_code = er->error_count > 0 ? 1 : 0;
    
    error_reporter_destroy(er);
    return exit_code;
}
```

## 扩展功能（TODO）

- [ ] 源代码上下文显示（显示错误所在行和标记位置）
- [ ] 错误修复建议（"你是不是想写..."）
- [ ] 多语言支持（英文/中文切换）
- [ ] 错误日志文件输出
- [ ] 集成到 IDE（JSON 格式输出）
- [ ] 错误恢复策略（尝试继续编译）

## 参考资料

- [GCC 诊断消息格式](https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Message-Formatting-Options.html)
- [Clang 诊断系统](https://clang.llvm.org/docs/InternalsManual.html#the-diagnostics-subsystem)
- [ANSI 转义码](https://en.wikipedia.org/wiki/ANSI_escape_code)

---

**版本**: 1.0  
**最后更新**: 2025-12-16  
**作者**: Mini-C 开发团队

