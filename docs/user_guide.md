# Mini-C 编译器用户指南

## 📖 快速开始

### 1. 环境准备

#### 1.1 系统要求

- **操作系统**: Linux 或 WSL (Windows Subsystem for Linux)
- **CPU架构**: x86-64
- **磁盘空间**: 至少 500MB

#### 1.2 安装依赖

**Ubuntu/Debian系统**:
```bash
sudo apt update
sudo apt install flex bison gcc nasm python3 python3-pip make git
```

**检查环境**:
```bash
bash scripts/check_env.sh
```

### 2. 编译编译器

```bash
# 克隆或进入项目目录
cd Mini-C

# 编译编译器
make

# 应该看到输出：编译器生成成功: compiler
```

### 3. 第一个程序

创建一个简单的测试文件 `hello.c`:

```c
void main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);
}
```

编译并运行:

```bash
# 编译 hello.c
make file=hello

# 运行
./hello
```

应该输出：`30`

## 💻 使用方法

### 编译C文件

```bash
# 基本用法
make file=<文件名>

# 示例：编译 test.c
make file=test

# 编译并运行
make run file=test
```

### 查看编译过程

编译完成后，可以查看各阶段的中间结果：

```bash
# 查看词法分析结果
make show-lexical

# 查看语法树
make show-grammar

# 查看中间代码（四元式）
make show-ir

# 查看汇编代码
make show-asm

# 查看所有中间结果
make show-all
```

也可以直接查看文件：

```bash
cat Lexical        # 词法分析结果
cat Grammatical    # 语法树
cat Innercode      # 中间代码
cat assembly.asm   # 汇编代码
```

### 运行测试

```bash
# 运行所有测试
make test

# 运行词法分析测试
make test-lexer

# 运行语法分析测试
make test-parser
```

### 清理文件

```bash
# 清理生成文件
make clean

# 深度清理（包括备份文件）
make clean-all
```

### 调试模式

```bash
# 编译带调试符号的版本
make debug

# 使用gdb调试
gdb ./compiler
```

## 📚 语言特性

### 支持的数据类型

- `int` - 整型（4字节）
- `float` - 浮点型（4字节）
- `char` - 字符型（1字节）
- `void` - 空类型（用于函数返回）

### 支持的运算符

**算术运算符**:
- `+` - 加法
- `-` - 减法
- `*` - 乘法
- `/` - 除法
- `%` - 取模

**关系运算符**:
- `<` - 小于
- `>` - 大于
- `<=` - 小于等于
- `>=` - 大于等于
- `==` - 等于
- `!=` - 不等于

**逻辑运算符**:
- `&&` - 逻辑与
- `||` - 逻辑或
- `!` - 逻辑非

### 支持的语句

**声明语句**:
```c
int a;              // 单个变量声明
int b = 10;         // 带初始化的声明
int x = 1, y = 2;   // 多变量声明
```

**赋值语句**:
```c
a = 10;
b = a + 5;
```

**if-else语句**:
```c
if (a > b) {
    output_int(a);
} else {
    output_int(b);
}
```

**while循环**:
```c
while (a > 0) {
    a = a - 1;
}
```

**for循环**:
```c
for (int i = 0; i < 10; i = i + 1) {
    output_int(i);
}
```

**输入输出**:
```c
input_int(a);       // 输入整数
output_int(a);      // 输出整数
```

### 注释

```c
// 单行注释

/*
 * 多行注释
 * 可以跨越多行
 */
```

## 📝 编程示例

### 示例1：计算阶乘

```c
void main() {
    int n = 5;
    int result = 1;
    int i;
    
    for (i = 1; i <= n; i = i + 1) {
        result = result * i;
    }
    
    output_int(result);  // 输出: 120
}
```

### 示例2：最大值

```c
void main() {
    int a = 15;
    int b = 23;
    int max;
    
    if (a > b) {
        max = a;
    } else {
        max = b;
    }
    
    output_int(max);  // 输出: 23
}
```

### 示例3：浮点运算

```c
void main() {
    float pi = 3.14;
    float r = 5.0;
    float area = pi * r * r;
    
    // 注意：当前版本output_int只能输出整数
    // 浮点输出在后续版本实现
}
```

### 示例4：常量折叠优化

```c
void main() {
    int a = 3 + 4 * 5;  // 编译期优化为 23
    
    if (0) {
        // 这段代码会被优化掉（死代码消除）
        output_int(999);
    }
    
    output_int(a);  // 输出: 23
}
```

## 🔧 常见问题

### Q1: 编译时报错 "flex: command not found"

**解决**: 安装flex
```bash
sudo apt install flex
```

### Q2: 运行时报错 "Segmentation fault"

**可能原因**:
1. 变量未初始化就使用
2. 数组越界访问（2.0版本）
3. 指针操作错误（2.0版本）

**调试方法**:
```bash
make debug
gdb ./your_program
```

### Q3: 输出结果不正确

**检查步骤**:
1. 查看中间代码：`make show-ir`
2. 查看汇编代码：`make show-asm`
3. 检查类型是否匹配（int vs float）

### Q4: 如何查看优化效果？

```bash
# 编译程序
make file=test

# 查看中间代码
cat Innercode

# 对比：在代码中添加常量表达式
# 如：int a = 1 + 2 + 3;
# 会被优化为：int a = 6;
```

### Q5: 支持递归函数吗？

**1.0版本**: 不支持函数定义和调用  
**2.0版本**: 将支持函数和递归

## 📊 性能优化

### 优化级别

- **默认**: 发布版本（-O2优化）
- **调试**: 无优化，包含调试符号（-O0 -g）

### 代码优化建议

1. **使用常量**: 编译期能计算的表达式会被优化
2. **避免无用代码**: if(0)等永不执行的代码会被消除
3. **合理使用变量**: 后续版本的活性分析会复用栈空间

## 📖 进阶使用

### 手动执行编译步骤

如果你想了解编译过程，可以手动执行每一步：

```bash
# 1. 生成词法分析器
flex c-complier-master/lex.l

# 2. 生成语法分析器
bison -d c-complier-master/yacc.y

# 3. 编译编译器
gcc -o compiler yacc.tab.c lex.yy.c c-complier-master/*.c -lfl

# 4. 运行编译器（生成中间代码）
./compiler test.c

# 5. 生成汇编代码
python3 c-complier-master/assembly.py

# 6. 汇编
nasm -f elf64 assembly.asm -o test.o

# 7. 链接
gcc -no-pie -o test test.o

# 8. 运行
./test
```

### 添加自定义测试

在 `tests/` 目录下创建测试文件：

```bash
# 创建测试文件
echo 'void main() { output_int(42); }' > tests/integration/my_test.c

# 编译测试
make file=tests/integration/my_test
```

## 🚀 后续版本预告

### 2.0版本（开发中）
- 数组支持：`int arr[10];`
- 指针支持：`int *ptr = &a;`
- 函数定义：`int add(int x, int y) { return x + y; }`
- 结构体：`struct Point { int x; int y; };`
- 活性分析优化：栈空间复用

### 3.0版本（规划中）
- Web可视化界面
- 语法高亮编辑器
- AST可视化展示
- FIRST/FOLLOW集计算工具

## 📞 获取帮助

### 查看帮助信息

```bash
make help
```

### 文档资源

- 设计文档：`docs/design.md`
- 开发计划：`DEV_PLAN.md`
- 项目README：`README.md`

### 报告问题

在GitHub上提交Issue：
[https://github.com/xiaowenhao404/MINI-C/issues](https://github.com/xiaowenhao404/MINI-C/issues)

---

**文档版本**: v1.0  
**最后更新**: 2025-12-16  
**适用版本**: Mini-C 1.0

