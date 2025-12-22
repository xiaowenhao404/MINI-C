# Mini-C 编译器 WSL 命令行演示文档

> 本文档详细记录了在 WSL (Windows Subsystem for Linux) 环境中使用 Mini-C 编译器的完整操作流程。

## 📋 目录

- [环境准备](#环境准备)
- [编译器构建](#编译器构建)
- [基本使用流程](#基本使用流程)
- [功能演示](#功能演示)
- [完整测试流程](#完整测试流程)
- [调试技巧](#调试技巧)
- [常见问题](#常见问题)

---

## 环境准备

### 1. 安装 WSL（如未安装）

在 Windows PowerShell（管理员模式）中执行：

```powershell
wsl --install
```

### 2. 进入 WSL 环境

```powershell
wsl
```

### 3. 安装必要工具

```bash
# 更新软件包列表
sudo apt update

# 安装编译工具链
sudo apt install -y gcc flex bison nasm python3 make

# 验证安装
gcc --version
flex --version
bison --version
nasm --version
python3 --version
```

### 4. 进入项目目录

```bash
# 假设项目在 Windows 桌面
cd /mnt/c/Users/<用户名>/Desktop/Mini-C

# 或者使用相对路径
cd /mnt/c/path/to/Mini-C
```

---

## 编译器构建

### 步骤 1：清理旧文件

```bash
make clean
```

**输出示例：**

```
=== 清理生成文件 ===
rm -f lex.yy.c yacc.tab.c yacc.tab.h
rm -f compiler compiler.exe
rm -f *.o *.out test test.exe
rm -f Lexical Grammatical Innercode assembly.asm *.asm
清理完成
```

### 步骤 2：构建编译器

```bash
make
```

**输出示例：**

```
=== 生成词法分析器 ===
flex src/frontend/lex.l
=== 生成语法分析器 ===
bison -d -Wno-conflicts-sr src/frontend/yacc.y
=== 编译编译器核心 ===
gcc -Wall -std=c11 -O2 -Isrc/utils -o compiler \
    yacc.tab.c lex.yy.c src/utils/tree.c ...
编译器生成成功: compiler
=== 编译完成 ===
```

### 步骤 3：验证编译器

```bash
ls -la compiler
```

**输出示例：**

```
-rwxr-xr-x 1 user user 123456 Dec 23 12:00 compiler
```

---

## 基本使用流程

### 完整编译流程（5 步）

```bash
# 步骤 1：运行编译器（生成中间代码）
./compiler temp.c

# 步骤 2：生成汇编代码
python3 scripts/asm_generator.py

# 步骤 3：汇编（生成目标文件）
nasm -f elf64 assembly.asm -o temp.o

# 步骤 4：链接（生成可执行文件）
gcc -no-pie -o program temp.o

# 步骤 5：运行程序
./program
```

### 使用 Makefile 简化

```bash
# 编译并运行（一条命令）
make run file=temp

# 仅编译
make compile file=temp
```

---

## 功能演示

### 演示 1：整数运算

**创建测试文件：**

```bash
cat > test_int.c << 'EOF'
void main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    output(c);
}
EOF
```

**编译运行：**

```bash
./compiler test_int.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_int.o
gcc -no-pie -o test_int test_int.o
./test_int
```

**预期输出：**

```
30
```

---

### 演示 2：浮点运算

**创建测试文件：**

```bash
cat > test_float.c << 'EOF'
void main() {
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output(sum);
}
EOF
```

**编译运行：**

```bash
./compiler test_float.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_float.o
gcc -no-pie -o test_float test_float.o
./test_float
```

**预期输出：**

```
5.14
```

---

### 演示 3：函数定义与调用

**创建测试文件：**

```bash
cat > test_func.c << 'EOF'
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 30);
    output(result);
}
EOF
```

**编译运行：**

```bash
./compiler test_func.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_func.o
gcc -no-pie -o test_func test_func.o
./test_func
```

**预期输出：**

```
40
```

**查看中间代码：**

```bash
cat Innercode
```

**中间代码示例：**

```
1: FUNC_BEGIN add
2: param a
3: param b
4: t1 = a + b
5: return t1
6: FUNC_END add
...
```

---

### 演示 4：if-else 控制流

**创建测试文件：**

```bash
cat > test_ifelse.c << 'EOF'
void main() {
    int a = 10;
    int b = 20;

    if (a < b) {
        output(100);
    } else {
        output(0);
    }
}
EOF
```

**编译运行：**

```bash
./compiler test_ifelse.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_ifelse.o
gcc -no-pie -o test_ifelse test_ifelse.o
./test_ifelse
```

**预期输出：**

```
100
```

---

### 演示 5：while 循环

**创建测试文件：**

```bash
cat > test_while.c << 'EOF'
void main() {
    int i = 0;
    while (i < 3) {
        output(i);
        i = i + 1;
    }
}
EOF
```

**编译运行：**

```bash
./compiler test_while.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_while.o
gcc -no-pie -o test_while test_while.o
./test_while
```

**预期输出：**

```
0
1
2
```

---

### 演示 6：for 循环

**创建测试文件：**

```bash
cat > test_for.c << 'EOF'
void main() {
    int j;
    for (j = 0; j < 3; j = j + 1) {
        output(j);
    }
}
EOF
```

**预期输出：**

```
0
1
2
```

---

### 演示 7：数组操作

**创建测试文件：**

```bash
cat > test_array.c << 'EOF'
void main() {
    int arr[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = arr[0] + arr[1];

    output(arr[0]);
    output(arr[1]);
    output(arr[2]);
}
EOF
```

**编译运行：**

```bash
./compiler test_array.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_array.o
gcc -no-pie -o test_array test_array.o
./test_array
```

**预期输出：**

```
10
20
30
```

**查看中间代码（数组相关指令）：**

```bash
grep -E "alloc|store|load" Innercode
```

---

### 演示 8：指针操作

**创建测试文件：**

```bash
cat > test_ptr.c << 'EOF'
void main() {
    int val = 42;
    int* ptr;
    ptr = &val;

    output(*ptr);    // 解引用

    *ptr = 100;      // 通过指针修改值
    output(val);
}
EOF
```

**编译运行：**

```bash
./compiler test_ptr.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o test_ptr.o
gcc -no-pie -o test_ptr test_ptr.o
./test_ptr
```

**预期输出：**

```
42
100
```

**查看中间代码（指针相关指令）：**

```bash
grep -E "addr|load|store" Innercode
```

---

### 演示 9：浮点乘法

**创建测试文件：**

```bash
cat > test_fmul.c << 'EOF'
void main() {
    float pi = 3.14;
    float doubled = pi * 2.0;
    output(doubled);
}
EOF
```

**预期输出：**

```
6.28
```

---

### 演示 10：常量折叠优化

**创建测试文件：**

```bash
cat > test_const.c << 'EOF'
void main() {
    int result = 3 + 4 * 5;
    output(result);
}
EOF
```

**预期输出：**

```
23
```

---

## 完整测试流程

### 使用示例文件 temp.c

```bash
# 确保在项目根目录
cd /path/to/Mini-C

# 查看示例代码
cat temp.c

# 完整编译流程
./compiler temp.c
python3 scripts/asm_generator.py
nasm -f elf64 assembly.asm -o temp.o
gcc -no-pie -o program temp.o

# 运行程序
./program
```

**完整预期输出：**

```
30
5.14
23
40
100
0
1
2
6.28
10
20
30
42
100
0
1
2
```

### 查看编译过程各阶段输出

```bash
# 1. 词法分析结果
head -50 Lexical

# 2. 语法分析结果
head -50 Grammatical

# 3. 中间代码
cat Innercode

# 4. 汇编代码
head -100 assembly.asm
```

---

## 调试技巧

### 1. 查看词法分析结果

```bash
./compiler test.c
cat Lexical | head -30
```

**输出格式：**

```
<类型, 值, 行号>
例如：
<KEYWORD, int, 1>
<ID, main, 1>
<LPAREN, (, 1>
...
```

### 2. 查看语法分析过程

```bash
cat Grammatical | tail -20
```

### 3. 查看中间代码

```bash
# 完整查看
cat Innercode

# 只看跳转指令
grep -E "goto|if" Innercode

# 只看函数相关
grep -E "FUNC|call|return|param" Innercode
```

### 4. 查看汇编代码

```bash
# 查看数据段
grep -A 10 "section .data" assembly.asm

# 查看代码段开头
grep -A 30 "section .text" assembly.asm

# 查看函数调用
grep "call" assembly.asm
```

### 5. 使用 GDB 调试可执行文件

```bash
# 编译时添加调试信息
gcc -no-pie -g -o program temp.o

# 使用 GDB
gdb ./program

# GDB 命令
(gdb) break main
(gdb) run
(gdb) step
(gdb) print $eax
(gdb) quit
```

---

## 常见问题

### Q1: `./compiler: Permission denied`

**解决方案：**

```bash
chmod +x compiler
```

### Q2: `nasm: command not found`

**解决方案：**

```bash
sudo apt install nasm
```

### Q3: `Segmentation fault` 运行时崩溃

**可能原因：**

1. 数组越界访问
2. 空指针解引用
3. 栈溢出

**调试方法：**

```bash
# 使用 GDB
gdb ./program
(gdb) run
(gdb) bt  # 查看调用栈
```

### Q4: 汇编器报错 `symbol not defined`

**可能原因：**

1. 函数名拼写错误
2. 中间代码生成不完整

**调试方法：**

```bash
# 检查中间代码
cat Innercode | grep call
```

### Q5: 输出结果不正确

**调试步骤：**

1. 检查源代码逻辑
2. 检查中间代码 `cat Innercode`
3. 检查汇编代码 `cat assembly.asm`
4. 使用 GDB 单步调试

### Q6: 浮点数精度问题

**说明：**

- Mini-C 使用 32 位 float
- 精度约 6-7 位有效数字
- 显示精度为小数点后 2 位

---

## 快速参考

### 常用命令速查

```bash
# 构建编译器
make

# 清理
make clean

# 编译源文件
./compiler file.c

# 生成汇编
python3 scripts/asm_generator.py

# 汇编
nasm -f elf64 assembly.asm -o file.o

# 链接
gcc -no-pie -o program file.o

# 运行
./program

# 一键编译运行
make run file=temp
```

### 输出文件说明

| 文件           | 内容                |
| -------------- | ------------------- |
| `Lexical`      | 词法分析 Token 序列 |
| `Grammatical`  | 语法分析树          |
| `Innercode`    | 三地址码            |
| `assembly.asm` | x86-64 汇编代码     |
| `*.o`          | 目标文件            |
| `program`      | 可执行文件          |

---

## 清理测试文件

演示完成后，清理生成的测试文件：

```bash
rm -f test_*.c test_*.o test_int test_float test_func \
      test_ifelse test_while test_for test_array test_ptr \
      test_fmul test_const program
make clean
```

---

**Mini-C 编译器 WSL 命令行演示文档** © 2024
