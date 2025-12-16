# Mini-C 编译器 2.0 版本开发计划

## 📋 版本信息

**版本号**: 2.0  
**版本名称**: 高级特性版  
**主要目标**: 支持函数、数组、指针、结构体及活性分析优化  
**预计工作量**: 9-13周  
**状态**: 📝 计划中

## 🎯 版本目标

在 1.0 版本基础上，增加对数组、指针、函数定义和调用、结构体的支持，实现活性分析优化。

### 核心特性
- 函数定义和调用（支持递归）
- 一维和多维数组
- 指针和指针运算
- 结构体定义和成员访问
- 控制流图构建
- 活性分析优化
- 栈槽复用优化

### 实施顺序建议

根据依赖关系，建议按以下顺序实施：

1. **TASK205-206**: 函数支持（最高优先级，是其他特性的基础）
2. **TASK201-202**: 数组支持
3. **TASK203-204**: 指针支持
4. **TASK207-208**: 结构体支持
5. **TASK209-211**: 活性分析优化
6. **TASK212**: 版本发布

---

## TASK205: 函数定义和调用

**版本**: 2.0  
**状态**: 📝 计划中  
**优先级**: P0（最高）  
**预计时间**: 4天  
**依赖**: 1.0版本完成

### 任务目标

实现函数定义和调用功能，支持参数传递、返回值、递归调用，遵循 System V x86-64 调用约定。

### 子任务清单

#### 5.1 扩展类型系统支持函数类型

**内容**:
- 实现 `new_function_type(Type *return_type, Type **param_types, int param_count)`
- 实现函数类型比较 `type_equal()` 的函数类型分支
- 实现函数签名字符串化（用于错误报告）

**AI助手提示词**:

```
请帮我实现 Mini-C 编译器的函数类型支持（2.0版本 TASK205 子任务5.1）：

## 背景
- 类型系统已预留函数类型字段：return_type, param_types, param_count
- Type 结构体定义在 src/semantic/type_system.h

## 任务

在 src/semantic/type_system.c 中添加以下函数：

### 1. 创建函数类型

```c
/**
 * 创建函数类型
 * @param return_type 返回类型
 * @param param_types 参数类型数组
 * @param param_count 参数个数
 * @return 函数类型对象
 */
Type* new_function_type(Type *return_type, Type **param_types, int param_count) {
    Type *func_type = create_type(TYPE_FUNCTION, 8); // 函数指针8字节
    func_type->return_type = return_type;
    func_type->param_count = param_count;
    
    // 复制参数类型数组
    if (param_count > 0) {
        func_type->param_types = malloc(sizeof(Type*) * param_count);
        if (!func_type->param_types) {
            fprintf(stderr, "错误: 内存分配失败\n");
            return NULL;
        }
        memcpy(func_type->param_types, param_types, sizeof(Type*) * param_count);
    } else {
        func_type->param_types = NULL;
    }
    
    return func_type;
}
```

### 2. 扩展 type_equal() 函数

在 type_equal() 函数的现有代码后添加函数类型比较逻辑：

```c
// 在 type_equal() 函数中已有的指针类型比较后添加：

// 对于函数类型，需要比较返回类型和所有参数类型
if (a->kind == TYPE_FUNCTION) {
    // 返回类型必须相同
    if (!type_equal(a->return_type, b->return_type)) {
        return false;
    }
    // 参数数量必须相同
    if (a->param_count != b->param_count) {
        return false;
    }
    // 每个参数类型必须相同
    for (int i = 0; i < a->param_count; i++) {
        if (!type_equal(a->param_types[i], b->param_types[i])) {
            return false;
        }
    }
    return true;
}
```

### 3. 扩展 type_to_string() 函数

修改 type_to_string() 函数，为函数类型生成更详细的字符串表示：

```c
// 在 type_to_string() 的 switch 语句中，修改 TYPE_FUNCTION 分支：

case TYPE_FUNCTION: {
    // 动态生成函数签名字符串
    static char func_sig[256];
    snprintf(func_sig, sizeof(func_sig), "%s(", 
             type_to_string(t->return_type));
    
    for (int i = 0; i < t->param_count; i++) {
        if (i > 0) strcat(func_sig, ", ");
        strcat(func_sig, type_to_string(t->param_types[i]));
    }
    strcat(func_sig, ")");
    
    return func_sig;
}
```

### 4. 在 src/semantic/type_system.h 中添加函数声明

```c
/**
 * 创建函数类型
 * @param return_type 返回类型
 * @param param_types 参数类型数组
 * @param param_count 参数个数
 * @return 函数类型对象
 */
Type* new_function_type(Type *return_type, Type **param_types, int param_count);
```

## 测试

编写简单的测试来验证函数类型创建和比较：

```c
// 测试代码
Type *int_type = new_int_type();
Type *float_type = new_float_type();

// 创建函数类型: int func(int, float)
Type *params1[] = {int_type, float_type};
Type *func_type1 = new_function_type(int_type, params1, 2);

// 创建相同的函数类型
Type *params2[] = {int_type, float_type};
Type *func_type2 = new_function_type(int_type, params2, 2);

// 应该返回 true
assert(type_equal(func_type1, func_type2));

// 创建不同的函数类型: float func(int, float)
Type *func_type3 = new_function_type(float_type, params1, 2);

// 应该返回 false
assert(!type_equal(func_type1, func_type3));
```

请使用中文注释，确保内存管理正确。
```

#### 5.2 扩展符号表支持函数符号

**内容**:
- 修改 `symbol_insert()` 支持 `SYM_FUNCTION`
- 为函数符号添加参数列表信息
- 实现函数重定义检测

**AI助手提示词**:

```
请帮我扩展符号表以支持函数符号（2.0版本 TASK205 子任务5.2）：

## 背景
- 符号表已支持 SYM_FUNCTION 枚举值
- Symbol 结构体定义在 src/semantic/symbol_table.h

## 任务

### 1. 确认 Symbol 结构体支持函数信息

检查 src/semantic/symbol_table.h 中的 Symbol 结构体，如果缺少函数相关字段，需要添加：

```c
typedef struct Symbol {
    char *name;             // 符号名称
    SymbolKind kind;        // 符号类型（SYM_VARIABLE 或 SYM_FUNCTION）
    Type *type;             // 数据类型（来自type_system）
    
    int scope_level;        // 作用域层级（0=全局）
    int offset;             // 栈帧偏移量（字节）
    bool is_global;         // 是否全局变量
    bool is_initialized;    // 是否已初始化
    
    int line;               // 定义所在行号（用于错误报告）
    
    // 函数特有信息（2.0版本）
    // 对于 SYM_FUNCTION，这些字段从 type->param_types 和 type->return_type 获取
    // 不需要额外存储，因为 type 字段已经是函数类型
    
    struct Symbol *next;    // 哈希表链表指针
} Symbol;
```

### 2. 修改 symbol_insert() 函数

在 src/semantic/symbol_table.c 中修改 symbol_insert() 函数，确保正确处理函数符号：

```c
Symbol* symbol_insert(SymbolTable *st, const char *name, Type *type, int line) {
    if (!st || !name || !type) {
        return NULL;
    }
    
    // 1. 检查当前作用域是否已存在同名符号
    Symbol *existing = symbol_lookup_current_scope(st, name);
    if (existing) {
        // 如果是函数，检查是否是重定义
        if (existing->kind == SYM_FUNCTION && type->kind == TYPE_FUNCTION) {
            // 函数重定义错误
            return NULL;
        }
        // 变量重定义
        return NULL;
    }
    
    // 2. 创建新符号
    Symbol *sym = (Symbol*)malloc(sizeof(Symbol));
    if (!sym) {
        fprintf(stderr, "错误: 内存分配失败\n");
        return NULL;
    }
    
    // 3. 初始化符号字段
    sym->name = strdup(name);
    sym->type = type;
    sym->scope_level = st->scope_level;
    sym->line = line;
    sym->is_global = (st->scope_level == 0);
    sym->is_initialized = false;
    
    // 4. 根据类型设置符号种类
    if (type->kind == TYPE_FUNCTION) {
        sym->kind = SYM_FUNCTION;
        sym->offset = 0;  // 函数不需要栈偏移量
    } else {
        sym->kind = SYM_VARIABLE;
        // 为变量分配栈空间
        sym->offset = allocate_offset(st, type_size(type));
    }
    
    // 5. 插入哈希表
    unsigned int hash = hash_string(name) % st->size;
    sym->next = st->buckets[hash];
    st->buckets[hash] = sym;
    
    return sym;
}
```

### 3. 添加函数符号辅助函数

```c
/**
 * 检查符号是否为函数
 */
bool symbol_is_function(Symbol *sym) {
    return sym && sym->kind == SYM_FUNCTION;
}

/**
 * 获取函数参数数量
 */
int symbol_get_param_count(Symbol *sym) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return -1;
    }
    return sym->type->param_count;
}

/**
 * 获取函数参数类型
 */
Type* symbol_get_param_type(Symbol *sym, int index) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return NULL;
    }
    if (index < 0 || index >= sym->type->param_count) {
        return NULL;
    }
    return sym->type->param_types[index];
}

/**
 * 获取函数返回类型
 */
Type* symbol_get_return_type(Symbol *sym) {
    if (!sym || sym->kind != SYM_FUNCTION) {
        return NULL;
    }
    return sym->type->return_type;
}
```

### 4. 在 symbol_table.h 中添加声明

```c
/**
 * 检查符号是否为函数
 */
bool symbol_is_function(Symbol *sym);

/**
 * 获取函数参数数量
 */
int symbol_get_param_count(Symbol *sym);

/**
 * 获取函数参数类型
 */
Type* symbol_get_param_type(Symbol *sym, int index);

/**
 * 获取函数返回类型
 */
Type* symbol_get_return_type(Symbol *sym);
```

## 测试

```c
// 测试函数符号插入
SymbolTable *st = symbol_table_create(127);

// 创建函数类型: int add(int, int)
Type *int_type = new_int_type();
Type *params[] = {int_type, int_type};
Type *func_type = new_function_type(int_type, params, 2);

// 插入函数符号
Symbol *add_sym = symbol_insert(st, "add", func_type, 1);
assert(add_sym != NULL);
assert(add_sym->kind == SYM_FUNCTION);
assert(symbol_get_param_count(add_sym) == 2);

// 尝试重定义应该失败
Symbol *dup = symbol_insert(st, "add", func_type, 2);
assert(dup == NULL);

// 查找函数符号
Symbol *found = symbol_lookup(st, "add");
assert(found == add_sym);
assert(symbol_is_function(found));
```

请使用中文注释。
```

#### 5.3 扩展语法分析器

**内容**:
- 在 yacc.y 添加函数定义规则：`function_definition`
- 添加参数列表规则：`parameter_list`, `parameter_declaration`
- 添加 return 语句规则

**AI助手提示词**:

```
请帮我扩展语法分析器以支持函数定义（2.0版本 TASK205 子任务5.3）：

## 背景
- 当前项目使用的是 c-complier-master/yacc.y
- 需要扩展语法规则以支持函数定义、参数列表和 return 语句

## 任务

注意：由于当前项目结构，实际的 yacc.y 文件在 c-complier-master/ 目录下。

### 1. 在词法分析器中添加 RETURN 关键字

首先检查 c-complier-master/lex.l 是否已定义 RETURN token，如果没有则添加：

```lex
"return"    { return RETURN; }
```

### 2. 在 yacc.y 中添加 token 声明

```yacc
%token RETURN
```

### 3. 添加函数定义规则

在 yacc.y 的语法规则部分，找到顶层声明的位置，添加函数定义规则：

```yacc
/* 程序由多个声明或函数定义组成 */
program
    : external_declaration
    | program external_declaration
    ;

external_declaration
    : declaration
    | function_definition
    ;

/* 函数定义规则 */
function_definition
    : type_specifier ID '(' parameter_list ')' compound_statement {
        $$ = create_tree("FUNC_DEF", 4, $1, $2, $4, $6);
        $$->line = yylineno;
    }
    | type_specifier ID '(' ')' compound_statement {
        $$ = create_tree("FUNC_DEF", 3, $1, $2, $5);
        $$->line = yylineno;
    }
    | type_specifier ID '(' VOID ')' compound_statement {
        $$ = create_tree("FUNC_DEF", 3, $1, $2, $6);
        $$->line = yylineno;
    }
    ;

/* 参数列表规则 */
parameter_list
    : parameter_declaration {
        $$ = $1;
    }
    | parameter_list ',' parameter_declaration {
        $$ = create_tree("PARAM_LIST", 2, $1, $3);
        $$->line = yylineno;
    }
    ;

/* 单个参数声明 */
parameter_declaration
    : type_specifier ID {
        $$ = create_tree("PARAM", 2, $1, $2);
        $$->line = yylineno;
    }
    | type_specifier {
        // 无名参数（C语言允许）
        $$ = create_tree("PARAM", 1, $1);
        $$->line = yylineno;
    }
    ;
```

### 4. 添加 return 语句规则

在语句规则部分添加 return 语句：

```yacc
statement
    : expression_statement
    | compound_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    ;

jump_statement
    : RETURN expression ';' {
        $$ = create_tree("RETURN", 1, $2);
        $$->line = yylineno;
    }
    | RETURN ';' {
        $$ = create_tree("RETURN", 0);
        $$->line = yylineno;
    }
    ;
```

### 5. 确保复合语句规则正确

确保 compound_statement 规则能够正确处理函数体：

```yacc
compound_statement
    : '{' '}' {
        $$ = create_tree("COMPOUND", 0);
        $$->line = yylineno;
    }
    | '{' block_item_list '}' {
        $$ = create_tree("COMPOUND", 1, $2);
        $$->line = yylineno;
    }
    ;

block_item_list
    : block_item
    | block_item_list block_item {
        $$ = create_tree("BLOCK_LIST", 2, $1, $2);
        $$->line = yylineno;
    }
    ;

block_item
    : declaration
    | statement
    ;
```

### 6. 修改程序入口规则

确保程序可以包含多个函数定义：

```yacc
/* 原有的 program 规则可能是：
program
    : m_declaration

需要修改为支持函数定义的版本（见上面的 program 规则）
*/
```

## 测试

创建测试文件 test_function_syntax.c：

```c
// 测试1: 简单函数定义
int add(int a, int b) {
    return a + b;
}

// 测试2: 无参数函数
int get_value() {
    return 42;
}

// 测试3: void 返回类型
void print_hello() {
    return;
}

// 测试4: 多个语句的函数
int calculate(int x, int y) {
    int result = x + y;
    int doubled = result * 2;
    return doubled;
}

// 测试5: main 函数
void main() {
    int result = add(10, 20);
    output_int(result);
}
```

编译测试：
```bash
bison -d c-complier-master/yacc.y
flex c-complier-master/lex.l
gcc -o test_parser yacc.tab.c lex.yy.c c-complier-master/tree.c -lfl
./test_parser test_function_syntax.c
```

检查是否能正确解析，不应有语法错误。

请使用中文注释，注意保持与现有代码风格一致。
```

#### 5.4 实现语义分析

**内容**:
- 函数定义：插入函数符号到符号表
- 参数处理：进入函数作用域，插入参数符号
- return 语句：检查返回类型匹配
- 函数调用：参数数量和类型检查

**AI助手提示词**:

```
请帮我实现函数相关的语义分析（2.0版本 TASK205 子任务5.4）：

## 背景
- 语义分析器在 src/semantic/semantic_analyzer.c
- AST 节点由 yacc.y 生成，节点名称如 "FUNC_DEF", "PARAM", "RETURN" 等
- 已有类型系统和符号表支持

## 任务

### 1. 在 semantic_analyzer.c 中添加函数定义分析函数

```c
/**
 * 分析函数定义
 * @param sa 语义分析器
 * @param func_def 函数定义 AST 节点
 */
void analyze_function_definition(SemanticAnalyzer *sa, Tree *func_def) {
    if (!sa || !func_def) {
        return;
    }
    
    // 1. 提取函数信息
    Tree *return_type_node = func_def->child[0];  // 返回类型
    Tree *name_node = func_def->child[1];         // 函数名
    Tree *param_list = NULL;
    Tree *body = NULL;
    
    // 判断是否有参数列表
    if (func_def->childnum == 4) {
        // 有参数: type name (params) { body }
        param_list = func_def->child[2];
        body = func_def->child[3];
    } else if (func_def->childnum == 3) {
        // 无参数: type name () { body }
        param_list = NULL;
        body = func_def->child[2];
    }
    
    // 2. 构造函数类型
    Type *return_type = get_type_from_specifier(return_type_node);
    if (!return_type) {
        semantic_error(sa, func_def->line, "无效的返回类型");
        return;
    }
    
    // 提取参数类型
    Type **param_types = NULL;
    char **param_names = NULL;
    int param_count = 0;
    
    if (param_list) {
        extract_parameters(param_list, &param_types, &param_names, &param_count);
    }
    
    Type *func_type = new_function_type(return_type, param_types, param_count);
    
    // 3. 插入函数符号到全局作用域
    Symbol *func_sym = symbol_insert(sa->symbol_table, name_node->value, 
                                     func_type, func_def->line);
    if (!func_sym) {
        semantic_error(sa, func_def->line, 
                      "函数 '%s' 重定义", name_node->value);
        return;
    }
    func_sym->kind = SYM_FUNCTION;
    
    // 4. 记录当前函数（用于 return 语句检查）
    Type *prev_func_ret_type = sa->current_function_return_type;
    sa->current_function_return_type = return_type;
    
    // 5. 进入函数作用域
    enter_scope(sa->symbol_table);
    
    // 6. 插入参数符号到函数作用域
    for (int i = 0; i < param_count; i++) {
        Symbol *param_sym = symbol_insert(sa->symbol_table, param_names[i],
                                          param_types[i], func_def->line);
        if (!param_sym) {
            semantic_error(sa, func_def->line,
                          "参数 '%s' 重复定义", param_names[i]);
        } else {
            param_sym->kind = SYM_VARIABLE;
            param_sym->is_initialized = true;  // 参数视为已初始化
        }
    }
    
    // 7. 分析函数体
    if (body) {
        analyze_statement(sa, body);
    }
    
    // 8. 退出函数作用域
    exit_scope(sa->symbol_table);
    
    // 9. 恢复当前函数返回类型
    sa->current_function_return_type = prev_func_ret_type;
    
    // 10. 清理临时数组
    if (param_types) free(param_types);
    if (param_names) {
        for (int i = 0; i < param_count; i++) {
            free(param_names[i]);
        }
        free(param_names);
    }
}

/**
 * 提取参数列表信息
 */
void extract_parameters(Tree *param_list, Type ***param_types, 
                       char ***param_names, int *param_count) {
    if (!param_list) {
        *param_types = NULL;
        *param_names = NULL;
        *param_count = 0;
        return;
    }
    
    // 递归计数参数
    int count = count_parameters(param_list);
    *param_count = count;
    
    // 分配数组
    *param_types = (Type**)malloc(sizeof(Type*) * count);
    *param_names = (char**)malloc(sizeof(char*) * count);
    
    // 递归提取参数
    int index = 0;
    extract_parameters_recursive(param_list, *param_types, *param_names, &index);
}

/**
 * 递归计数参数
 */
int count_parameters(Tree *param_list) {
    if (!param_list) return 0;
    
    if (strcmp(param_list->name, "PARAM") == 0) {
        return 1;
    } else if (strcmp(param_list->name, "PARAM_LIST") == 0) {
        return count_parameters(param_list->child[0]) + 
               count_parameters(param_list->child[1]);
    }
    return 0;
}

/**
 * 递归提取参数信息
 */
void extract_parameters_recursive(Tree *param_list, Type **types, 
                                 char **names, int *index) {
    if (!param_list) return;
    
    if (strcmp(param_list->name, "PARAM") == 0) {
        // 单个参数: type name
        Type *param_type = get_type_from_specifier(param_list->child[0]);
        char *param_name = strdup(param_list->child[1]->value);
        
        types[*index] = param_type;
        names[*index] = param_name;
        (*index)++;
    } else if (strcmp(param_list->name, "PARAM_LIST") == 0) {
        // 参数列表
        extract_parameters_recursive(param_list->child[0], types, names, index);
        extract_parameters_recursive(param_list->child[1], types, names, index);
    }
}
```

### 2. 添加 return 语句分析函数

```c
/**
 * 分析 return 语句
 */
void analyze_return_statement(SemanticAnalyzer *sa, Tree *return_stmt) {
    if (!sa || !return_stmt) {
        return;
    }
    
    // 检查是否在函数内部
    if (!sa->current_function_return_type) {
        semantic_error(sa, return_stmt->line, 
                      "return 语句只能在函数内部使用");
        return;
    }
    
    // 检查是否有返回值
    if (return_stmt->childnum > 0) {
        // 有返回值
        Tree *return_expr = return_stmt->child[0];
        Type *return_type = analyze_expression(sa, return_expr);
        
        // 检查返回值类型是否匹配
        if (!type_compatible(return_type, sa->current_function_return_type)) {
            semantic_error(sa, return_stmt->line,
                          "返回类型不匹配：期望 %s，实际 %s",
                          type_to_string(sa->current_function_return_type),
                          type_to_string(return_type));
        }
    } else {
        // 无返回值（return;）
        if (sa->current_function_return_type->kind != TYPE_VOID) {
            semantic_error(sa, return_stmt->line,
                          "函数应返回 %s 类型的值",
                          type_to_string(sa->current_function_return_type));
        }
    }
}
```

### 3. 添加函数调用分析函数

```c
/**
 * 分析函数调用表达式
 */
Type* analyze_function_call(SemanticAnalyzer *sa, Tree *call_node) {
    if (!sa || !call_node) {
        return NULL;
    }
    
    // call_node 结构: CALL(ID, ARG_LIST) 或 CALL(ID)
    Tree *func_name_node = call_node->child[0];
    Tree *arg_list = (call_node->childnum > 1) ? call_node->child[1] : NULL;
    
    // 1. 查找函数符号
    Symbol *func_sym = symbol_lookup(sa->symbol_table, func_name_node->value);
    if (!func_sym) {
        semantic_error(sa, call_node->line, 
                      "未定义的函数 '%s'", func_name_node->value);
        return NULL;
    }
    
    if (func_sym->kind != SYM_FUNCTION) {
        semantic_error(sa, call_node->line,
                      "'%s' 不是函数", func_name_node->value);
        return NULL;
    }
    
    Type *func_type = func_sym->type;
    
    // 2. 检查参数数量
    int arg_count = count_arguments(arg_list);
    if (arg_count != func_type->param_count) {
        semantic_error(sa, call_node->line,
                      "函数 '%s' 需要 %d 个参数，但提供了 %d 个",
                      func_name_node->value, 
                      func_type->param_count, 
                      arg_count);
        return func_type->return_type;
    }
    
    // 3. 检查参数类型
    if (arg_list) {
        Type **arg_types = (Type**)malloc(sizeof(Type*) * arg_count);
        extract_argument_types(sa, arg_list, arg_types, 0);
        
        for (int i = 0; i < arg_count; i++) {
            Type *param_type = func_type->param_types[i];
            Type *arg_type = arg_types[i];
            
            if (!type_compatible(arg_type, param_type)) {
                semantic_error(sa, call_node->line,
                              "参数 %d 类型不匹配：期望 %s，实际 %s",
                              i + 1,
                              type_to_string(param_type),
                              type_to_string(arg_type));
            }
        }
        
        free(arg_types);
    }
    
    // 4. 返回函数返回类型
    return func_type->return_type;
}

/**
 * 计数参数
 */
int count_arguments(Tree *arg_list) {
    if (!arg_list) return 0;
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        return count_arguments(arg_list->child[0]) + 1;
    }
    return 1;  // 单个参数
}

/**
 * 提取参数类型
 */
void extract_argument_types(SemanticAnalyzer *sa, Tree *arg_list, 
                            Type **types, int index) {
    if (!arg_list) return;
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        extract_argument_types(sa, arg_list->child[0], types, index);
        types[index + count_arguments(arg_list->child[0])] = 
            analyze_expression(sa, arg_list->child[1]);
    } else {
        types[index] = analyze_expression(sa, arg_list);
    }
}
```

### 4. 修改 SemanticAnalyzer 结构体

在 src/semantic/semantic_analyzer.h 中添加字段：

```c
typedef struct SemanticAnalyzer {
    SymbolTable *symbol_table;
    int error_count;
    Type *current_function_return_type;  // 新增：当前函数返回类型
} SemanticAnalyzer;
```

### 5. 在主分析函数中添加调用

修改 analyze_statement() 或创建 analyze_declaration() 函数：

```c
void analyze_declaration(SemanticAnalyzer *sa, Tree *decl) {
    if (!decl) return;
    
    if (strcmp(decl->name, "FUNC_DEF") == 0) {
        analyze_function_definition(sa, decl);
    } else {
        // 处理变量声明...
    }
}
```

## 测试

创建测试文件测试函数语义分析：

```c
// 测试1: 正常函数定义和调用
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);  // 应该通过
}

// 测试2: 返回类型不匹配（应报错）
int get_value() {
    return 3.14;  // 错误: 返回 float 但声明为 int
}

// 测试3: 参数数量不匹配（应报错）
void test() {
    int x = add(10);  // 错误: 缺少参数
}

// 测试4: 参数类型不匹配（应报错）
void test2() {
    int x = add(10, 3.14);  // 警告或错误: 第二个参数类型不匹配
}
```

请使用中文注释，确保错误报告清晰。
```

#### 5.5 扩展 IR 指令

**内容**:
- 完善 `IR_PARAM`, `IR_CALL`, `IR_RETURN`
- 添加 `IR_FUNC_BEGIN`, `IR_FUNC_END` 标记函数边界
- 实现函数调用的参数传递序列

**AI助手提示词**:

```
请帮我扩展 IR 指令集以支持函数（2.0版本 TASK205 子任务5.5）：

## 背景
- IR 定义在 src/ir/ir.h
- IR 构建器在 src/ir/ir_builder.c

## 任务

### 1. 在 src/ir/ir.h 中扩展 IROpcode 枚举

```c
typedef enum {
    // ... 现有指令（算术、逻辑、控制流等）...
    
    // 函数相关指令（2.0版本新增）
    IR_FUNC_BEGIN,  // 函数开始标记: FUNC_BEGIN func_name
    IR_FUNC_END,    // 函数结束标记: FUNC_END func_name
    IR_PARAM,       // 参数传递: PARAM arg
    IR_CALL,        // 函数调用: result = CALL func_name arg_count
    IR_RETURN,      // 返回语句: RETURN value 或 RETURN (void)
} IROpcode;
```

### 2. 在 src/ir/ir.c 中添加指令字符串转换

修改 `ir_opcode_to_string()` 函数：

```c
const char* ir_opcode_to_string(IROpcode op) {
    switch (op) {
        // ... 现有 case ...
        
        case IR_FUNC_BEGIN: return "FUNC_BEGIN";
        case IR_FUNC_END: return "FUNC_END";
        case IR_PARAM: return "PARAM";
        case IR_CALL: return "CALL";
        case IR_RETURN: return "RETURN";
        
        default: return "UNKNOWN";
    }
}
```

### 3. 在 src/ir/ir_builder.c 中添加翻译函数

#### 翻译函数定义

```c
/**
 * 翻译函数定义
 * @param builder IR构建器
 * @param func_def 函数定义 AST 节点
 */
void translate_function_definition(IRBuilder *builder, Tree *func_def) {
    if (!builder || !func_def) {
        return;
    }
    
    // 提取函数名
    Tree *name_node = func_def->child[1];
    char *func_name = name_node->value;
    
    Tree *param_list = NULL;
    Tree *body = NULL;
    
    if (func_def->childnum == 4) {
        param_list = func_def->child[2];
        body = func_def->child[3];
    } else {
        body = func_def->child[2];
    }
    
    // 1. 生成函数开始标记
    IRInstruction *begin = create_ir_instruction(IR_FUNC_BEGIN, 
                                                 func_name, NULL, NULL);
    emit(builder, begin);
    
    // 2. 为参数生成 IR 指令
    // 参数在调用时通过寄存器或栈传递，这里只需要记录参数名
    // 实际的参数赋值由调用约定处理
    if (param_list) {
        translate_parameters(builder, param_list);
    }
    
    // 3. 翻译函数体
    if (body) {
        translate_statement(builder, body);
    }
    
    // 4. 生成函数结束标记
    IRInstruction *end = create_ir_instruction(IR_FUNC_END, 
                                               func_name, NULL, NULL);
    emit(builder, end);
}

/**
 * 翻译参数列表（生成参数接收指令）
 */
void translate_parameters(IRBuilder *builder, Tree *param_list) {
    if (!param_list) return;
    
    if (strcmp(param_list->name, "PARAM") == 0) {
        // 单个参数
        char *param_name = param_list->child[1]->value;
        // 参数在函数入口时已经在寄存器/栈中，符号表已记录
        // 这里不需要生成特殊的 IR 指令
    } else if (strcmp(param_list->name, "PARAM_LIST") == 0) {
        // 递归处理参数列表
        translate_parameters(builder, param_list->child[0]);
        translate_parameters(builder, param_list->child[1]);
    }
}
```

#### 翻译函数调用

```c
/**
 * 翻译函数调用表达式
 * @param builder IR构建器
 * @param call_node 函数调用 AST 节点
 * @param expr_type 表达式类型
 * @return 存储结果的临时变量名
 */
char* translate_function_call(IRBuilder *builder, Tree *call_node, Type *expr_type) {
    if (!builder || !call_node) {
        return NULL;
    }
    
    // 提取函数名和参数列表
    Tree *func_name_node = call_node->child[0];
    char *func_name = func_name_node->value;
    
    Tree *arg_list = (call_node->childnum > 1) ? call_node->child[1] : NULL;
    
    // 1. 翻译参数表达式，为每个参数生成 PARAM 指令
    int arg_count = 0;
    if (arg_list) {
        arg_count = translate_arguments(builder, arg_list);
    }
    
    // 2. 生成 CALL 指令
    char *result = new_temp(builder);
    char arg_count_str[32];
    snprintf(arg_count_str, sizeof(arg_count_str), "%d", arg_count);
    
    IRInstruction *call = create_ir_instruction(IR_CALL, 
                                                func_name, 
                                                arg_count_str, 
                                                result);
    emit(builder, call);
    
    return result;
}

/**
 * 翻译参数列表，生成 PARAM 指令
 * @return 参数数量
 */
int translate_arguments(IRBuilder *builder, Tree *arg_list) {
    if (!arg_list) {
        return 0;
    }
    
    int count = 0;
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        // 递归处理参数列表
        count += translate_arguments(builder, arg_list->child[0]);
        
        // 处理当前参数
        char *arg_result = translate_expression(builder, arg_list->child[1], NULL);
        IRInstruction *param = create_ir_instruction(IR_PARAM, 
                                                     arg_result, NULL, NULL);
        emit(builder, param);
        count++;
    } else {
        // 单个参数
        char *arg_result = translate_expression(builder, arg_list, NULL);
        IRInstruction *param = create_ir_instruction(IR_PARAM, 
                                                     arg_result, NULL, NULL);
        emit(builder, param);
        count = 1;
    }
    
    return count;
}
```

#### 翻译 return 语句

```c
/**
 * 翻译 return 语句
 */
void translate_return_statement(IRBuilder *builder, Tree *return_stmt) {
    if (!builder || !return_stmt) {
        return;
    }
    
    if (return_stmt->childnum > 0) {
        // 有返回值: return expr;
        Tree *return_expr = return_stmt->child[0];
        char *result = translate_expression(builder, return_expr, NULL);
        
        IRInstruction *ret = create_ir_instruction(IR_RETURN, 
                                                   result, NULL, NULL);
        emit(builder, ret);
    } else {
        // 无返回值: return;
        IRInstruction *ret = create_ir_instruction(IR_RETURN, 
                                                   NULL, NULL, NULL);
        emit(builder, ret);
    }
}
```

### 4. 在 src/ir/ir_builder.h 中添加函数声明

```c
/**
 * 翻译函数定义
 */
void translate_function_definition(IRBuilder *builder, Tree *func_def);

/**
 * 翻译函数调用
 */
char* translate_function_call(IRBuilder *builder, Tree *call_node, Type *expr_type);

/**
 * 翻译 return 语句
 */
void translate_return_statement(IRBuilder *builder, Tree *return_stmt);
```

### 5. 修改主翻译函数

在 translate_statement() 中添加对 return 语句的处理：

```c
void translate_statement(IRBuilder *builder, Tree *stmt) {
    if (!stmt) return;
    
    if (strcmp(stmt->name, "RETURN") == 0) {
        translate_return_statement(builder, stmt);
    }
    // ... 其他语句类型 ...
}
```

在 translate_expression() 中添加对函数调用的处理：

```c
char* translate_expression(IRBuilder *builder, Tree *expr, Type *expr_type) {
    if (!expr) return NULL;
    
    if (strcmp(expr->name, "CALL") == 0 || strcmp(expr->name, "FUNC_CALL") == 0) {
        return translate_function_call(builder, expr, expr_type);
    }
    // ... 其他表达式类型 ...
}
```

## IR 输出格式示例

对于以下源代码：

```c
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);
}
```

生成的 IR 应该类似：

```
FUNC_BEGIN add
t0 = a + b
RETURN t0
FUNC_END add

FUNC_BEGIN main
PARAM 10
PARAM 20
t1 = CALL add 2
result = t1
PARAM result
CALL output_int 1
FUNC_END main
```

请使用中文注释，确保 IR 格式清晰易懂。
```

#### 5.6 扩展代码生成

**内容**:
- 函数序言和尾声（prologue/epilogue）
- 参数传递（遵循 System V ABI：rdi, rsi, rdx, rcx, r8, r9）
- 返回值处理（rax 或 xmm0）
- 调用约定（栈对齐到16字节）

**AI助手提示词**:

```
请帮我实现函数的汇编代码生成（2.0版本 TASK205 子任务5.6）：

## 背景
- 代码生成器是 Python 脚本：scripts/asm_generator.py
- 输入是 IR 指令（Innercode 文件）
- 输出是 x86-64 汇编代码（NASM 语法）
- 需要遵循 System V x86-64 调用约定

## System V x86-64 调用约定要点

### 参数传递规则
- 前 6 个整数/指针参数: rdi, rsi, rdx, rcx, r8, r9
- 前 8 个浮点参数: xmm0-xmm7
- 超过6个的整数参数从右到左压栈
- 栈必须保持 16 字节对齐

### 返回值规则
- 整数/指针返回值: rax
- 浮点返回值: xmm0

### 被调用者保存寄存器
- rbx, rbp, r12-r15 必须由被调用者保存

## 任务

### 1. 在 scripts/asm_generator.py 中添加函数代码生成

首先，修改主生成循环以识别函数边界：

```python
def generate_assembly(ir_instructions):
    """从 IR 指令生成汇编代码"""
    assembly = []
    current_function = None
    function_instructions = []
    
    # 数据段
    assembly.append("section .data")
    assembly.append("    int_format db '%d', 10, 0")
    assembly.append("    float_format db '%f', 10, 0")
    assembly.append("")
    
    # 代码段
    assembly.append("section .text")
    assembly.append("    global main")
    assembly.append("    extern printf")
    assembly.append("")
    
    i = 0
    while i < len(ir_instructions):
        inst = ir_instructions[i]
        
        if inst.startswith("FUNC_BEGIN"):
            # 函数开始
            func_name = inst.split()[1]
            current_function = func_name
            function_instructions = []
            
        elif inst.startswith("FUNC_END"):
            # 函数结束，生成完整函数代码
            func_code = generate_function(function_instructions, current_function)
            assembly.extend(func_code)
            assembly.append("")
            
            current_function = None
            function_instructions = []
            
        elif current_function:
            # 在函数内部，收集指令
            function_instructions.append(inst)
        else:
            # 不在函数内部，可能是全局代码
            pass
        
        i += 1
    
    return "\n".join(assembly)
```

### 2. 实现 generate_function() 函数

```python
def generate_function(instructions, func_name):
    """
    为一个函数生成汇编代码
    
    Args:
        instructions: 函数体的 IR 指令列表
        func_name: 函数名
    
    Returns:
        汇编代码行列表
    """
    result = []
    
    # 函数标签
    result.append(f"{func_name}:")
    
    # 1. 分析函数需要的栈空间
    stack_size = calculate_stack_size(instructions)
    # 确保栈大小是 16 字节对齐
    stack_size = ((stack_size + 15) // 16) * 16
    
    # 2. 函数序言（Prologue）
    result.append("    push rbp")
    result.append("    mov rbp, rsp")
    if stack_size > 0:
        result.append(f"    sub rsp, {stack_size}")
    
    # 3. 保存参数（如果这是函数定义）
    # 假设我们从符号表或 IR 中知道参数信息
    # 这里简化处理：从前几条 IR 指令推断参数
    param_count = count_parameters(instructions)
    param_regs_int = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']
    
    for i in range(min(param_count, 6)):
        # 将寄存器中的参数保存到栈上
        param_offset = (i + 1) * 8
        result.append(f"    mov [rbp-{param_offset}], {param_regs_int[i]}")
    
    # 超过 6 个参数的处理（从调用者栈帧获取）
    for i in range(6, param_count):
        # 参数在 rbp + 16 + (i-6)*8 的位置
        caller_offset = 16 + (i - 6) * 8
        param_offset = (i + 1) * 8
        result.append(f"    mov rax, [rbp+{caller_offset}]")
        result.append(f"    mov [rbp-{param_offset}], rax")
    
    # 4. 翻译函数体指令
    for inst in instructions:
        inst_asm = translate_instruction(inst)
        result.extend(inst_asm)
    
    # 5. 函数尾声（Epilogue）
    # 注意：如果有 RETURN 指令，它会跳转到这里
    result.append(f".{func_name}_exit:")
    result.append("    leave")  # 等价于 mov rsp, rbp; pop rbp
    result.append("    ret")
    
    return result
```

### 3. 实现 CALL 指令翻译

```python
def translate_call_instruction(inst):
    """
    翻译 CALL 指令
    
    格式: result = CALL func_name arg_count
    或: CALL func_name arg_count (无返回值)
    """
    result = []
    
    # 解析指令
    parts = inst.split()
    if '=' in inst:
        # 有返回值
        result_var = parts[0]
        func_name = parts[3]
        arg_count = int(parts[4])
    else:
        # 无返回值
        result_var = None
        func_name = parts[1]
        arg_count = int(parts[2])
    
    # 1. 栈对齐检查
    # System V ABI 要求调用前栈对齐到 16 字节
    # 当前 rbp 已对齐，如果压入奇数个 8 字节参数，需要额外对齐
    if arg_count > 6:
        extra_args = arg_count - 6
        if extra_args % 2 == 1:
            result.append("    sub rsp, 8  ; 栈对齐")
    
    # 2. 传递参数
    # 注意：参数已经通过 PARAM 指令准备好
    # 这里需要从栈或临时变量加载到寄存器
    
    # 参数寄存器
    param_regs = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']
    
    # 假设参数按顺序存储在变量或临时变量中
    # 实际实现需要维护参数列表
    
    # 简化实现：假设参数已在正确位置
    # 完整实现需要在遇到 PARAM 指令时记录参数
    
    # 3. 调用函数
    result.append(f"    call {func_name}")
    
    # 4. 清理栈（超过 6 个参数的情况）
    if arg_count > 6:
        extra_args = arg_count - 6
        bytes_to_clean = extra_args * 8
        if extra_args % 2 == 1:
            bytes_to_clean += 8  # 包括对齐填充
        result.append(f"    add rsp, {bytes_to_clean}")
    
    # 5. 保存返回值
    if result_var:
        # 整数返回值在 rax，浮点在 xmm0
        # 这里假设是整数
        offset = get_variable_offset(result_var)
        result.append(f"    mov [rbp-{offset}], rax  ; 保存返回值")
    
    return result
```

### 4. 实现 PARAM 指令翻译

```python
# 全局参数栈
param_stack = []

def translate_param_instruction(inst):
    """
    翻译 PARAM 指令
    
    格式: PARAM value
    
    参数按遇到的顺序压入参数栈，
    在遇到 CALL 指令时统一加载到寄存器或栈
    """
    global param_stack
    
    parts = inst.split()
    param_value = parts[1]
    
    # 将参数加入参数栈
    param_stack.append(param_value)
    
    return []  # PARAM 指令本身不生成代码


def prepare_call_arguments():
    """
    在 CALL 之前准备参数
    
    将参数栈中的参数加载到寄存器或压栈
    """
    global param_stack
    result = []
    
    param_regs_int = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']
    
    # 前 6 个参数放入寄存器
    for i in range(min(len(param_stack), 6)):
        param = param_stack[i]
        
        # 如果参数是立即数
        if param.isdigit() or (param[0] == '-' and param[1:].isdigit()):
            result.append(f"    mov {param_regs_int[i]}, {param}")
        else:
            # 参数是变量，从栈加载
            offset = get_variable_offset(param)
            result.append(f"    mov {param_regs_int[i]}, [rbp-{offset}]")
    
    # 超过 6 个的参数压栈（从右到左）
    for i in range(len(param_stack) - 1, 5, -1):
        param = param_stack[i]
        
        if param.isdigit() or (param[0] == '-' and param[1:].isdigit()):
            result.append(f"    push {param}")
        else:
            offset = get_variable_offset(param)
            result.append(f"    push qword [rbp-{offset}]")
    
    # 清空参数栈
    param_stack = []
    
    return result
```

### 5. 实现 RETURN 指令翻译

```python
def translate_return_instruction(inst, func_name):
    """
    翻译 RETURN 指令
    
    格式: RETURN value
    或: RETURN (无返回值)
    """
    result = []
    
    parts = inst.split()
    
    if len(parts) > 1:
        # 有返回值
        return_value = parts[1]
        
        # 将返回值加载到 rax（整数）或 xmm0（浮点）
        if return_value.isdigit() or (return_value[0] == '-' and return_value[1:].isdigit()):
            result.append(f"    mov rax, {return_value}")
        else:
            offset = get_variable_offset(return_value)
            result.append(f"    mov rax, [rbp-{offset}]")
    
    # 跳转到函数退出
    result.append(f"    jmp .{func_name}_exit")
    
    return result
```

### 6. 辅助函数

```python
def calculate_stack_size(instructions):
    """
    计算函数需要的栈空间
    
    分析所有临时变量和局部变量
    """
    variables = set()
    
    for inst in instructions:
        # 提取变量名（简化实现）
        # 实际需要更复杂的解析
        if '=' in inst:
            parts = inst.split('=')
            var = parts[0].strip()
            if var.startswith('t') or var.isidentifier():
                variables.add(var)
    
    # 每个变量 8 字节（包括临时变量）
    return len(variables) * 8


def count_parameters(instructions):
    """
    从函数体 IR 推断参数数量
    
    这是简化实现，实际应该从符号表获取
    """
    # 可以通过分析前几条指令或符号表确定
    return 0  # 实际实现需要改进
```

## 测试

测试文件 test_function_call.c：

```c
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);
}
```

编译并运行：

```bash
# 生成 IR
./compiler test_function_call.c

# 生成汇编
python3 scripts/asm_generator.py < Innercode > output.asm

# 汇编和链接
nasm -f elf64 output.asm -o output.o
gcc output.o -o output -no-pie

# 运行
./output  # 应输出 30
```

请使用中文注释，确保调用约定正确实现。
```

### 验收标准

- [ ] 能正确解析函数定义语法
- [ ] 函数符号正确插入符号表
- [ ] 参数类型检查正确
- [ ] 返回值类型检查正确
- [ ] 生成正确的 IR 指令序列
- [ ] 生成的汇编代码可编译运行
- [ ] 支持递归调用
- [ ] 遵循 System V 调用约定
- [ ] 栈对齐正确（16字节）
- [ ] 参数传递正确（前6个用寄存器）
- [ ] 返回值处理正确

### 注意事项

- **调用约定**：严格遵循 System V x86-64 ABI
  - 参数顺序：rdi, rsi, rdx, rcx, r8, r9
  - 超过6个参数使用栈传递（从右到左压栈）
  - 栈必须保持 16 字节对齐

- **寄存器保存**：
  - 被调用者保存：rbx, rbp, r12-r15
  - 调用者保存：其他通用寄存器

- **浮点支持**：
  - 浮点参数使用 xmm0-xmm7
  - 浮点返回值使用 xmm0
  - printf 调用浮点参数时需要设置 rax

- **内存管理**：
  - 函数类型的 param_types 数组需要正确释放
  - 参数名称字符串需要正确管理

- **错误处理**：
  - 参数数量不匹配
  - 参数类型不匹配
  - 返回类型不匹配
  - 函数重定义
  - 在非函数内使用 return

- **测试覆盖**：
  - 无参数函数
  - 多参数函数（≤6个和>6个）
  - 递归函数
  - 嵌套函数调用
  - void 返回类型
  - 不同类型的参数和返回值

---

## TASK206: 函数参数和返回值优化

**版本**: 2.0  
**状态**: 📝 计划中  
**优先级**: P1  
**预计时间**: 2天  
**依赖**: TASK205

### 任务目标

支持数组和指针作为函数参数，实现数组参数退化为指针，优化结构体参数传递。

### 子任务清单

#### 6.1 支持数组和指针作为参数

**内容**:
- 数组参数退化为指针
- 指针参数传递

**AI助手提示词**:

```
请帮我实现数组和指针作为函数参数（2.0版本 TASK206 子任务6.1）：

## 背景
- 函数基础功能已实现（TASK205）
- 类型系统已支持数组和指针类型

## 任务

在 C 语言中，数组作为函数参数时会退化为指针。需要在语义分析阶段实现这个转换。

### 1. 修改参数处理函数

在 src/semantic/semantic_analyzer.c 的 extract_parameters() 函数中添加数组退化逻辑：

```c
/**
 * 提取参数列表信息（支持数组退化）
 */
void extract_parameters(Tree *param_list, Type ***param_types, 
                       char ***param_names, int *param_count) {
    if (!param_list) {
        *param_types = NULL;
        *param_names = NULL;
        *param_count = 0;
        return;
    }
    
    // 递归计数参数
    int count = count_parameters(param_list);
    *param_count = count;
    
    // 分配数组
    *param_types = (Type**)malloc(sizeof(Type*) * count);
    *param_names = (char**)malloc(sizeof(char*) * count);
    
    // 递归提取参数，应用数组退化规则
    int index = 0;
    extract_parameters_with_decay(param_list, *param_types, *param_names, &index);
}

/**
 * 递归提取参数信息（应用数组退化）
 */
void extract_parameters_with_decay(Tree *param_list, Type **types, 
                                   char **names, int *index) {
    if (!param_list) return;
    
    if (strcmp(param_list->name, "PARAM") == 0) {
        // 单个参数
        Type *param_type = get_type_from_specifier(param_list->child[0]);
        char *param_name = strdup(param_list->child[1]->value);
        
        // 数组退化为指针
        if (param_type->kind == TYPE_ARRAY) {
            param_type = new_pointer_type(param_type->base);
        }
        
        types[*index] = param_type;
        names[*index] = param_name;
        (*index)++;
    } else if (strcmp(param_list->name, "PARAM_LIST") == 0) {
        // 参数列表
        extract_parameters_with_decay(param_list->child[0], types, names, index);
        extract_parameters_with_decay(param_list->child[1], types, names, index);
    }
}
```

### 2. 在函数调用时处理数组实参

在函数调用分析中，当传递数组时，需要传递数组的地址：

```c
/**
 * 分析函数调用参数（处理数组传递）
 */
void check_function_call_arguments(SemanticAnalyzer *sa, Tree *call_node,
                                   Symbol *func_sym) {
    Tree *arg_list = (call_node->childnum > 1) ? call_node->child[1] : NULL;
    Type *func_type = func_sym->type;
    
    if (!arg_list && func_type->param_count == 0) {
        return;  // 无参数，匹配
    }
    
    // 检查每个参数
    int arg_index = 0;
    check_arguments_recursive(sa, arg_list, func_type, &arg_index);
}

void check_arguments_recursive(SemanticAnalyzer *sa, Tree *arg, 
                               Type *func_type, int *arg_index) {
    if (!arg) return;
    
    if (strcmp(arg->name, "ARG_LIST") == 0) {
        check_arguments_recursive(sa, arg->child[0], func_type, arg_index);
        check_arguments_recursive(sa, arg->child[1], func_type, arg_index);
    } else {
        // 单个参数
        Type *arg_type = analyze_expression(sa, arg);
        Type *param_type = func_type->param_types[*arg_index];
        
        // 数组自动退化为指针
        Type *effective_arg_type = arg_type;
        if (arg_type->kind == TYPE_ARRAY) {
            effective_arg_type = new_pointer_type(arg_type->base);
        }
        
        // 检查类型兼容性
        if (!type_compatible(effective_arg_type, param_type)) {
            semantic_error(sa, arg->line,
                          "参数 %d 类型不匹配：期望 %s，实际 %s",
                          *arg_index + 1,
                          type_to_string(param_type),
                          type_to_string(effective_arg_type));
        }
        
        (*arg_index)++;
    }
}
```

### 3. IR 生成时处理数组参数

在 src/ir/ir_builder.c 中，当数组作为参数传递时，生成取地址指令：

```c
/**
 * 翻译参数（处理数组退化）
 */
char* translate_argument(IRBuilder *builder, Tree *arg) {
    // 分析参数类型
    Type *arg_type = get_expression_type(arg);
    
    if (arg_type->kind == TYPE_ARRAY) {
        // 数组参数：传递数组首地址
        if (arg->kind == NODE_ID) {
            // 简单的数组变量：生成取地址指令
            char *array_name = arg->value;
            char *addr_temp = new_temp(builder);
            
            IRInstruction *addr_of = create_ir_instruction(IR_ADDR_OF,
                                                          array_name, 
                                                          NULL, 
                                                          addr_temp);
            emit(builder, addr_of);
            
            return addr_temp;
        } else {
            // 复杂表达式：正常翻译
            return translate_expression(builder, arg, arg_type);
        }
    } else {
        // 非数组参数：正常翻译
        return translate_expression(builder, arg, arg_type);
    }
}

/**
 * 修改 translate_arguments() 使用新的参数翻译函数
 */
int translate_arguments(IRBuilder *builder, Tree *arg_list) {
    if (!arg_list) {
        return 0;
    }
    
    int count = 0;
    
    if (strcmp(arg_list->name, "ARG_LIST") == 0) {
        count += translate_arguments(builder, arg_list->child[0]);
        
        // 处理当前参数（使用数组退化逻辑）
        char *arg_result = translate_argument(builder, arg_list->child[1]);
        IRInstruction *param = create_ir_instruction(IR_PARAM, 
                                                     arg_result, NULL, NULL);
        emit(builder, param);
        count++;
    } else {
        // 单个参数
        char *arg_result = translate_argument(builder, arg_list);
        IRInstruction *param = create_ir_instruction(IR_PARAM, 
                                                     arg_result, NULL, NULL);
        emit(builder, param);
        count = 1;
    }
    
    return count;
}
```

### 4. 代码生成时处理指针参数

在 scripts/asm_generator.py 中，指针参数按8字节整数处理：

```python
def prepare_pointer_argument(param_value):
    """
    准备指针类型的参数
    
    指针和数组都作为 8 字节地址传递
    """
    result = []
    
    # 如果是地址（临时变量）
    if param_value.startswith('t'):
        # 临时变量，从栈加载
        offset = get_temp_offset(param_value)
        result.append(f"    mov rax, [rbp-{offset}]")
    else:
        # 变量地址，使用 lea
        offset = get_variable_offset(param_value)
        result.append(f"    lea rax, [rbp-{offset}]")
    
    return result
```

## 测试

测试文件 test_array_param.c：

```c
// 测试：数组作为参数
void modify_array(int arr[], int len) {
    for (int i = 0; i < len; i++) {
        arr[i] = arr[i] * 2;
    }
}

void main() {
    int data[5] = {1, 2, 3, 4, 5};
    
    modify_array(data, 5);
    
    output_int(data[0]);  // 应输出 2
    output_int(data[4]);  // 应输出 10
}
```

测试文件 test_pointer_param.c：

```c
// 测试：指针作为参数
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void main() {
    int x = 10, y = 20;
    swap(&x, &y);
    
    output_int(x);  // 应输出 20
    output_int(y);  // 应输出 10
}
```

请使用中文注释。
```

### 验收标准

- [ ] 数组参数正确退化为指针
- [ ] 指针参数正确传递
- [ ] 数组元素可在被调函数中修改
- [ ] 指针解引用正确
- [ ] 生成的汇编代码正确

### 注意事项

- **数组退化规则**：
  - 数组名在表达式中自动转换为指向首元素的指针
  - 数组参数声明 `int arr[]` 等价于 `int *arr`
  - 多维数组第一维可省略大小

- **指针传递**：
  - 指针作为8字节整数传递
  - 使用 lea 指令获取变量地址
  - 指针算术需要考虑元素大小

- **内存安全**：
  - 不进行运行时数组边界检查
  - 需要程序员确保不越界访问

---

## TASK201-204: 数组和指针支持

由于篇幅限制，这里提供关键任务的简要说明。完整的 AI 助手提示词格式与 TASK205 相同。

### TASK201: 一维数组支持
- 数组声明和初始化
- 数组下标访问
- 数组地址计算 IR 指令
- 栈上数组分配

### TASK202: 多维数组支持
- 多维数组声明
- 行优先内存布局
- 多维下标计算

### TASK203: 指针基础支持
- 指针声明
- 取地址运算符 `&`
- 解引用运算符 `*`

### TASK204: 指针运算
- 指针算术（加减整数）
- 指针比较
- 多重指针

---

## TASK207-208: 结构体支持

### TASK207: 结构体定义
- 结构体类型定义
- 成员偏移量计算
- 内存对齐

### TASK208: 结构体成员访问
- `.` 运算符
- `->` 运算符
- 结构体赋值

---

## TASK209-211: 活性分析优化（创新点）

### TASK209: 控制流图(CFG)构建
- 基本块划分算法
- 前驱后继关系构建
- CFG 数据结构

### TASK210: 活性分析实现
- USE/DEF 集合计算
- 活跃变量迭代求解
- 数据流方程

### TASK211: 栈槽复用优化
- 干涉图构建
- 图着色算法
- 栈空间优化

---

## TASK212: 2.0版本发布

**版本**: 2.0  
**状态**: 📝 计划中  
**优先级**: P0  
**预计时间**: 3天  
**依赖**: TASK211

### 任务目标

完成 2.0 版本的最终验收、测试、文档更新和发布。

### 子任务清单

#### 12.1 完整功能测试

- 运行所有测试用例
- 函数测试（基础调用、递归、多参数）
- 数组测试（一维、多维、初始化）
- 指针测试（基础、运算、多重指针）
- 结构体测试（定义、成员访问）
- 活性分析效果验证

#### 12.2 性能测试和优化效果量化

- 编译速度测试
- 生成代码性能测试
- 活性分析前后对比
  - 栈空间使用量
  - 变量数量
  - 优化百分比

#### 12.3 文档更新

- 更新 README.md
- 更新 CHANGELOG.md
- 完善 docs/design.md
- 编写 RELEASE_NOTES_v2.0.md

#### 12.4 代码审查和清理

- 检查代码规范
- 添加必要注释
- 清理调试代码
- 内存泄漏检查

#### 12.5 Git 发布操作

- 创建 release-2.0 分支
- 打 v2.0 标签
- 推送到 GitHub
- 创建 Release

### 验收标准

- [ ] 所有测试用例通过（目标 95%+）
- [ ] 支持函数定义和调用（包括递归）
- [ ] 支持一维和多维数组
- [ ] 支持指针和指针运算
- [ ] 支持结构体定义和成员访问
- [ ] 活性分析正确实现
- [ ] 栈空间优化有效（减少 30%+）
- [ ] 文档完整更新
- [ ] 代码质量良好
- [ ] 成功发布到 GitHub

### 注意事项

- 确保与 1.0 版本的向后兼容性
- 文档要详细说明新特性
- 提供完整的使用示例
- 记录已知问题和限制
- 准备演示程序展示新特性

---

## 📊 总体进度跟踪

| 任务 | 名称 | 状态 | 预计时间 |
|------|------|------|---------|
| TASK205 | 函数定义和调用 | 📝 计划中 | 4天 |
| TASK206 | 函数参数优化 | 📝 计划中 | 2天 |
| TASK201 | 一维数组支持 | 📝 计划中 | 3天 |
| TASK202 | 多维数组支持 | 📝 计划中 | 2天 |
| TASK203 | 指针基础支持 | 📝 计划中 | 3天 |
| TASK204 | 指针运算 | 📝 计划中 | 2天 |
| TASK207 | 结构体定义 | 📝 计划中 | 3天 |
| TASK208 | 结构体成员访问 | 📝 计划中 | 2天 |
| TASK209 | CFG构建 | 📝 计划中 | 3天 |
| TASK210 | 活性分析 | 📝 计划中 | 4天 |
| TASK211 | 栈槽复用 | 📝 计划中 | 3天 |
| TASK212 | 版本发布 | 📝 计划中 | 3天 |

**总计**: 34天

---

**文档版本**: v1.0  
**创建日期**: 2025-12-16  
**维护者**: Mini-C 开发团队

