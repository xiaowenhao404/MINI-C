# 函数类型和符号测试说明

## 测试文件

- `test_function_type.c` - 测试函数类型系统（TASK205.1）
- `test_function_symbol.c` - 测试函数符号表支持（TASK205.2）

## 编译方法

由于 Windows PowerShell 对中文路径支持有限，请使用以下方法之一：

### 方法 1: 使用 Git Bash（推荐）

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C/tests/semantic

# 编译函数类型测试
gcc -o test_function_type test_function_type.c ../../src/semantic/type_system.c -I../../src/semantic -std=c99

# 运行
./test_function_type

# 编译函数符号测试
gcc -o test_function_symbol test_function_symbol.c ../../src/semantic/symbol_table.c ../../src/semantic/type_system.c -I../../src/semantic -std=c99

# 运行
./test_function_symbol
```

### 方法 2: 使用 Makefile

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C/tests/semantic
make all
make test
```

### 方法 3: 手动编译（CMD）

在 CMD 中执行：

```cmd
cd /d C:\Users\36774\Desktop\资料\绩点\大三上资料\编译原理课设\Mini-C\tests\semantic

gcc -o test_function_symbol.exe test_function_symbol.c ..\..\src\semantic\symbol_table.c ..\..\src\semantic\type_system.c -I..\..\src\semantic -std=c99

test_function_symbol.exe
```

## 预期输出

### test_function_type（函数类型测试）

```
========================================
Mini-C 编译器 - 函数类型测试
测试 TASK205 子任务 5.1
========================================

测试 1: 函数类型创建...
  ✓ 函数类型创建成功
  函数签名: int(int, float)

测试 2: 无参数函数类型...
  ✓ 无参数函数类型创建成功
  函数签名: void()

测试 3: 函数类型相等性比较...
  ✓ 相同函数类型判断为相等
  ✓ 不同返回类型的函数类型判断为不相等
  ✓ 不同参数数量的函数类型判断为不相等
  ✓ 不同参数类型的函数类型判断为不相等

测试 4: 函数类型字符串表示...
  函数 1: int(int, float, char)
  函数 2: void()
  函数 3: float(int)
  ✓ 函数类型字符串表示正确

测试 5: 复杂函数类型...
  ✓ 指针参数的函数类型创建成功
  函数签名: float(pointer, pointer)

========================================
所有测试通过！✓
========================================
```

### test_function_symbol（函数符号测试）

```
========================================
Mini-C 编译器 - 函数符号测试
测试 TASK205 子任务 5.2
========================================

测试 1: 函数符号插入...
  ✓ 函数符号插入成功
  函数名: add
  函数类型: int(int, int)

测试 2: 函数重定义检测...
  ✓ 第一次插入成功
  ✓ 重定义检测成功（返回 NULL）

测试 3: 函数符号查找...
  ✓ 函数符号查找正确
  已插入函数: add, calculate, print

测试 4: 函数辅助函数...
  ✓ symbol_is_function() 正确
  ✓ symbol_get_param_count() = 3
  ✓ 参数 0 类型: int
  ✓ 参数 1 类型: float
  ✓ 参数 2 类型: char
  ✓ 返回类型: int
  ✓ 无效参数索引返回 NULL

测试 5: 函数与变量符号共存...
  ✓ 插入变量 'a'
  ✓ 插入函数 'b'
  ✓ 插入变量 'c'
  ✓ 函数和变量符号可正确共存

测试 6: 不同作用域的函数符号...
  ✓ 全局函数插入成功（作用域级别 0）
  ✓ 局部变量插入成功（作用域级别 1）
  ✓ 退出作用域后全局函数仍可访问
  ✓ 局部变量已正确删除

========================================
所有测试通过！✓
========================================
```

## 实现的功能

✅ 函数符号插入  
✅ 函数重定义检测  
✅ 函数符号查找  
✅ 函数辅助函数（is_function, get_param_count, get_param_type, get_return_type）  
✅ 函数与变量共存  
✅ 作用域管理

---

**状态**: TASK205 子任务 5.1 和 5.2 已完成  
**下一步**: 子任务 5.3 - 扩展语法分析器

