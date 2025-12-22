# Mini-C 编译器

<div align="center">

![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)

**一个功能完整的 Mini-C 语言编译器实现**

[快速开始](#-快速开始) • [功能特性](#-功能特性) • [项目结构](#-项目结构) • [文档](#-文档)

</div>

---

## 📖 项目简介

Mini-C 编译器是一个教学用的 C 语言子集编译器，实现了从词法分析到代码生成的完整编译流程。项目采用模块化设计，代码结构清晰，适合用于编译原理课程设计和学习参考。

### 核心特性

- ✅ **完整的编译流程**: 词法分析 → 语法分析 → 语义分析 → 中间代码生成 → 代码优化 → 目标代码生成
- ✅ **丰富的语言特性**: 支持函数、数组、指针、结构体等高级特性
- ✅ **代码优化**: 实现了常量折叠、死代码消除、活性分析等优化算法
- ✅ **Web 可视化界面**: 提供在线编译和中间结果展示功能
- ✅ **完善的文档**: 包含设计文档、API 文档、用户指南等

---

## ✨ 功能特性

### 语言支持

| 特性         | 支持情况 | 说明                               |
| ------------ | -------- | ---------------------------------- |
| **基本类型** | ✅       | int, float, char, void             |
| **函数**     | ✅       | 函数定义、调用、递归、参数传递     |
| **数组**     | ✅       | 一维数组、二维数组、数组初始化     |
| **指针**     | ✅       | 指针声明、取地址、解引用、指针运算 |
| **结构体**   | ✅       | 结构体定义、成员访问、指针访问     |
| **控制流**   | ✅       | if-else, while, for 循环           |
| **运算符**   | ✅       | 算术、关系、逻辑运算符             |
| **类型转换** | ✅       | 隐式类型转换、显式类型转换         |

### 优化功能

- **常量折叠**: 编译期计算常量表达式
- **死代码消除**: 删除不可达代码
- **活性分析**: 基于数据流分析的变量活跃性分析
- **栈槽复用**: 基于活性分析的栈空间优化（节省 20-50%栈空间）

### 工具支持

- **命令行编译器**: 完整的命令行编译工具
- **Web IDE**: 基于 Flask 的在线编译和可视化界面
- **测试套件**: 50+个测试用例，覆盖所有功能模块

---

## 🚀 快速开始

### 环境要求

- **操作系统**: Linux 或 WSL (Windows Subsystem for Linux)
- **CPU 架构**: x86-64
- **必需工具**:
  - Flex (词法分析器生成器)
  - Bison (语法分析器生成器)
  - GCC (C 编译器)
  - NASM (汇编器)
  - Python 3 (用于代码生成脚本)
  - Make (构建工具)

### 安装依赖

**Ubuntu/Debian 系统**:

```bash
sudo apt update
sudo apt install flex bison gcc nasm python3 python3-pip make git
```

**检查环境**:

```bash
bash scripts/check_env.sh
```

### 编译编译器

```bash
# 克隆或进入项目目录
cd Mini-C

# 编译编译器
make

# 应该看到输出：编译器生成成功: compiler
```

### 第一个程序

创建一个测试文件 `hello.c`:

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

---

## 📚 使用方法

### 基本编译

```bash
# 编译指定的C文件（不含.c扩展名）
make file=test

# 编译并运行
make run file=test
```

### 查看编译过程

```bash
# 查看词法分析结果
make show-lexical

# 查看语法分析结果
make show-grammar

# 查看中间代码（四元式）
make show-ir

# 查看汇编代码
make show-asm

# 查看所有中间结果
make show-all
```

### 调试版本

```bash
# 编译调试版本（包含调试符号）
make debug
```

### 运行测试

```bash
# 运行测试套件
make test

# 测试特定模块
make test-lexer    # 测试词法分析
make test-parser   # 测试语法分析
```

### 清理构建文件

```bash
# 清理生成文件
make clean

# 深度清理（包括备份文件）
make clean-all
```

---

## 📁 项目结构

```
Mini-C/
├── docs/                    # 项目文档
│   ├── api.md              # API接口文档
│   ├── design.md           # 设计文档
│   ├── user_guide.md       # 用户指南
│   └── ...
├── src/                     # 源代码
│   ├── frontend/           # 词法和语法分析
│   ├── semantic/           # 语义分析模块
│   ├── ir/                 # 中间代码生成
│   ├── optimization/       # 代码优化模块
│   └── utils/              # 工具模块
├── scripts/                 # 辅助脚本
│   ├── asm_generator_v2.py # 汇编代码生成器
│   └── ...
├── tests/                   # 测试用例
│   ├── array/              # 数组测试
│   ├── function/           # 函数测试
│   ├── pointer/            # 指针测试
│   ├── struct/             # 结构体测试
│   └── integration/        # 集成测试
├── web/                     # Web可视化界面
│   ├── app.py              # Flask后端
│   └── ...
└── Makefile                 # 构建文件
```

详细目录结构说明请查看 [DIRECTORY_STRUCTURE.md](DIRECTORY_STRUCTURE.md)

---

## 📖 文档

### 主要文档

- **[快速开始指南](QUICKSTART.md)** - 5 分钟快速上手
- **[目录结构说明](DIRECTORY_STRUCTURE.md)** - 详细的目录和文件说明
- **[用户指南](docs/user_guide.md)** - 完整的使用说明
- **[设计文档](docs/design.md)** - 架构设计和实现细节
- **[API 文档](docs/api.md)** - 各模块 API 接口说明
- **[测试指南](TESTING_GUIDE_v2.0.md)** - 测试方法和用例说明

### 功能文档

- **[语法规则](docs/grammar.md)** - Mini-C 支持的语法
- **[代码生成手册](docs/codegen_manual.md)** - 代码生成器使用说明
- **[优化文档](docs/optimization.md)** - 优化算法说明
- **[错误处理指南](docs/error_handling_guide.md)** - 错误处理机制

### 版本信息

- **[发布说明](RELEASE_NOTES_v2.0.md)** - v2.0 版本新功能和改进
- **[项目完成报告](PROJECT_COMPLETION_REPORT.md)** - 项目开发总结

---

## 🎯 使用示例

### 示例 1: 函数定义和调用

```c
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);  // 输出: 30
}
```

### 示例 2: 数组操作

```c
void main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int sum = 0;
    int i = 0;
    while (i < 5) {
        sum = sum + arr[i];
        i = i + 1;
    }
    output_int(sum);  // 输出: 150
}
```

### 示例 3: 指针操作

```c
void main() {
    int x = 100;
    int *p = &x;
    *p = 200;
    output_int(x);  // 输出: 200
}
```

### 示例 4: 结构体

```c
struct Point {
    int x;
    int y;
};

void main() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    output_int(p.x);  // 输出: 10
    output_int(p.y);  // 输出: 20
}
```

更多示例请查看 `tests/` 目录下的测试文件。

---

## 🌐 Web 可视化界面

项目提供了基于 Flask 的 Web IDE，支持在线编译和中间结果可视化。

### 启动 Web 界面

```bash
# 进入web目录
cd web

# 安装依赖（首次运行）
pip install -r requirements.txt

# 启动服务器
python app.py
```

访问 `http://127.0.0.1:5000` 即可使用。

详细说明请查看 [web/README.md](web/README.md) 和 [web/QUICKSTART.md](web/QUICKSTART.md)

---

## 🧪 测试

项目包含 50+个测试用例，覆盖所有功能模块。

### 运行测试

```bash
# 运行所有测试
make test

# 运行特定测试
cd tests/integration
make file=test_comprehensive
```

### 测试覆盖

- ✅ 词法分析测试
- ✅ 语法分析测试
- ✅ 语义分析测试
- ✅ 类型系统测试
- ✅ 函数功能测试
- ✅ 数组功能测试
- ✅ 指针功能测试
- ✅ 结构体功能测试
- ✅ 优化功能测试
- ✅ 集成测试

详细测试说明请查看 [TESTING_GUIDE_v2.0.md](TESTING_GUIDE_v2.0.md)

---

## 🔧 技术架构

### 编译流程

```
源代码(.c)
    ↓
词法分析 (Flex) → Token流
    ↓
语法分析 (Bison) → AST
    ↓
语义分析 → 类型标注的AST
    ↓
中间代码生成 → 四元式IR
    ↓
代码优化 → 优化后的IR
    ↓
汇编代码生成 → x86-64汇编
    ↓
汇编和链接 → 可执行文件
```

### 核心模块

- **前端**: 词法分析、语法分析（Flex + Bison）
- **语义分析**: 类型检查、符号表管理
- **中间代码**: 四元式 IR 生成
- **优化**: 常量折叠、死代码消除、活性分析
- **代码生成**: x86-64 汇编生成（NASM 格式）

### 技术栈

- **语言**: C, Python
- **工具**: Flex, Bison, GCC, NASM, Make
- **Web 框架**: Flask
- **前端**: HTML5, CSS3, JavaScript, CodeMirror

---

## 📊 项目统计

- **核心代码**: ~3700 行
- **测试代码**: ~2000 行
- **文档**: ~3000 行
- **总计**: ~8700 行
- **文件数**: 68 个文件
- **测试用例**: 50+个

---

## 🎓 适用场景

- ✅ **编译原理课程设计**: 完整的编译器实现案例
- ✅ **学习参考**: 理解编译器工作原理
- ✅ **毕业设计**: 可作为毕业设计项目基础
- ✅ **研究基础**: 可扩展的编译器框架

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

### 贡献指南

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 代码规范

- 使用中文注释
- 遵循现有代码风格
- 添加必要的测试用例
- 更新相关文档

---

## 📝 许可证

本项目采用 MIT 许可证。详见 LICENSE 文件。

---

## 🙏 致谢

- 感谢所有为项目做出贡献的开发者
- 参考了《编译原理》（龙书）等经典教材
- 使用了 Flex、Bison 等开源工具

---

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 提交 Issue: [GitHub Issues](项目地址/issues)
- 邮件: [联系邮箱]

---

## 🔮 未来计划

### v2.1 计划

- 枚举类型支持
- 联合体支持
- 更多优化算法

### v3.0 愿景

- 完整的 C89 标准支持
- 标准库实现
- 多后端支持（ARM、RISC-V）
- LLVM IR 生成

---

<div align="center">

**Mini-C 编译器 v2.0**  
_一个功能完整、代码清晰、文档完善的编译器实现_

[⬆ 回到顶部](#mini-c-编译器)

</div>
