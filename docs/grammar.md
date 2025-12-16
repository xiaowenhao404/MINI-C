# Mini-C 编译器文法定义

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
     | [0-9]+\.?[0-9]*[eE][+-]?[0-9]+
```

**示例**：
- `3.14`, `0.5`, `2.0` ✅
- `3.14e-2`, `1.5E+3` ✅（科学计数法，2.0版本）

#### 字符常量（Character Constants）

```
char = '[^']'
```

**示例**：`'a'`, `'0'`, `'\n'`

#### 字符串常量（String Constants）

```
string = "[^"]*"
```

**示例**：`"hello"`, `"world"`, `"test\n"`

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

空白字符会被忽略，但用于分隔token。

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
    ;
```

### 2.3 声明（Declarations）

```
declaration
    : type_specifier ID ';'
    | type_specifier ID '=' expression ';'
    | type_specifier ID ',' declaration_list ';'
    ;
```

**示例**：
```c
int a;
int b = 10;
int x = 1, y = 2, z;
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
    output_int(a);
} else {
    output_int(b);
}
```

### 2.6 迭代语句（Iteration Statements）

```
iteration_statement
    : WHILE '(' expression ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    | FOR '(' expression_statement expression_statement ')' statement
    ;
```

**示例**：
```c
while (i < 10) {
    i = i + 1;
}

for (int i = 0; i < 10; i = i + 1) {
    output_int(i);
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
    | STRING
    ;
```

#### 一元表达式

```
unary_expression
    : postfix_expression
    | '-' unary_expression
    | '+' unary_expression
    | '!' unary_expression
    ;
```

#### 后缀表达式

```
postfix_expression
    : primary_expression
    | postfix_expression '[' expression ']'    // 数组访问（2.0版本）
    | postfix_expression '(' ')'                // 函数调用
    | postfix_expression '(' argument_list ')'  // 函数调用（2.0版本）
    | postfix_expression '.' ID                  // 结构体成员（2.0版本）
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
2. **一元运算符** `+`, `-`, `!`
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
```

## 3. 语义约束（Semantic Constraints）

### 3.1 类型约束

#### 类型兼容性

- **赋值兼容**：
  - `int` ← `int` ✅
  - `float` ← `float` ✅
  - `float` ← `int` ✅（隐式转换）
  - `int` ← `float` ⚠️（需要显式转换，1.0版本警告）

#### 运算类型要求

- **算术运算**：
  - `+`, `-`, `*`, `/`, `%`：操作数必须为数值类型（int/float）
  - 混合类型：int 自动提升为 float

- **关系运算**：
  - `<`, `>`, `<=`, `>=`, `==`, `!=`：操作数必须为数值类型
  - 结果类型：int（0或1）

- **逻辑运算**：
  - `&&`, `||`, `!`：操作数必须为数值类型（0为假，非0为真）
  - 结果类型：int（0或1）

### 3.2 作用域规则

- **全局作用域**：函数定义、全局变量
- **局部作用域**：函数参数、局部变量
- **块作用域**：`{ }` 内的变量
- **符号遮蔽**：内层作用域可以遮蔽外层同名符号

### 3.3 变量规则

- **声明前使用**：变量必须先声明后使用
- **重复声明**：同一作用域内不能重复声明同名变量
- **初始化**：变量可以声明时初始化，也可以先声明后赋值

### 3.4 函数规则（2.0版本）

- **函数定义**：必须在使用前定义
- **参数匹配**：调用时参数数量和类型必须匹配
- **返回值**：有返回类型的函数必须返回对应类型的值

## 4. 扩展语法（2.0版本）

### 4.1 数组声明

```
declaration
    : type_specifier ID '[' INTEGER ']' ';'
    | type_specifier ID '[' INTEGER ']' '=' '{' initializer_list '}' ';'
    ;
```

### 4.2 指针声明

```
declaration
    : type_specifier '*' ID ';'
    | type_specifier '*' ID '=' '&' ID ';'
    ;
```

### 4.3 结构体定义

```
struct_specifier
    : STRUCT ID '{' struct_declaration_list '}' ';'
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : type_specifier ID ';'
    ;
```

## 5. 语法示例

### 5.1 完整程序示例

```c
void main() {
    int a = 10;
    int b = 20;
    int c;
    
    c = a + b;
    
    if (c > 25) {
        output_int(c);
    } else {
        output_int(0);
    }
}
```

### 5.2 复杂表达式示例

```c
void main() {
    int a = 1 + 2 * 3;        // 优先级：* 高于 +
    int b = (1 + 2) * 3;     // 括号改变优先级
    int c = a > b && a < 10; // 关系运算高于逻辑运算
    int d = a = b + c;       // 赋值优先级最低
}
```

### 5.3 控制流示例

```c
void main() {
    int i = 0;
    
    while (i < 10) {
        if (i % 2 == 0) {
            output_int(i);
        }
        i = i + 1;
    }
}
```

## 6. 语法错误示例

### 6.1 常见语法错误

```c
// 错误1：缺少分号
int a = 10  // ❌ 缺少分号

// 错误2：括号不匹配
if (a > b {  // ❌ 缺少右括号
    output_int(a);
}

// 错误3：类型不匹配
int a = 3.14;  // ⚠️ 警告：float赋值给int

// 错误4：未定义变量
int a = b;  // ❌ b未定义
```

## 7. 参考

- [ANSI C Grammar](http://www.quut.com/c/ANSI-C-grammar-y.html)
- [Flex & Bison Manual](https://www.gnu.org/software/bison/manual/)

---

**文档版本**: v1.0  
**最后更新**: 2025-12-16  
**维护者**: Mini-C 开发团队

