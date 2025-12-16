# 一维数组测试

## 测试 TASK201：一维数组支持

### 实现的功能

#### 1. 语法支持（c-complier-master/yacc.y）

✅ **数组声明语法**:
```yacc
declare_expression
    | type ID '[' INT10 ']'  // int arr[10];
```

✅ **数组初始化语法**:
```yacc
    | type ID '[' INT10 ']' '=' '{' assignment_expression_list '}'  // int arr[3] = {1, 2, 3};
```

✅ **数组访问语法**（已存在）:
```yacc
postfix_expression
    | postfix_expression '[' operate_expression ']'  // arr[i]
```

✅ **初始化列表**:
```yacc
assignment_expression_list
    : assignment_expression
    | assignment_expression_list ',' assignment_expression
```

#### 2. 语义分析（src/semantic/semantic_analyzer.c）

✅ **数组声明分析** (`analyze_array_declaration`):
- 验证数组大小为正整数
- 创建数组类型
- 插入符号表
- 检查初始化列表

✅ **数组访问分析** (`analyze_array_access`):
- 检查是否为数组类型
- 检查下标类型（必须是整数）
- 编译期常量越界检查
- 返回元素类型

✅ **辅助函数**:
- `count_initializers()` - 计数初始化值
- `check_initializer_types()` - 检查初始值类型

#### 3. IR 生成（src/ir/）

✅ **新增 IR 指令**:
- `IR_ARRAY_ADDR` - 数组元素地址计算

✅ **数组访问翻译** (`translate_array_access`):
```
步骤：
1. 翻译下标表达式 → index_result
2. 计算偏移量: offset = index * element_size
3. 计算元素地址: addr = array_base + offset
4. 加载元素值: value = *addr
```

✅ **IR 输出格式**:
```
t0 = index * 4        ; 计算偏移量（假设 int 数组）
t1 = array_addr arr t0  ; 计算元素地址
t2 = load t1           ; 加载元素值
```

### 测试用例

#### test_basic_array.c

包含 6 个测试函数：

1. **test_array_init**: 数组声明和初始化
   ```c
   int arr[5] = {10, 20, 30, 40, 50};
   ```

2. **test_array_modify**: 数组元素修改
   ```c
   data[0] = 100;
   data[1] = 200;
   ```

3. **test_array_sum**: 数组遍历和求和
   ```c
   for (int i = 0; i < 5; i++) {
       sum = sum + numbers[i];
   }
   ```

4. **test_float_array**: float 类型数组

5. **test_char_array**: char 类型数组

6. **test_matrix_simulation**: 使用一维数组模拟二维矩阵

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
./compiler tests/array/test_basic_array.c

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

测试1 (test_array_init):
```
10
50
```

测试2 (test_array_modify):
```
100
200
300
```

测试3 (test_array_sum):
```
15
```

### 验收标准

- [ ] 正确解析数组声明语法
- [ ] 数组类型正确创建（TYPE_ARRAY）
- [ ] 数组符号正确插入符号表
- [ ] 数组下标访问类型检查正确
- [ ] 编译期常量越界检查有效
- [ ] 数组初始化语法正确解析
- [ ] 生成正确的 IR 指令
- [ ] 数组元素地址计算正确

### 注意事项

1. **内存布局**: 数组在栈上连续分配
2. **元素大小**: int=4字节, float=4字节, char=1字节
3. **地址计算**: `address = base + index * element_size`
4. **越界检查**: 仅对编译期常量下标进行检查
5. **类型系统**: 已有 `new_array_type()` 函数可直接使用

### 数据结构

**Type 结构体**（已支持）:
```c
typedef struct Type {
    TypeKind kind;     // TYPE_ARRAY
    int size;          // 数组总大小（元素大小 * 长度）
    struct Type *base; // 元素类型
    int array_len;     // 数组长度
} Type;
```

**Symbol 结构体**（已支持）:
```c
typedef struct Symbol {
    char *name;        // 数组名
    Type *type;        // 数组类型
    int offset;        // 栈偏移量（数组起始地址）
} Symbol;
```

---

**状态**: TASK201 子任务 1.1-1.3 已完成  
**待完成**: 子任务 1.4 - 代码生成（asm_generator_v2.py 中添加数组支持）

