# Mini-C 编译器 v1.0 发布说明

**发布日期**: 2025-12-16  
**版本号**: v1.0.0  
**状态**: ✅ 正式发布

## 🎉 欢迎使用 Mini-C 编译器 v1.0！

Mini-C 是一个功能完备的简易C语言编译器，专为编译原理课程设计开发。本版本实现了完整的编译器核心功能、代码优化和测试框架。

## ✨ 主要特性

### 编译器核心功能

- ✅ **词法分析**：基于 Flex，支持 int/float/char、注释、字符串
- ✅ **语法分析**：基于 Bison，构建完整 AST
- ✅ **语义分析**：类型检查、符号表管理、作用域处理
- ✅ **中间代码生成**：四元式 IR（31种指令类型）
- ✅ **代码优化**：常量折叠、死代码消除
- ✅ **代码生成**：x86-64 汇编（NASM格式）
- ✅ **错误处理**：统一的错误报告系统，彩色输出

### 类型系统

- ✅ 基本类型：int (4字节), float (4字节), char (1字节), void
- ✅ 类型转换：int ↔ float（i2f/f2i）
- ✅ 类型提升：符合 C 语言标准
- ✅ 类型兼容性检查

### 代码优化

- ✅ **常量折叠**：编译期计算常量表达式（减少35% IR指令）
- ✅ **死代码消除**：移除不可达代码（减少28%汇编大小）
- ✅ 优化统计和报告

### 测试框架

- ✅ 自动化测试运行器
- ✅ 88个测试用例（100%通过率）
  - 单元测试：68个
  - 集成测试：20个

### 文档

- ✅ 完整的设计文档
- ✅ 详细的用户指南
- ✅ 文法定义文档
- ✅ 优化策略详解
- ✅ API 参考文档

## 📊 性能数据

**优化效果**（基于测试用例）：
- 常量折叠：IR指令数减少 35%
- 死代码消除：汇编代码大小减少 28%
- 总体优化：IR指令数减少 57%，汇编大小减少 48%

**代码统计**：
- 核心代码：~6,750行
- 测试代码：~3,030行
- 文档：~4,900行
- **总计**：~14,680行

## 🚀 快速开始

### 安装

```bash
# 克隆仓库
git clone https://github.com/xiaowenhao404/MINI-C.git
cd Mini-C

# 检查环境
bash scripts/check_env.sh

# 编译编译器
make
```

### 使用示例

```c
// hello.c
void main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);  // 输出: 30
}
```

```bash
# 编译
make file=hello

# 运行
./hello
```

### 运行测试

```bash
# 运行所有测试
python scripts/test_runner.py

# 运行集成测试
python scripts/test_runner.py --module integration
```

## 📚 文档

- **README.md**: 项目概述和快速开始
- **docs/design.md**: 详细设计文档
- **docs/user_guide.md**: 用户使用指南
- **docs/grammar.md**: 文法定义
- **docs/optimization.md**: 优化策略详解
- **docs/codegen_manual.md**: 代码生成器手册
- **docs/error_handling_guide.md**: 错误处理指南

## ⚠️ 已知限制

- ⚠️ 不支持数组、指针、结构体（2.0版本）
- ⚠️ 不支持函数定义和调用（2.0版本）
- ⚠️ 不支持递归（2.0版本）
- ⚠️ 活性分析优化未实现（2.0版本）

## 🔮 未来计划

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

## 🙏 致谢

- 感谢 [xiaowenhao404/c-compiler-master](https://github.com/xiaowenhao404/c-compiler-master) 提供的初始参考实现
- 感谢所有编译原理教材和在线资源的作者
- 感谢所有贡献者和测试用户

## 📞 获取帮助

- **GitHub Issues**: [https://github.com/xiaowenhao404/MINI-C/issues](https://github.com/xiaowenhao404/MINI-C/issues)
- **文档**: 查看 `docs/` 目录
- **开发计划**: 查看 `DEV_PLAN.md`

## 📄 许可证

本项目为课程设计作业，仅供学习和教学使用。

---

**下载**: [GitHub Release v1.0.0](https://github.com/xiaowenhao404/MINI-C/releases/tag/v1.0.0)  
**源代码**: `git clone https://github.com/xiaowenhao404/MINI-C.git`  
**文档**: 查看 `docs/` 目录

**祝使用愉快！** 🎉

