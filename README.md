# Mini-C 编译器 v3.0

> 一个支持整数、浮点数、数组、指针、函数的简易 C 语言编译器，具备完整的编译流程和 Web 可视化界面。

## 📋 目录

- [项目概述](#项目概述)
- [功能特性](#功能特性)
- [项目结构](#项目结构)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [使用方式](#使用方式)
- [编译器架构](#编译器架构)
- [示例代码](#示例代码)
- [文档索引](#文档索引)
- [开发说明](#开发说明)

---

## 项目概述

Mini-C 是一个教学用途的 C 语言子集编译器，实现了从源代码到 x86-64 汇编代码的完整编译流程。项目包含：

- **前端**：词法分析（Flex）+ 语法分析（Bison）
- **中间表示**：三地址码（四元式）
- **后端**：x86-64 NASM 汇编代码生成
- **Web 界面**：Flask 实现的可视化编译平台

---

## 功能特性

### ✅ 支持的语法特性

| 类别         | 特性     | 说明                             |
| ------------ | -------- | -------------------------------- |
| **数据类型** | `int`    | 32 位整数                        |
|              | `float`  | 32 位浮点数                      |
|              | `int*`   | 整型指针                         |
|              | `int[]`  | 一维数组                         |
| **运算符**   | 算术     | `+`, `-`, `*`, `/`, `%`          |
|              | 关系     | `<`, `>`, `<=`, `>=`, `==`, `!=` |
|              | 逻辑     | `&&`, `\|\|`, `!`                |
|              | 赋值     | `=`                              |
|              | 指针     | `*`(解引用), `&`(取地址)         |
| **控制流**   | 条件     | `if`, `if-else`                  |
|              | 循环     | `while`, `for`                   |
| **函数**     | 定义     | 支持参数和返回值                 |
|              | 调用     | 支持递归调用                     |
|              | 内置     | `output()`, `input()`            |
| **优化**     | 常量折叠 | 编译时计算常量表达式             |

### ✅ 编译器输出

- **词法分析结果** (`Lexical`)：Token 序列
- **语法分析结果** (`Grammatical`)：语法树结构
- **中间代码** (`Innercode`)：三地址码
- **汇编代码** (`assembly.asm`)：x86-64 NASM 格式
- **可执行文件**：Linux ELF64 格式

---

## 项目结构

```
Mini-C/
├── src/                          # 源代码目录
│   ├── frontend/                 # 编译器前端
│   │   ├── lex.l                 # Flex 词法分析器
│   │   └── yacc.y                # Bison 语法分析器
│   ├── utils/                    # 工具函数库
│   │   ├── tree.c/h              # AST 树结构
│   │   ├── hashMap.c/h           # 哈希表（符号表）
│   │   ├── stack.c/h             # 栈结构
│   │   ├── inner.c/h             # 中间代码处理
│   │   └── linkList.c/h          # 链表结构
│   ├── ir/                       # 中间表示（扩展）
│   ├── semantic/                 # 语义分析（扩展）
│   └── optimization/             # 代码优化（扩展）
│
├── scripts/                      # 脚本工具
│   ├── asm_generator.py          # 汇编代码生成器（Python）
│   ├── check_env.sh              # 环境检查脚本
│   └── test_runner.py            # 测试运行器
│
├── web/                          # Web 可视化界面
│   ├── app.py                    # Flask 后端服务
│   ├── static/                   # 静态资源
│   │   ├── style.css             # 样式表
│   │   └── script.js             # 前端逻辑
│   ├── templates/                # HTML 模板
│   │   └── index.html            # 主页面
│   └── requirements.txt          # Python 依赖
│
├── docs/                         # 文档目录
│   ├── grammar.md                # 语法规范
│   ├── design.md                 # 设计文档
│   ├── codegen_manual.md         # 代码生成手册
│   ├── optimization.md           # 优化说明
│   ├── user_guide.md             # 用户指南
│   ├── api.md                    # API 文档
│   └── error_handling_guide.md   # 错误处理指南
│
├── tests/                        # 测试用例
│   ├── integration/              # 集成测试
│   ├── codegen/                  # 代码生成测试
│   ├── array/                    # 数组测试
│   ├── pointer/                  # 指针测试
│   └── ...                       # 其他测试
│
├── temp.c                        # 示例/测试源文件
├── Makefile                      # 构建脚本
└── README.md                     # 本文件
```

---

## 环境要求

### 必需工具

| 工具          | 版本要求 | 用途           |
| ------------- | -------- | -------------- |
| **GCC**       | ≥ 7.0    | C 编译器       |
| **Flex**      | ≥ 2.6    | 词法分析器生成 |
| **Bison**     | ≥ 3.0    | 语法分析器生成 |
| **NASM**      | ≥ 2.14   | 汇编器         |
| **Python**    | ≥ 3.6    | 汇编生成脚本   |
| **WSL/Linux** | -        | 运行环境       |

### 可选工具

| 工具      | 用途       |
| --------- | ---------- |
| **Flask** | Web 界面   |
| **Make**  | 自动化构建 |

### 安装依赖（Ubuntu/WSL）

```bash
# 安装编译工具
sudo apt update
sudo apt install -y gcc flex bison nasm python3 python3-pip make

# 安装 Web 界面依赖（可选）
pip3 install flask
```

---

## 快速开始

### 方式一：命令行编译

```bash
# 1. 构建编译器
make

# 2. 编译示例程序
./compiler temp.c

# 3. 生成汇编代码
python3 scripts/asm_generator.py

# 4. 汇编并链接
nasm -f elf64 assembly.asm -o temp.o
gcc -no-pie -o program temp.o

# 5. 运行程序
./program
```

### 方式二：Web 界面

```bash
# 1. 进入 web 目录
cd web

# 2. 启动服务
python3 app.py

# 3. 打开浏览器访问
# http://127.0.0.1:5000
```

---

## 使用方式

### Makefile 命令

```bash
make              # 构建编译器
make clean        # 清理生成文件
make compile file=temp   # 编译指定文件（不含 .c 扩展名）
make run file=temp       # 编译并运行
```

### 编译器输出文件

| 文件           | 说明                       |
| -------------- | -------------------------- |
| `Lexical`      | 词法分析结果（Token 列表） |
| `Grammatical`  | 语法分析结果（语法树）     |
| `Innercode`    | 中间代码（三地址码）       |
| `assembly.asm` | 汇编代码                   |

---

## 编译器架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        Mini-C 编译器架构                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐      │
│  │ 源代码  │ -> │ 词法    │ -> │ 语法    │ -> │ 中间    │      │
│  │ (.c)    │    │ 分析    │    │ 分析    │    │ 代码    │      │
│  │         │    │ (Flex)  │    │ (Bison) │    │ 生成    │      │
│  └─────────┘    └─────────┘    └─────────┘    └─────────┘      │
│       │              │              │              │            │
│       v              v              v              v            │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐      │
│  │ temp.c  │    │ Lexical │    │Grammati-│    │Innercode│      │
│  │         │    │         │    │  cal    │    │         │      │
│  └─────────┘    └─────────┘    └─────────┘    └─────────┘      │
│                                                    │            │
│                                                    v            │
│                                             ┌─────────┐        │
│                                             │ 汇编    │        │
│                                             │ 生成    │        │
│                                             │(Python) │        │
│                                             └─────────┘        │
│                                                    │            │
│                                                    v            │
│                                             ┌─────────┐        │
│                                             │assembly │        │
│                                             │  .asm   │        │
│                                             └─────────┘        │
│                                                    │            │
│                             ┌──────────────────────┘            │
│                             v                                   │
│                      ┌─────────────┐                           │
│                      │ NASM + GCC  │                           │
│                      │   链接器    │                           │
│                      └─────────────┘                           │
│                             │                                   │
│                             v                                   │
│                      ┌─────────────┐                           │
│                      │  可执行文件  │                           │
│                      │   (ELF64)   │                           │
│                      └─────────────┘                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 编译阶段说明

1. **词法分析**：使用 Flex 将源代码分解为 Token 序列
2. **语法分析**：使用 Bison 构建抽象语法树（AST）
3. **中间代码生成**：将 AST 转换为三地址码（四元式）
4. **汇编代码生成**：将三地址码转换为 x86-64 汇编
5. **汇编与链接**：使用 NASM 和 GCC 生成可执行文件

---

## 示例代码

### temp.c - 综合功能测试

```c
// Mini-C 编译器 v3.0 综合功能测试

// 自定义函数：加法
int add(int a, int b) {
    return a + b;
}

void main() {
    // 1. 整数运算
    int a = 10;
    int b = 20;
    int c = a + b;
    output(c);  // 输出: 30

    // 2. 浮点运算
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output(sum);  // 输出: 5.14

    // 3. 常量折叠优化
    int result = 3 + 4 * 5;
    output(result);  // 输出: 23

    // 4. 函数调用
    int sum2 = add(10, 30);
    output(sum2);  // 输出: 40

    // 5. if-else 控制流
    if (a < b) {
        output(100);  // 输出: 100
    } else {
        output(0);
    }

    // 6. while 循环
    int i = 0;
    while (i < 3) {
        output(i);  // 输出: 0, 1, 2
        i = i + 1;
    }

    // 7. 浮点乘法
    float pi = 3.14;
    float doubled = pi * 2.0;
    output(doubled);  // 输出: 6.28

    // 8. 数组操作
    int arr[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = arr[0] + arr[1];
    output(arr[0]);  // 输出: 10
    output(arr[1]);  // 输出: 20
    output(arr[2]);  // 输出: 30

    // 9. 指针操作
    int val = 42;
    int* ptr;
    ptr = &val;
    output(*ptr);    // 输出: 42
    *ptr = 100;
    output(val);     // 输出: 100

    // 10. for 循环
    int j;
    for (j = 0; j < 3; j = j + 1) {
        output(j);  // 输出: 0, 1, 2
    }
}

// 预期输出序列：
// 30, 5.14, 23, 40, 100, 0, 1, 2, 6.28, 10, 20, 30, 42, 100, 0, 1, 2
```

---

## 文档索引

| 文档                                             | 说明               |
| ------------------------------------------------ | ------------------ |
| [docs/grammar.md](docs/grammar.md)               | Mini-C 语法规范    |
| [docs/design.md](docs/design.md)                 | 编译器设计文档     |
| [docs/codegen_manual.md](docs/codegen_manual.md) | 代码生成手册       |
| [docs/DEMO_WEB.md](docs/DEMO_WEB.md)             | Web 界面演示文档   |
| [docs/DEMO_WSL.md](docs/DEMO_WSL.md)             | WSL 命令行演示文档 |
| [docs/CONTRIBUTION.md](docs/CONTRIBUTION.md)     | 参考代码与贡献说明 |
| [web/README.md](web/README.md)                   | Web 界面说明       |

---

## 开发说明

### 核心文件说明

| 文件                       | 功能                   |
| -------------------------- | ---------------------- |
| `src/frontend/lex.l`       | 词法规则定义           |
| `src/frontend/yacc.y`      | 语法规则与语义动作     |
| `src/utils/tree.c`         | AST 节点与中间代码生成 |
| `src/utils/inner.c`        | 标签替换与代码处理     |
| `scripts/asm_generator.py` | 汇编代码生成           |
| `web/app.py`               | Web 后端服务           |

### 扩展开发

如需扩展编译器功能，可参考：

1. **添加新数据类型**：修改 `lex.l` 和 `yacc.y`
2. **添加新运算符**：修改 `yacc.y` 和 `asm_generator.py`
3. **添加新控制流**：修改 `tree.c` 和 `yacc.y`
4. **优化改进**：参考 `src/optimization/` 目录

---

## 许可证

本项目仅供学习和教学使用。

---

## 致谢

- 参考项目：[SimpleOnlineCompiler](https://github.com/cyclesw/SimpleOnlineCompiler)
- 工具链：Flex, Bison, NASM, GCC, Flask

---

**Mini-C 编译器 v3.0** © 2024
