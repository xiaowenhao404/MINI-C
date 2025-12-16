# Mini-C 代码生成器使用手册

## 概述

`scripts/asm_generator.py` 是 Mini-C 编译器的代码生成器，负责将中间代码（Innercode四元式）翻译为 x86-64 汇编代码。

## 功能特性

### 1. 整数运算
- 支持操作：`+`, `-`, `*`, `/`, `%`
- 使用通用寄存器：`eax`, `ebx`, `ecx`, `edx`
- 指令示例：`add eax,ebx`, `imul ebx`, `idiv ebx`

### 2. 浮点运算（新增 ✨）
- 支持操作：`f+`, `f-`, `f*`, `f/`
- 使用SSE寄存器：`xmm0`, `xmm1`, `xmm2`
- 指令示例：`addss xmm0,xmm1`, `mulss xmm0,xmm1`

### 3. 类型转换（新增 ✨）
- `i2f`：int → float
  * 指令：`cvtsi2ss xmm0, eax`
  * 示例：`t1 i2f a` → 将整数a转为浮点数t1
- `f2i`：float → int（截断）
  * 指令：`cvttss2si eax, xmm0`
  * 示例：`t1 f2i x` → 将浮点数x转为整数t1

### 4. 输入输出
- `output_int(x)`：输出整数（格式：%d）
- `output_float(x)`：输出浮点数（格式：%.2f，保留2位小数）✨
- `input()`：输入整数

### 5. 控制流
- 标签：`CODE1:`, `CODE2:`, ...
- 跳转：`jmp`, `je`, `jne`, `ja`, `jb`, `jae`, `jbe`
- 条件：支持 `>`, `<`, `>=`, `<=`, `==`, `!=`

## 中间代码格式（Innercode）

### 基本格式
```
行号 [目标变量] [运算符] [操作数1] [操作数2]
```

### 指令类型

#### 赋值
```
1 a = 10          # 整数赋值
2 x = 3.14        # 浮点赋值
3 b = a           # 变量赋值
```

#### 整数运算
```
4 t1 = a + b      # 加法
5 t2 = a - b      # 减法
6 t3 = a * b      # 乘法
7 t4 = a / b      # 除法
8 t5 = a % b      # 取模
```

#### 浮点运算
```
9  t1 = x f+ y    # 浮点加法
10 t2 = x f- y    # 浮点减法
11 t3 = x f* y    # 浮点乘法
12 t4 = x f/ y    # 浮点除法
```

#### 类型转换
```
13 t1 i2f a       # int → float
14 t2 f2i x       # float → int
```

#### 控制流
```
15 t1 = a > b     # 比较
16 if t1 goto 20  # 条件跳转
17 goto 25        # 无条件跳转
```

#### 函数调用
```
18 arg a          # 传递参数
19 call output_int # 调用函数
20 return         # 返回
```

## 使用流程

### 完整编译流程
```bash
# 1. 词法和语法分析（生成AST）
./lexer input.c

# 2. 语义分析和IR生成（生成Innercode）
./semantic_analyzer

# 3. 代码生成（生成assembly.asm）
python scripts/asm_generator.py

# 4. 汇编
nasm -f elf64 assembly.asm -o assembly.o

# 5. 链接
gcc assembly.o -o output -no-pie

# 6. 运行
./output
```

### 快速测试
```bash
# 直接提供Innercode测试代码生成
echo "1 a = 10
2 b = 3
3 t1 = a + b
4 arg t1
5 call output_int
6 return" > Innercode

python scripts/asm_generator.py
cat assembly.asm
```

## 生成的汇编结构

```nasm
extern printf, scanf
global main

section .text
main:
        enter 120, 0  ; 分配栈帧

        ; 变量分配和运算
        mov eax, 10
        mov [rbp-4], eax      ; a = 10
        
        movss xmm0, [rbp-8]
        movss xmm1, [rbp-12]
        addss xmm0, xmm1      ; 浮点加法
        movss [rbp-16], xmm0
        
        ; 函数调用
        movss xmm0, [rbp-16]
        mov rdi, out_format_float
        mov rax, 1            ; 1个浮点参数
        call printf
        
        leave
        ret

section .data
        out_format_int: db "%d", 10, 0
        out_format_float: db "%.2f", 10, 0
        in_format_int: db "%d", 0

section .bss
        number resb 4
```

## 栈帧布局

```
rbp+0   ← 保存的rbp
rbp-4   ← 第1个变量
rbp-8   ← 第2个变量
rbp-12  ← 第3个变量
...
rbp-120 ← 栈底
```

- 每个变量分配4字节（无论int还是float）
- 变量按声明顺序从rbp-4开始依次分配

## 寄存器约定

### 整数寄存器
- `eax`：第一操作数、运算结果、返回值
- `ebx`：第二操作数
- `ecx`, `edx`：临时变量、除法余数

### 浮点寄存器（SSE）
- `xmm0`：第一浮点操作数、运算结果、返回值
- `xmm1`：第二浮点操作数
- `xmm2-xmm7`：保留扩展

### 参数传递（x86-64 System V ABI）
- 整数参数：`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`
- 浮点参数：`xmm0-xmm7`
- 返回值：整数用`rax`，浮点用`xmm0`

## 指令映射表

| 中间代码 | 汇编指令 | 说明 |
|---------|---------|------|
| `a = 10` | `mov eax, 10`<br>`mov [rbp-4], eax` | 整数赋值 |
| `x = 3.14` | `mov eax, __float32__(3.14)`<br>`mov [rbp-4], eax` | 浮点赋值 |
| `t = a + b` | `mov eax, [rbp-4]`<br>`mov ebx, [rbp-8]`<br>`add eax, ebx`<br>`mov [rbp-12], eax` | 整数加法 |
| `t = x f+ y` | `movss xmm0, [rbp-4]`<br>`movss xmm1, [rbp-8]`<br>`addss xmm0, xmm1`<br>`movss [rbp-12], xmm0` | 浮点加法 |
| `t i2f a` | `mov eax, [rbp-4]`<br>`cvtsi2ss xmm0, eax`<br>`movss [rbp-8], xmm0` | int→float |
| `t f2i x` | `movss xmm0, [rbp-4]`<br>`cvttss2si eax, xmm0`<br>`mov [rbp-8], eax` | float→int |

## 调试技巧

### 查看生成的汇编
```bash
cat assembly.asm | less
```

### 查看特定指令
```bash
grep -E "(movss|addss|cvt)" assembly.asm
```

### GDB调试
```bash
gdb ./output
(gdb) break main
(gdb) run
(gdb) info registers xmm0  # 查看浮点寄存器
(gdb) x/f $rbp-8           # 查看内存中的float
(gdb) x/d $rbp-4           # 查看内存中的int
```

### 反汇编
```bash
objdump -d assembly.o | less
```

## 常见错误和解决

### 错误1：浮点立即数加载失败
**问题**：`mov eax, 3.14` 不被支持
**解决**：使用 `__float32__(3.14)` 伪指令或通过内存

### 错误2：printf浮点参数错误
**问题**：x86-64中printf的float参数需要为double
**解决**：使用 `cvtss2sd xmm0, xmm0` 转换

### 错误3：栈未对齐
**问题**：调用printf前栈未16字节对齐
**解决**：`sub rsp, 8` 调整栈

### 错误4：寄存器覆盖
**问题**：eax或xmm0被意外覆盖
**解决**：合理安排指令顺序或使用push/pop保存

## 扩展功能（TODO）

- [ ] double类型支持（8字节）
- [ ] 浮点比较运算（`comiss`, `ucomiss`）
- [ ] SIMD向量运算（`addps`, `mulps`）
- [ ] 数学库函数（`sin`, `cos`, `sqrt`）
- [ ] 寄存器分配优化
- [ ] 栈空间优化（按需分配）

## 参考资料

- [x86-64 Instruction Set Reference](https://www.felixcloutier.com/x86/)
- [Intel SSE Programming Reference](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)
- [System V AMD64 ABI](https://wiki.osdev.org/System_V_ABI)
- [NASM Documentation](https://www.nasm.us/doc/)

---

**版本**: 1.0  
**最后更新**: 2025-12-16  
**作者**: Mini-C 开发团队

