# Mini-C 编译器 API 参考文档

本文档描述 Mini-C 编译器各模块的核心 API 接口。

## 目录

- [类型系统 API](#类型系统-api)
- [符号表 API](#符号表-api)
- [中间代码 API](#中间代码-api)
- [优化器 API](#优化器-api)
- [错误处理 API](#错误处理-api)

## 类型系统 API

### 类型定义

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
    struct StructMember *members;
} Type;
```

### 函数接口

#### `Type* new_int_type()`
创建int类型。

**返回值**: 指向int类型对象的指针

#### `Type* new_float_type()`
创建float类型。

**返回值**: 指向float类型对象的指针

#### `Type* new_char_type()`
创建char类型。

**返回值**: 指向char类型对象的指针

#### `int type_size(Type *t)`
计算类型大小。

**参数**:
- `t`: 类型对象指针

**返回值**: 类型大小（字节数）

#### `bool type_equal(Type *a, Type *b)`
判断两个类型是否相同。

**参数**:
- `a`: 第一个类型
- `b`: 第二个类型

**返回值**: 相同返回true，否则返回false

#### `bool type_compatible(Type *a, Type *b)`
判断两个类型是否兼容（可隐式转换）。

**参数**:
- `a`: 第一个类型
- `b`: 第二个类型

**返回值**: 兼容返回true，否则返回false

## 符号表 API

### 数据结构

```c
typedef struct Symbol {
    char *name;
    Type *type;
    int scope_level;
    int offset;
    bool is_global;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol **buckets;
    int size;
    int scope_level;
} SymbolTable;
```

### 函数接口

#### `SymbolTable* symbol_table_create(int size)`
创建符号表。

**参数**:
- `size`: 哈希表大小（建议使用素数）

**返回值**: 符号表指针

#### `Symbol* symbol_insert(SymbolTable *st, char *name, Type *type)`
插入符号到符号表。

**参数**:
- `st`: 符号表指针
- `name`: 符号名称
- `type`: 符号类型

**返回值**: 插入的符号指针，如果重定义则返回NULL

#### `Symbol* symbol_lookup(SymbolTable *st, char *name)`
查找符号（支持作用域链搜索）。

**参数**:
- `st`: 符号表指针
- `name`: 符号名称

**返回值**: 找到的符号指针，未找到返回NULL

#### `void enter_scope(SymbolTable *st)`
进入新作用域。

**参数**:
- `st`: 符号表指针

#### `void exit_scope(SymbolTable *st)`
退出当前作用域（删除当前层符号）。

**参数**:
- `st`: 符号表指针

## 中间代码 API

### 数据结构

```c
typedef enum {
    IR_ADD, IR_SUB, IR_MUL, IR_DIV,
    IR_FADD, IR_FSUB, IR_FMUL, IR_FDIV,
    IR_I2F, IR_F2I,
    IR_LT, IR_GT, IR_LE, IR_GE, IR_EQ, IR_NE,
    IR_ASSIGN,
    IR_LABEL, IR_GOTO, IR_IF_FALSE,
    IR_PARAM, IR_CALL, IR_RETURN
} IROpcode;

typedef struct IRInstruction {
    IROpcode op;
    char *arg1;
    char *arg2;
    char *result;
} IRInstruction;
```

### 函数接口

#### `IRBuilder* ir_builder_create()`
创建IR构建器。

**返回值**: IR构建器指针

#### `char* new_temp(IRBuilder *builder)`
生成新的临时变量名。

**参数**:
- `builder`: IR构建器指针

**返回值**: 临时变量名（如"t0", "t1"）

#### `char* new_label(IRBuilder *builder)`
生成新的标签名。

**参数**:
- `builder`: IR构建器指针

**返回值**: 标签名（如"L1", "L2"）

#### `void emit(IRBuilder *builder, IRInstruction *inst)`
添加IR指令。

**参数**:
- `builder`: IR构建器指针
- `inst`: IR指令指针

## 优化器 API

### 常量折叠

#### `Tree* fold_constants(Tree *node)`
对AST进行常量折叠优化。

**参数**:
- `node`: AST节点指针

**返回值**: 优化后的AST节点指针

### 死代码消除

#### `Tree* eliminate_dead_code(Tree *node)`
消除AST中的死代码。

**参数**:
- `node`: AST节点指针

**返回值**: 优化后的AST节点指针

## 错误处理 API

### 数据结构

```c
typedef enum {
    ERROR_LEXICAL,
    ERROR_SYNTAX,
    ERROR_SEMANTIC,
    ERROR_INTERNAL
} ErrorType;

typedef struct ErrorReporter {
    int error_count;
    int warning_count;
    int max_errors;
    FILE *output;
} ErrorReporter;
```

### 函数接口

#### `ErrorReporter* error_reporter_create(int max_errors)`
创建错误报告器。

**参数**:
- `max_errors`: 最大错误数量

**返回值**: 错误报告器指针

#### `void report_error(ErrorReporter *er, const char *filename, int line, int column, ErrorType type, const char *format, ...)`
报告错误。

**参数**:
- `er`: 错误报告器指针
- `filename`: 文件名
- `line`: 行号
- `column`: 列号
- `type`: 错误类型
- `format`: 格式化字符串（可变参数）

#### `bool should_stop_compilation(ErrorReporter *er)`
判断是否应该停止编译。

**参数**:
- `er`: 错误报告器指针

**返回值**: 应该停止返回true，否则返回false

---

**文档版本**: v1.0  
**最后更新**: 2025-12-16  
**状态**: 部分接口待实现

