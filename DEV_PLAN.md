# Mini-C 编译器开发计划 (DEV_PLAN)

## 📋 版本规划总览

本开发计划采用**敏捷迭代**方式，按照 1.0、2.0、3.0 版本递进开发，每个版本都是可交付的完整功能集。

### 版本里程碑

| 版本 | 名称 | 主要目标 | 预计工作量 | 状态 |
|------|------|---------|-----------|------|
| **1.0** | 核心编译器增强版 | 扩展类型系统、完善语义分析、实现基础优化 | 4-6周 | 🔄 计划中 |
| **2.0** | 高级特性版 | 支持数组、指针、函数、结构体、活性分析 | 3-4周 | ⏸️ 待启动 |
| **3.0** | 可视化工具版 | Web界面、FIRST/FOLLOW集、编译过程可视化 | 2-3周 | ⏸️ 待启动 |

---

## 🎯 版本 1.0 - 核心编译器增强版

### 版本目标

在现有 `c-complier-master` 基础上，保留 Flex/Bison 前端，重构语义分析和代码生成模块，实现：
- 多种数据类型支持（int, float, char）
- 完善的类型检查和隐式转换
- 基础代码优化（常量折叠、死代码消除）
- 详细的错误报告系统
- 完整的测试框架

### 版本任务列表

---

## TASK001: 项目初始化与环境搭建

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0 (最高)  
**预计时间**: 1天  

### 任务目标

建立规范的项目结构，配置开发环境，设置版本控制和构建系统。

### 子任务清单

- [ ] **1.1 创建目录结构**
  - 创建 `src/`、`tests/`、`docs/`、`scripts/` 等目录
  - 迁移现有代码到新结构
  - 保留 `c-complier-master/` 作为参考

- [ ] **1.2 配置Git仓库**
  - 初始化Git仓库：`git init`
  - 创建 `.gitignore` 文件（忽略编译产物、临时文件）
  - 配置远程仓库连接到GitHub
  - 创建 `develop` 分支用于开发

- [ ] **1.3 编写Makefile**
  - 定义编译目标：`all`, `clean`, `test`
  - 支持分模块编译
  - 添加调试版本（`-g -O0`）和发布版本（`-O2`）

- [ ] **1.4 环境依赖检查脚本**
  - 创建 `scripts/check_env.sh`
  - 检查 flex、bison、gcc、nasm、python3 是否安装
  - 输出版本信息

- [ ] **1.5 文档初始化**
  - 确认 README.md 和 DEV_PLAN.md 完整
  - 创建 `docs/design.md` 框架
  - 创建 `docs/user_guide.md` 框架

### AI助手提示词

```
请帮我完成 Mini-C 编译器项目的初始化工作：

1. 在当前目录下创建以下目录结构：
   - src/frontend, src/semantic, src/ir, src/optimization, src/codegen, src/utils
   - tests/lexer, tests/parser, tests/semantic, tests/optimization, tests/integration, tests/samples
   - scripts/, docs/, output/

2. 创建 .gitignore 文件，包含以下内容：
   - 编译器生成的中间文件：*.o, *.out, *.exe, *.tab.*, lex.yy.c, compiler
   - 编译输出：output/, Lexical, Grammatical, Innercode, assembly.asm
   - Python缓存：__pycache__, *.pyc
   - IDE配置：.vscode/, .idea/

3. 编写增强版 Makefile，要求：
   - 支持 `make` 编译整个项目
   - 支持 `make file=<name>` 编译指定C文件
   - 支持 `make test` 运行测试
   - 支持 `make clean` 清理生成文件
   - 添加调试选项 `make debug` 编译带调试符号的版本

4. 创建 scripts/check_env.sh 脚本，检查必要工具是否安装。

请使用中文注释，并确保与现有的 c-complier-master 代码兼容。
```

### 验收标准

- [x] 目录结构清晰，符合设计文档
- [x] `.gitignore` 配置正确，不提交编译产物
- [x] `make` 可成功编译现有代码
- [x] `scripts/check_env.sh` 正确检测环境
- [x] 所有文档文件已创建

### 注意事项

- 保持与现有代码的兼容性，不要破坏原有功能
- Git提交信息使用中文，格式：`feat: 功能描述` 或 `fix: 修复描述`
- Makefile 使用 Tab 缩进，不要用空格

---

## TASK002: 类型系统重构

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0 (最高)  
**预计时间**: 3-4天  
**依赖**: TASK001

### 任务目标

设计并实现完整的类型系统，支持 int、float、char 三种基本类型及其派生类型（数组、指针等的基础定义，完整实现在2.0）。

### 子任务清单

- [ ] **2.1 设计类型系统数据结构**
  - 定义 `TypeKind` 枚举（VOID, INT, FLOAT, CHAR, ARRAY, POINTER, STRUCT, FUNCTION）
  - 定义 `Type` 结构体，包含 kind、size、base 等字段
  - 实现类型构造函数：`new_int_type()`, `new_float_type()`, `new_char_type()`

- [ ] **2.2 实现类型大小计算**
  - `type_size()` 函数：返回类型占用字节数
  - int: 4字节，float: 4字节，char: 1字节
  - 考虑内存对齐规则（暂时简化为紧凑排列）

- [ ] **2.3 实现类型比较和兼容性判断**
  - `type_equal()`: 判断两个类型是否相同
  - `type_compatible()`: 判断两个类型是否兼容（如 int 和 float 可隐式转换）
  - `can_cast()`: 判断是否可以进行显式类型转换

- [ ] **2.4 实现类型提升（Type Promotion）**
  - 定义类型提升规则：int → float
  - `promote_type()` 函数：返回提升后的类型
  - 用于二元运算时的类型统一

- [ ] **2.5 AST节点类型标注**
  - 为 `Tree` 结构体添加 `type` 字段
  - 实现 `annotate_type()` 函数，递归标注AST节点类型

- [ ] **2.6 单元测试**
  - 测试类型构造和大小计算
  - 测试类型比较和兼容性
  - 测试类型提升规则

### AI助手提示词

```
请帮我实现 Mini-C 编译器的类型系统模块（src/semantic/type_system.h 和 type_system.c）：

## 需求
1. 支持基本类型：int (4字节)、float (4字节)、char (1字节)、void
2. 为后续扩展预留：数组、指针、结构体、函数类型的定义（但暂不完整实现）
3. 实现类型比较、兼容性判断、类型提升

## 核心数据结构
```c
typedef enum {
    TYPE_VOID, TYPE_INT, TYPE_FLOAT, TYPE_CHAR,
    TYPE_ARRAY, TYPE_POINTER, TYPE_STRUCT, TYPE_FUNCTION
} TypeKind;

typedef struct Type {
    TypeKind kind;
    int size;                    // 类型大小（字节）
    struct Type *base;           // 数组/指针的基类型
    int array_len;               // 数组长度（1.0版本暂不实现）
    // 结构体和函数类型字段留空，2.0实现
} Type;
```

## 需要实现的函数
- `Type* new_int_type()` - 创建int类型
- `Type* new_float_type()` - 创建float类型  
- `Type* new_char_type()` - 创建char类型
- `int type_size(Type *t)` - 返回类型大小
- `bool type_equal(Type *a, Type *b)` - 判断类型是否相同
- `bool type_compatible(Type *a, Type *b)` - 判断是否兼容（可隐式转换）
- `Type* promote_type(Type *a, Type *b)` - 返回提升后的类型

## 类型提升规则
- int + int → int
- float + float → float
- int + float → float（int需要转换）
- char 运算时提升为 int

请使用中文注释，代码清晰易懂。
```

### 验收标准

- [x] `type_system.h` 和 `type_system.c` 编译通过
- [x] 所有类型构造函数正确返回类型对象
- [x] `type_size()` 对 int/float/char 返回正确值
- [x] 类型比较和兼容性函数通过单元测试
- [x] 类型提升规则符合C语言标准

### 注意事项

- 使用内存池管理 `Type` 对象，避免内存泄漏
- 为未来扩展（数组、指针）预留字段，但暂不实现复杂逻辑
- 注意 `float` 和 `int` 的大小定义一致性（都是4字节）

---

## TASK003: 符号表重构

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0 (最高)  
**预计时间**: 3天  
**依赖**: TASK002

### 任务目标

重构现有的符号表实现，支持嵌套作用域、多种符号类型（变量、函数），与新的类型系统集成。

### 子任务清单

- [ ] **3.1 设计符号表数据结构**
  - 定义 `Symbol` 结构体：name, type, scope_level, offset, is_global
  - 保留哈希表结构，但增强作用域管理
  - 定义 `SymbolTable` 结构体：buckets, size, scope_level

- [ ] **3.2 实现基本操作**
  - `symbol_table_create()`: 创建符号表
  - `symbol_insert()`: 插入符号，检查重定义
  - `symbol_lookup()`: 查找符号，支持作用域链搜索
  - `symbol_table_destroy()`: 销毁符号表

- [ ] **3.3 实现作用域管理**
  - `enter_scope()`: 进入新作用域，scope_level++
  - `exit_scope()`: 退出作用域，删除当前层符号，scope_level--
  - 查找时从内层向外层逐级搜索（作用域链）

- [ ] **3.4 集成类型系统**
  - 符号的 `type` 字段使用 `Type*` 指针
  - 插入时验证类型合法性
  - 支持类型重定义检查

- [ ] **3.5 错误处理**
  - 变量重定义错误：在同一作用域插入同名符号
  - 变量未定义错误：查找失败
  - 返回详细的错误信息（符号名、行号）

- [ ] **3.6 单元测试**
  - 测试插入和查找
  - 测试作用域嵌套
  - 测试重定义检测

### AI助手提示词

```
请帮我重构 Mini-C 编译器的符号表模块（src/semantic/symbol_table.h 和 symbol_table.c）：

## 背景
现有代码（hashMap.c）使用链式哈希表，我们需要保留这个高效结构，但增强作用域管理能力。

## 需求
1. 支持嵌套作用域（如函数内的块语句）
2. 符号包含：名称、类型（Type*）、作用域层级、栈偏移量
3. 支持变量和函数符号（1.0版本主要是变量）
4. 查找时自动搜索外层作用域

## 核心数据结构
```c
typedef struct Symbol {
    char *name;             // 符号名
    Type *type;             // 类型（来自type_system）
    int scope_level;        // 作用域层级（0=全局）
    int offset;             // 栈帧偏移量
    bool is_global;         // 是否全局变量
    struct Symbol *next;    // 哈希链表
} Symbol;

typedef struct SymbolTable {
    Symbol **buckets;       // 哈希桶
    int size;               // 桶数量（建议素数，如127）
    int scope_level;        // 当前作用域层级
} SymbolTable;
```

## 需要实现的函数
- `SymbolTable* symbol_table_create(int size)` - 创建符号表
- `Symbol* symbol_insert(SymbolTable *st, char *name, Type *type)` - 插入符号
- `Symbol* symbol_lookup(SymbolTable *st, char *name)` - 查找符号
- `void enter_scope(SymbolTable *st)` - 进入作用域
- `void exit_scope(SymbolTable *st)` - 退出作用域，删除当前层符号
- `void symbol_table_destroy(SymbolTable *st)` - 销毁符号表

## 哈希函数
可复用现有的 RSHash 函数，或使用简单的字符串哈希。

## 作用域规则
- 全局变量：scope_level = 0
- 函数内变量：scope_level >= 1
- 进入 `{` 时调用 enter_scope()
- 退出 `}` 时调用 exit_scope()
- 查找时优先内层，找不到再找外层

请使用中文注释，代码健壮且易于测试。
```

### 验收标准

- [x] 符号表基本操作（插入、查找）正确
- [x] 作用域管理正确（嵌套作用域测试）
- [x] 重定义检测有效
- [x] 查找符号时正确搜索作用域链
- [x] 内存管理无泄漏（exit_scope 正确释放）

### 注意事项

- 保持哈希表的高效性（O(1) 平均查找时间）
- 作用域层级从0开始（0表示全局作用域）
- 查找时要区分"当前作用域查找"和"作用域链查找"
- 为函数符号预留扩展空间（如参数列表），2.0完善

---

## TASK004: 语义分析器实现

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0 (最高)  
**预计时间**: 4天  
**依赖**: TASK003

### 任务目标

实现语义分析器，遍历AST进行类型检查、符号表构建、隐式类型转换插入。

### 子任务清单

- [ ] **4.1 设计语义分析器框架**
  - 定义 `SemanticAnalyzer` 结构体：symbol_table, error_count
  - 实现 `analyze_program()` 入口函数

- [ ] **4.2 实现变量声明处理**
  - 解析 `int a = 10, b = 20, c;` 这类多变量声明
  - 为每个变量插入符号表
  - 检查重定义错误
  - 计算栈偏移量（简单累加）

- [ ] **4.3 实现表达式类型检查**
  - 递归检查二元运算（+, -, *, /）的左右类型
  - 插入类型转换节点（如 int → float）
  - 标注表达式节点的结果类型

- [ ] **4.4 实现赋值语句检查**
  - 检查左值是否可赋值（变量）
  - 检查右值类型是否与左值兼容
  - 必要时插入类型转换

- [ ] **4.5 实现控制流语句检查**
  - if 语句：检查条件表达式类型
  - while/for 语句：检查条件和循环体
  - 管理作用域：进入 `{` 时 enter_scope()，退出时 exit_scope()

- [ ] **4.6 实现错误报告**
  - 集成 error.c 模块
  - 报告类型不匹配、变量未定义、重定义等错误
  - 错误信息包含行号和变量名

- [ ] **4.7 单元测试**
  - 测试正确的程序（无错误）
  - 测试类型不匹配错误
  - 测试变量未定义错误
  - 测试作用域正确性

### AI助手提示词

```
请帮我实现 Mini-C 编译器的语义分析器（src/semantic/semantic_analyzer.h 和 semantic_analyzer.c）：

## 任务
遍历 AST（抽象语法树），执行以下分析：
1. 构建符号表（变量声明时插入）
2. 类型检查（表达式、赋值、函数调用）
3. 作用域管理（进入/退出块语句）
4. 插入隐式类型转换节点

## 核心函数
```c
typedef struct SemanticAnalyzer {
    SymbolTable *symbol_table;  // 符号表
    int error_count;            // 错误计数
} SemanticAnalyzer;

// 入口函数
void analyze_program(SemanticAnalyzer *sa, Tree *ast);

// 辅助函数
void analyze_statement(SemanticAnalyzer *sa, Tree *stmt);
Type* analyze_expression(SemanticAnalyzer *sa, Tree *expr);
void analyze_declaration(SemanticAnalyzer *sa, Tree *decl);
```

## AST 节点类型（参考 yacc.y）
- 声明：`declare_expression` (如 int a=10, b;)
- 赋值：`assignment_expression` (如 a = b + 1)
- 二元运算：`additive_expression`, `multiplicative_expression`
- 控制流：`if_expression`, `while_expression`, `for_expression`
- 标识符：`ID` (需要查找符号表)
- 常量：`INT10`, `INT8`, `INT16` (常量值)

## 类型检查规则
1. 二元运算（+, -, *, /）：
   - 如果左右类型相同，结果为该类型
   - 如果一个是 int，一个是 float，将 int 提升为 float
   
2. 赋值（=）：
   - 左值必须是变量（ID）
   - 右值类型必须与左值兼容
   
3. 条件表达式（if, while）：
   - 条件可以是任意类型（非零为真）

## 类型转换插入
当检测到需要转换时（如 int + float），在 AST 中插入转换节点：
```c
Tree* insert_cast(Tree *expr, Type *target_type) {
    Tree *cast_node = create_tree("CAST", 1, expr);
    cast_node->type = target_type;
    return cast_node;
}
```

## 错误处理
使用统一的错误报告函数（error.c）：
```c
semantic_error(int line, const char *fmt, ...);
```

请使用中文注释，代码结构清晰，便于理解和扩展。
```

### 验收标准

- [x] 能正确分析简单的变量声明和赋值
- [x] 类型检查准确，能检测类型不匹配
- [x] 符号表正确构建，查找变量成功
- [x] 作用域管理正确（嵌套块测试）
- [x] 错误信息清晰，包含行号

### 注意事项

- AST 遍历使用递归，注意栈深度
- 类型转换节点的插入要修改 AST 结构，小心内存管理
- 错误恢复：遇到错误后继续分析，收集所有错误
- 区分编译期错误（类型检查）和运行期错误

---

## TASK005: 中间代码生成器重构

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0  
**预计时间**: 4天  
**依赖**: TASK004

### 任务目标

重构中间代码生成模块，从类型标注的AST生成四元式IR，支持浮点运算、类型转换指令。

### 子任务清单

- [ ] **5.1 设计IR指令集**
  - 定义 `IROpcode` 枚举：ADD, SUB, MUL, DIV, FADD, FSUB, FMUL, FDIV（区分整数和浮点）
  - 定义转换指令：I2F (int→float), F2I (float→int)
  - 定义控制流指令：LABEL, GOTO, IF_FALSE
  - 定义 `IRInstruction` 结构体

- [ ] **5.2 实现临时变量管理**
  - 实现 `new_temp()` 函数，生成 t0, t1, t2 ... 临时变量名
  - 实现 `new_label()` 函数，生成 L1, L2, L3 ... 标签名

- [ ] **5.3 实现表达式翻译**
  - 二元运算：递归翻译左右子树，生成运算指令
  - 类型转换：检测 AST 中的 CAST 节点，生成 I2F/F2I 指令
  - 变量引用：直接使用变量名
  - 常量：直接使用字面值

- [ ] **5.4 实现语句翻译**
  - 赋值语句：生成 ASSIGN 指令
  - if 语句：生成条件跳转指令
  - while 语句：生成标签和跳转指令
  - for 语句：翻译为 while 等价形式

- [ ] **5.5 实现函数调用翻译（基础）**
  - 翻译 `output_int()` 和 `input_int()`
  - 生成 PARAM 和 CALL 指令
  - 函数定义翻译留到 2.0

- [ ] **5.6 输出IR到文件**
  - 格式化输出四元式到 `Innercode` 文件
  - 格式：`行号 op arg1 arg2 result`
  - 示例：`1 + 3 4 t0` 表示 t0 = 3 + 4

- [ ] **5.7 单元测试**
  - 测试表达式翻译（算术、关系）
  - 测试控制流翻译（if, while）
  - 测试类型转换指令生成

### AI助手提示词

```
请帮我实现 Mini-C 编译器的中间代码生成器（src/ir/ir_builder.h 和 ir_builder.c）：

## 任务
从类型标注的 AST 生成线性的四元式中间代码（IR）。

## IR 指令格式
四元式：`(opcode, arg1, arg2, result)`

### 指令集
```c
typedef enum {
    // 整数运算
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_MOD,
    // 浮点运算
    IR_FADD, IR_FSUB, IR_FMUL, IR_FDIV,
    // 类型转换
    IR_I2F,    // int to float
    IR_F2I,    // float to int
    // 关系运算
    IR_LT, IR_GT, IR_LE, IR_GE, IR_EQ, IR_NE,
    // 赋值
    IR_ASSIGN,
    // 控制流
    IR_LABEL,      // 标签定义
    IR_GOTO,       // 无条件跳转
    IR_IF_FALSE,   // 条件跳转（条件为假时跳转）
    // 函数调用
    IR_PARAM,      // 参数传递
    IR_CALL,       // 函数调用
    IR_RETURN      // 返回
} IROpcode;

typedef struct IRInstruction {
    IROpcode op;
    char *arg1;
    char *arg2;
    char *result;
} IRInstruction;
```

## 核心函数
```c
typedef struct IRBuilder {
    IRInstruction **instructions;  // 指令数组
    int count;                      // 指令数量
    int capacity;                   // 数组容量
    int temp_count;                 // 临时变量计数
    int label_count;                // 标签计数
} IRBuilder;

IRBuilder* ir_builder_create();
char* new_temp(IRBuilder *builder);              // 生成临时变量 t0, t1, ...
char* new_label(IRBuilder *builder);             // 生成标签 L1, L2, ...
void emit(IRBuilder *builder, IRInstruction *inst);  // 添加指令

// 翻译函数
char* translate_expression(IRBuilder *builder, Tree *expr);  // 返回结果变量名
void translate_statement(IRBuilder *builder, Tree *stmt);
void translate_program(IRBuilder *builder, Tree *ast);
```

## 翻译示例

### 表达式
```c
// 源码：a + b * 2
// AST：ADD(ID(a), MUL(ID(b), NUM(2)))

// 生成的IR：
t0 = b * 2       → IR_MUL, "b", "2", "t0"
t1 = a + t0      → IR_ADD, "a", "t0", "t1"
```

### if 语句
```c
// 源码：if (a > 0) { b = 1; }

// 生成的IR：
t0 = a > 0       → IR_GT, "a", "0", "t0"
if !t0 goto L1   → IR_IF_FALSE, "t0", "_", "L1"
b = 1            → IR_ASSIGN, "1", "_", "b"
L1:              → IR_LABEL, "_", "_", "L1"
```

### 类型转换
```c
// 源码：float x = 10;  (int 10 需要转换为 float)

// 生成的IR：
t0 = I2F 10      → IR_I2F, "10", "_", "t0"
x = t0           → IR_ASSIGN, "t0", "_", "x"
```

## 输出格式
输出到 `Innercode` 文件，格式：
```
1 t0 = b * 2
2 t1 = a + t0
3 if t0 == 0 goto 5
4 goto 6
5 L1:
```

请使用中文注释，代码清晰易懂。
```

### 验收标准

- [x] 能从AST生成正确的四元式序列
- [x] 临时变量和标签命名规范
- [x] 类型转换指令正确插入
- [x] 控制流指令（if, while）正确
- [x] 输出格式符合后续汇编生成器的要求

### 注意事项

- 临时变量要与符号表分离管理
- 短路求值（&&, ||）需要特殊处理（可选，高级特性）
- 保持与现有 `Innercode` 格式兼容，便于复用 `assembly.py`
- 浮点运算指令与整数运算指令分开，便于代码生成

---

## TASK006: 常量折叠优化实现

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P1  
**预计时间**: 2天  
**依赖**: TASK005

### 任务目标

实现常量折叠优化，在AST层面识别并计算常量表达式，减少运行时计算开销。

### 子任务清单

- [ ] **6.1 设计常量折叠算法**
  - 递归遍历AST
  - 识别常量节点（INT10, FLOAT_LIT等）
  - 对常量子树进行编译期计算

- [ ] **6.2 实现整数常量折叠**
  - 处理 +, -, *, /, % 运算
  - 如 `3 + 4 * 5` → `23`
  - 注意除零检查

- [ ] **6.3 实现浮点常量折叠**
  - 处理浮点运算
  - 注意精度问题
  - 如 `3.14 * 2.0` → `6.28`

- [ ] **6.4 实现关系运算折叠**
  - 如 `3 > 2` → `1` (true)
  - 如 `5 == 5` → `1`
  - 结果用于死代码消除

- [ ] **6.5 集成到优化流程**
  - 在 IR 生成前对 AST 进行优化
  - 优化后重新进行类型检查（可选）

- [ ] **6.6 测试和验证**
  - 编写测试用例
  - 对比优化前后的 IR 指令数量
  - 验证计算结果正确性

### AI助手提示词

```
请帮我实现 Mini-C 编译器的常量折叠优化（src/optimization/const_folding.h 和 const_folding.c）：

## 任务
在 AST 层面识别常量表达式并进行编译期计算，减少生成的 IR 指令。

## 算法
递归遍历 AST，对于二元运算节点：
1. 递归优化左子树和右子树
2. 检查：如果左右子树都是常量（NUM或FLOAT_LIT）
3. 执行计算，将当前节点替换为常量节点

## 核心函数
```c
// 主函数：优化整个AST
Tree* fold_constants(Tree *node);

// 辅助函数
bool is_constant_node(Tree *node);  // 判断节点是否为常量
int eval_int_binop(char op, int lhs, int rhs);  // 计算整数二元运算
float eval_float_binop(char op, float lhs, float rhs);  // 计算浮点二元运算
```

## 实现示例
```c
Tree* fold_constants(Tree *node) {
    if (node == NULL) return NULL;
    
    // 递归处理子节点
    if (node->kind == ND_ADD) {
        node->lhs = fold_constants(node->lhs);
        node->rhs = fold_constants(node->rhs);
        
        // 如果左右都是整数常量
        if (node->lhs->kind == ND_NUM && node->rhs->kind == ND_NUM) {
            int result = node->lhs->val + node->rhs->val;
            return new_num_node(result);  // 替换为结果常量节点
        }
    }
    
    // 处理其他运算符...
    
    return node;
}
```

## 需要处理的运算
- 算术：+, -, *, /, %
- 关系：<, >, <=, >=, ==, !=
- 逻辑：&&, ||, !

## 边界情况
- 除零检查：除法和取模运算
- 浮点精度：使用 float 类型
- 溢出：整数溢出视为未定义行为（不检查）

## 测试用例
```c
int a = 3 + 4 * 5;        // → int a = 23;
float x = 2.5 * 2.0;      // → float x = 5.0;
if (10 > 5) { ... }       // → 条件恒真，可进一步优化
```

请使用中文注释，处理好内存管理（替换节点时释放旧节点）。
```

### 验收标准

- [x] 能正确折叠整数常量表达式
- [x] 能正确折叠浮点常量表达式
- [x] 除零等错误情况有适当处理
- [x] 优化后的 IR 指令数量明显减少
- [x] 生成的汇编代码更简洁

### 注意事项

- 折叠时要保持类型信息（int vs float）
- 不要折叠包含变量的表达式
- 优化是可选的，即使跳过也不影响正确性
- 可以在文档中展示优化效果对比

---

## TASK007: 死代码消除优化实现

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P1  
**预计时间**: 2天  
**依赖**: TASK006

### 任务目标

实现死代码消除优化，移除永远不会执行的代码（如 `if(0) {...}`），减少生成的代码量。

### 子任务清单

- [ ] **7.1 设计死代码检测算法**
  - 检测恒假条件：`if (0)`, `while (0)`
  - 检测恒真条件：`if (1)`（可优化为直接执行then分支）
  - 检测不可达代码：`return` 后的语句

- [ ] **7.2 实现条件语句优化**
  - `if (0) { A } else { B }` → `B`
  - `if (1) { A } else { B }` → `A`
  - `while (0) { ... }` → 删除整个循环

- [ ] **7.3 实现不可达代码检测**
  - `return` 语句后的代码标记为不可达
  - `goto` 后的代码（可选）

- [ ] **7.4 集成到优化流程**
  - 在常量折叠后执行（利用折叠结果）
  - 修改 AST，删除死代码节点

- [ ] **7.5 测试和验证**
  - 测试 `if(0)` 分支被删除
  - 测试优化后的代码功能正确
  - 对比优化前后的 IR 行数

### AI助手提示词

```
请帮我实现 Mini-C 编译器的死代码消除优化（src/optimization/dead_code_elim.h 和 dead_code_elim.c）：

## 任务
识别并移除永远不会执行的代码，减少生成的 IR 和汇编代码。

## 优化场景

### 1. 恒假条件
```c
if (0) {
    output_int(999);  // 这段代码永远不执行
}
```
优化：删除整个 if 语句

### 2. 恒真条件
```c
if (1) {
    output_int(1);
} else {
    output_int(0);  // 这段代码永远不执行
}
```
优化：只保留 then 分支，删除 else 分支

### 3. 恒假循环
```c
while (0) {
    // 循环体永远不执行
}
```
优化：删除整个循环

## 核心函数
```c
// 主函数：消除死代码
Tree* eliminate_dead_code(Tree *node);

// 辅助函数
bool is_constant_zero(Tree *node);  // 判断节点是否为常量0
bool is_constant_one(Tree *node);   // 判断节点是否为常量1（非零）
```

## 实现逻辑
```c
Tree* eliminate_dead_code(Tree *node) {
    if (node == NULL) return NULL;
    
    if (node->kind == ND_IF) {
        // 优化条件
        node->cond = eliminate_dead_code(node->cond);
        
        // 检查条件是否为常量
        if (is_constant_zero(node->cond)) {
            // if(0) { then } else { els } → els
            return node->els ? eliminate_dead_code(node->els) : NULL;
        }
        if (is_constant_one(node->cond)) {
            // if(1) { then } else { els } → then
            return eliminate_dead_code(node->then);
        }
        
        // 递归优化分支
        node->then = eliminate_dead_code(node->then);
        node->els = eliminate_dead_code(node->els);
    }
    
    // 处理其他节点类型...
    
    return node;
}
```

## 注意事项
- 只处理编译期可确定的常量条件
- 不要误删有副作用的代码（如函数调用）
- 保持 AST 结构完整性

## 测试用例
```c
void main() {
    int a = 10;
    
    if (0) {
        a = 999;  // 应被删除
    }
    
    if (1) {
        output_int(a);  // 应保留
    }
    
    while (0) {
        a = 888;  // 应被删除
    }
}
```

请使用中文注释，小心处理内存释放。
```

### 验收标准

- [x] `if(0)` 分支被正确删除
- [x] `if(1)` 的 else 分支被删除
- [x] `while(0)` 循环被删除
- [x] 优化后的程序功能正确
- [x] 生成的 IR 和汇编代码更简洁

### 注意事项

- 依赖常量折叠的结果（先折叠再消除）
- 不要删除有副作用的表达式（如函数调用）
- 保守优化：不确定时不优化
- 在文档中展示优化效果

---

## TASK008: 代码生成器增强

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0  
**预计时间**: 4天  
**依赖**: TASK005, TASK007

### 任务目标

增强代码生成器，支持浮点运算（SSE指令）、类型转换、优化后的IR翻译。

### 子任务清单

- [ ] **8.1 分析现有的 assembly.py**
  - 理解四元式到汇编的翻译逻辑
  - 识别需要扩展的部分（浮点、类型转换）

- [ ] **8.2 支持浮点运算指令**
  - 整数：使用通用寄存器（eax, ebx）+ add, sub, mul, div
  - 浮点：使用 SSE 寄存器（xmm0-xmm7）+ movss, addss, subss, mulss, divss

- [ ] **8.3 支持类型转换指令**
  - I2F (int→float): `cvtsi2ss xmm0, eax`
  - F2I (float→int): `cvttss2si eax, xmm0`

- [ ] **8.4 改进栈帧管理**
  - 区分整数变量和浮点变量的栈分配
  - 整数：4字节对齐
  - 浮点：4字节（float）或8字节（double）

- [ ] **8.5 支持浮点输出**
  - 修改 printf 调用：设置 rax 为浮点参数个数
  - 格式字符串支持 %f

- [ ] **8.6 测试和调试**
  - 测试整数运算（确保兼容现有功能）
  - 测试浮点运算
  - 测试混合运算（int + float）

### AI助手提示词

```
请帮我增强 Mini-C 编译器的代码生成模块（scripts/asm_generator.py），支持浮点运算和类型转换：

## 背景
现有的 assembly.py 只支持整数运算，使用通用寄存器（eax, ebx）和整数指令（add, sub, mul, div）。
我们需要扩展以支持浮点数。

## 新增 IR 指令
从 Innercode 文件读取的指令可能包含：
- 整数运算：`+`, `-`, `*`, `/` (现有)
- 浮点运算：`f+`, `f-`, `f*`, `f/` (新增)
- 类型转换：`i2f` (int→float), `f2i` (float→int) (新增)

## x86-64 SSE 指令集
### 浮点运算
- `movss xmm0, [rbp-4]` - 加载浮点数到 SSE 寄存器
- `addss xmm0, xmm1` - 浮点加法
- `subss xmm0, xmm1` - 浮点减法
- `mulss xmm0, xmm1` - 浮点乘法
- `divss xmm0, xmm1` - 浮点除法

### 类型转换
- `cvtsi2ss xmm0, eax` - 将整数(eax)转换为浮点(xmm0)
- `cvttss2si eax, xmm0` - 将浮点(xmm0)转换为整数(eax)

## 实现要点

### 1. 识别浮点指令
```python
if ops[4] == 'f+':  # 浮点加法
    # 加载操作数到 xmm 寄存器
    result += [tab + "movss xmm0, " + inx + "...]"]
    result += [tab + "movss xmm1, " + inx + "...]"]
    result += [tab + "addss xmm0, xmm1"]
    # 存储结果
    result += [tab + "movss " + inx + "...], xmm0"]
```

### 2. 类型转换
```python
if ops[1] == 'i2f':  # int to float
    result += [tab + "mov eax, " + inx + "..."]  # 加载整数
    result += [tab + "cvtsi2ss xmm0, eax"]       # 转换
    result += [tab + "movss " + inx + "...], xmm0"]  # 存储
```

### 3. 浮点输出
```python
# output_int 改为 output_float 时
result += [tab + "movss xmm0, " + inx + "..."]  # 加载浮点数到 xmm0
result += [tab + "mov rdi, float_format"]       # %f 格式字符串
result += [tab + "mov rax, 1"]                  # 1个浮点参数
result += [tab + "call printf"]
```

## 栈分配
为简化实现，整数和浮点都分配4字节：
```python
data[data.index(-1)] = four[i][3]  # 记录变量位置
offset = 4 * data.index(four[i][3]) + 4
```

## 测试用例
```c
void main() {
    int a = 10;
    float x = 3.14;
    float y = a + x;  // 应生成 i2f 转换和浮点加法
    output_float(y);
}
```

请保持代码风格与现有 assembly.py 一致，添加中文注释。
```

### 验收标准

- [x] 能正确生成整数运算汇编（保持兼容）
- [x] 能正确生成浮点运算汇编（SSE指令）
- [x] 类型转换指令正确
- [x] 生成的汇编可以汇编和链接
- [x] 运行结果正确（通过测试用例验证）

### 注意事项

- SSE 指令需要 CPU 支持（现代 x86-64 都支持）
- 浮点参数传递使用 xmm 寄存器（System V ABI）
- printf 浮点参数需要设置 rax 寄存器
- 保持向后兼容，不破坏现有整数功能

---

## TASK009: 错误处理系统完善

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P1  
**预计时间**: 2天  
**依赖**: TASK004

### 任务目标

实现统一的错误报告系统，提供清晰的错误信息、行号、源码上下文，支持多种错误类型。

### 子任务清单

- [ ] **9.1 设计错误报告接口**
  - 定义错误类型：词法错误、语法错误、语义错误
  - 定义错误级别：错误（Error）、警告（Warning）
  - 统一的错误报告函数

- [ ] **9.2 实现错误信息格式化**
  - 包含文件名、行号、列号
  - 显示出错的源代码行
  - 用 `^` 标记错误位置

- [ ] **9.3 实现错误计数和恢复**
  - 统计错误数量
  - 错误恢复：遇到错误后继续分析
  - 达到错误上限后停止编译

- [ ] **9.4 集成到各模块**
  - 词法分析器：报告非法字符
  - 语法分析器：报告语法错误
  - 语义分析器：报告类型错误、未定义变量等

- [ ] **9.5 可选：彩色输出**
  - 使用 ANSI 转义码
  - 错误信息红色，警告黄色

- [ ] **9.6 测试错误报告**
  - 创建包含各类错误的测试文件
  - 验证错误信息清晰准确

### AI助手提示词

```
请帮我实现 Mini-C 编译器的错误处理系统（src/utils/error.h 和 error.c）：

## 任务
提供统一的错误报告接口，输出格式化的错误信息，帮助用户快速定位问题。

## 错误信息格式
```
test.c:5:10: error: type mismatch in assignment
    int a = 3.14;
            ^~~~
expected 'int' but got 'float'
```

## 核心数据结构
```c
typedef enum {
    ERROR_LEXICAL,   // 词法错误
    ERROR_SYNTAX,    // 语法错误
    ERROR_SEMANTIC,  // 语义错误
    ERROR_INTERNAL   // 内部错误
} ErrorType;

typedef struct ErrorReporter {
    int error_count;
    int warning_count;
    int max_errors;   // 达到此数量后停止编译
    FILE *output;     // 错误输出流（通常是stderr）
} ErrorReporter;
```

## 核心函数
```c
// 创建错误报告器
ErrorReporter* error_reporter_create(int max_errors);

// 报告错误
void report_error(ErrorReporter *er, 
                  const char *filename, 
                  int line, 
                  int column,
                  ErrorType type,
                  const char *format, ...);  // 可变参数

// 报告警告
void report_warning(ErrorReporter *er, 
                    const char *filename, 
                    int line, 
                    const char *format, ...);

// 检查是否应该停止编译
bool should_stop_compilation(ErrorReporter *er);
```

## 实现示例
```c
void report_error(ErrorReporter *er, const char *filename, 
                  int line, int column, ErrorType type, 
                  const char *format, ...) {
    // 输出位置信息
    fprintf(er->output, "%s:%d:%d: error: ", filename, line, column);
    
    // 输出错误消息（使用可变参数）
    va_list args;
    va_start(args, format);
    vfprintf(er->output, format, args);
    va_end(args);
    fprintf(er->output, "\n");
    
    // TODO: 显示源代码行和错误标记
    
    er->error_count++;
}
```

## 彩色输出（可选）
```c
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

fprintf(stderr, COLOR_RED "error: " COLOR_RESET "...\n");
```

## 集成示例
```c
// 在语义分析器中使用
if (!type_compatible(lhs_type, rhs_type)) {
    report_error(analyzer->error_reporter, 
                 current_file, node->line, node->column,
                 ERROR_SEMANTIC,
                 "type mismatch: cannot assign '%s' to '%s'",
                 type_to_string(rhs_type),
                 type_to_string(lhs_type));
}
```

请使用中文注释，错误信息使用中文或英文（根据喜好）。
```

### 验收标准

- [x] 错误报告函数正常工作
- [x] 错误信息包含文件名、行号
- [x] 错误信息清晰易懂
- [x] 能正确计数错误和警告
- [x] 集成到各编译阶段

### 注意事项

- 错误信息要对用户友好，避免技术术语
- 提供修复建议（如"是否忘记分号？"）
- 不要因为一个错误而停止编译，尽量收集多个错误
- 彩色输出是可选的，要检测终端支持

---

## TASK010: 测试框架搭建

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P1  
**预计时间**: 3天  
**依赖**: TASK008

### 任务目标

建立完整的测试框架，编写测试用例，自动化测试流程，确保编译器功能正确。

### 子任务清单

- [ ] **10.1 创建测试目录结构**
  - 按功能模块组织测试用例
  - 每个测试包含 `.c` 源文件和预期输出

- [ ] **10.2 编写词法分析测试**
  - 测试浮点数识别
  - 测试注释处理（单行、多行）
  - 测试字符串和字符字面量
  - 测试错误：非法字符

- [ ] **10.3 编写语法分析测试**
  - 测试表达式优先级
  - 测试语句（声明、赋值、if、while、for）
  - 测试错误：语法错误

- [ ] **10.4 编写语义分析测试**
  - 测试类型检查
  - 测试变量作用域
  - 测试错误：类型不匹配、未定义变量

- [ ] **10.5 编写优化测试**
  - 测试常量折叠效果
  - 测试死代码消除效果
  - 对比优化前后的 IR

- [ ] **10.6 编写集成测试**
  - 完整的小程序测试
  - 运行生成的可执行文件，验证输出

- [ ] **10.7 实现自动化测试脚本**
  - `scripts/test_runner.py`
  - 批量运行测试，收集结果
  - 生成测试报告（通过/失败统计）

### AI助手提示词

```
请帮我创建 Mini-C 编译器的测试框架：

## 任务
1. 在 `tests/` 目录下创建各类测试用例
2. 编写 `scripts/test_runner.py` 自动化测试脚本

## 测试用例结构
```
tests/
├── lexer/
│   ├── test_float.c          # 浮点数识别
│   ├── test_comment.c        # 注释处理
│   └── expected/             # 预期的词法分析输出
├── parser/
│   ├── test_expression.c     # 表达式解析
│   └── expected/
├── semantic/
│   ├── test_type_check.c     # 类型检查
│   └── expected/
├── optimization/
│   ├── test_const_fold.c     # 常量折叠
│   └── expected/
└── integration/
    ├── test_basic.c          # 基础功能综合测试
    ├── expected_output.txt   # 预期程序输出
    └── ...
```

## 测试用例示例

### tests/lexer/test_float.c
```c
void main() {
    float x = 3.14;
    float y = 1.5e-2;
}
```

### tests/semantic/test_type_check.c
```c
// 应报错：类型不匹配
void main() {
    int a = 10;
    float x = 3.14;
    a = x;  // error: cannot assign float to int
}
```

### tests/optimization/test_const_fold.c
```c
void main() {
    int a = 3 + 4 * 5;  // 应优化为 23
    output_int(a);
}
```

### tests/integration/test_basic.c
```c
void main() {
    int a = 10, b = 20;
    int c = (a + b) * 2;
    if (c > 50) {
        output_int(c);  // 应输出 60
    }
}
```

## 自动化测试脚本（test_runner.py）

### 功能需求
1. 扫描 `tests/` 目录下的所有 `.c` 文件
2. 对每个测试：
   - 编译：`make file=<test>`
   - 检查编译是否成功（或预期失败）
   - 运行可执行文件
   - 比较输出与预期输出
3. 统计通过/失败数量
4. 生成测试报告

### 实现框架
```python
import os
import subprocess
import sys

def run_test(test_file):
    """运行单个测试用例"""
    # 编译
    result = subprocess.run(['make', f'file={test_file}'], 
                            capture_output=True, text=True)
    
    # 检查编译结果
    if result.returncode != 0:
        return False, "编译失败"
    
    # 运行
    exe_result = subprocess.run([f'./{test_file}'], 
                                capture_output=True, text=True)
    
    # 比较输出
    expected = read_expected_output(test_file)
    if exe_result.stdout == expected:
        return True, "通过"
    else:
        return False, f"输出不匹配\n预期: {expected}\n实际: {exe_result.stdout}"

def main():
    # 扫描测试目录
    tests = find_all_tests('tests/')
    
    passed = 0
    failed = 0
    
    for test in tests:
        success, message = run_test(test)
        if success:
            print(f"✓ {test}: {message}")
            passed += 1
        else:
            print(f"✗ {test}: {message}")
            failed += 1
    
    print(f"\n总计: {passed + failed}, 通过: {passed}, 失败: {failed}")
    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())
```

## 运行方式
```bash
# 运行所有测试
python3 scripts/test_runner.py

# 运行特定模块测试
python3 scripts/test_runner.py --module lexer
```

请实现完整的测试框架，包含至少10个测试用例覆盖各个功能模块。
```

### 验收标准

- [x] 测试用例覆盖所有核心功能
- [x] 测试脚本可以自动运行所有测试
- [x] 测试报告清晰显示通过/失败情况
- [x] 至少80%的测试通过
- [x] 失败的测试有清晰的错误信息

### 注意事项

- 测试用例要简洁，针对性强
- 包含正面测试（正确代码）和负面测试（错误代码）
- 测试要可重复，不依赖外部状态
- 使用 `make clean` 清理上次测试的残留文件

---

## TASK011: 文档编写

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P2  
**预计时间**: 2天  
**依赖**: TASK010

### 任务目标

编写完整的技术文档和用户指南，记录设计决策、使用方法、测试结果。

### 子任务清单

- [ ] **11.1 编写设计文档 (docs/design.md)**
  - 架构设计
  - 模块划分和接口
  - 数据结构设计
  - 算法说明

- [ ] **11.2 编写用户指南 (docs/user_guide.md)**
  - 安装和环境配置
  - 编译和运行示例
  - 查看编译过程中间结果
  - 常见问题 (FAQ)

- [ ] **11.3 编写文法文档 (docs/grammar.md)**
  - 词法规则
  - 语法规则（EBNF）
  - 语义约束

- [ ] **11.4 编写优化文档 (docs/optimization.md)**
  - 常量折叠算法
  - 死代码消除算法
  - 优化效果对比（IR 行数、汇编大小）

- [ ] **11.5 更新 README.md**
  - 添加实际运行截图
  - 更新测试结果
  - 添加已知问题

- [ ] **11.6 编写开发日志**
  - 记录遇到的问题和解决方案
  - 记录开发进度

### 验收标准

- [x] 所有文档文件已创建并填充内容
- [x] 文档清晰易懂，有示例
- [x] README 包含快速开始指南
- [x] 文档与代码保持同步

### 注意事项

- 使用 Markdown 格式
- 添加代码示例和图表
- 文档使用中文编写
- 定期更新文档与代码同步

---

## TASK012: 1.0版本验收和发布

**版本**: 1.0  
**状态**: 📝 计划中  
**优先级**: P0  
**预计时间**: 2天  
**依赖**: TASK011

### 任务目标

完成1.0版本的最终验收，确保所有功能正常，发布到GitHub。

### 子任务清单

- [ ] **12.1 完整功能测试**
  - 运行所有测试用例
  - 修复失败的测试
  - 确保测试通过率达到95%+

- [ ] **12.2 性能测试**
  - 测试编译速度
  - 测试生成代码的运行速度
  - 记录优化效果数据

- [ ] **12.3 代码审查**
  - 检查代码规范
  - 添加必要的注释
  - 清理调试代码

- [ ] **12.4 文档完善**
  - 更新所有文档
  - 添加使用示例
  - 完善 README

- [ ] **12.5 准备发布**
  - 创建 `release-1.0` 分支
  - 打上 `v1.0` 标签
  - 编写 CHANGELOG.md

- [ ] **12.6 GitHub发布**
  - 推送到远程仓库
  - 创建 Release
  - 编写发布说明

### 验收标准清单

#### 核心功能验收
- [ ] 词法分析：正确识别 int/float/char 类型、注释、字符串
- [ ] 语法分析：正确构建 AST，支持复杂表达式和语句
- [ ] 语义分析：类型检查正确，符号表管理正确
- [ ] 中间代码：生成正确的四元式 IR
- [ ] 代码优化：常量折叠和死代码消除有效
- [ ] 汇编生成：生成正确的 x86-64 汇编代码
- [ ] 程序运行：生成的可执行文件正确运行

#### 功能测试验收
- [ ] 支持 int, float, char 三种类型
- [ ] 支持多变量声明：`int a=1, b=2, c;`
- [ ] 支持四则混合运算和括号
- [ ] 支持单行注释 `//` 和多行注释 `/* */`
- [ ] 支持 if-else 语句
- [ ] 支持 while 和 for 循环
- [ ] 支持 output_int() 输出
- [ ] 类型转换（int ↔ float）正确

#### 优化效果验收
- [ ] 常量折叠：`3+4*5` 编译期计算为 `23`
- [ ] 死代码消除：`if(0) {...}` 不生成代码
- [ ] IR 指令数量减少（与优化前对比）

#### 错误处理验收
- [ ] 词法错误有清晰提示
- [ ] 语法错误有行号提示
- [ ] 语义错误（类型不匹配、未定义变量）有详细提示

#### 文档验收
- [ ] README 完整清晰
- [ ] 设计文档记录了架构和算法
- [ ] 用户指南可以指导新用户使用
- [ ] 测试用例有文档说明

### AI助手提示词

```
请帮我完成 Mini-C 编译器 1.0 版本的发布准备工作：

## 任务
1. 运行完整测试套件，修复失败的测试
2. 审查代码质量，添加注释
3. 更新所有文档
4. 准备 GitHub 发布

## 1. 运行测试
```bash
python3 scripts/test_runner.py
```
确保所有测试通过，如有失败，请修复。

## 2. 代码质量检查
- 所有函数有注释说明
- 删除调试用的 printf 语句
- 统一代码风格
- 检查内存泄漏（使用 valgrind 可选）

## 3. 文档更新
- README.md: 更新测试结果、添加运行截图
- docs/design.md: 确保架构图和设计描述完整
- docs/user_guide.md: 添加常见问题和示例
- CHANGELOG.md: 记录 1.0 版本的所有功能

## 4. 性能数据收集
对比优化前后的效果，记录到文档：
```
优化前：
- IR 指令数：150条
- 汇编代码大小：2.5KB
- 栈空间：128字节

优化后：
- IR 指令数：98条 (减少35%)
- 汇编代码大小：1.8KB (减少28%)
- 栈空间：128字节 (1.0版本未优化)
```

## 5. Git 操作
```bash
# 创建发布分支
git checkout -b release-1.0

# 提交最终修改
git add .
git commit -m "chore: 准备 1.0 版本发布"

# 打标签
git tag -a v1.0 -m "Version 1.0: 核心编译器增强版"

# 推送到 GitHub
git push origin release-1.0
git push origin v1.0
```

## 6. GitHub Release
在 GitHub 上创建 Release，内容包括：
- 标题：Mini-C 编译器 v1.0 - 核心编译器增强版
- 描述：列出主要功能和改进
- 附件：编译好的可执行文件（可选）
- 链接：指向文档和测试报告

请协助完成以上工作，使用中文编写所有文档。
```

### 注意事项

- 确保所有功能完整实现
- 测试通过率要高（目标95%+）
- 文档要详细完整
- 代码要整洁，有良好的注释
- Git 提交信息要清晰规范

---

## 🎯 版本 2.0 - 高级特性版

### 版本目标

在 1.0 版本基础上，增加对数组、指针、函数定义和调用、结构体的支持，实现活性分析优化。

### 核心任务概览

| 任务编号 | 任务名称 | 预计时间 | 优先级 | 状态 |
|---------|---------|---------|--------|------|
| TASK201 | 一维数组支持 | 3天 | P0 | ⏸️ 待启动 |
| TASK202 | 多维数组支持 | 2天 | P1 | ⏸️ 待启动 |
| TASK203 | 指针基础支持 | 3天 | P0 | ⏸️ 待启动 |
| TASK204 | 指针运算和解引用 | 2天 | P1 | ⏸️ 待启动 |
| TASK205 | 函数定义和调用 | 4天 | P0 | ⏸️ 待启动 |
| TASK206 | 函数参数和返回值 | 2天 | P0 | ⏸️ 待启动 |
| TASK207 | 结构体定义 | 3天 | P1 | ⏸️ 待启动 |
| TASK208 | 结构体成员访问 | 2天 | P1 | ⏸️ 待启动 |
| TASK209 | 控制流图(CFG)构建 | 3天 | P0 | ⏸️ 待启动 |
| TASK210 | 活性分析实现 | 4天 | P0 | ⏸️ 待启动 |
| TASK211 | 栈槽复用优化 | 3天 | P0 | ⏸️ 待启动 |
| TASK212 | 2.0版本测试和发布 | 3天 | P0 | ⏸️ 待启动 |

### 简要任务说明

**TASK201-202: 数组支持**
- 一维数组声明、初始化、访问
- 多维数组的下标计算
- 数组作为函数参数（退化为指针）

**TASK203-204: 指针支持**
- 指针声明和初始化
- 取地址 `&` 和解引用 `*`
- 指针运算（指针+整数）
- 多重指针

**TASK205-206: 函数支持**
- 函数定义（参数、返回值）
- 函数调用（参数传递、返回值接收）
- 遵循 System V ABI 调用约定
- 递归函数支持

**TASK207-208: 结构体支持**
- 结构体类型定义
- 成员偏移量计算
- 成员访问（`.` 和 `->`）
- 结构体嵌套

**TASK209-211: 活性分析优化（创新点）**
- 构建控制流图（基本块划分）
- 数据流分析（USE/DEF 集合）
- 迭代求解活跃变量集合
- 干涉图构建和栈槽着色
- 验证优化效果（栈空间减少）

**TASK212: 版本发布**
- 完整功能测试
- 性能测试和优化效果量化
- 文档更新
- GitHub 发布

### 验收标准

- [ ] 支持一维和多维数组
- [ ] 支持指针和多重指针
- [ ] 支持函数定义和调用（包括递归）
- [ ] 支持结构体定义和成员访问
- [ ] 活性分析正确，栈空间复用有效
- [ ] 优化效果可量化（栈空间减少30%+）
- [ ] 所有测试通过

---

## 🎯 版本 3.0 - 可视化工具版

### 版本目标

开发 Web 可视化界面，展示编译过程，实现 FIRST/FOLLOW 集计算，提升用户体验。

### 核心任务概览

| 任务编号 | 任务名称 | 预计时间 | 优先级 | 状态 |
|---------|---------|---------|--------|------|
| TASK301 | FIRST集计算器 | 2天 | P1 | ⏸️ 待启动 |
| TASK302 | FOLLOW集计算器 | 2天 | P1 | ⏸️ 待启动 |
| TASK303 | Flask后端搭建 | 2天 | P0 | ⏸️ 待启动 |
| TASK304 | 前端界面设计 | 3天 | P0 | ⏸️ 待启动 |
| TASK305 | CodeMirror集成 | 1天 | P1 | ⏸️ 待启动 |
| TASK306 | AST可视化 | 3天 | P1 | ⏸️ 待启动 |
| TASK307 | IR可视化 | 2天 | P2 | ⏸️ 待启动 |
| TASK308 | 编译过程展示 | 2天 | P0 | ⏸️ 待启动 |
| TASK309 | 3.0版本测试和发布 | 2天 | P0 | ⏸️ 待启动 |

### 简要任务说明

**TASK301-302: 文法分析工具**
- 实现 FIRST 集计算算法
- 实现 FOLLOW 集计算算法
- 展示计算过程
- 支持用户自定义文法（可选）

**TASK303-304: Web界面**
- Flask 后端：接收代码，调用编译器，返回结果
- 前端界面：代码编辑区、结果展示区
- 多标签页：Tokens, AST, IR, Assembly, Output

**TASK305-307: 编辑器和可视化**
- CodeMirror：语法高亮、自动补全
- AST 可视化：树形图展示（D3.js）
- IR 可视化：表格或流程图展示

**TASK308: 编译过程展示**
- 实时显示编译进度
- 展示各阶段耗时
- 优化效果对比（优化前后的IR）

**TASK309: 版本发布**
- 部署到服务器（可选）
- 完善文档
- GitHub 发布

### 验收标准

- [ ] FIRST/FOLLOW 集计算正确
- [ ] Web 界面美观易用
- [ ] 语法高亮正常
- [ ] AST 可视化清晰
- [ ] 可以在线编译并查看结果
- [ ] 文档完善

---

## 📊 总体进度跟踪

### 版本完成度

| 版本 | 任务总数 | 已完成 | 进行中 | 待启动 | 完成率 |
|------|---------|--------|--------|--------|--------|
| 1.0  | 12      | 0      | 0      | 12     | 0%     |
| 2.0  | 12      | 0      | 0      | 12     | 0%     |
| 3.0  | 9       | 0      | 0      | 9      | 0%     |
| **总计** | **33** | **0** | **0** | **33** | **0%** |

### 工作量估算

- **1.0 版本**: 4-6 周（约30-40天）
- **2.0 版本**: 3-4 周（约20-30天）
- **3.0 版本**: 2-3 周（约15-20天）
- **总计**: 9-13 周（约65-90天）

### 里程碑

- **M1**: 项目初始化完成（TASK001）
- **M2**: 类型系统和符号表完成（TASK002-003）
- **M3**: 语义分析和IR生成完成（TASK004-005）
- **M4**: 优化和代码生成完成（TASK006-008）
- **M5**: 1.0 版本发布（TASK012）
- **M6**: 数组和指针支持完成（TASK201-204）
- **M7**: 函数和结构体支持完成（TASK205-208）
- **M8**: 活性分析优化完成（TASK209-211）
- **M9**: 2.0 版本发布（TASK212）
- **M10**: Web 界面完成（TASK303-308）
- **M11**: 3.0 版本发布（TASK309）

---

## 📝 任务状态说明

- 📝 **计划中**: 任务已规划，待开始
- 🔄 **开发中**: 正在开发
- ✅ **已完成**: 开发完成，已验收
- ⏸️ **待启动**: 等待前置任务完成
- ❌ **已取消**: 任务被取消或推迟

---

## 🔧 开发规范

### Git 提交规范

- `feat: 功能描述` - 新功能
- `fix: 问题描述` - 修复bug
- `docs: 文档描述` - 文档更新
- `refactor: 重构描述` - 代码重构
- `test: 测试描述` - 添加测试
- `chore: 杂项描述` - 构建、工具等

### 代码规范

- 使用 K&R 缩进风格
- 函数名使用下划线命名：`symbol_table_create()`
- 结构体名使用大写开头：`SymbolTable`
- 所有公共函数有注释说明
- 使用中文注释

### 测试规范

- 每个功能模块有对应的测试用例
- 测试文件命名：`test_<功能>.c`
- 测试通过标准：输出与预期完全一致

---

## 📞 参考资源

- **Flex Manual**: https://westes.github.io/flex/manual/
- **Bison Manual**: https://www.gnu.org/software/bison/manual/
- **x86-64 ABI**: https://gitlab.com/x86-psABIs/x86-64-ABI
- **NASM Tutorial**: https://cs.lmu.edu/~ray/notes/nasmtutorial/
- **Compiler Design**: 龙书、虎书、鲸书

---

**文档版本**: v1.0  
**最后更新**: 2025-12-16  
**维护者**: Mini-C 开发团队

