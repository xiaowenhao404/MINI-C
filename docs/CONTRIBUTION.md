# Mini-C 编译器 - 参考代码与贡献说明

> 本文档记录了项目的参考来源、自主实现的功能、以及代码修改的主要思路。

## 📋 目录

- [参考项目](#参考项目)
- [原始项目分析](#原始项目分析)
- [自主贡献内容](#自主贡献内容)
- [核心修改详解](#核心修改详解)
- [技术难点与解决方案](#技术难点与解决方案)
- [创新点总结](#创新点总结)

---

## 参考项目

### 主要参考

| 项目 | 链接 | 说明 |
|------|------|------|
| **SimpleOnlineCompiler** | [https://github.com/cyclesw/SimpleOnlineCompiler](https://github.com/cyclesw/SimpleOnlineCompiler) | 基础编译器框架和 Web 界面参考 |

### 参考资源

| 资源 | 说明 |
|------|------|
| 《编译原理》(龙书) | 编译器理论基础 |
| Flex & Bison 官方文档 | 词法/语法分析器实现 |
| NASM 官方手册 | x86-64 汇编指令参考 |
| System V AMD64 ABI | 函数调用约定 |

---

## 原始项目分析

### 原始项目功能

原始 SimpleOnlineCompiler 项目提供了：

1. **基础词法分析**：识别关键字、标识符、整数常量
2. **基础语法分析**：简单的表达式和语句解析
3. **简单中间代码**：基本的三地址码生成
4. **Web 界面框架**：Flask 后端 + HTML 前端

### 原始项目局限

| 局限 | 描述 |
|------|------|
| 数据类型单一 | 仅支持整数，无浮点数 |
| 无数组支持 | 不支持数组声明和访问 |
| 无指针支持 | 不支持指针操作 |
| 控制流问题 | while/for 循环跳转行号错误 |
| 汇编生成简单 | 仅支持基础运算 |
| 函数支持有限 | 仅支持简单函数定义 |

---

## 自主贡献内容

### 新增功能列表

| 类别 | 功能 | 实现文件 | 贡献度 |
|------|------|----------|--------|
| **浮点运算** | float 类型支持 | `yacc.y`, `asm_generator.py` | 100% 自主 |
| **数组操作** | 一维数组声明、访问、赋值 | `yacc.y`, `tree.c`, `asm_generator.py` | 100% 自主 |
| **指针操作** | 取地址、解引用、指针赋值 | `yacc.y`, `tree.c`, `asm_generator.py` | 100% 自主 |
| **标签系统** | 符号标签替代行号跳转 | `yacc.y`, `inner.c` | 100% 自主 |
| **64位汇编** | x86-64 代码生成优化 | `asm_generator.py` | 100% 自主 |
| **Web 界面增强** | 多标签页输出显示 | `app.py`, `script.js` | 80% 自主 |

### 关键 Bug 修复

| Bug | 原因 | 修复方案 | 文件 |
|-----|------|----------|------|
| while 循环跳转错误 | 硬编码行号 | 实现标签系统 | `yacc.y`, `inner.c` |
| for 循环跳转错误 | 同上 | 同上 | `yacc.y`, `inner.c` |
| 指针中间代码错误 | `createTree(1)` 返回原节点 | 保存临时变量 | `yacc.y` |
| 栈帧大小不足 | 硬编码 144 字节 | 扩展至 1024 字节 | `asm_generator.py` |
| 64位地址处理错误 | 使用 32 位寄存器 | 改用 rax/rbx | `asm_generator.py` |
| store 指令解析错误 | 格式解析不完整 | 完善解析逻辑 | `asm_generator.py` |

---

## 核心修改详解

### 1. 标签系统实现

**问题**：原始代码使用硬编码行号进行跳转，但中间代码行号在生成过程中会变化。

**解决方案**：实现符号标签系统

```c
// yacc.y 中的修改

// 新增全局标签计数器
int label_id = 0;

// while 语句生成标签
while_expression
    : WHILE { 
        // 生成唯一标签
        char* start = malloc(32);
        char* body = malloc(32);
        char* end = malloc(32);
        sprintf(start, "WHILE_%d_START", label_id);
        sprintf(body, "WHILE_%d_BODY", label_id);
        sprintf(end, "WHILE_%d_END", label_id++);
        // 保存标签供后续使用
    }
    '(' expression ')' statement
    {
        $$ = whileOpr("while", start, body, end, $4, $6);
    }
;
```

```c
// inner.c 中的 replaceLabels 函数

void replaceLabels(char *code) {
    // 第一遍：收集标签定义及其对应行号
    // @WHILE_0_START: 对应后面第一个有行号的行
    
    // 第二遍：替换 goto @LABEL 为 goto 行号
    // goto @WHILE_0_END -> goto 25
    
    // 第三遍：删除标签定义
    // 移除 @WHILE_0_START:
}
```

### 2. 数组操作实现

**新增中间代码指令**：

| 指令 | 格式 | 说明 |
|------|------|------|
| `alloc` | `arr = alloc 20` | 分配 20 字节数组空间 |
| `store` | `store value addr` | 存储值到地址 |
| `load` | `result = load addr` | 从地址加载值 |

**yacc.y 修改**：

```c
// 数组声明
declaration
    : type_specifier IDENTIFIER '[' NUMBER ']'
    {
        // 计算数组大小 = 元素数量 * 4 (int 为 4 字节)
        int size = $4 * 4;
        // 生成 alloc 指令
        $$->code = mergeCode(5, "#", $2, " = alloc ", toString(size), "\n");
    }
;

// 数组访问（读取）
postfix_expression
    : IDENTIFIER '[' expression ']'
    {
        // 计算偏移量
        char* temp_offset = mergeCode(2, "t", toString(inner_count++));
        char* temp_addr = mergeCode(2, "t", toString(inner_count++));
        char* temp_result = mergeCode(2, "t", toString(inner_count++));
        
        // 生成代码：
        // temp_offset = index * 4
        // temp_addr = arr + temp_offset
        // temp_result = load temp_addr
    }
;

// 数组赋值
assignment_expression
    : unary_expression '=' assignment_expression
    {
        if (/* 左侧是数组访问 */) {
            // 生成 store 指令
            // store value addr
        }
    }
;
```

**asm_generator.py 修改**：

```python
# 处理 alloc 指令
elif op_type == "内存分配":
    arr_name = four[i][1]
    size = int(four[i][2])
    
    # 分配栈空间
    offset = 8 * data.index(arr_name) + 8
    
    # 预留数组空间，防止后续变量覆盖
    num_slots = (size + 3) // 4
    for _ in range(num_slots):
        data[data.index(-1)] = f"__arr_reserved_{arr_name}_{_}"
    
    # 生成 lea 指令获取数组基址
    result += [tab + f"lea rax, [rbp-{offset + size}]"]
    result += [tab + f"mov [rbp-{offset}], rax"]

# 处理 store 指令
elif op_type == "内存写入":
    value_var = four[i][1]
    addr_var = four[i][2]
    
    # 加载值到 ebx
    if isinstance(value_var, int):
        result += [tab + f"mov ebx, {value_var}"]
    else:
        result += [tab + f"mov ebx, [rbp-{val_offset}]"]
    
    # 加载地址到 rax，存储到内存
    result += [tab + f"mov rax, [rbp-{addr_offset}]"]
    result += [tab + f"mov [rax], ebx"]
```

### 3. 指针操作实现

**新增中间代码指令**：

| 指令 | 格式 | 说明 |
|------|------|------|
| `addr` | `result = addr var` | 取变量地址 |
| `load` | `result = load ptr` | 解引用（读取） |
| `store` | `store value ptr` | 解引用赋值（写入） |

**yacc.y 修改**：

```c
// 取地址运算符 &
unary_expression
    : '&' cast_expression
    {
        char* src_inner = $2->inner;  // 保存原始值
        $$ = createTree("ADDR_OF", 1, $2);
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        $$->code = mergeCode(6, $2->code, 
            "#", $$->inner, " = addr ", src_inner, "\n");
    }
;

// 解引用运算符 *
unary_expression
    : '*' cast_expression
    {
        // 关键：先保存 $2->inner，避免被覆盖
        char* src_inner = $2->inner;
        char* src_code = $2->code;
        
        $$ = createTree("DEREF", 1, $2);
        $$->content = src_inner;  // 保存指针变量名，用于赋值
        $$->inner = mergeCode(2, "t", toString(inner_count++));
        $$->code = mergeCode(6, src_code,
            "#", $$->inner, " = load ", src_inner, "\n");
    }
;
```

**关键 Bug 修复**：

```c
// 问题：createTree(name, 1, $2) 返回 $2 本身，导致 $$ == $2
// 结果：$$->inner = "t47" 会覆盖 $2->inner
// 导致生成 "t47 = addr t47" 的错误代码

// 解决方案：先保存 $2 的值
char* src_inner = $2->inner;  // 先保存！
$$ = createTree("DEREF", 1, $2);
$$->inner = ...;  // 现在安全赋值
$$->code = mergeCode(..., src_inner, ...);  // 使用保存的值
```

### 4. 浮点运算实现

**yacc.y 修改**：

```c
// 浮点数字面量识别（lex.l）
{float}    { yylval.fval = atof(yytext); return FLOAT_NUM; }

// 浮点运算（yacc.y）
expression
    : expression '+' expression
    {
        if (is_float($1) || is_float($3)) {
            // 浮点加法
            $$->inner = mergeCode(2, "t", toString(inner_count++));
            $$->code = mergeCode(8, $1->code, $3->code,
                "#", $$->inner, " = ", $1->inner, " f+ ", $3->inner, "\n");
        } else {
            // 整数加法
            ...
        }
    }
;
```

**asm_generator.py 修改**：

```python
# 浮点运算使用 SSE 指令
elif op_type == "浮点运算":
    # 加载操作数到 xmm 寄存器
    result += [tab + f"movss xmm0, [rbp-{offset1}]"]
    result += [tab + f"movss xmm1, [rbp-{offset2}]"]
    
    # 执行运算
    if four[i][0] == "f+":
        result += [tab + "addss xmm0, xmm1"]
    elif four[i][0] == "f*":
        result += [tab + "mulss xmm0, xmm1"]
    
    # 存储结果
    result += [tab + f"movss [rbp-{result_offset}], xmm0"]
```

### 5. 64 位汇编优化

**问题**：原始代码使用 32 位寄存器处理指针地址，导致地址截断。

**解决方案**：

```python
# 变量偏移量从 4 字节改为 8 字节
offset = 8 * data.index(var) + 8  # 原来是 4 *

# 栈帧大小扩展
result += [tab + "sub rsp, 1024"]  # 原来是 144

# 指针操作使用 64 位寄存器
if is_pointer:
    result += [tab + f"mov rax, [rbp-{src_offset}]"]  # 64位
    result += [tab + f"mov [rbp-{offset}], rax"]
else:
    result += [tab + f"mov eax, [rbp-{src_offset}]"]  # 32位
    result += [tab + f"mov [rbp-{offset}], eax"]
```

---

## 技术难点与解决方案

### 难点 1：循环跳转行号错误

**现象**：while 循环无限执行或跳过

**根因分析**：
```c
// 原始代码在生成中间代码时使用 line_count
while_expression : WHILE ...
{
    // line_count 此时的值可能不准确
    $$ = whileOpr(..., line_count, line_count + 5, ...);
}
```

**解决方案**：符号标签系统
- 使用字符串标签替代数字行号
- 在代码生成完成后统一替换

### 难点 2：createTree(1) 的陷阱

**现象**：指针中间代码生成 `t47 = addr t47`

**根因分析**：
```c
Tree *createTree(char *name, int number, ...) {
    if (number == 1) {
        return t1;  // 直接返回输入的树！
    }
    // ...
}
```

当 `number == 1` 时，`$$` 和 `$2` 指向同一对象。

**解决方案**：先保存再赋值
```c
char* saved_inner = $2->inner;  // 先保存
$$ = createTree("DEREF", 1, $2);
$$->inner = newValue;  // 赋值
$$->code = ... saved_inner ...;  // 使用保存的值
```

### 难点 3：栈帧溢出

**现象**：Segmentation fault，或输出垃圾值

**根因分析**：
- 变量偏移从 4 字节改为 8 字节
- 最大偏移量从 ~200 增加到 ~460
- 栈帧仍是 144 字节

**解决方案**：
```python
# 扩展栈帧
result += [tab + "sub rsp, 1024"]
```

### 难点 4：数组元素覆盖

**现象**：`arr[0]` 正确，`arr[1]` 正确，`arr[2]` 错误

**根因分析**：
- `alloc` 只分配了数组指针的空间
- 后续变量分配覆盖了数组元素

**解决方案**：预留数组空间
```python
# alloc 时预留槽位
num_slots = (size + 3) // 4
for _ in range(num_slots):
    data[data.index(-1)] = f"__arr_reserved_{arr_name}_{_}"
```

---

## 创新点总结

### 1. 完整的类型系统扩展

从仅支持整数扩展到支持：
- 整数 (`int`)
- 浮点数 (`float`)
- 整型指针 (`int*`)
- 一维数组 (`int[]`)

### 2. 健壮的标签系统

- 完全解决了控制流跳转问题
- 支持嵌套循环和条件
- 代码生成与行号无关

### 3. 完善的 64 位支持

- 正确处理 64 位地址
- 合理分配栈空间
- 遵循 System V ABI

### 4. 模块化代码设计

- 清晰的职责分离
- 易于扩展新功能
- 完善的错误处理

---

## 贡献统计

| 类别 | 新增/修改行数 | 主要文件 |
|------|--------------|----------|
| 词法分析扩展 | ~50 | `lex.l` |
| 语法分析扩展 | ~500 | `yacc.y` |
| 中间代码生成 | ~200 | `tree.c` |
| 标签处理 | ~150 | `inner.c` |
| 汇编生成 | ~400 | `asm_generator.py` |
| Web 界面 | ~200 | `app.py`, `script.js` |
| 文档 | ~1500 | `docs/*.md` |
| **总计** | **~3000** | - |

---

## 参考文献

1. Alfred V. Aho, Monica S. Lam, Ravi Sethi, Jeffrey D. Ullman. *Compilers: Principles, Techniques, and Tools* (2nd Edition). 2006.
2. John R. Levine. *flex & bison*. O'Reilly Media. 2009.
3. Intel® 64 and IA-32 Architectures Software Developer's Manual.
4. System V Application Binary Interface AMD64 Architecture Processor Supplement.

---

**Mini-C 编译器 - 参考代码与贡献说明** © 2024

