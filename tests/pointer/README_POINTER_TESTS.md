# 指针基础测试

## 测试 TASK203：指针基础支持

### 实现的功能

#### 1. 语法支持（c-complier-master/yacc.y）

✅ **指针声明语法**:
```yacc
declare_expression
    | type '*' ID  // int *ptr;
    | type '*' ID '=' operate_expression  // int *ptr = &var;
```

✅ **取地址运算符**:
```yacc
unary_expression
    | '&' cast_expression  // &var
```

✅ **解引用运算符**:
```yacc
unary_expression
    | '*' cast_expression  // *ptr
```

#### 2. 语义分析（src/semantic/semantic_analyzer.c）

✅ **指针声明分析** (`analyze_pointer_declaration`):
- 创建指针类型（使用 `new_pointer_type()`）
- 插入符号表
- 检查初始化表达式类型
- 支持指针类型和整数类型初始化

✅ **取地址分析** (`analyze_addr_of`):
- 检查操作数是否为左值
- 返回指向操作数类型的指针类型
- 错误报告

✅ **解引用分析** (`analyze_deref`):
- 检查操作数是否为指针类型
- 返回指针指向的基类型
- 错误报告

✅ **辅助函数**:
- `is_lvalue()` - 检查表达式是否为左值

#### 3. IR 生成（src/ir/）

✅ **使用现有 IR 指令**:
- `IR_ADDR` - 取地址: `result = &var`
- `IR_LOAD` - 解引用: `result = *ptr`

✅ **指针翻译函数**:
- `translate_addr_of()` - 翻译取地址表达式
- `translate_deref()` - 翻译解引用表达式

✅ **IR 输出格式**:
```
t0 = &var        ; 取地址
t1 = load t0     ; 解引用
```

### 测试用例

#### test_basic_pointer.c

包含 6 个测试函数：

1. **test_pointer_declaration**: 指针声明和取地址
   ```c
   int a = 100;
   int *p = &a;
   ```

2. **test_deref_read**: 解引用读取值
   ```c
   int *ptr = &x;
   int y = *ptr;
   ```

3. **test_pointer_modify**: 通过指针修改变量值
   ```c
   int *ptr = &x;
   *ptr = 20;
   ```

4. **test_pointer_assignment**: 指针赋值
   ```c
   int *p1 = &a;
   int *p2 = &b;
   p1 = p2;
   ```

5. **test_pointer_chain**: 多级指针（基础，完整支持需要 TASK204）

6. **test_pointer_array**: 指针和数组结合
   ```c
   int arr[3] = {1, 2, 3};
   int *p = &arr[0];
   ```

### 编译和运行

在 Git Bash 中：

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C

# 重新生成语法分析器
flex c-complier-master/lex.l
bison -d c-complier-master/yacc.y

# 重新编译编译器
make clean
make

# 编译测试
./compiler tests/pointer/test_basic_pointer.c

# 查看 IR
cat Innercode

# 生成汇编（使用 v2 生成器）
python3 scripts/asm_generator_v2.py

# 汇编和链接
nasm -f elf64 assembly.asm -o output.o
gcc -no-pie output.o -o output

# 运行
./output
```

### 预期输出

测试1 (test_pointer_declaration):
```
100
```

测试2 (test_deref_read):
```
42
```

测试3 (test_pointer_modify):
```
20
```

测试4 (test_pointer_assignment):
```
200
```

测试6 (test_pointer_array):
```
1
1
```

### 验收标准

- [x] 正确解析指针声明语法
- [x] 指针类型正确创建（TYPE_POINTER）
- [x] 取地址运算符正确（&var）
- [x] 解引用运算符正确（*ptr）
- [x] 通过指针修改变量值（*ptr = value）
- [x] 指针赋值正确（p1 = p2）
- [x] 左值检查有效
- [x] 类型检查正确

### 注意事项

1. **指针大小**: 在 x86-64 架构下，指针是 8 字节
2. **左值要求**: 取地址运算符的操作数必须是左值（变量、数组元素、解引用表达式）
3. **类型检查**: 解引用运算符的操作数必须是指针类型
4. **初始化**: 指针可以初始化为另一个指针或整数（整数转换为指针）
5. **类型系统**: 使用 `new_pointer_type()` 创建指针类型

### 数据结构

**Type 结构体**（已支持）:
```c
typedef struct Type {
    TypeKind kind;     // TYPE_POINTER
    int size;          // 8 字节（x86-64）
    struct Type *base; // 指向的类型
} Type;
```

**左值判断规则**:
- 变量标识符（ID）是左值 ✓
- 数组访问是左值 ✓
- 解引用表达式是左值 ✓
- 常量、运算结果不是左值 ✗

---

**状态**: TASK203 子任务 3.1-3.3 已完成  
**待完成**: 子任务 3.4 - 代码生成（asm_generator_v2.py 中添加指针支持）

**下一步**: TASK204 - 指针运算和解引用（指针算术、多重指针）

