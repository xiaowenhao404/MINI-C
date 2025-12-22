# Mini-C 编译器快速开始指南

本指南将帮助您在5分钟内开始使用Mini-C编译器。

---

## ⚡ 5分钟快速上手

### 步骤1: 环境准备（2分钟）

#### 1.1 安装依赖

**Ubuntu/Debian系统**:
```bash
sudo apt update
sudo apt install flex bison gcc nasm python3 python3-pip make
```

**检查环境**:
```bash
# 验证工具是否安装成功
flex --version
bison --version
gcc --version
nasm --version
python3 --version
make --version
```

#### 1.2 进入项目目录

```bash
cd Mini-C
```

---

### 步骤2: 编译编译器（1分钟）

```bash
# 编译编译器
make

# 应该看到输出：
# === 编译完成 ===
# 编译器生成成功: compiler
```

如果编译失败，请检查：
- 是否安装了所有依赖工具
- `src/frontend/`目录是否包含`lex.l`和`yacc.y`文件
- `src/utils/`目录是否包含辅助源文件

---

### 步骤3: 编写第一个程序（1分钟）

创建文件 `hello.c`:

```c
void main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);
}
```

---

### 步骤4: 编译并运行（1分钟）

```bash
# 编译 hello.c
make file=hello

# 运行
./hello
```

**预期输出**: `30`

---

## 📝 更多示例

### 示例1: 浮点运算

创建 `float_test.c`:

```c
void main() {
    float pi = 3.14;
    float r = 5.0;
    float area = pi * r * r;
    output_float(area);
}
```

编译运行:
```bash
make file=float_test
./float_test
```

### 示例2: 控制流

创建 `loop_test.c`:

```c
void main() {
    int i = 0;
    while (i < 5) {
        output_int(i);
        i = i + 1;
    }
}
```

编译运行:
```bash
make file=loop_test
./loop_test
```

### 示例3: 函数调用

创建 `function_test.c`:

```c
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);
}
```

编译运行:
```bash
make file=function_test
./function_test
```

---

## 🔍 查看编译过程

Mini-C编译器会生成多个中间文件，您可以查看编译的各个阶段：

```bash
# 查看词法分析结果（Token流）
make show-lexical

# 查看语法分析结果（语法树）
make show-grammar

# 查看中间代码（四元式IR）
make show-ir

# 查看汇编代码（x86-64 NASM）
make show-asm

# 查看所有中间结果
make show-all
```

或者直接查看生成的文件：
- `Lexical` - 词法分析结果
- `Grammatical` - 语法分析结果
- `Innercode` - 中间代码
- `assembly.asm` - 汇编代码

---

## 🌐 使用Web界面

项目提供了Web可视化界面，更方便使用：

### 启动Web服务器

```bash
# 进入web目录
cd web

# 安装依赖（首次运行）
pip install flask

# 启动服务器
python app.py
```

访问 `http://127.0.0.1:5000` 即可使用在线编译器。

详细说明请查看 [web/QUICKSTART.md](web/QUICKSTART.md)

---

## 🧪 运行测试用例

项目包含大量测试用例，您可以参考或直接运行：

```bash
# 运行集成测试
cd tests/integration
make file=test_basic
./test_basic

# 运行函数测试
cd ../function
make file=test_array_parameter
./test_array_parameter
```

---

## ❓ 常见问题

### Q1: 编译失败，提示找不到lex.l或yacc.y？

**A**: 需要确保有参考代码目录（如`c-complier-master`）包含词法和语法文件。检查Makefile中的`REF_DIR`配置。

### Q2: 运行`make file=hello`时提示找不到文件？

**A**: 确保：
1. 文件`hello.c`存在于当前目录
2. 文件名正确（不含.c扩展名）

### Q3: 运行程序时提示找不到output_int函数？

**A**: `output_int`和`output_float`是Mini-C的内置函数，用于输出。确保代码中正确调用了这些函数。

### Q4: 如何查看编译错误？

**A**: 编译过程中的错误会直接输出到终端。也可以查看生成的中间文件来定位问题。

### Q5: 如何清理编译生成的文件？

**A**: 
```bash
# 清理所有生成文件
make clean

# 深度清理
make clean-all
```

---

## 📚 下一步

- 查看 [README.md](README.md) 了解完整功能
- 查看 [docs/user_guide.md](docs/user_guide.md) 学习详细用法
- 查看 [DIRECTORY_STRUCTURE.md](DIRECTORY_STRUCTURE.md) 了解项目结构
- 查看 `tests/` 目录下的测试用例学习更多示例

---

## 🎯 快速命令参考

```bash
# 编译编译器
make

# 编译C文件
make file=<文件名>

# 编译并运行
make run file=<文件名>

# 查看中间结果
make show-all

# 运行测试
make test

# 清理文件
make clean

# 查看帮助
make help
```

---

## 💡 提示

1. **文件名**: 使用`make file=xxx`时，不需要包含`.c`扩展名
2. **输出函数**: Mini-C使用`output_int()`和`output_float()`输出，不是`printf()`
3. **调试**: 使用`make debug`编译调试版本，包含调试符号
4. **查看过程**: 使用`make show-all`查看完整的编译过程

---

**祝您使用愉快！** 🎉

如有问题，请查看 [README.md](README.md) 或提交Issue。

