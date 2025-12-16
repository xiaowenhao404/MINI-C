---
name: Mini-C 编译器开发计划
overview: 构建一个功能完备的简易C语言编译器，支持实型数据、数组、指针、函数调用、结构体等高级特性，并实现常量折叠、死代码消除、活性分析等优化技术。项目采用部分重构策略，保留Flex/Bison前端，重写语义分析和代码生成模块，最终提供Web可视化界面展示编译过程。
todos:
  - id: readme
    content: 创建项目README.md蓝图文档
    status: completed
  - id: devplan
    content: 创建详细的DEV_PLAN.md开发计划
    status: completed
    dependencies:
      - readme
---

# Mini-C 编译器综合开发计划

## 项目架构概览

本项目将构建一个教学级但功能完备的C语言子集编译器，采用**多遍扫描架构**：

```mermaid
graph LR
    A[源代码.c] --> B[词法分析Flex]
    B --> C[语法分析Bison]
    C --> D[语义分析]
    D --> E[中间代码IR]
    E --> F[代码优化]
    F --> G[汇编生成x86-64]
    G --> H[可执行文件]
```

### 技术栈

- **编译器核心**: C/C++ (保留Flex/Bison + 重构语义分析/优化/代码生成模块)
- **汇编生成**: Python脚本（增强版）
- **可视化界面**: Python Flask + HTML/JavaScript (CodeMirror)
- **目标平台**: x86-64 Linux/WSL

### 核心设计决策

1. **保留现有优势**: Flex/Bison成熟稳定，保留词法/语法分析框架
2. **重构关键模块**: 类型系统、符号表、中间代码、优化器需要重新设计以支持高级特性
3. **分阶段实现**: 1.0基础扩展 → 2.0高级特性 → 3.0可视化工具

## 文件结构（目标架构）

[Mini-C/](Mini-C/)

```
Mini-C/
├── src/                          # 编译器核心源码
│   ├── frontend/                 # 前端：词法+语法分析
│   │   ├── lex.l                 # 词法规则（扩展float/char/字符串/注释）
│   │   ├── yacc.y                # 语法规则（扩展函数/数组/指针/结构体）
│   │   └── frontend.h            # 前端接口
│   ├── semantic/                 # 语义分析模块（新增）
│   │   ├── type_system.h/c       # 类型系统：int/float/char/array/pointer/struct
│   │   ├── symbol_table.h/c      # 符号表：支持嵌套作用域
│   │   ├── type_check.h/c        # 类型检查与隐式转换
│   │   └── semantic_analyzer.h/c # 语义分析主模块
│   ├── ir/                       # 中间代码生成（重构）
│   │   ├── ir.h/c                # IR指令定义（四元式）
│   │   ├── ir_builder.h/c        # AST到IR的翻译
│   │   └── cfg.h/c                # 控制流图构建
│   ├── optimization/             # 代码优化模块（新增）
│   │   ├── const_folding.h/c     # 常量折叠
│   │   ├── dead_code_elim.h/c    # 死代码消除
│   │   ├── liveness_analysis.h/c # 活性分析（2.0版本）
│   │   └── optimizer.h/c         # 优化器主模块
│   ├── codegen/                  # 代码生成（重构）
│   │   ├── asm_gen.c/h           # 汇编代码生成
│   │   ├── register_alloc.h/c    # 寄存器分配
│   │   └── codegen_utils.h/c     # 代码生成辅助
│   ├── utils/                    # 工具模块
│   │   ├── error.h/c             # 错误报告（带行号和上下文）
│   │   ├── memory_pool.h/c       # 内存管理
│   │   └── utils.h/c             # 通用工具
│   └── main.c                    # 编译器入口
├── scripts/                      # 辅助脚本
│   ├── asm_generator.py          # Python汇编生成脚本（增强版）
│   └── test_runner.py            # 自动化测试运行器
├── web/                          # Web可视化界面（3.0版本）
│   ├── app.py                    # Flask后端
│   ├── static/                   # 前端资源
│   │   ├── css/
│   │   └── js/
│   └── templates/                # HTML模板
├── tests/                        # 测试用例
│   ├── lexer/                    # 词法分析测试
│   ├── parser/                   # 语法分析测试
│   ├── semantic/                 # 语义分析测试
│   ├── integration/              # 集成测试
│   └── samples/                  # 完整程序样例
├── docs/                         # 文档
│   ├── design.md                 # 设计文档
│   ├── user_guide.md             # 使用指南
│   └── api.md                    # API文档
├── Makefile                      # 构建脚本
├── README.md                     # 项目蓝图
└── DEV_PLAN.md                   # 开发计划
```

## 版本规划总览

### 1.0 - 核心编译器增强版（优先）

- **目标**: 扩展类型系统、完善语义分析、实现基础优化
- **核心功能**: float/char类型、多变量声明与初始化、四则混合运算、注释、基本输入输出、if-else、错误报告
- **优化**: 常量折叠、死代码消除
- **测试**: 完善的测试用例和验收标准

### 2.0 - 高级特性版

- **目标**: 支持数组、指针、函数、结构体
- **优化**: 活性分析与栈槽复用
- **增强**: 更复杂的类型检查、作用域管理

### 3.0 - 可视化工具版

- **目标**: Web界面、FIRST/FOLLOW集计算、编译过程可视化
- **增强**: 语法高亮、AST可视化、IR可视化

## 关键技术要点

### 类型系统设计

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
    int size;                    // 类型大小
    struct Type *base;           // 数组/指针的基类型
    int array_len;               // 数组长度
    struct StructMember *members; // 结构体成员
} Type;
```

### 符号表设计

- 链式哈希表 + 作用域栈
- 支持变量、函数、类型定义
- 支持嵌套作用域的符号查找

### 中间代码格式（四元式）

```
(op, arg1, arg2, result)
示例：
  ADD, t0, t1, t2     // t2 = t0 + t1
  JZ, t0, _, L1       // if t0 == 0 goto L1
  CALL, func, n, t0   // t0 = func(n个参数)
```

### 优化策略

1. **常量折叠**: AST遍历，编译期计算常量表达式
2. **死代码消除**: 检测不可达代码块（if(0)等）
3. **活性分析**: 构建CFG→数据流分析→变量生命周期→栈槽复用

## 关键里程碑

- **M1**: 完成类型系统重构（支持int/float/char）
- **M2**: 完成符号表和语义分析器
- **M3**: 完成IR生成和基础优化
- **M4**: 完成代码生成和测试框架
- **M5**: 完成1.0版本验收

## 风险与挑战

1. **类型系统复杂性**: float需要SSE指令，类型转换规则复杂
2. **符号表作用域**: 嵌套作用域的正确处理
3. **活性分析难度**: 2.0版本的核心挑战，需要CFG和数据流算法
4. **测试覆盖度**: 需要大量测试用例保证正确性

## 成功标准

- 通过所有功能测试用例
- 能编译并正确执行包含所有支持特性的综合程序
- 优化效果可量化（栈空间减少、常量计算减少）
- 代码质量：清晰的模块化、充分的注释、规范的错误处理