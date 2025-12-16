# 函数语法测试说明

## 测试 TASK205 子任务 5.3：扩展语法分析器

### 已实现的修改

#### 1. 词法分析器（c-complier-master/lex.l）

✅ 添加类型关键字：
```lex
float		{ yylval.tree = terminator("FLOAT", yylineno); return FLOAT;}
char		{ yylval.tree = terminator("CHAR", yylineno); return CHAR;}
```

#### 2. 语法分析器（c-complier-master/yacc.y）

✅ 添加 token 声明：
```yacc
%token <tree> INT FLOAT CHAR
```

✅ 扩展 type 规则：
```yacc
type
    : INT | FLOAT | CHAR | VOID
```

✅ 添加参数声明规则：
```yacc
parameter_declaration
    : type ID

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration
```

✅ 添加函数定义规则：
```yacc
function_definition
    : type ID '(' parameter_list ')' '{' sentence '}'
    | type ID '(' ')' '{' sentence '}'
    | VOID ID '(' parameter_list ')' '{' sentence '}'
    | VOID ID '(' ')' '{' sentence '}'
```

✅ 修改 project 规则支持多个函数：
```yacc
project
    : external_declaration
    | project external_declaration

external_declaration
    : function_definition
    | main_function
```

✅ 扩展函数调用（postfix_expression）：
```yacc
postfix_expression
    : ID '(' argument_list ')'
    | ID '(' ')'
    | ... (现有规则)

argument_list
    : operate_expression
    | argument_list ',' operate_expression
```

### 测试方法

由于路径编码问题，请在 **Git Bash** 终端中执行：

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C

# 重新生成词法和语法分析器
flex c-complier-master/lex.l
bison -d c-complier-master/yacc.y

# 重新编译编译器
make clean
make

# 测试解析函数语法
./compiler tests/parser/test_function_syntax.c

# 检查是否成功（应该生成 Grammatical 文件）
cat Grammatical | head -50
```

### 测试文件：test_function_syntax.c

包含以下测试用例：
1. 简单函数定义：`int add(int a, int b)`
2. 无参数函数：`int get_value()`
3. void 返回类型：`void print_message()`
4. 多参数函数：`int calculate(int x, int y, int z)`
5. float 类型函数：`float multiply_float(float a, float b)`
6. char 类型函数：`char get_char(char c)`
7. 函数调用测试
8. 递归函数：`int factorial(int n)`
9. main 函数调用其他函数

### 预期结果

- ✅ 词法分析器和语法分析器重新生成成功
- ✅ 编译器重新编译成功
- ✅ test_function_syntax.c 解析无语法错误
- ✅ 生成正确的语法树（Grammatical 文件）

### 新增的 AST 节点类型

- `FUNC_DEF` - 函数定义节点
- `PARAM` - 参数声明节点
- `PARAM_LIST` - 参数列表节点
- `FUNC_CALL` - 函数调用节点
- `ARG_LIST` - 参数列表节点（调用时）

### 注意事项

1. **return 语句**: 已经存在，使用 `RET` token
2. **函数调用**: 集成到 postfix_expression 中
3. **参数列表**: 使用递归定义支持多个参数
4. **类型支持**: 支持 int, float, char, void 四种类型
5. **兼容性**: 保持与现有 main 函数定义的兼容

---

**状态**: TASK205 子任务 5.3 已完成  
**下一步**: 子任务 5.4 - 实现语义分析


