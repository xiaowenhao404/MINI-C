# Mini-C 代码生成器手册

## 概述

`scripts/asm_generator.py` 是 Mini-C 编译器的代码生成器，负责将中间代码（Innercode 三地址码）翻译为 x86-64 汇编代码。

## 功能特性

### 1. 整数运算

- 支持操作：`+`, `-`, `*`, `/`, `%`
- 使用寄存器：`eax`, `ebx`
- 指令示例：`add eax, ebx`, `imul ebx`, `idiv ebx`

### 2. 浮点运算

- 支持操作：`f+`, `f-`, `f*`, `f/`
- 使用 SSE 寄存器：`xmm0`, `xmm1`
- 指令示例：`addss xmm0, xmm1`, `mulss xmm0, xmm1`

### 3. 内存操作

| 指令    | 格式               | 说明                 |
| ------- | ------------------ | -------------------- |
| `alloc` | `arr = alloc size` | 分配数组空间         |
| `addr`  | `t = addr var`     | 获取变量地址         |
| `load`  | `t = load addr`    | 从地址加载值         |
| `store` | `store val addr`   | 存储值到地址         |

### 4. 输入输出

- `output(x)`: 调用 `printf` 输出整数或浮点数
- `input()`: 调用 `scanf` 读取整数

### 5. 控制流

- 标签：`CODE1:`, `CODE2:`, ...
- 跳转：`jmp`, `je`, `jne`, `ja`, `jb`, `jae`, `jbe`
- 条件：支持 `>`, `<`, `>=`, `<=`, `==`, `!=`

## 中间代码格式（Innercode）

### 基本格式

```
行号: [目标变量] [运算符] [操作数1] [操作数2]
```

### 指令类型

#### 赋值

```
1: a = 10          // 整数赋值
2: x = 3.14        // 浮点赋值
3: b = a           // 变量赋值
```

#### 整数运算

```
4: t1 = a + b      // 加法
5: t2 = a - b      // 减法
6: t3 = a * b      // 乘法
7: t4 = a / b      // 除法
8: t5 = a % b      // 取模
```

#### 浮点运算

```
9: t1 = x f+ y     // 浮点加法
10: t2 = x f- y    // 浮点减法
11: t3 = x f* y    // 浮点乘法
12: t4 = x f/ y    // 浮点除法
```

#### 内存操作

```
13: arr = alloc 20     // 分配 20 字节数组空间
14: t1 = addr val      // 获取 val 的地址
15: t2 = load t1       // 从 t1 指向的地址加载值
16: store 10 t1        // 将 10 存储到 t1 指向的地址
```

#### 控制流

```
17: t1 = a > b     // 比较
18: if t1 goto 22  // 条件跳转
19: goto 25        // 无条件跳转
```

#### 函数调用

```
20: arg a          // 传递参数
21: call output    // 调用函数
22: return         // 返回
```

## 汇编生成详解

### 变量分配

每个变量分配 8 字节栈空间：

```python
offset = 8 * data.index(var_name) + 8
# 变量访问：[rbp-offset]
```

### 整数运算生成

```nasm
; t = a + b
mov eax, [rbp-8]      ; 加载 a
mov ebx, [rbp-16]     ; 加载 b
add eax, ebx          ; 计算
mov [rbp-24], eax     ; 存储结果 t
```

### 浮点运算生成

```nasm
; t = x f+ y
movss xmm0, [rbp-8]   ; 加载 x
movss xmm1, [rbp-16]  ; 加载 y
addss xmm0, xmm1      ; 计算
movss [rbp-24], xmm0  ; 存储结果 t
```

### 地址操作生成

```nasm
; t = addr val
lea rax, [rbp-8]      ; 获取 val 的地址
mov [rbp-16], rax     ; 存储到 t（64位）

; t = load ptr
mov rax, [rbp-16]     ; 加载指针值
mov eax, [rax]        ; 解引用
mov [rbp-24], eax     ; 存储结果

; store val ptr
mov ebx, [rbp-8]      ; 加载要存储的值
mov rax, [rbp-16]     ; 加载目标地址
mov [rax], ebx        ; 存储到内存
```

### 数组操作生成

```nasm
; arr = alloc 20
lea rax, [rbp-48]     ; 计算数组基地址
mov [rbp-8], rax      ; 存储到 arr

; 访问 arr[i]
; 1. 计算偏移：t_off = i * 4
; 2. 计算地址：t_addr = arr + t_off
; 3. 加载/存储
```

### 函数调用生成

```nasm
; call output (整数)
mov eax, [rbp-8]          ; 加载参数
mov edi, eax              ; 第一参数
mov rsi, out_format_int   ; 格式串
xor eax, eax              ; 浮点参数数量 = 0
call printf

; call output (浮点)
movss xmm0, [rbp-8]       ; 加载浮点参数
cvtss2sd xmm0, xmm0       ; 转换为 double
mov rdi, out_format_float
mov eax, 1                ; 浮点参数数量 = 1
call printf
```

## 生成的汇编结构

```nasm
extern printf, scanf
global main

section .text
main:
    push rbp
    mov rbp, rsp
    sub rsp, 1024         ; 分配栈空间

    ; === 用户代码 ===
    
CODE1:                    ; 标签
    ; 指令...
    
CODE2:
    ; 指令...

END_PROGRAM:
    xor eax, eax          ; 返回 0
    leave
    ret

section .data
    out_format_int: db "%d", 10, 0
    out_format_float: db "%.2f", 10, 0
    in_format_int: db "%d", 0
    ; 浮点常量...

section .bss
    number resb 8
```

## 栈帧布局

```
rbp+0    ← 保存的 rbp
rbp-8    ← 第 1 个变量
rbp-16   ← 第 2 个变量
rbp-24   ← 第 3 个变量
...
rbp-1024 ← 栈底
```

- 每个变量分配 8 字节
- 变量按声明顺序从 `rbp-8` 开始依次分配
- 数组分配额外空间用于元素存储

## 寄存器约定

### 整数寄存器

- `rax/eax`: 第一操作数、运算结果、返回值、64 位地址
- `rbx/ebx`: 第二操作数
- `ecx`, `edx`: 除法操作

### 浮点寄存器（SSE）

- `xmm0`: 第一浮点操作数、运算结果、返回值
- `xmm1`: 第二浮点操作数

### 参数传递（System V ABI）

- 整数参数：`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`
- 浮点参数：`xmm0-xmm7`
- 返回值：整数用 `rax`，浮点用 `xmm0`

## 使用方法

### 完整编译流程

```bash
# 1. 运行编译器生成中间代码
./compiler source.c

# 2. 生成汇编代码
python3 scripts/asm_generator.py

# 3. 汇编
nasm -f elf64 assembly.asm -o output.o

# 4. 链接
gcc -no-pie -o program output.o

# 5. 运行
./program
```

### 查看生成的汇编

```bash
# 查看完整汇编
cat assembly.asm

# 查看特定指令
grep -E "(movss|addss)" assembly.asm

# 查看内存操作
grep -E "(lea|load|store)" assembly.asm
```

## 调试技巧

### 查看中间代码

```bash
cat Innercode
```

### 使用 GDB 调试

```bash
# 编译带调试信息
gcc -no-pie -g -o program output.o

# GDB 调试
gdb ./program
(gdb) break main
(gdb) run
(gdb) info registers eax
(gdb) info registers xmm0
(gdb) x/d $rbp-8           # 查看栈上的整数
(gdb) x/f $rbp-16          # 查看栈上的浮点数
```

### 反汇编

```bash
objdump -d output.o | less
```

## 常见错误和解决

### 错误 1：变量未找到

**错误**：`'var_name' is not in list`  
**原因**：变量未在数据列表中注册  
**解决**：检查中间代码中变量声明

### 错误 2：浮点输出错误

**问题**：浮点数显示不正确  
**原因**：x86-64 的 printf 需要 double 参数  
**解决**：使用 `cvtss2sd` 转换

### 错误 3：段错误

**问题**：运行时 Segmentation fault  
**可能原因**：
1. 栈帧不够大
2. 数组越界
3. 空指针解引用

**解决**：增大栈帧或检查内存访问

---

**版本**: 3.0  
**最后更新**: 2024-12-23  
**作者**: Mini-C 开发团队
