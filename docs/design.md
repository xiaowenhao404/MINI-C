# Mini-C 编译器设计文档

## 1. 项目概述

**项目名称**: Mini-C 编译器  
**项目目标**: 构建一个功能完备的简易C语言编译器，支持实型数据、数组、指针、函数调用、结构体等高级特性，并实现代码优化  
**目标平台**: x86-64 Linux/WSL  
**开发语言**: C/C++, Python  

## 2. 总体架构

### 2.1 编译流程

```
源代码(.c) 
    ↓
词法分析 (Flex) → Token流
    ↓
语法分析 (Bison) → AST
    ↓
语义分析 → 类型标注的AST
    ↓
中间代码生成 → 四元式IR
    ↓
代码优化 → 优化后的IR
    ↓
汇编代码生成 → x86-64汇编
    ↓
汇编和链接 → 可执行文件
```

### 2.2 模块划分

| 模块 | 功能 | 实现位置 |
|------|------|---------|
| 词法分析 | Token识别 | `src/frontend/lex.l` |
| 语法分析 | AST构建 | `src/frontend/yacc.y` |
| 语义分析 | 类型检查、符号表 | `src/semantic/` |
| 中间代码 | IR生成 | `src/ir/` |
| 代码优化 | 常量折叠、DCE、活性分析 | `src/optimization/` |
| 代码生成 | x86-64汇编 | `src/codegen/` |
| 工具模块 | 错误报告、内存管理 | `src/utils/` |

## 3. 类型系统设计

### 3.1 支持的类型

- **基本类型**: void, int, float, char
- **派生类型**: 数组、指针、结构体、函数

### 3.2 类型定义

```c
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_FUNCTION
} TypeKind;

typedef struct Type {
    TypeKind kind;
    int size;
    struct Type *base;
    int array_len;
    // ... 其他字段
} Type;
```

### 3.3 类型转换规则

- int → float: 隐式转换
- float → int: 需要显式转换
- char → int: 隐式提升

## 4. 符号表设计

### 4.1 数据结构

采用**链式哈希表 + 作用域栈**结构：

```c
typedef struct Symbol {
    char *name;
    Type *type;
    int scope_level;
    int offset;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol **buckets;
    int size;
    int scope_level;
} SymbolTable;
```

### 4.2 作用域管理

- 全局作用域：scope_level = 0
- 函数作用域：scope_level = 1
- 块作用域：scope_level >= 2

## 5. 中间代码设计

### 5.1 四元式格式

```
(op, arg1, arg2, result)
```

示例：
```
ADD, a, b, t0      // t0 = a + b
JZ, t0, _, L1      // if t0 == 0 goto L1
CALL, func, n, t1  // t1 = func(n个参数)
```

### 5.2 指令集

- 算术运算：ADD, SUB, MUL, DIV, MOD
- 浮点运算：FADD, FSUB, FMUL, FDIV
- 类型转换：I2F, F2I
- 控制流：LABEL, GOTO, IF_FALSE
- 函数调用：PARAM, CALL, RETURN

## 6. 代码优化设计

### 6.1 常量折叠

编译期计算常量表达式：
- `3 + 4 * 5` → `23`
- `2.0 * 3.14` → `6.28`

### 6.2 死代码消除

移除永不执行的代码：
- `if(0) { ... }` → 删除
- `while(0) { ... }` → 删除

### 6.3 活性分析（2.0版本）

- 构建控制流图（CFG）
- 计算活跃变量集合
- 栈槽复用优化

## 7. 代码生成设计

### 7.1 目标架构

- **架构**: x86-64
- **调用约定**: System V AMD64 ABI
- **指令集**: 通用指令 + SSE（浮点）

### 7.2 寄存器分配

- 整数：rax, rbx, rcx, rdx
- 浮点：xmm0-xmm7
- 栈帧：rbp, rsp

### 7.3 栈帧布局

```
高地址
+-----------------+
| 返回地址        |
+-----------------+
| 保存的rbp       | <- rbp
+-----------------+
| 局部变量1       |
+-----------------+
| 局部变量2       |
+-----------------+
| ...             |
+-----------------+ <- rsp
低地址
```

## 8. 错误处理设计

### 8.1 错误类型

- 词法错误：非法字符
- 语法错误：语法不匹配
- 语义错误：类型不匹配、未定义变量等

### 8.2 错误报告格式

```
test.c:5:10: error: type mismatch
    int a = 3.14;
            ^~~~
```

## 9. 测试策略

### 9.1 单元测试

- 每个模块独立测试
- 覆盖正常情况和边界情况

### 9.2 集成测试

- 完整编译流程测试
- 运行生成的可执行文件验证

### 9.3 回归测试

- 代码修改后重新运行所有测试
- 确保新功能不破坏旧功能

## 10. 版本规划

### 1.0版本
- 基本类型支持（int, float, char）
- 基础优化（常量折叠、死代码消除）
- 完善的错误报告

### 2.0版本
- 数组、指针支持
- 函数定义和调用
- 结构体支持
- 活性分析优化

### 3.0版本
- Web可视化界面
- FIRST/FOLLOW集计算
- 编译过程可视化

---

**文档版本**: v1.0  
**最后更新**: 2025-12-16  
**维护者**: Mini-C 开发团队

