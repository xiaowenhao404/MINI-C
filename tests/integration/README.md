# Mini-C 编译器集成测试

本目录包含 Mini-C 编译器的集成测试用例，测试编译器的端到端功能。

## 测试用例列表

### 1. test_basic.c
**测试功能**：基础功能
- 变量声明和初始化
- 简单算术运算
- if 条件语句
- 输出函数

**预期输出**：`60`

---

### 2. test_float_ops.c
**测试功能**：浮点运算
- 浮点变量声明
- 浮点加减乘运算
- 浮点输出（保留2位小数）

**预期输出**：
```
5.50
1.50
7.00
```

---

### 3. test_type_conversion.c
**测试功能**：类型转换
- int → float 转换（i2f）
- float → int 转换（f2i，截断）
- 混合类型运算

**预期输出**：
```
42.00
3
13.14
```

---

### 4. test_control_flow.c
**测试功能**：控制流
- if-else 语句
- while 循环
- 比较运算

**预期输出**：
```
1
0
1
2
```

---

### 5. test_const_folding.c
**测试功能**：常量折叠优化
- 编译期常量计算
- 复杂表达式优化
- 验证优化后的执行结果

**预期输出**：
```
23
14
100
```

---

### 6. test_dead_code.c
**测试功能**：死代码消除优化
- if(0) 分支删除
- if(1) 的 else 分支删除
- while(0) 循环删除

**预期输出**：
```
10
10
```

---

### 7. test_arithmetic.c
**测试功能**：算术运算
- 整数加减乘除
- 取模运算
- 运算优先级

**预期输出**：
```
23
17
60
6
2
```

---

### 8. test_comparison.c
**测试功能**：比较运算
- 大于、小于
- 大于等于、小于等于
- 等于、不等于

**预期输出**：
```
1
1
1
1
```

---

### 9. test_nested_if.c
**测试功能**：嵌套 if 语句
- 多层嵌套的 if-else
- 多重条件判断

**预期输出**：`80`

---

### 10. test_complex_expr.c
**测试功能**：复杂表达式
- 复杂的算术表达式
- 括号优先级
- 混合运算

**预期输出**：
```
15
26
5
```

---

## 运行测试

### 使用自动化测试脚本

```bash
# 运行所有集成测试
python scripts/test_runner.py --module integration

# 详细输出模式
python scripts/test_runner.py --module integration --verbose

# 运行所有测试（包括其他模块）
python scripts/test_runner.py
```

### 手动运行单个测试

```bash
# 1. 编译测试
./compiler tests/integration/test_basic.c

# 2. 生成汇编
python scripts/asm_generator.py

# 3. 汇编和链接（Linux/WSL）
nasm -f elf64 assembly.asm -o assembly.o
gcc assembly.o -o test_output -no-pie

# 4. 运行
./test_output

# 5. 比较输出
./test_output > actual.txt
diff actual.txt tests/integration/test_basic.expected
```

## 测试用例编写规范

### 文件命名
- 测试文件：`test_<name>.c`
- 预期输出：`test_<name>.expected`

### 代码风格
```c
/**
 * Mini-C 编译器 - <测试名称>
 * 
 * 测试场景：
 * - <场景1>
 * - <场景2>
 * - ...
 */

void main() {
    // 测试代码
    int a = 10;
    output_int(a);  // 预期输出：10
}
```

### 预期输出格式
- 每个输出独占一行
- 整数：`42`
- 浮点数：`3.14`（保留2位小数）
- 行尾不要有空格
- 文件末尾有换行符

**示例**：
```
10
3.14
hello
```

## 测试覆盖范围

### 已覆盖功能
- ✅ 基本变量声明和初始化
- ✅ 整数算术运算（+, -, *, /, %）
- ✅ 浮点算术运算（f+, f-, f*, f/）
- ✅ 类型转换（i2f, f2i）
- ✅ 比较运算（>, <, >=, <=, ==, !=）
- ✅ if-else 条件语句
- ✅ while 循环
- ✅ 嵌套控制流
- ✅ 复杂表达式和括号优先级
- ✅ 常量折叠优化
- ✅ 死代码消除优化

### 待添加测试
- ⏸️ for 循环
- ⏸️ 函数定义和调用
- ⏸️ 数组操作
- ⏸️ 指针操作
- ⏸️ 结构体
- ⏸️ 逻辑运算（&&, ||, !）
- ⏸️ 字符串处理
- ⏸️ 输入函数（input）
- ⏸️ 错误情况处理

## 添加新测试

### 步骤
1. 创建 `test_<name>.c` 文件
2. 编写测试代码（包含注释说明）
3. 创建 `test_<name>.expected` 文件
4. 手动运行验证
5. 添加到测试列表

### 示例：添加 for 循环测试

**test_for_loop.c**:
```c
/**
 * Mini-C 编译器 - for 循环测试
 * 
 * 测试场景：
 * - 基本 for 循环
 * - 循环计数
 */

void main() {
    int i;
    for (i = 0; i < 5; i = i + 1) {
        output_int(i);
    }
}
```

**test_for_loop.expected**:
```
0
1
2
3
4
```

## 调试技巧

### 查看中间代码
```bash
# 查看四元式
cat Innercode

# 查看汇编代码
cat assembly.asm
```

### GDB 调试
```bash
# 编译时带调试信息
gcc -g assembly.o -o test_output -no-pie

# 使用 GDB
gdb ./test_output
(gdb) break main
(gdb) run
(gdb) print variable_name
```

### 比较差异
```bash
# 使用 diff 查看详细差异
diff -u expected.txt actual.txt

# 或使用 vimdiff
vimdiff expected.txt actual.txt
```

## 常见问题

### Q1: 测试失败但输出看起来正确？
**A**: 检查行尾空格和换行符。使用 `cat -A file.txt` 查看不可见字符。

### Q2: 浮点输出不匹配？
**A**: 确保浮点输出格式为 `%.2f`（保留2位小数）。

### Q3: 如何测试编译错误？
**A**: 创建预期失败的测试，检查编译器是否正确报错。（待实现）

### Q4: 测试运行超时？
**A**: 检查是否有无限循环或死循环。

## 贡献指南

欢迎添加新的测试用例！请确保：
1. 测试用例有清晰的注释
2. 预期输出准确
3. 测试通过后再提交
4. 更新本 README

---

**版本**: 1.0  
**最后更新**: 2025-12-16  
**作者**: Mini-C 开发团队

