---
name: Mini-C 2.0 详细开发计划
overview: 为 Mini-C 编译器 2.0 版本创建详细的任务计划，包含函数、数组、指针、结构体支持以及活性分析优化。每个任务包含详细的子任务清单、AI助手提示词、验收标准和注意事项。
todos:
  - id: task205
    content: 实现函数定义和调用支持
    status: completed
  - id: task206
    content: 实现函数参数和返回值优化
    status: pending
    dependencies:
      - task205
  - id: task201
    content: 实现一维数组支持
    status: completed
    dependencies:
      - task205
  - id: task202
    content: 实现多维数组支持
    status: completed
    dependencies:
      - task201
  - id: task203
    content: 实现指针基础支持
    status: pending
    dependencies:
      - task201
  - id: task204
    content: 实现指针运算和解引用
    status: pending
    dependencies:
      - task203
  - id: task207
    content: 实现结构体定义
    status: pending
    dependencies:
      - task203
  - id: task208
    content: 实现结构体成员访问
    status: pending
    dependencies:
      - task207
  - id: task209
    content: 实现控制流图(CFG)构建
    status: pending
    dependencies:
      - task206
      - task202
      - task204
      - task208
  - id: task210
    content: 实现活性分析算法
    status: pending
    dependencies:
      - task209
  - id: task211
    content: 实现栈槽复用优化
    status: pending
    dependencies:
      - task210
  - id: task212
    content: 2.0版本测试和发布
    status: pending
    dependencies:
      - task211
---

# Mini-C 编译器 2.0 版本详细开发计划

## 概述

为 Mini-C 编译器 2.0 版本的 12 个任务（TASK201-TASK212）创建详细的开发计划，格式参考 1.0 版本。基于代码审查，类型系统已预留所有必要字段，符号表已支持嵌套作用域，可直接进行扩展。

## 实施顺序建议

根据依赖关系，建议调整实施顺序：

1. **TASK205-206**：函数支持（最高优先级，是其他特性的基础）
2. **TASK201-202**：数组支持
3. **TASK203-204**：指针支持
4. **TASK207-208**：结构体支持
5. **TASK209-211**：活性分析优化
6. **TASK212**：版本发布

## 核心文件

将修改以下现有文件：

- [`src/semantic/type_system.h`](src/semantic/type_system.h) - 已预留字段，需实现函数类型相关函数
- [`src/semantic/type_system.c`](src/semantic/type_system.c) - 已实现数组/指针构造函数
- [`src/semantic/symbol_table.h`](src/semantic/symbol_table.h) - 已支持 SYM_FUNCTION
- [`src/frontend/yacc.y`](src/frontend/yacc.y) - 需要扩展语法规则
- [`src/ir/ir_builder.c`](src/ir/ir_builder.c) - 需要扩展 IR 指令集

## 详细任务计划

### TASK205: 函数定义和调用（优先实现）

**预计时间**: 4天

**依赖**: 1.0版本完成

#### 子任务清单

- **5.1 扩展类型系统支持函数类型**
                                                                                                                                - 实现 `new_function_type(Type *return_type, Type **param_types, int param_count)`
                                                                                                                                - 实现函数类型比较 `type_equal()` 的函数类型分支
                                                                                                                                - 实现函数签名字符串化（用于错误报告）

- **5.2 扩展符号表支持函数符号**
                                                                                                                                - 修改 `symbol_insert()` 支持 `SYM_FUNCTION`
                                                                                                                                - 为函数符号添加参数列表信息
                                                                                                                                - 实现函数重载检测（参数不同）

- **5.3 扩展语法分析器**
                                                                                                                                - 在 yacc.y 添加函数定义规则：`function_definition`
                                                                                                                                - 添加参数列表规则：`parameter_list`, `parameter_declaration`
                                                                                                                                - 添加 return 语句规则

- **5.4 实现语义分析**
                                                                                                                                - 函数定义：插入函数符号到符号表
                                                                                                                                - 参数处理：进入函数作用域，插入参数符号
                                                                                                                                - return 语句：检查返回类型匹配
                                                                                                                                - 函数调用：参数数量和类型检查

- **5.5 扩展 IR 指令**
                                                                                                                                - 完善 `IR_PARAM`, `IR_CALL`, `IR_RETURN`
                                                                                                                                - 添加 `IR_FUNC_BEGIN`, `IR_FUNC_END` 标记函数边界
                                                                                                                                - 实现函数调用的参数传递序列

- **5.6 扩展代码生成**
                                                                                                                                - 函数序言和尾声（prologue/epilogue）
                                                                                                                                - 参数传递（遵循 System V ABI：rdi, rsi, rdx, rcx, r8, r9）
                                                                                                                                - 返回值处理（rax 或 xmm0）
                                                                                                                                - 调用约定（栈对齐到16字节）

#### AI助手提示词

````
请帮我实现 Mini-C 编译器的函数定义和调用功能（2.0版本 TASK205）：

## 背景
- 类型系统已预留函数类型字段：return_type, param_types, param_count
- 符号表已支持 SYM_FUNCTION 符号类型
- IR 已有 PARAM, CALL, RETURN 指令

## 需求
1. 支持函数定义：`int add(int a, int b) { return a + b; }`
2. 支持函数调用：`int c = add(10, 20);`
3. 支持递归调用
4. 遵循 System V x86-64 调用约定

## 任务1: 扩展类型系统 (src/semantic/type_system.c)

添加函数类型构造函数：
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
        memcpy(func_type->param_types, param_types, sizeof(Type*) * param_count);
    }
    
    return func_type;
}
````

扩展 `type_equal()` 支持函数类型比较：

```c
// 在 type_equal() 函数中添加：
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

## 任务2: 扩展语法分析 (src/frontend/yacc.y)

添加函数定义规则：

```yacc
function_definition
    : type_specifier ID '(' parameter_list ')' compound_statement {
        $$ = create_tree("FUNC_DEF", 4, $1, $2, $4, $6);
    }
    | type_specifier ID '(' ')' compound_statement {
        $$ = create_tree("FUNC_DEF", 3, $1, $2, $5);
    }
    ;

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration {
        $$ = create_tree("PARAM_LIST", 2, $1, $3);
    }
    ;

parameter_declaration
    : type_specifier ID {
        $$ = create_tree("PARAM", 2, $1, $2);
    }
    ;

return_statement
    : RETURN expression ';' {
        $$ = create_tree("RETURN", 1, $2);
    }
    | RETURN ';' {
        $$ = create_tree("RETURN", 0);
    }
    ;
```

## 任务3: 语义分析 (src/semantic/semantic_analyzer.c)

函数定义处理：

```c
void analyze_function_definition(SemanticAnalyzer *sa, Tree *func_def) {
    // 1. 提取函数信息
    Tree *return_type_node = func_def->child[0];
    Tree *name_node = func_def->child[1];
    Tree *param_list = func_def->child[2];
    Tree *body = func_def->child[3];
    
    // 2. 构造函数类型
    Type *return_type = get_type_from_node(return_type_node);
    Type **param_types = extract_param_types(param_list);
    int param_count = count_params(param_list);
    Type *func_type = new_function_type(return_type, param_types, param_count);
    
    // 3. 插入函数符号（全局作用域）
    Symbol *func_sym = symbol_insert(sa->symbol_table, name_node->value, func_type, func_def->line);
    if (!func_sym) {
        semantic_error(func_def->line, "函数 '%s' 重定义", name_node->value);
        return;
    }
    func_sym->kind = SYM_FUNCTION;
    
    // 4. 进入函数作用域
    enter_scope(sa->symbol_table);
    
    // 5. 插入参数符号
    insert_parameters(sa, param_list);
    
    // 6. 分析函数体
    analyze_compound_statement(sa, body);
    
    // 7. 退出函数作用域
    exit_scope(sa->symbol_table);
}
```

函数调用检查：

```c
Type* analyze_function_call(SemanticAnalyzer *sa, Tree *call_node) {
    Tree *func_name = call_node->child[0];
    Tree *arg_list = call_node->child[1];
    
    // 1. 查找函数符号
    Symbol *func_sym = symbol_lookup(sa->symbol_table, func_name->value);
    if (!func_sym || func_sym->kind != SYM_FUNCTION) {
        semantic_error(call_node->line, "未定义的函数 '%s'", func_name->value);
        return NULL;
    }
    
    Type *func_type = func_sym->type;
    
    // 2. 检查参数数量
    int arg_count = count_arguments(arg_list);
    if (arg_count != func_type->param_count) {
        semantic_error(call_node->line, 
            "函数 '%s' 需要 %d 个参数，但提供了 %d 个",
            func_name->value, func_type->param_count, arg_count);
        return NULL;
    }
    
    // 3. 检查参数类型
    for (int i = 0; i < arg_count; i++) {
        Type *arg_type = analyze_expression(sa, get_argument(arg_list, i));
        Type *param_type = func_type->param_types[i];
        
        if (!type_compatible(arg_type, param_type)) {
            semantic_error(call_node->line,
                "参数 %d 类型不匹配：期望 %s，实际 %s",
                i+1, type_to_string(param_type), type_to_string(arg_type));
        }
    }
    
    // 4. 返回函数返回类型
    return func_type->return_type;
}
```

## 任务4: IR 生成 (src/ir/ir_builder.c)

添加函数边界指令：

```c
typedef enum {
    // ... 现有指令 ...
    IR_FUNC_BEGIN,  // 函数开始
    IR_FUNC_END,    // 函数结束
} IROpcode;
```

翻译函数定义：

```c
void translate_function_definition(IRBuilder *builder, Tree *func_def) {
    char *func_name = func_def->child[1]->value;
    
    // 1. 生成函数开始标记
    emit(builder, new_ir_inst(IR_FUNC_BEGIN, func_name, NULL, NULL));
    
    // 2. 为参数生成赋值指令（从寄存器/栈加载）
    Tree *param_list = func_def->child[2];
    translate_parameters(builder, param_list);
    
    // 3. 翻译函数体
    Tree *body = func_def->child[3];
    translate_compound_statement(builder, body);
    
    // 4. 生成函数结束标记
    emit(builder, new_ir_inst(IR_FUNC_END, func_name, NULL, NULL));
}
```

翻译函数调用：

```c
char* translate_function_call(IRBuilder *builder, Tree *call_node) {
    char *func_name = call_node->child[0]->value;
    Tree *arg_list = call_node->child[1];
    
    // 1. 翻译参数表达式，生成 PARAM 指令
    int arg_count = count_arguments(arg_list);
    for (int i = 0; i < arg_count; i++) {
        char *arg_result = translate_expression(builder, get_argument(arg_list, i));
        emit(builder, new_ir_inst(IR_PARAM, arg_result, NULL, NULL));
    }
    
    // 2. 生成 CALL 指令
    char *result = new_temp(builder);
    emit(builder, new_ir_inst(IR_CALL, func_name, int_to_str(arg_count), result));
    
    return result;
}
```

## 任务5: 代码生成 (scripts/asm_generator.py)

函数定义汇编生成：

```python
def generate_function(ir_instructions, func_name):
    result = []
    result.append(f"{func_name}:")
    
    # 函数序言
    result.append("    push rbp")
    result.append("    mov rbp, rsp")
    result.append(f"    sub rsp, {stack_size}")  # 分配栈空间
    
    # 保存参数到局部变量
    # rdi -> 第1个参数, rsi -> 第2个, rdx -> 第3个, rcx -> 第4个, r8 -> 第5个, r9 -> 第6个
    param_regs = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']
    for i, param in enumerate(parameters):
        if i < 6:
            result.append(f"    mov [rbp-{param_offset}], {param_regs[i]}")
        else:
            # 超过6个参数从栈上取
            result.append(f"    mov rax, [rbp+{16 + (i-6)*8}]")
            result.append(f"    mov [rbp-{param_offset}], rax")
    
    # ... 函数体代码生成 ...
    
    # 函数尾声
    result.append("    leave")
    result.append("    ret")
    
    return result
```

函数调用汇编生成：

```python
def generate_call(func_name, arg_count, args):
    result = []
    
    # 1. 栈对齐检查（System V ABI要求16字节对齐）
    if arg_count > 6:
        extra_args = arg_count - 6
        if extra_args % 2 == 1:
            result.append("    sub rsp, 8")  # 对齐
    
    # 2. 传递参数
    param_regs = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']
    for i in range(min(arg_count, 6)):
        result.append(f"    mov {param_regs[i]}, {args[i]}")
    
    # 超过6个参数压栈（从右到左）
    for i in range(arg_count-1, 5, -1):
        result.append(f"    push {args[i]}")
    
    # 3. 调用函数
    result.append(f"    call {func_name}")
    
    # 4. 清理栈
    if arg_count > 6:
        result.append(f"    add rsp, {(arg_count-6)*8}")
    
    # 5. 结果在 rax（整数）或 xmm0（浮点）
    
    return result
```

## 测试用例

创建 `tests/function/test_basic_function.c`：

```c
// 基础函数调用测试
int add(int a, int b) {
    return a + b;
}

int multiply(int x, int y) {
    return x * y;
}

void main() {
    int result1 = add(10, 20);
    int result2 = multiply(3, 4);
    int result3 = add(result1, result2);
    output_int(result3);  // 应输出 42
}
```

创建 `tests/function/test_recursive.c`：

```c
// 递归函数测试
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

void main() {
    int result = factorial(5);
    output_int(result);  // 应输出 120
}
```

请按顺序实现以上功能，使用中文注释。

```

#### 验收标准

- [ ] 能正确解析函数定义语法
- [ ] 函数符号正确插入符号表
- [ ] 参数类型检查正确
- [ ] 返回值类型检查正确
- [ ] 生成正确的 IR 指令序列
- [ ] 生成的汇编代码可编译运行
- [ ] 支持递归调用
- [ ] 遵循 System V 调用约定

#### 注意事项

- 函数参数超过6个需要使用栈传递
- 栈必须保持16字节对齐
- 浮点参数使用 xmm 寄存器传递
- 函数返回前必须恢复 rbp 和 rsp

---

### TASK206: 函数参数和返回值优化

**预计时间**: 2天  
**依赖**: TASK205

#### 子任务清单

- **6.1 支持数组和指针作为参数**
  - 数组参数退化为指针
  - 指针参数传递

- **6.2 支持结构体参数**
  - 小结构体（≤16字节）通过寄存器传递
  - 大结构体通过指针传递

- **6.3 支持可变参数（可选）**
  - 实现 `printf` 风格的可变参数
  - 使用 va_list

- **6.4 返回值优化**
  - 结构体返回值优化（RVO）
  - 多返回值通过指针参数

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的函数参数和返回值高级特性（2.0版本 TASK206）：

## 任务1: 数组参数退化

在语义分析中，将数组类型参数转换为指针：

```c
void process_function_parameters(SemanticAnalyzer *sa, Tree *param_list) {
    for (each parameter) {
        Type *param_type = get_type(param);
        
        // 数组退化为指针
        if (param_type->kind == TYPE_ARRAY) {
            param_type = new_pointer_type(param_type->base);
        }
        
        // 插入参数符号
        symbol_insert(sa->symbol_table, param_name, param_type, line);
    }
}
```

## 任务2: 结构体参数传递

根据大小选择传递方式：

```c
void generate_struct_param_passing(Type *struct_type, int param_index) {
    if (struct_type->size <= 16) {
        // 小结构体：通过寄存器传递
        // 将结构体成员打包到 rdi, rsi 等寄存器
    } else {
        // 大结构体：通过栈传递（传递指针）
        result.append(f"    lea rax, [rbp-{struct_offset}]")
        result.append(f"    mov {param_regs[param_index]}, rax")
    }
}
```

## 任务3: 返回值优化

为大结构体实现返回值优化：

```c
// 调用方传递返回值缓冲区地址（作为隐藏的第一个参数）
char* translate_struct_returning_call(IRBuilder *builder, Tree *call) {
    // 1. 分配返回值缓冲区
    char *ret_buf = allocate_temp_struct(builder, return_type);
    
    // 2. 作为隐藏参数传递
    emit(builder, new_ir_inst(IR_PARAM, ret_buf, NULL, NULL));
    
    // 3. 正常参数
    // ...
    
    // 4. 调用函数
    emit(builder, new_ir_inst(IR_CALL, func_name, arg_count_str, ret_buf));
    
    return ret_buf;
}
```

测试用例：

```c
void process_array(int arr[], int len) {
    for (int i = 0; i < len; i++) {
        arr[i] = arr[i] * 2;
    }
}

void main() {
    int data[5] = {1, 2, 3, 4, 5};
    process_array(data, 5);
    output_int(data[0]);  // 应输出 2
}
```

请使用中文注释，严格遵循 System V ABI。

```

#### 验收标准

- [ ] 数组参数正确退化为指针
- [ ] 指针参数正确传递
- [ ] 结构体参数根据大小选择传递方式
- [ ] 返回值正确处理
- [ ] 大结构体返回使用隐藏参数

---

### TASK201: 一维数组支持

**预计时间**: 3天  
**依赖**: TASK205

#### 子任务清单

- **1.1 扩展语法分析器**
  - 添加数组声明语法：`int arr[10];`
  - 添加数组初始化：`int arr[3] = {1, 2, 3};`
  - 添加数组下标访问：`arr[i]`

- **1.2 语义分析**
  - 验证数组大小为正整数常量
  - 检查数组下标类型（必须是整数）
  - 数组越界检查（编译期常量）

- **1.3 IR 生成**
  - 添加数组地址计算指令：`IR_ARRAY_ADDR`
  - 数组元素访问翻译

- **1.4 代码生成**
  - 栈上数组分配
  - 数组元素地址计算：`base + index * element_size`
  - 数组初始化代码生成

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的一维数组支持（2.0版本 TASK201）：

## 背景

- 类型系统已有 `new_array_type(Type *base, int length)` 函数
- Type 结构体已有 `base` 和 `array_len` 字段

## 任务1: 扩展语法 (src/frontend/yacc.y)

```yacc
declaration
    : type_specifier declarator_list ';'
    | type_specifier declarator '[' INT10 ']' ';' {
        $$ = create_tree("ARRAY_DECL", 3, $1, $2, $4);
    }
    | type_specifier declarator '[' INT10 ']' '=' initializer_list ';' {
        $$ = create_tree("ARRAY_DECL_INIT", 4, $1, $2, $4, $6);
    }
    ;

initializer_list
    : '{' expression_list '}' {
        $$ = $2;
    }
    ;

postfix_expression
    : primary_expression '[' expression ']' {
        $$ = create_tree("ARRAY_ACCESS", 2, $1, $3);
    }
    ;
```

## 任务2: 语义分析 (src/semantic/semantic_analyzer.c)

数组声明处理：

```c
void analyze_array_declaration(SemanticAnalyzer *sa, Tree *decl) {
    Type *base_type = get_type_from_node(decl->child[0]);
    char *name = decl->child[1]->value;
    int length = atoi(decl->child[2]->value);
    
    // 验证长度
    if (length <= 0) {
        semantic_error(decl->line, "数组长度必须为正整数");
        return;
    }
    
    // 创建数组类型
    Type *array_type = new_array_type(base_type, length);
    
    // 插入符号表
    Symbol *sym = symbol_insert(sa->symbol_table, name, array_type, decl->line);
    if (!sym) {
        semantic_error(decl->line, "数组 '%s' 重定义", name);
        return;
    }
    
    // 分配栈空间
    sym->offset = allocate_offset(sa->symbol_table, array_type->size);
}
```

数组访问检查：

```c
Type* analyze_array_access(SemanticAnalyzer *sa, Tree *access) {
    Tree *array_node = access->child[0];
    Tree *index_node = access->child[1];
    
    // 1. 检查数组类型
    Symbol *array_sym = symbol_lookup(sa->symbol_table, array_node->value);
    if (!array_sym || array_sym->type->kind != TYPE_ARRAY) {
        semantic_error(access->line, "'%s' 不是数组", array_node->value);
        return NULL;
    }
    
    // 2. 检查下标类型
    Type *index_type = analyze_expression(sa, index_node);
    if (!is_integer_type(index_type)) {
        semantic_error(access->line, "数组下标必须是整数类型");
        return NULL;
    }
    
    // 3. 编译期越界检查
    if (index_node->kind == NODE_CONST) {
        int index = atoi(index_node->value);
        if (index < 0 || index >= array_sym->type->array_len) {
            semantic_error(access->line, "数组下标越界：%d（数组长度：%d）",
                index, array_sym->type->array_len);
        }
    }
    
    // 4. 返回元素类型
    return array_sym->type->base;
}
```

## 任务3: IR 生成 (src/ir/ir_builder.c)

添加数组地址计算指令：

```c
typedef enum {
    // ... 现有指令 ...
    IR_ARRAY_ADDR,  // 计算数组元素地址: result = base + index * size
    IR_LOAD,        // 从地址加载: result = *addr
    IR_STORE,       // 存储到地址: *addr = value
} IROpcode;
```

翻译数组访问：

```c
char* translate_array_access(IRBuilder *builder, Tree *access) {
    char *array_name = access->child[0]->value;
    char *index_result = translate_expression(builder, access->child[1]);
    
    // 获取数组符号信息
    Symbol *array_sym = symbol_lookup(symbol_table, array_name);
    int element_size = array_sym->type->base->size;
    
    // 1. 计算偏移量: offset = index * element_size
    char *offset_temp = new_temp(builder);
    emit(builder, new_ir_inst(IR_MUL, index_result, 
        int_to_str(element_size), offset_temp));
    
    // 2. 计算元素地址: addr = array_base + offset
    char *addr_temp = new_temp(builder);
    emit(builder, new_ir_inst(IR_ARRAY_ADDR, array_name, 
        offset_temp, addr_temp));
    
    // 3. 加载元素值
    char *value_temp = new_temp(builder);
    emit(builder, new_ir_inst(IR_LOAD, addr_temp, NULL, value_temp));
    
    return value_temp;
}
```

## 任务4: 代码生成 (scripts/asm_generator.py)

数组地址计算：

```python
def generate_array_access(array_name, index, element_size):
    result = []
    
    # 1. 加载下标到寄存器
    result.append(f"    mov rax, {index}")
    
    # 2. 乘以元素大小
    result.append(f"    imul rax, {element_size}")
    
    # 3. 计算绝对地址
    result.append(f"    lea rbx, [rbp-{array_offset}]")  # 数组基地址
    result.append(f"    add rax, rbx")
    
    # 4. 访问元素
    result.append(f"    mov ecx, [rax]")  # 读取元素值
    
    return result
```

数组初始化：

```python
def generate_array_initialization(array_name, init_values):
    result = []
    
    for i, value in enumerate(init_values):
        offset = array_base_offset + i * element_size
        result.append(f"    mov dword [rbp-{offset}], {value}")
    
    return result
```

## 测试用例

`tests/array/test_basic_array.c`:

```c
void main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int sum = 0;
    
    for (int i = 0; i < 5; i++) {
        sum = sum + arr[i];
    }
    
    output_int(sum);  // 应输出 150
}
```

`tests/array/test_array_modify.c`:

```c
void main() {
    int data[3];
    data[0] = 100;
    data[1] = 200;
    data[2] = 300;
    
    output_int(data[1]);  // 应输出 200
}
```

请使用中文注释，确保内存布局正确。

```

#### 验收标准

- [ ] 正确解析数组声明语法
- [ ] 数组类型正确创建
- [ ] 数组下标访问类型检查正确
- [ ] 编译期常量越界检查有效
- [ ] 数组初始化正确
- [ ] 生成的汇编代码正确访问数组元素

---

### TASK202: 多维数组支持

**预计时间**: 2天  
**依赖**: TASK201

#### 子任务清单

- **2.1 扩展语法**
  - 多维数组声明：`int matrix[3][4];`
  - 多维数组初始化

- **2.2 类型系统**
  - 递归构造多维数组类型
  - 类型推导

- **2.3 地址计算**
  - 行优先布局
  - 多维下标转一维偏移

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的多维数组支持（2.0版本 TASK202）：

## 核心算法

多维数组地址计算（行优先）：

```
address = base + (i * col_size + j) * element_size
```

对于三维数组 `arr[d1][d2][d3]`：

```
address = base + ((i * d2 + j) * d3 + k) * element_size
```

## 类型构造

```c
// 创建二维数组类型
Type* create_2d_array_type(Type *base, int rows, int cols) {
    // 先创建列数组：base[cols]
    Type *col_array = new_array_type(base, cols);
    
    // 再创建行数组：col_array[rows]
    Type *row_array = new_array_type(col_array, rows);
    
    return row_array;
}
```

## 地址计算示例

```c
char* translate_2d_array_access(IRBuilder *builder, Tree *access) {
    // arr[i][j]
    char *array_name = access->child[0]->value;
    char *i_result = translate_expression(builder, access->child[1]);
    char *j_result = translate_expression(builder, access->child[2]);
    
    Type *array_type = get_symbol_type(array_name);
    int col_size = array_type->base->array_len;  // 列数
    int element_size = array_type->base->base->size;
    
    // offset = (i * col_size + j) * element_size
    char *temp1 = new_temp(builder);
    emit(builder, new_ir_inst(IR_MUL, i_result, int_to_str(col_size), temp1));
    
    char *temp2 = new_temp(builder);
    emit(builder, new_ir_inst(IR_ADD, temp1, j_result, temp2));
    
    char *offset = new_temp(builder);
    emit(builder, new_ir_inst(IR_MUL, temp2, int_to_str(element_size), offset));
    
    // 加载元素
    char *addr = new_temp(builder);
    emit(builder, new_ir_inst(IR_ARRAY_ADDR, array_name, offset, addr));
    
    char *value = new_temp(builder);
    emit(builder, new_ir_inst(IR_LOAD, addr, NULL, value));
    
    return value;
}
```

测试用例：

```c
void main() {
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    
    int sum = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            sum = sum + matrix[i][j];
        }
    }
    
    output_int(sum);  // 应输出 21
}
```

请使用中文注释。

```

#### 验收标准

- [ ] 支持二维数组声明和访问
- [ ] 支持三维数组（可选）
- [ ] 地址计算正确
- [ ] 多维数组初始化正确

---

### TASK203: 指针基础支持

**预计时间**: 3天  
**依赖**: TASK201

#### 子任务清单

- **3.1 扩展语法**
  - 指针声明：`int *ptr;`
  - 取地址运算符：`&var`
  - 解引用运算符：`*ptr`

- **3.2 语义分析**
  - 指针类型创建
  - 取地址：检查左值
  - 解引用：检查指针类型

- **3.3 IR 生成**
  - `IR_ADDR_OF`: 取地址
  - `IR_DEREF`: 解引用

- **3.4 代码生成**
  - 指针赋值
  - 指针解引用

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的指针基础支持（2.0版本 TASK203）：

## 背景

- 类型系统已有 `new_pointer_type(Type *base)` 函数
- 指针在 x86-64 上是 8 字节

## 任务1: 扩展语法 (src/frontend/yacc.y)

```yacc
declaration
    : type_specifier '*' declarator ';' {
        $$ = create_tree("POINTER_DECL", 2, $1, $3);
    }
    | type_specifier '*' declarator '=' expression ';' {
        $$ = create_tree("POINTER_DECL_INIT", 3, $1, $3, $5);
    }
    ;

unary_expression
    : '&' unary_expression {
        $$ = create_tree("ADDR_OF", 1, $2);
    }
    | '*' unary_expression {
        $$ = create_tree("DEREF", 1, $2);
    }
    ;
```

## 任务2: 语义分析 (src/semantic/semantic_analyzer.c)

取地址分析：

```c
Type* analyze_addr_of(SemanticAnalyzer *sa, Tree *addr_of) {
    Tree *operand = addr_of->child[0];
    
    // 1. 检查是否为左值
    if (!is_lvalue(operand)) {
        semantic_error(addr_of->line, "取地址运算符的操作数必须是左值");
        return NULL;
    }
    
    // 2. 获取操作数类型
    Type *operand_type = analyze_expression(sa, operand);
    
    // 3. 返回指针类型
    return new_pointer_type(operand_type);
}
```

解引用分析：

```c
Type* analyze_deref(SemanticAnalyzer *sa, Tree *deref) {
    Tree *operand = deref->child[0];
    
    // 1. 分析操作数类型
    Type *operand_type = analyze_expression(sa, operand);
    
    // 2. 检查是否为指针类型
    if (operand_type->kind != TYPE_POINTER) {
        semantic_error(deref->line, "解引用运算符的操作数必须是指针类型");
        return NULL;
    }
    
    // 3. 返回基类型
    return operand_type->base;
}
```

## 任务3: IR 生成 (src/ir/ir_builder.c)

```c
typedef enum {
    // ... 现有指令 ...
    IR_ADDR_OF,  // 取地址: result = &var
    IR_DEREF,    // 解引用: result = *ptr
} IROpcode;
```

翻译取地址：

```c
char* translate_addr_of(IRBuilder *builder, Tree *addr_of) {
    Tree *operand = addr_of->child[0];
    char *var_name = operand->value;
    
    // 生成取地址指令
    char *result = new_temp(builder);
    emit(builder, new_ir_inst(IR_ADDR_OF, var_name, NULL, result));
    
    return result;
}
```

翻译解引用：

```c
char* translate_deref(IRBuilder *builder, Tree *deref) {
    Tree *operand = deref->child[0];
    char *ptr_result = translate_expression(builder, operand);
    
    // 生成解引用指令（实际是加载）
    char *result = new_temp(builder);
    emit(builder, new_ir_inst(IR_LOAD, ptr_result, NULL, result));
    
    return result;
}
```

## 任务4: 代码生成 (scripts/asm_generator.py)

取地址：

```python
def generate_addr_of(var_name):
    # 计算变量地址
    offset = get_variable_offset(var_name)
    result.append(f"    lea rax, [rbp-{offset}]")  # 加载有效地址
    result.append(f"    mov [rbp-{temp_offset}], rax")  # 保存指针
```

解引用：

```python
def generate_deref(ptr_temp):
    result.append(f"    mov rax, [rbp-{ptr_offset}]")  # 加载指针值
    result.append(f"    mov ebx, [rax]")  # 通过指针读取值
```

## 测试用例

`tests/pointer/test_basic_pointer.c`:

```c
void main() {
    int a = 100;
    int *p = &a;
    int b = *p;
    
    output_int(b);  // 应输出 100
}
```

`tests/pointer/test_pointer_modify.c`:

```c
void main() {
    int x = 10;
    int *ptr = &x;
    *ptr = 20;
    
    output_int(x);  // 应输出 20
}
```

请使用中文注释，注意指针大小为8字节。

```

#### 验收标准

- [ ] 正确解析指针声明
- [ ] 取地址运算符正确
- [ ] 解引用运算符正确
- [ ] 通过指针修改变量值
- [ ] 指针赋值正确

---

### TASK204: 指针运算和解引用

**预计时间**: 2天  
**依赖**: TASK203

#### 子任务清单

- **4.1 指针算术运算**
  - `ptr + n`: 指针加整数
  - `ptr - n`: 指针减整数
  - `ptr1 - ptr2`: 指针相减

- **4.2 指针比较**
  - `ptr1 == ptr2`, `ptr1 != ptr2`
  - `ptr1 < ptr2`, `ptr1 > ptr2`

- **4.3 多重指针**
  - `int **pp`: 指向指针的指针
  - `***ppp`: 多层解引用

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的指针运算（2.0版本 TASK204）：

## 指针算术规则

1. **指针 + 整数**：`ptr + n` 实际偏移 `n * sizeof(*ptr)` 字节
2. **指针 - 整数**：`ptr - n` 实际偏移 `-n * sizeof(*ptr)` 字节
3. **指针 - 指针**：`(ptr1 - ptr2) / sizeof(*ptr)`

## 语义分析

```c
Type* analyze_pointer_arithmetic(SemanticAnalyzer *sa, Tree *binop) {
    Type *left_type = analyze_expression(sa, binop->child[0]);
    Type *right_type = analyze_expression(sa, binop->child[1]);
    
    // 情况1: 指针 + 整数
    if (left_type->kind == TYPE_POINTER && is_integer_type(right_type)) {
        return left_type;  // 结果类型是指针
    }
    
    // 情况2: 整数 + 指针
    if (is_integer_type(left_type) && right_type->kind == TYPE_POINTER) {
        return right_type;
    }
    
    // 情况3: 指针 - 整数
    if (binop->op == '-' && left_type->kind == TYPE_POINTER && is_integer_type(right_type)) {
        return left_type;
    }
    
    // 情况4: 指针 - 指针
    if (binop->op == '-' && left_type->kind == TYPE_POINTER && right_type->kind == TYPE_POINTER) {
        if (!type_equal(left_type->base, right_type->base)) {
            semantic_error(binop->line, "指针相减必须指向相同类型");
        }
        return new_int_type();  // 结果是整数（元素个数差）
    }
    
    semantic_error(binop->line, "非法的指针运算");
    return NULL;
}
```

## IR 生成

```c
char* translate_pointer_add(IRBuilder *builder, Tree *add) {
    char *ptr_result = translate_expression(builder, add->child[0]);
    char *offset_result = translate_expression(builder, add->child[1]);
    Type *ptr_type = get_expression_type(add->child[0]);
    
    int element_size = ptr_type->base->size;
    
    // 1. 将偏移量乘以元素大小
    char *byte_offset = new_temp(builder);
    emit(builder, new_ir_inst(IR_MUL, offset_result, 
        int_to_str(element_size), byte_offset));
    
    // 2. 指针加偏移
    char *result = new_temp(builder);
    emit(builder, new_ir_inst(IR_ADD, ptr_result, byte_offset, result));
    
    return result;
}
```

## 多重指针

```c
Type* analyze_multi_level_pointer(SemanticAnalyzer *sa, Tree *decl) {
    Type *base_type = get_base_type(decl);
    int pointer_level = count_asterisks(decl);  // 数星号个数
    
    // 递归构造指针类型
    Type *result = base_type;
    for (int i = 0; i < pointer_level; i++) {
        result = new_pointer_type(result);
    }
    
    return result;
}
```

测试用例：

```c
void main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;  // 数组退化为指针
    
    int a = *(p + 2);  // 等价于 arr[2]
    output_int(a);  // 应输出 30
    
    int **pp = &p;  // 二级指针
    int b = **pp;
    output_int(b);  // 应输出 10
}
```

请使用中文注释。

```

#### 验收标准

- [ ] 指针加减整数正确
- [ ] 指针相减正确
- [ ] 多重指针支持
- [ ] 指针和数组配合使用

---

### TASK207-208: 结构体支持（简要版）

由于篇幅限制，结构体部分提供简要指导：

- 定义 `StructMember` 链表结构
- 实现 `new_struct_type(char *name, StructMember *members)`
- 计算成员偏移量（考虑对齐）
- 支持 `.` 和 `->` 运算符
- 结构体嵌套和数组

---

### TASK209-211: 活性分析优化（创新点）

**预计时间**: 10天（3+4+3）

#### TASK209: 控制流图(CFG)构建

**子任务清单**：

- **9.1 基本块划分**
  - 识别基本块入口（函数入口、标签、跳转目标）
  - 识别基本块出口（跳转、返回）

- **9.2 构建CFG**
  - 基本块节点
  - 前驱后继边

#### AI助手提示词

```

请帮我实现 Mini-C 编译器的控制流图构建（2.0版本 TASK209）：

## 核心数据结构

```c
typedef struct BasicBlock {
    int id;                         // 基本块编号
    IRInstruction **instructions;   // 指令列表
    int inst_count;                 // 指令数量
    
    struct BasicBlock **predecessors;  // 前驱基本块
    int pred_count;
    
    struct BasicBlock **successors;    // 后继基本块
    int succ_count;
    
    // 活性分析使用
    Set *live_in;    // 块入口活跃变量集合
    Set *live_out;   // 块出口活跃变量集合
    Set *use;        // 使用的变量
    Set *def;        // 定义的变量
} BasicBlock;

typedef struct CFG {
    BasicBlock **blocks;
    int block_count;
    BasicBlock *entry;
    BasicBlock *exit;
} CFG;
```

## 基本块划分算法

```c
CFG* build_cfg(IRInstruction **instructions, int count) {
    // 1. 找到所有 leader（基本块的第一条指令）
    bool *is_leader = calloc(count, sizeof(bool));
    is_leader[0] = true;  // 第一条指令是 leader
    
    for (int i = 0; i < count; i++) {
        IRInstruction *inst = instructions[i];
        
        // 跳转指令的目标是 leader
        if (inst->op == IR_GOTO || inst->op == IR_IF_FALSE) {
            int target_index = find_label_index(inst->result);
            is_leader[target_index] = true;
        }
        
        // 跳转指令的下一条是 leader
        if (inst->op == IR_GOTO || inst->op == IR_IF_FALSE || inst->op == IR_RETURN) {
            if (i + 1 < count) {
                is_leader[i + 1] = true;
            }
        }
        
        // 函数入口是 leader
        if (inst->op == IR_FUNC_BEGIN) {
            is_leader[i] = true;
        }
    }
    
    // 2. 根据 leader 划分基本块
    CFG *cfg = create_cfg();
    BasicBlock *current_block = NULL;
    
    for (int i = 0; i < count; i++) {
        if (is_leader[i]) {
            // 创建新基本块
            current_block = create_basic_block(cfg->block_count++);
            add_block_to_cfg(cfg, current_block);
        }
        
        // 添加指令到当前基本块
        add_instruction_to_block(current_block, instructions[i]);
    }
    
    // 3. 建立前驱后继关系
    for (int i = 0; i < cfg->block_count; i++) {
        BasicBlock *block = cfg->blocks[i];
        IRInstruction *last_inst = block->instructions[block->inst_count - 1];
        
        if (last_inst->op == IR_GOTO) {
            // 无条件跳转：添加目标基本块为后继
            BasicBlock *target = find_block_by_label(cfg, last_inst->result);
            add_successor(block, target);
        } else if (last_inst->op == IR_IF_FALSE) {
            // 条件跳转：添加两个后继（跳转目标和顺序执行）
            BasicBlock *target = find_block_by_label(cfg, last_inst->result);
            add_successor(block, target);
            if (i + 1 < cfg->block_count) {
                add_successor(block, cfg->blocks[i + 1]);
            }
        } else if (last_inst->op != IR_RETURN) {
            // 顺序执行：添加下一个基本块为后继
            if (i + 1 < cfg->block_count) {
                add_successor(block, cfg->blocks[i + 1]);
            }
        }
    }
    
    return cfg;
}
```

测试：为简单的 if-else 程序构建CFG，验证基本块划分正确。

请使用中文注释。

```

#### TASK210: 活性分析实现

**核心算法**：数据流方程迭代求解

```

USE[B] = 指令使用但未定义的变量

DEF[B] = 指令定义的变量

LIVE_OUT[B] = ∪ LIVE_IN[S] (S是B的后继)

LIVE_IN[B] = USE[B] ∪ (LIVE_OUT[B] - DEF[B])

````

#### TASK211: 栈槽复用优化

**核心思想**：构造干涉图，使用图着色算法分配栈槽

```c
// 如果两个变量的活跃范围重叠，则它们"干涉"，不能共享栈槽
void build_interference_graph(CFG *cfg) {
    for each block in cfg {
        for each variable v1 in LIVE_OUT[block] {
            for each variable v2 in LIVE_OUT[block] {
                if (v1 != v2) {
                    add_interference_edge(v1, v2);
                }
            }
        }
    }
}

// 贪心着色算法
void allocate_stack_slots(InterferenceGraph *ig) {
    for each variable v {
        // 找一个与 v 的邻居颜色都不冲突的颜色（栈槽）
        int slot = find_available_slot(v, ig);
        assign_slot(v, slot);
    }
}
````

---

### TASK212: 2.0版本发布

参考 1.0 版本的 TASK012，包括：

- 完整功能测试（函数、数组、指针、结构体）
- 活性分析效果量化（栈空间减少百分比）
- 文档更新
- Git 标签和 GitHub Release

## 总结

本计划为 Mini-C 2.0 版本的 12 个任务提供了详细的实施指南，包括：

- 完整的子任务分解
- 可直接使用的AI助手提示词
- 核心数据结构和算法示例
- 验收标准清单

建议按照调整后的顺序实施，优先完成函数支持，再逐步添加数组、指针、结构体，最后实现活性分析优化。