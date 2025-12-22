# Mini-C 编译器项目目录结构说明

本文档详细说明 Mini-C 编译器项目的目录结构和每个文件/文件夹的作用。

## 📁 项目根目录

```
Mini-C/
├── docs/                    # 项目文档目录
├── scripts/                 # 辅助脚本目录
├── src/                     # 源代码目录
├── tests/                   # 测试用例目录
├── web/                     # Web可视化界面目录
├── Makefile                 # 项目构建文件
├── .gitignore              # Git忽略文件配置
├── commit_msg.txt          # Git提交信息模板
├── PROJECT_COMPLETION_REPORT.md  # 项目完成报告
├── RELEASE_NOTES_v2.0.md   # v2.0版本发布说明
└── TESTING_GUIDE_v2.0.md   # 测试指南文档
```

---

## 📂 详细目录说明

### 1. `docs/` - 项目文档目录

存放项目的所有技术文档和用户文档。

| 文件                      | 作用                                       |
| ------------------------- | ------------------------------------------ |
| `api.md`                  | API 接口文档，说明各模块的接口定义         |
| `codegen_manual.md`       | 代码生成器使用手册                         |
| `design.md`               | 项目设计文档，包含架构设计、类型系统设计等 |
| `error_handling_guide.md` | 错误处理指南                               |
| `grammar.md`              | 语法规则文档，说明 Mini-C 支持的语法       |
| `optimization.md`         | 优化模块文档，说明优化算法和实现           |
| `user_guide.md`           | 用户使用指南，详细的使用说明               |

### 2. `src/` - 源代码目录

编译器的核心源代码，按功能模块组织。

#### 2.1 `src/frontend/` - 前端模块（词法/语法分析）

实现词法分析和语法分析。

| 文件     | 功能                                              |
| -------- | ------------------------------------------------- |
| `lex.l`  | Flex 词法分析器定义文件，定义 Token 识别规则      |
| `yacc.y` | Bison 语法分析器定义文件，定义语法规则和 AST 构建 |

#### 2.2 `src/semantic/` - 语义分析模块

实现语义分析、类型检查和符号表管理。

| 文件                    | 功能                                                     |
| ----------------------- | -------------------------------------------------------- |
| `semantic_analyzer.c/h` | 语义分析器主模块，负责类型检查、作用域分析等             |
| `symbol_table.c/h`      | 符号表实现，管理变量、函数、结构体等符号                 |
| `type_system.c/h`       | 类型系统实现，支持基本类型、数组、指针、结构体、函数类型 |

#### 2.3 `src/ir/` - 中间代码生成模块

将 AST 转换为中间代码（IR，四元式）。

| 文件             | 功能                               |
| ---------------- | ---------------------------------- |
| `ir.c/h`         | IR 指令定义和基础操作              |
| `ir_builder.c/h` | IR 构建器，负责从 AST 生成 IR 指令 |

#### 2.4 `src/optimization/` - 代码优化模块

实现各种代码优化算法。

| 文件                 | 功能                                            |
| -------------------- | ----------------------------------------------- |
| `cfg.c/h`            | 控制流图（CFG）构建，划分基本块并建立控制流关系 |
| `liveness.c/h`       | 活性分析算法，计算变量的活跃区间                |
| `stack_alloc.c/h`    | 栈槽分配优化，基于活性分析复用栈空间            |
| `const_folding.c/h`  | 常量折叠优化，编译期计算常量表达式              |
| `dead_code_elim.c/h` | 死代码消除优化，删除不可达代码                  |
| `README.md`          | 优化模块说明文档                                |

#### 2.5 `src/utils/` - 工具模块

提供错误处理和通用工具函数。

| 文件           | 功能                                 |
| -------------- | ------------------------------------ |
| `error.c/h`    | 错误报告系统，统一处理编译错误和警告 |
| `tree.c/h`     | 语法树（AST）数据结构和操作函数      |
| `hashMap.c/h`  | 哈希表实现，用于符号表管理           |
| `stack.c/h`    | 栈数据结构实现                       |
| `inner.c/h`    | 中间代码生成辅助函数                 |
| `linkList.c/h` | 链表数据结构实现                     |

### 3. `scripts/` - 辅助脚本目录

存放编译、测试、部署等辅助脚本。

| 文件               | 功能                                                         |
| ------------------ | ------------------------------------------------------------ |
| `asm_generator.py` | 汇编代码生成器，将中间代码转换为 x86-64 汇编（支持整数/浮点运算） |
| `check_env.sh`     | 环境检查脚本，验证开发环境是否配置正确                       |
| `git_commit.ps1`   | Git 提交脚本（PowerShell 版本）                              |
| `test_runner.py`   | 测试运行器，自动化运行测试套件                               |

### 4. `tests/` - 测试用例目录

按功能模块组织的测试用例。

#### 4.1 `tests/array/` - 数组功能测试

| 文件                    | 功能                 |
| ----------------------- | -------------------- |
| `test_basic_array.c`    | 一维数组基础功能测试 |
| `test_2d_array.c`       | 二维数组功能测试     |
| `README_ARRAY_TESTS.md` | 数组测试说明文档     |

#### 4.2 `tests/codegen/` - 代码生成测试

| 文件                         | 功能                 |
| ---------------------------- | -------------------- |
| `test_float_ops.c`           | 浮点运算代码生成测试 |
| `test_type_conversion.c`     | 类型转换代码生成测试 |
| `test_asm_generator.py`      | 汇编生成器测试脚本   |
| `example_innercode.txt`      | 中间代码示例         |
| `README.md`                  | 代码生成测试说明     |
| `README_FUNCTION_CODEGEN.md` | 函数代码生成说明     |

#### 4.3 `tests/function/` - 函数功能测试

| 文件                     | 功能             |
| ------------------------ | ---------------- |
| `test_array_parameter.c` | 数组参数传递测试 |

#### 4.4 `tests/integration/` - 集成测试

综合功能测试，验证各模块协同工作。

| 文件                               | 功能             |
| ---------------------------------- | ---------------- |
| `test_arithmetic.c/.expected`      | 算术运算集成测试 |
| `test_basic.c/.expected`           | 基础功能集成测试 |
| `test_comparison.c/.expected`      | 比较运算集成测试 |
| `test_complex_expr.c/.expected`    | 复杂表达式测试   |
| `test_const_folding.c/.expected`   | 常量折叠优化测试 |
| `test_control_flow.c/.expected`    | 控制流测试       |
| `test_dead_code.c/.expected`       | 死代码消除测试   |
| `test_float_ops.c/.expected`       | 浮点运算测试     |
| `test_nested_if.c/.expected`       | 嵌套 if 语句测试 |
| `test_type_conversion.c/.expected` | 类型转换测试     |
| `test_comprehensive.c`             | 综合功能测试     |
| `README.md`                        | 集成测试说明     |

#### 4.5 `tests/ir/` - 中间代码测试

| 文件                    | 功能             |
| ----------------------- | ---------------- |
| `test_ir_builder.c`     | IR 构建器测试    |
| `test_function_ir.c`    | 函数 IR 生成测试 |
| `README_FUNCTION_IR.md` | 函数 IR 测试说明 |

#### 4.6 `tests/lexer/` - 词法分析测试

**说明**: 词法分析测试目录（目前为空）

#### 4.7 `tests/optimization/` - 优化模块测试

| 文件                    | 功能             |
| ----------------------- | ---------------- |
| `test_const_folding.c`  | 常量折叠优化测试 |
| `test_dead_code_elim.c` | 死代码消除测试   |
| `test_liveness.c`       | 活性分析测试     |

#### 4.8 `tests/parser/` - 语法分析测试

| 文件                        | 功能             |
| --------------------------- | ---------------- |
| `test_function_syntax.c`    | 函数语法测试     |
| `README_FUNCTION_SYNTAX.md` | 函数语法测试说明 |

#### 4.9 `tests/pointer/` - 指针功能测试

| 文件                        | 功能             |
| --------------------------- | ---------------- |
| `test_basic_pointer.c`      | 指针基础功能测试 |
| `test_pointer_arithmetic.c` | 指针算术运算测试 |
| `README_POINTER_TESTS.md`   | 指针测试说明     |

#### 4.10 `tests/semantic/` - 语义分析测试

| 文件                              | 功能             |
| --------------------------------- | ---------------- |
| `test_function_semantics.c`       | 函数语义分析测试 |
| `test_function_symbol.c`          | 函数符号表测试   |
| `test_function_type.c`            | 函数类型系统测试 |
| `test_semantic_analyzer.c`        | 语义分析器测试   |
| `test_symbol_table.c`             | 符号表测试       |
| `test_type_system.c`              | 类型系统测试     |
| `Makefile`                        | 语义测试构建文件 |
| `README_FUNCTION_TESTS.md`        | 函数测试说明     |
| `README_SEMANTICS_INTEGRATION.md` | 语义集成测试说明 |

#### 4.11 `tests/struct/` - 结构体功能测试

| 文件                  | 功能               |
| --------------------- | ------------------ |
| `test_basic_struct.c` | 结构体基础功能测试 |

#### 4.12 `tests/samples/` - 示例代码

**说明**: 示例代码目录（目前为空）

#### 4.13 `tests/utils/` - 工具模块测试

| 文件           | 功能         |
| -------------- | ------------ |
| `error_demo.c` | 错误处理演示 |
| `test_error.c` | 错误处理测试 |

### 5. `web/` - Web 可视化界面

基于 Flask 的在线编译器 IDE。

| 文件/目录              | 功能                           |
| ---------------------- | ------------------------------ |
| `app.py`               | Flask 后端服务器，处理编译请求 |
| `requirements.txt`     | Python 依赖列表                |
| `README.md`            | Web 界面使用说明               |
| `QUICKSTART.md`        | Web 界面快速开始指南           |
| `TESTING.md`           | Web 界面测试指南               |
| `templates/index.html` | 前端 HTML 模板                 |
| `static/style.css`     | 界面样式文件                   |
| `static/script.js`     | 前端交互逻辑                   |

---

## 📄 根目录文件说明

| 文件                           | 功能                                     |
| ------------------------------ | ---------------------------------------- |
| `Makefile`                     | 项目构建文件，定义编译、测试、清理等命令 |
| `.gitignore`                   | Git 版本控制忽略文件配置                 |
| `commit_msg.txt`               | Git 提交信息模板                         |
| `PROJECT_COMPLETION_REPORT.md` | 项目完成报告，总结开发过程和成果         |
| `RELEASE_NOTES_v2.0.md`        | v2.0 版本发布说明，新功能和技术亮点      |
| `TESTING_GUIDE_v2.0.md`        | 测试指南，说明如何运行各种测试           |

---

## 🔍 关键文件查找指南

### 查找特定功能实现

- **词法分析**: `src/frontend/lex.l`
- **语法分析**: `src/frontend/yacc.y`
- **语法树**: `src/utils/tree.c`
- **符号表**: `src/semantic/symbol_table.c` 和 `src/utils/hashMap.c`
- **语义分析**: `src/semantic/semantic_analyzer.c`
- **类型系统**: `src/semantic/type_system.c`
- **IR 生成**: `src/ir/ir_builder.c` 和 `src/utils/inner.c`
- **代码优化**: `src/optimization/` 目录下各文件
- **汇编生成**: `scripts/asm_generator.py`

### 查找测试用例

- **函数测试**: `tests/function/`、`tests/semantic/test_function_*.c`
- **数组测试**: `tests/array/`
- **指针测试**: `tests/pointer/`
- **结构体测试**: `tests/struct/`
- **集成测试**: `tests/integration/`

### 查找文档

- **设计文档**: `docs/design.md`
- **用户指南**: `docs/user_guide.md`
- **API 文档**: `docs/api.md`
- **优化文档**: `docs/optimization.md`、`src/optimization/README.md`

---

## 📊 项目统计

- **源代码文件**: 约 20 个核心模块文件
- **测试文件**: 50+个测试用例
- **文档文件**: 15+个文档文件
- **代码行数**: 约 8700 行（核心代码 3700 行 + 测试 2000 行 + 文档 3000 行）

---

**最后更新**: 2025-12-16  
**维护**: Mini-C 开发团队
