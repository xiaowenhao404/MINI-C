"""
Mini-C 编译器 - 增强型代码生成器
支持整数和浮点运算、类型转换、SSE指令集

功能：
- 整数运算：+, -, *, /, %
- 浮点运算：f+, f-, f*, f/
- 类型转换：i2f (int→float), f2i (float→int)
- 控制流：if/goto/label
- 函数调用：output_int, output_float
- 寄存器使用：
  * 整数：eax, ebx, ecx, edx
  * 浮点：xmm0, xmm1, xmm2
"""

# 读取中间代码文件
four = []
file = open('Innercode', 'r', encoding='utf-8')
stmts = file.readlines()
for i in range(len(stmts)):
    stmts[i] = stmts[i].replace('\n', '')

# 解析四元式
for stmt in stmts:
    ops = stmt.split()
    for i in range(len(ops)):
        try:
            # 尝试转换为整数
            ops[i] = int(ops[i])
        except:
            try:
                # 尝试转换为浮点数
                ops[i] = float(ops[i])
            except:
                pass
    
    # 解析不同类型的四元式
    if ops[1] == "if":
        four.append(("j==", ops[2], 1, ops[4]))
    elif ops[1] == "goto":
        four.append(('j', '_', '_', ops[2]))
    elif ops[1] == "arg":
        four.append(('arg', '_', '_', ops[2]))
    elif ops[1] == "call":
        four.append(('call', '_', '_', ops[2]))
    elif ops[1] == "return":
        val = ops[2] if len(ops) > 2 else '_'
        four.append(('ret', '_', '_', val))
    elif len(ops) > 3 and ops[3] == '!':
        four.append((ops[3], ops[4], '_', ops[1]))
    else:
        if (ops[1][0] == 't'):
            # t1 = a + b 格式
            four.append((ops[4], ops[3], ops[5], ops[1]))
        else:
            # a = b 格式
            four.append((ops[2], ops[3], '_', ops[1]))

# 输出文件准备
output_file = open('assembly.asm', 'w', encoding='utf-8')
result = []
t = []
data = [-1] * 100  # 变量栈位置表
var_type = {}      # 变量类型表（'int' 或 'float'）

# 操作符规则表（扩展支持浮点和类型转换）
rule = {
    # 判断和控制流
    "j==": ["判断", "JE"],
    "j": ["转移", "jmp"],
    # 整数运算
    "=": ["赋值", "mov"],
    "+": ["整数运算", "add"],
    "-": ["整数运算", "sub"],
    "*": ["整数运算", "mul"],
    "/": ["整数运算", "div"],
    "%": ["整数运算", "mod"],
    "^": ["整数运算", "power"],
    # 浮点运算（新增）
    "f+": ["浮点运算", "addss"],
    "f-": ["浮点运算", "subss"],
    "f*": ["浮点运算", "mulss"],
    "f/": ["浮点运算", "divss"],
    # 类型转换（新增）
    "i2f": ["类型转换", "cvtsi2ss"],  # int to float
    "f2i": ["类型转换", "cvttss2si"], # float to int
    # 关系运算
    ">=": ["关系运算", "GE"],
    "<=": ["关系运算", "LE"],
    ">": ["关系运算", "GT"],
    "<": ["关系运算", "LT"],
    "==": ["关系运算", "EQ"],
    "!=": ["关系运算", "NE"],
    # 逻辑运算
    "||": ["逻辑运算", "or"],
    "&&": ["逻辑运算", "and"],
    "!": ["逻辑运算", "not"],
    # 函数相关
    "ret": ["返回", "RET"],
    "arg": ["函数", "arg"],
    "call": ["函数", "call"],
}

# 第一遍：标记跳转目标
n = 0
for line in four:
    if rule[line[0]][0] == "判断":
        t.append(n)
        t.append(line[3] - 1)
    elif rule[line[0]][0] == "转移":
        t.append(n)
        t.append(line[3] - 1)
    n = n + 1

t = list(set(t))
four_in = {}
m = 1
for i in t:
    four_in[i] = "CODE" + str(m)
    m += 1

# 生成汇编代码
tab = "        "
result += ["extern printf, scanf"]
result += ["global main"]
result += [""]
result += ["section .text"]
result += ["main:"]
result += [tab + "enter 120, 0  ; 分配栈空间"]
inx = "[rbp-"

# 第二遍：生成代码
for i in range(len(four)):
    # 插入标签
    if i in four_in:
        result += [four_in[i] + ":"]
    
    op_type = rule[four[i][0]][0]
    
    # ============ 判断语句 ============
    if op_type == "判断":
        if four[i - 1][0] in ['>', '<', '>=', '<=', '==', '!=']:
            # 比较运算
            if isinstance(four[i - 1][1], (int, float)):
                result += [tab + "mov eax," + str(four[i - 1][1])]
            else:
                result += [tab + "mov eax," + inx + str(4 * data.index(four[i - 1][1]) + 4) + "]"]
            
            if isinstance(four[i - 1][2], (int, float)):
                result += [tab + "mov ebx," + str(four[i - 1][2])]
            else:
                result += [tab + "mov ebx," + inx + str(4 * data.index(four[i - 1][2]) + 4) + "]"]
            
            result += [tab + "cmp eax,ebx"]
            
            # 根据比较类型选择跳转指令
            jump_map = {
                '>': 'ja', '<': 'jb', '>=': 'jae',
                '<=': 'jbe', '==': 'je', '!=': 'jne'
            }
            result += [tab + jump_map[four[i - 1][0]] + " " + four_in[four[i][3] - 1]]
        else:
            # 布尔值判断
            if isinstance(four[i][2], (int, float)):
                result += [tab + "mov eax," + str(four[i][2])]
            else:
                result += [tab + "mov eax," + inx + str(4 * data.index(four[i][2]) + 4) + "]"]
            result += [tab + "cmp eax,1"]
            result += [tab + "je " + four_in[four[i][3] - 1]]
        result += [""]
    
    # ============ 赋值语句 ============
    elif op_type == "赋值":
        data[data.index(-1)] = four[i][3]  # 分配栈位置
        offset = 4 * data.index(four[i][3]) + 4
        
        if isinstance(four[i][1], (int, float)):
            # 立即数赋值
            if isinstance(four[i][1], float):
                # 浮点立即数（通过内存加载）
                var_type[four[i][3]] = 'float'
                result += [tab + f"mov eax, __float32__({four[i][1]})  ; 浮点常量"]
            else:
                # 整数立即数
                var_type[four[i][3]] = 'int'
                result += [tab + "mov eax," + str(four[i][1])]
            result += [tab + "mov " + inx + str(offset) + "],eax"]
        else:
            # 变量赋值
            src_offset = 4 * data.index(four[i][1]) + 4
            result += [tab + "mov eax," + inx + str(src_offset) + "]"]
            result += [tab + "mov " + inx + str(offset) + "],eax"]
            # 继承类型
            if four[i][1] in var_type:
                var_type[four[i][3]] = var_type[four[i][1]]
    
    # ============ 整数运算 ============
    elif op_type == "整数运算":
        # 加载第一个操作数
        if isinstance(four[i][1], (int, float)):
            result += [tab + "mov eax," + str(four[i][1])]
        elif four[i][1] != '_':
            result += [tab + "mov eax," + inx + str(4 * data.index(four[i][1]) + 4) + "]"]
        
        # 加载第二个操作数
        if isinstance(four[i][2], (int, float)):
            result += [tab + "mov ebx," + str(four[i][2])]
        elif four[i][2] != '_':
            result += [tab + "mov ebx," + inx + str(4 * data.index(four[i][2]) + 4) + "]"]
        
        # 执行运算
        if four[i][0] in ["+", "-"]:
            result += [tab + rule[four[i][0]][1] + " eax,ebx"]
        elif four[i][0] == "*":
            result += [tab + "imul ebx  ; 有符号乘法"]
        elif four[i][0] == "/":
            result += [tab + "xor edx, edx  ; 清除edx"]
            result += [tab + "idiv ebx  ; 有符号除法"]
        elif four[i][0] == "%":
            result += [tab + "xor edx, edx"]
            result += [tab + "idiv ebx"]
            result += [tab + "mov eax,edx  ; 取余数"]
        elif four[i][0] == "^":
            result += [tab + f"times {four[i][2]} imul eax  ; 幂运算"]
        elif four[i][0] in ['>', '<', '>=', '<=', '==', '!=']:
            pass  # 关系运算在判断语句中处理
        elif four[i][0] in ["&&", "||"]:
            result += [tab + rule[four[i][0]][1] + " eax,ebx"]
        elif four[i][0] == "!":
            result += [tab + "xor eax, 1  ; 逻辑取反"]
        else:
            result += [tab + f"; 未知运算符: {four[i][0]}"]
        
        # 存储结果
        data[data.index(-1)] = four[i][3]
        offset = 4 * data.index(four[i][3]) + 4
        result += [tab + "mov " + inx + str(offset) + "],eax"]
        var_type[four[i][3]] = 'int'
    
    # ============ 浮点运算（新增）============
    elif op_type == "浮点运算":
        # 加载第一个操作数到xmm0
        if isinstance(four[i][1], float):
            result += [tab + f"mov eax, __float32__({four[i][1]})"]
            result += [tab + "movd xmm0, eax  ; 浮点立即数→xmm0"]
        elif isinstance(four[i][1], int):
            result += [tab + f"mov eax, {four[i][1]}"]
            result += [tab + "cvtsi2ss xmm0, eax  ; int→float"]
        elif four[i][1] != '_':
            offset1 = 4 * data.index(four[i][1]) + 4
            result += [tab + "movss xmm0," + inx + str(offset1) + "]  ; 加载float操作数1"]
        
        # 加载第二个操作数到xmm1
        if isinstance(four[i][2], float):
            result += [tab + f"mov eax, __float32__({four[i][2]})"]
            result += [tab + "movd xmm1, eax  ; 浮点立即数→xmm1"]
        elif isinstance(four[i][2], int):
            result += [tab + f"mov eax, {four[i][2]}"]
            result += [tab + "cvtsi2ss xmm1, eax  ; int→float"]
        elif four[i][2] != '_':
            offset2 = 4 * data.index(four[i][2]) + 4
            result += [tab + "movss xmm1," + inx + str(offset2) + "]  ; 加载float操作数2"]
        
        # 执行浮点运算（SSE指令）
        result += [tab + rule[four[i][0]][1] + " xmm0,xmm1  ; 浮点" + four[i][0][1:]]
        
        # 存储结果
        data[data.index(-1)] = four[i][3]
        offset = 4 * data.index(four[i][3]) + 4
        result += [tab + "movss " + inx + str(offset) + "],xmm0  ; 存储float结果"]
        var_type[four[i][3]] = 'float'
    
    # ============ 类型转换（新增）============
    elif op_type == "类型转换":
        data[data.index(-1)] = four[i][3]
        offset_src = 4 * data.index(four[i][1]) + 4
        offset_dst = 4 * data.index(four[i][3]) + 4
        
        if four[i][0] == "i2f":
            # int → float
            result += [tab + "mov eax," + inx + str(offset_src) + "]  ; 加载int"]
            result += [tab + "cvtsi2ss xmm0, eax  ; int→float转换"]
            result += [tab + "movss " + inx + str(offset_dst) + "],xmm0  ; 存储float"]
            var_type[four[i][3]] = 'float'
        
        elif four[i][0] == "f2i":
            # float → int（截断）
            result += [tab + "movss xmm0," + inx + str(offset_src) + "]  ; 加载float"]
            result += [tab + "cvttss2si eax, xmm0  ; float→int转换（截断）"]
            result += [tab + "mov " + inx + str(offset_dst) + "],eax  ; 存储int"]
            var_type[four[i][3]] = 'int'
    
    # ============ 转移语句 ============
    elif op_type == "转移":
        result += [tab + "jmp " + str(four_in[four[i][3] - 1])]
        result += [""]
    
    # ============ 返回语句 ============
    elif op_type == "返回":
        result += [tab + "leave"]
        result += [tab + "ret"]
    
    # ============ 函数调用 ============
    elif op_type == "函数":
        if four[i][0] == "arg":
            # 准备参数
            result += [tab + "sub rsp, 8  ; 对齐栈"]
            offset = 4 * data.index(four[i][3]) + 4
            
            # 判断参数类型
            if four[i][3] in var_type and var_type[four[i][3]] == 'float':
                # 浮点参数：加载到xmm0
                result += [tab + "movss xmm0," + inx + str(offset) + "]  ; float参数→xmm0"]
                result += [tab + "cvtss2sd xmm0, xmm0  ; float→double（printf需要）"]
            else:
                # 整数参数：加载到rsi
                result += [tab + "mov rsi," + inx + str(offset) + "]  ; int参数→rsi"]
        
        elif four[i][0] == "call":
            if four[i][3] == "output_int" or four[i][3] == "output":
                # 输出整数
                result += [tab + "mov rdi, out_format_int  ; %d格式"]
                result += [tab + "xor rax, rax  ; 0个浮点参数"]
                result += [tab + "call printf"]
                result += [tab + "add rsp, 8  ; 清理栈"]
            
            elif four[i][3] == "output_float":
                # 输出浮点数
                result += [tab + "mov rdi, out_format_float  ; %f格式"]
                result += [tab + "mov rax, 1  ; 1个浮点参数在xmm0"]
                result += [tab + "call printf"]
                result += [tab + "add rsp, 8  ; 清理栈"]
            
            elif four[i][3] == "input":
                # 输入（整数）
                result += [tab + 'mov rsi, number']
                result += [tab + 'mov rdi, in_format_int']
                result += [tab + "xor rax, rax"]
                result += [tab + 'call scanf']
                result += [tab + 'mov rbx, [number]']
                result += [tab + 'add rsp, 8']

# 数据段
result += [""]
result += ["section .data"]
result += [tab + 'out_format_int: db "%d", 10, 0  ; 整数输出格式']
result += [tab + 'out_format_float: db "%.2f", 10, 0  ; 浮点输出格式（保留2位小数）']
result += [tab + 'in_format_int: db "%d", 0  ; 整数输入格式']

# BSS段（未初始化数据）
result += [""]
result += ["section .bss"]
result += [tab + 'number resb 4  ; 输入缓冲区']

# 写入文件
for line in result:
    output_file.write(f'{line}\n')

output_file.close()
print(f"✓ 汇编代码生成成功！已写入 assembly.asm")
print(f"  - 共生成 {len(result)} 行汇编代码")
print(f"  - 使用变量: {len([x for x in data if x != -1])} 个")
print(f"  - 整数变量: {len([k for k, v in var_type.items() if v == 'int'])} 个")
print(f"  - 浮点变量: {len([k for k, v in var_type.items() if v == 'float'])} 个")

