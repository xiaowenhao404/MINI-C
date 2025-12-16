# 函数 IR 生成测试

## 测试 TASK205 子任务 5.5：扩展 IR 指令集

### 实现的功能

#### 1. 新增 IR 指令（src/ir/ir.h）

✅ **IR_FUNC_BEGIN** - 函数开始标记
```
格式: FUNC_BEGIN func_name
示例: FUNC_BEGIN add
```

✅ **IR_FUNC_END** - 函数结束标记
```
格式: FUNC_END func_name
示例: FUNC_END add
```

✅ **IR_PARAM** (已有，增强说明)
```
格式: PARAM value
示例: PARAM 10
```

✅ **IR_CALL** (已有，增强说明)
```
格式: result = CALL func_name arg_count
示例: t1 = CALL add 2
```

✅ **IR_RETURN** (已有，增强说明)
```
格式: RETURN value 或 RETURN
示例: RETURN t0
```

#### 2. IR 指令字符串转换（src/ir/ir.c）

扩展 `opcode_to_string()` 函数：
- IR_FUNC_BEGIN → "FUNC_BEGIN"
- IR_FUNC_END → "FUNC_END"

扩展 `ir_instruction_to_string()` 函数：
- 格式化 FUNC_BEGIN 和 FUNC_END 指令输出

#### 3. IR 构建器翻译函数（src/ir/ir_builder.c）

✅ **translate_function_definition()**
- 生成函数开始标记
- 翻译函数体
- 生成函数结束标记

✅ **translate_function_call()**
- 递归翻译参数列表
- 为每个参数生成 PARAM 指令
- 生成 CALL 指令（包含参数数量）
- 返回临时变量存储返回值

✅ **translate_return_statement()**
- 翻译返回值表达式
- 生成 RETURN 指令

✅ **集成到主翻译流程**
- `translate_statement()` 中添加 FUNC_DEF 和 return_expression 处理
- `translate_expression()` 中添加 FUNC_CALL 处理

### 测试方法

在 Git Bash 中执行：

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C

# 重新编译编译器（包含新的 IR 生成代码）
make clean
make

# 编译测试文件（会生成 IR）
./compiler tests/ir/test_function_ir.c

# 查看生成的 IR
cat Innercode
```

### 预期 IR 输出

对于 `test_function_ir.c`，预期生成的 IR：

```
FUNC_BEGIN add
t0 = a + b
return t0
FUNC_END add

FUNC_BEGIN main
arg 10
arg 20
t1 = call add 2
result = t1
arg result
call output_int 1
FUNC_END main
```

### IR 格式说明

#### 函数定义块
```
FUNC_BEGIN <function_name>
    ... function body instructions ...
    RETURN <value>  (可选)
FUNC_END <function_name>
```

#### 函数调用序列
```
arg <param1>
arg <param2>
...
<result> = call <function_name> <param_count>
```

### 验证点

1. **函数边界标记**：每个函数都有 FUNC_BEGIN 和 FUNC_END
2. **参数传递**：PARAM 指令按参数顺序生成
3. **函数调用**：CALL 指令包含函数名和参数数量
4. **返回值**：RETURN 指令正确生成
5. **临时变量**：函数调用结果存储在新的临时变量中

### 与现有代码的兼容性

- ✅ 保持与现有 IR 格式兼容
- ✅ 扩展 IROpcode 枚举
- ✅ 不影响现有的算术、逻辑、控制流指令
- ✅ PARAM, CALL, RETURN 指令复用现有定义

### 数据结构

**IRInstruction** 四元式结构（保持不变）：
```c
typedef struct IRInstruction {
    IROpcode op;      // 操作码
    char *arg1;       // 第一操作数
    char *arg2;       // 第二操作数
    char *result;     // 结果
    int line;         // 行号
} IRInstruction;
```

**函数相关指令使用方式：**
- `IR_FUNC_BEGIN`: arg1=函数名, arg2=NULL, result=NULL
- `IR_FUNC_END`: arg1=函数名, arg2=NULL, result=NULL
- `IR_PARAM`: arg1=参数值, arg2=NULL, result=NULL
- `IR_CALL`: arg1=函数名, arg2=参数数量, result=返回值变量
- `IR_RETURN`: arg1=返回值, arg2=NULL, result=NULL

---

**状态**: TASK205 子任务 5.5 已完成  
**下一步**: 子任务 5.6 - 扩展代码生成

