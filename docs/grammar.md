# Mini-C 编译器语法规范

本文档定义 Mini-C 编译器的词法和语法规则。

## 1. 词法规则（Lexical Rules）

### 1.1 关键字（Keywords）

```
void    int     float   char
if      else    while   for
return  break   continue
```

### 1.2 标识符（Identifiers）

```
identifier = [a-zA-Z_][a-zA-Z0-9_]*
```

**规则**：

- 以字母或下划线开头
- 后续字符可以是字母、数字或下划线
- 区分大小写

**示例**：

- `main`, `x`, `temp_var`, `_count` ✅
- `123var`, `var-name` ❌

### 1.3 字面量（Literals）

#### 整数常量（Integer Constants）

```
integer = [0-9]+
```

**示例**：`0`, `42`, `100`

#### 浮点常量（Float Constants）

```
float = [0-9]+\.[0-9]+
```

**示例**：`3.14`, `0.5`, `2.0`

#### 字符常量（Character Constants）

```
char = '[^']'
```

**示例**：`'a'`, `'0'`, `'\n'`

### 1.4 运算符（Operators）

#### 算术运算符

```
+   -   *   /   %
```

#### 关系运算符

```
<   >   <=  >=  ==  !=
```

#### 逻辑运算符

```
&&  ||  !
```

#### 赋值运算符

```
=
```

#### 指针运算符

```
*   &
```

- `*` - 解引用（获取指针指向的值）
- `&` - 取地址（获取变量的地址）

#### 其他运算符

```
(   )   {   }   [   ]   ;   ,   .
```

### 1.5 注释（Comments）

#### 单行注释

```
// [^\n]*
```

#### 多行注释

```
/* ... */
```

**规则**：

- 单行注释：从 `//` 到行尾
- 多行注释：从 `/*` 到 `*/`，可以跨行
- 注释会被词法分析器忽略

**示例**：

```c
// 这是单行注释
int a = 10;  // 行尾注释

/*
 * 这是多行注释
 * 可以跨越多行
 */
```

### 1.6 空白字符（Whitespace）

```
whitespace = [ \t\n\r]+
```

空白字符会被忽略，但用于分隔 token。

## 2. 语法规则（Grammar Rules）

### 2.1 程序结构（Program Structure）

```
program
    : function_list
    ;

function_list
    : function
    | function_list function
    ;

function
    : type_specifier ID '(' ')' compound_statement
    | type_specifier ID '(' parameter_list ')' compound_statement
    ;
```

### 2.2 类型说明符（Type Specifiers）

```
type_specifier
    : VOID
    | INT
    | FLOAT
    | CHAR
    | type_specifier '*'    // 指针类型
    ;
```

### 2.3 声明（Declarations）

```
declaration
    : type_specifier ID ';'
    | type_specifier ID '=' expression ';'
    | type_specifier ID '[' INTEGER ']' ';'    // 数组声明
    | type_specifier '*' ID ';'                 // 指针声明
    ;
```

**示例**：

```c
int a;
int b = 10;
int arr[5];
int* ptr;
```

### 2.4 语句（Statements）

```
statement
    : expression_statement
    | compound_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    | declaration
    ;

expression_statement
    : expression ';'
    | ';'
    ;

compound_statement
    : '{' '}'
    | '{' statement_list '}'
    ;

statement_list
    : statement
    | statement_list statement
    ;
```

### 2.5 选择语句（Selection Statements）

```
selection_statement
    : IF '(' expression ')' statement
    | IF '(' expression ')' statement ELSE statement
    ;
```

**示例**：

```c
if (a > b) {
    output(a);
} else {
    output(b);
}
```

### 2.6 迭代语句（Iteration Statements）

```
iteration_statement
    : WHILE '(' expression ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;
```

**示例**：

```c
while (i < 10) {
    i = i + 1;
}

for (j = 0; j < 10; j = j + 1) {
    output(j);
}
```

### 2.7 表达式（Expressions）

#### 基本表达式

```
primary_expression
    : ID
    | constant
    | '(' expression ')'
    ;

constant
    : INTEGER
    | FLOAT
    | CHAR
    ;
```

#### 一元表达式

```
unary_expression
    : postfix_expression
    | '-' unary_expression
    | '+' unary_expression
    | '!' unary_expression
    | '*' unary_expression      // 解引用
    | '&' unary_expression      // 取地址
    ;
```

#### 后缀表达式

```
postfix_expression
    : primary_expression
    | postfix_expression '[' expression ']'    // 数组访问
    | postfix_expression '(' ')'                // 函数调用
    | postfix_expression '(' argument_list ')'  // 带参数的函数调用
    ;
```

#### 乘除表达式

```
multiplicative_expression
    : unary_expression
    | multiplicative_expression '*' unary_expression
    | multiplicative_expression '/' unary_expression
    | multiplicative_expression '%' unary_expression
    ;
```

#### 加减表达式

```
additive_expression
    : multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;
```

#### 关系表达式

```
relational_expression
    : additive_expression
    | relational_expression '<' additive_expression
    | relational_expression '>' additive_expression
    | relational_expression '<=' additive_expression
    | relational_expression '>=' additive_expression
    ;
```

#### 相等表达式

```
equality_expression
    : relational_expression
    | equality_expression '==' relational_expression
    | equality_expression '!=' relational_expression
    ;
```

#### 逻辑与表达式

```
logical_and_expression
    : equality_expression
    | logical_and_expression '&&' equality_expression
    ;
```

#### 逻辑或表达式

```
logical_or_expression
    : logical_and_expression
    | logical_or_expression '||' logical_and_expression
    ;
```

#### 赋值表达式

```
assignment_expression
    : logical_or_expression
    | unary_expression '=' assignment_expression
    ;
```

#### 表达式（顶层）

```
expression
    : assignment_expression
    ;
```

### 2.8 运算符优先级

从高到低：

1. **括号** `()` - 最高优先级
2. **一元运算符** `+`, `-`, `!`, `*`(解引用), `&`(取地址)
3. **乘除模** `*`, `/`, `%`
4. **加减** `+`, `-`
5. **关系运算符** `<`, `>`, `<=`, `>=`
6. **相等运算符** `==`, `!=`
7. **逻辑与** `&&`
8. **逻辑或** `||`
9. **赋值** `=` - 最低优先级

**示例**：

```c
a + b * c      // 等价于 a + (b * c)
a > b && c < d // 等价于 (a > b) && (c < d)
a = b + c      // 等价于 a = (b + c)
*ptr + 1       // 等价于 (*ptr) + 1
```

## 3. 语义约束（Semantic Constraints）

### 3.1 类型约束

#### 类型兼容性

- **赋值兼容**：
  - `int` ← `int` ✅
  - `float` ← `float` ✅
  - `float` ← `int` ✅（隐式转换）
  - `int*` ← `&int` ✅（取地址赋值给指针）

#### 运算类型要求

- **算术运算**：
  - `+`, `-`, `*`, `/`, `%`：操作数必须为数值类型（int/float）
  - 混合类型：int 自动提升为 float

- **关系运算**：
  - `<`, `>`, `<=`, `>=`, `==`, `!=`：操作数必须为数值类型
  - 结果类型：int（0 或 1）

- **逻辑运算**：
  - `&&`, `||`, `!`：操作数必须为数值类型（0 为假，非 0 为真）
  - 结果类型：int（0 或 1）

- **指针运算**：
  - `*`(解引用)：操作数必须为指针类型
  - `&`(取地址)：操作数必须为左值

### 3.2 作用域规则

- **全局作用域**：函数定义、全局变量
- **局部作用域**：函数参数、局部变量
- **块作用域**：`{ }` 内的变量
- **符号遮蔽**：内层作用域可以遮蔽外层同名符号

### 3.3 变量规则

- **声明前使用**：变量必须先声明后使用
- **重复声明**：同一作用域内不能重复声明同名变量
- **初始化**：变量可以声明时初始化，也可以先声明后赋值

### 3.4 数组规则

- **声明**：必须指定数组大小，如 `int arr[5];`
- **访问**：使用下标访问，如 `arr[0]`, `arr[i]`
- **下标**：必须为整数表达式
- **赋值**：可以对数组元素赋值，如 `arr[0] = 10;`

### 3.5 指针规则

- **声明**：使用 `*` 声明指针，如 `int* ptr;`
- **取地址**：使用 `&` 获取变量地址，如 `ptr = &val;`
- **解引用**：使用 `*` 访问指向的值，如 `*ptr = 100;`
- **赋值**：可以通过解引用修改指向的变量

### 3.6 函数规则

- **函数定义**：必须在使用前定义
- **参数匹配**：调用时参数数量和类型必须匹配
- **返回值**：有返回类型的函数必须返回对应类型的值

## 4. 语法示例

### 4.1 完整程序示例

```c
// 自定义函数
int add(int a, int b) {
    return a + b;
}

void main() {
    // 整数运算
    int a = 10;
    int b = 20;
    int c = a + b;
    output(c);  // 30
    
    // 浮点运算
    float x = 3.14;
    float y = 2.0;
    output(x + y);  // 5.14
    
    // 函数调用
    int result = add(10, 30);
    output(result);  // 40
    
    // 条件语句
    if (a < b) {
        output(100);
    } else {
        output(0);
    }
    
    // while 循环
    int i = 0;
    while (i < 3) {
        output(i);
        i = i + 1;
    }
    
    // for 循环
    int j;
    for (j = 0; j < 3; j = j + 1) {
        output(j);
    }
    
    // 数组操作
    int arr[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = arr[0] + arr[1];
    output(arr[2]);  // 30
    
    // 指针操作
    int val = 42;
    int* ptr;
    ptr = &val;
    output(*ptr);    // 42
    *ptr = 100;
    output(val);     // 100
}
```

### 4.2 语法错误示例

```c
// 错误1：缺少分号
int a = 10  // ❌ 缺少分号

// 错误2：括号不匹配
if (a > b {  // ❌ 缺少右括号
    output(a);
}

// 错误3：未定义变量
int a = b;  // ❌ b 未定义

// 错误4：数组越界（运行时错误）
int arr[5];
arr[10] = 1;  // ❌ 越界访问
```

## 5. 参考

- [ANSI C Grammar](http://www.quut.com/c/ANSI-C-grammar-y.html)
- [Flex & Bison Manual](https://www.gnu.org/software/bison/manual/)

---

**文档版本**: v3.0  
**最后更新**: 2024-12-23  
**维护者**: Mini-C 开发团队
