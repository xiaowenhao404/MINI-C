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

import sys
import os
import traceback

# 读取中间代码文件
four = []

# 调试：打印当前工作目录
print(f"📁 工作目录: {os.getcwd()}")
print(f"📄 中间代码文件存在: {os.path.exists('Innercode')}")

try:
    file = open('Innercode', 'r', encoding='utf-8')
    stmts = file.readlines()
    file.close()
    print(f"📖 读取了 {len(stmts)} 行中间代码")
except FileNotFoundError:
    print("❌ 错误: 找不到中间代码文件 Innercode")
    print("💡 请先运行编译器生成中间代码")
    sys.exit(1)
except Exception as e:
    print(f"❌ 读取文件错误: {e}")
    sys.exit(1)

for i in range(len(stmts)):
    stmts[i] = stmts[i].replace('\n', '')

# 第一遍：建立行号到四元组索引的映射
line_to_index = {}  # 中间代码行号 -> 四元组索引
parsed_stmts = []   # 解析后的语句（行号，操作数列表）

for stmt in stmts:
    ops = stmt.split()
    # 跳过空行
    if len(ops) < 2:
        continue
    
    # 第一个元素是行号（带冒号，如 "51:"）
    line_num_str = ops[0]
    if line_num_str.endswith(':'):
        line_num = int(line_num_str[:-1])
    else:
        try:
            line_num = int(line_num_str)
        except:
            continue
    
    # 记录行号到索引的映射
    current_index = len(parsed_stmts)
    line_to_index[line_num] = current_index
    
    # 转换数值类型
    for i in range(len(ops)):
        try:
            ops[i] = int(ops[i])
        except:
            try:
                ops[i] = float(ops[i])
            except:
                pass
    
    parsed_stmts.append((line_num, ops))

# 调试：打印行号映射
print("🔢 行号到索引的映射:")
for ln, idx in sorted(line_to_index.items()):
    print(f"  第 {ln} 行 -> 索引 {idx}")

# 第二遍：解析四元式，使用行号映射转换跳转目标
for line_num, ops in parsed_stmts:
    # ops[0] 是行号，ops[1] 是操作符或变量名
    
    if ops[1] == "if":
        # if expr goto line_num 格式
        target_line = ops[4] if isinstance(ops[4], int) else int(ops[4])
        target_index = line_to_index.get(target_line, target_line - 1)
        four.append(("j==", ops[2], 1, target_index))
    elif ops[1] == "goto":
        target_line = ops[2] if isinstance(ops[2], int) else int(ops[2])
        target_index = line_to_index.get(target_line, target_line - 1)
        four.append(('j', '_', '_', target_index))
    elif ops[1] == "store":
        # store value addr 格式（内存写入）
        value = ops[2] if len(ops) > 2 else '_'
        addr = ops[3] if len(ops) > 3 else '_'
        four.append(('store', value, addr, '_'))
    elif ops[1] == "arg":
        # 处理 arg 指令，可能有空参数
        arg_val = ops[2] if len(ops) > 2 else '_'
        four.append(('arg', '_', '_', arg_val))
    elif ops[1] == "call":
        four.append(('call', '_', '_', ops[2]))
    elif ops[1] == "return":
        val = ops[2] if len(ops) > 2 else '_'
        four.append(('ret', '_', '_', val))
    # 新增：函数定义相关
    elif ops[1] == "FUNC_BEGIN":
        func_name = ops[2] if len(ops) > 2 else "unknown"
        four.append(('FUNC_BEGIN', func_name, '_', '_'))
    elif ops[1] == "FUNC_END":
        func_name = ops[2] if len(ops) > 2 else "unknown"
        four.append(('FUNC_END', func_name, '_', '_'))
    elif ops[1] == "param":
        param_name = ops[2] if len(ops) > 2 else "unknown"
        four.append(('param', param_name, '_', '_'))
    elif len(ops) > 3 and ops[3] == '!':
        four.append((ops[3], ops[4], '_', ops[1]))
    elif len(ops) > 3 and ops[3] == "call":
        # t6 = call add 格式（有返回值的函数调用）
        func_name = ops[4] if len(ops) > 4 else "unknown"
        four.append(('call', func_name, '_', ops[1]))
    elif len(ops) > 3 and ops[3] == "alloc":
        # arr = alloc 20 格式（数组分配）
        size = ops[4] if len(ops) > 4 else 0
        four.append(('alloc', ops[1], size, '_'))
    elif len(ops) > 4 and ops[3] == "load":
        # t30 = load t29 格式（内存读取）
        addr_var = ops[4]
        four.append(('load', addr_var, '_', ops[1]))
    elif len(ops) > 4 and ops[3] == "addr":
        # t47 = addr var 格式（取地址）
        src_var = ops[4]
        four.append(('addr', src_var, '_', ops[1]))
    else:
        if len(ops) > 1 and isinstance(ops[1], str) and len(ops[1]) > 0 and ops[1][0] == 't':
            # t1 = a + b 格式
            if len(ops) > 5:
                four.append((ops[4], ops[3], ops[5], ops[1]))
            elif len(ops) > 3:
                # t1 = a 格式（简单赋值）
                four.append(('=', ops[3], '_', ops[1]))
            else:
                continue
        else:
            # a = b 格式或 a = 10 格式
            if len(ops) > 3:
                four.append((ops[2], ops[3], '_', ops[1]))
            elif len(ops) > 2:
                # 处理 a = 10 这种简单赋值
                four.append(('=', ops[2] if len(ops) > 2 else '_', '_', ops[1] if len(ops) > 1 else '_'))

# 输出文件准备
output_file = open('assembly.asm', 'w', encoding='utf-8')
result = []
t = []
data = [-1] * 100  # 变量栈位置表
var_type = {}      # 变量类型表（'int' 或 'float'）
defined_functions = set()  # 已定义的函数集合（避免重复定义）
in_duplicate_func = False  # 当前是否在处理重复函数
pending_args = []  # 待传递的函数参数列表
func_param_count = 0  # 当前函数的参数计数

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
    "param": ["函数参数", "param"],
    "return": ["函数返回", "return"],
    "FUNC_BEGIN": ["函数开始", "FUNC_BEGIN"],
    "FUNC_END": ["函数结束", "FUNC_END"],
    # 数组和指针（新增）
    "alloc": ["内存分配", "alloc"],
    "load": ["内存读取", "load"],
    "store": ["内存写入", "store"],
    "addr": ["取地址", "addr"],
    # 结构体（新增）
    "STRUCT_DEF": ["结构体定义", "STRUCT_DEF"],
    "MEMBER": ["结构体成员", "MEMBER"],
}

# 第一遍：标记跳转目标
n = 0
max_line = len(four)
for line in four:
    if rule[line[0]][0] == "判断":
        t.append(n)
        target = line[3]  # 已经是索引了
        if 0 <= target < max_line:
            t.append(target)
    elif rule[line[0]][0] == "转移":
        t.append(n)
        target = line[3]  # 已经是索引了
        if 0 <= target < max_line:
            t.append(target)
    n = n + 1

# 添加程序结束标签（如果有跳转到程序外的情况）
t.append(max_line)

t = list(set(t))
t.sort()  # 排序标签位置
four_in = {}
m = 1
for i in t:
    if i == max_line:
        four_in[i] = "END_PROGRAM"  # 程序结束标签
    else:
        four_in[i] = "CODE" + str(m)
        m += 1

# 调试：打印标签映射
print("🏷️ 标签映射:")
for idx, label in sorted(four_in.items()):
    print(f"  索引 {idx} -> {label}")

# 生成汇编代码
tab = "        "
result += ["extern printf, scanf"]
result += ["global main"]
result += [""]
result += ["section .text"]
result += ["main:"]
result += [tab + "push rbp"]
result += [tab + "mov rbp, rsp"]
result += [tab + "sub rsp, 1024  ; 分配栈空间（8字节对齐变量，最多100个变量需要800字节，预留1024）"]
result += [tab + "and rsp, -16  ; 强制16字节对齐"]
result += [tab + "jmp _main_code  ; 跳过函数定义"]
inx = "[rbp-"
main_code_label_inserted = False
in_user_function = False  # 跟踪是否在用户定义的函数内部

# 第二遍：生成代码
for i in range(len(four)):
    try:
        # 插入标签
        if i in four_in:
            result += [four_in[i] + ":"]
        
        # 检查四元组有效性
        if four[i][0] not in rule:
            print(f"❌ 错误: 未知操作符 '{four[i][0]}' 在四元组 {i}: {four[i]}")
            continue
        
        op_type = rule[four[i][0]][0]
        
        # 跟踪是否在用户函数内部
        if op_type == "函数开始":
            in_user_function = True
        elif op_type == "函数结束":
            in_user_function = False
        
        # 在第一个非函数定义代码前插入 _main_code 标签
        # 条件：不在用户函数内、标签未插入、且不是函数相关操作
        if (not main_code_label_inserted and 
            not in_user_function and 
            op_type not in ["函数开始", "函数结束", "函数参数"]):
            result += [""]
            result += ["_main_code:"]
            main_code_label_inserted = True
    
        # ============ 判断语句 ============
        if op_type == "判断":
            if four[i - 1][0] in ['>', '<', '>=', '<=', '==', '!=']:
                # 比较运算
                if isinstance(four[i - 1][1], (int, float)):
                    result += [tab + "mov eax," + str(four[i - 1][1])]
                else:
                    result += [tab + "mov eax," + inx + str(8 * data.index(four[i - 1][1]) + 8) + "]"]
                
                if isinstance(four[i - 1][2], (int, float)):
                    result += [tab + "mov ebx," + str(four[i - 1][2])]
                else:
                    result += [tab + "mov ebx," + inx + str(8 * data.index(four[i - 1][2]) + 8) + "]"]
                
                result += [tab + "cmp eax,ebx"]
                
                # 根据比较类型选择跳转指令
                jump_map = {
                    '>': 'ja', '<': 'jb', '>=': 'jae',
                    '<=': 'jbe', '==': 'je', '!=': 'jne'
                }
                target = four[i][3]
                if target not in four_in:
                    print(f"⚠️ 警告: 跳转目标 {target} 不在标签映射中，使用 END_PROGRAM")
                    target_label = "END_PROGRAM"
                else:
                    target_label = four_in[target]
                result += [tab + jump_map[four[i - 1][0]] + " " + target_label]
            else:
                # 布尔值判断
                if isinstance(four[i][2], (int, float)):
                    result += [tab + "mov eax," + str(four[i][2])]
                else:
                    result += [tab + "mov eax," + inx + str(8 * data.index(four[i][2]) + 8) + "]"]
                result += [tab + "cmp eax,1"]
                target = four[i][3]
                if target not in four_in:
                    print(f"⚠️ 警告: 跳转目标 {target} 不在标签映射中，使用 END_PROGRAM")
                    target_label = "END_PROGRAM"
                else:
                    target_label = four_in[target]
                result += [tab + "je " + target_label]
            result += [""]
        
        # ============ 赋值语句 ============
        elif op_type == "赋值":
            data[data.index(-1)] = four[i][3]  # 分配栈位置
            offset = 8 * data.index(four[i][3]) + 8
            
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
                # 变量赋值 - 检查是否是指针类型（需要64位操作）
                src_offset = 8 * data.index(four[i][1]) + 8
                is_ptr = four[i][1] in var_type and var_type[four[i][1]] == 'ptr'
                if is_ptr:
                    # 指针类型使用64位寄存器
                    result += [tab + "mov rax," + inx + str(src_offset) + "]  ; 指针赋值(64位)"]
                    result += [tab + "mov " + inx + str(offset) + "],rax"]
                else:
                    result += [tab + "mov eax," + inx + str(src_offset) + "]"]
                    result += [tab + "mov " + inx + str(offset) + "],eax"]
                # 继承类型
                if four[i][1] in var_type:
                    var_type[four[i][3]] = var_type[four[i][1]]
        
        # ============ 整数运算 ============
        elif op_type == "整数运算":
            # 检测操作数是否为浮点数
            op1_is_float = (isinstance(four[i][1], float) or 
                           (four[i][1] in var_type and var_type[four[i][1]] == 'float'))
            op2_is_float = (isinstance(four[i][2], float) or 
                           (four[i][2] in var_type and var_type[four[i][2]] == 'float'))
            use_float = op1_is_float or op2_is_float
            
            if use_float and four[i][0] in ["+", "-", "*", "/"]:
                # 浮点运算
                # 加载第一个操作数到xmm0
                if isinstance(four[i][1], float):
                    result += [tab + f"mov eax, __float32__({four[i][1]})"]
                    result += [tab + "movd xmm0, eax  ; 浮点立即数→xmm0"]
                elif isinstance(four[i][1], int):
                    result += [tab + f"mov eax, {four[i][1]}"]
                    result += [tab + "cvtsi2ss xmm0, eax  ; int→float"]
                elif four[i][1] != '_':
                    offset1 = 8 * data.index(four[i][1]) + 8
                    result += [tab + "movss xmm0," + inx + str(offset1) + "]  ; 加载float操作数1"]
                
                # 加载第二个操作数到xmm1
                if isinstance(four[i][2], float):
                    result += [tab + f"mov eax, __float32__({four[i][2]})"]
                    result += [tab + "movd xmm1, eax  ; 浮点立即数→xmm1"]
                elif isinstance(four[i][2], int):
                    result += [tab + f"mov eax, {four[i][2]}"]
                    result += [tab + "cvtsi2ss xmm1, eax  ; int→float"]
                elif four[i][2] != '_':
                    offset2 = 8 * data.index(four[i][2]) + 8
                    result += [tab + "movss xmm1," + inx + str(offset2) + "]  ; 加载float操作数2"]
                
                # 执行浮点运算
                float_ops = {"+": "addss", "-": "subss", "*": "mulss", "/": "divss"}
                result += [tab + float_ops[four[i][0]] + " xmm0,xmm1  ; 浮点运算"]
                
                # 存储结果
                data[data.index(-1)] = four[i][3]
                offset = 8 * data.index(four[i][3]) + 8
                result += [tab + "movss " + inx + str(offset) + "],xmm0  ; 存储float结果"]
                var_type[four[i][3]] = 'float'
            else:
                # 整数运算
                # 检测是否涉及指针运算（需要64位）
                op1_is_ptr = four[i][1] in var_type and var_type[four[i][1]] == 'ptr'
                op2_is_ptr = four[i][2] in var_type and var_type[four[i][2]] == 'ptr'
                use_ptr = op1_is_ptr or op2_is_ptr
                
                if use_ptr and four[i][0] in ["+", "-"]:
                    # 指针运算：使用64位寄存器
                    # 加载第一个操作数
                    if isinstance(four[i][1], (int, float)):
                        result += [tab + "mov rax," + str(int(four[i][1]))]
                    elif four[i][1] != '_':
                        if op1_is_ptr:
                            result += [tab + "mov rax," + inx + str(8 * data.index(four[i][1]) + 8) + "]  ; 指针(64位)"]
                        else:
                            result += [tab + "movsx rax, dword " + inx + str(8 * data.index(four[i][1]) + 8) + "]  ; int→64位"]
                    
                    # 加载第二个操作数
                    if isinstance(four[i][2], (int, float)):
                        result += [tab + "mov rbx," + str(int(four[i][2]))]
                    elif four[i][2] != '_':
                        if op2_is_ptr:
                            result += [tab + "mov rbx," + inx + str(8 * data.index(four[i][2]) + 8) + "]  ; 指针(64位)"]
                        else:
                            result += [tab + "movsx rbx, dword " + inx + str(8 * data.index(four[i][2]) + 8) + "]  ; int→64位"]
                    
                    # 执行运算
                    result += [tab + rule[four[i][0]][1] + " rax,rbx  ; 指针运算"]
                    
                    # 存储结果（64位）
                    data[data.index(-1)] = four[i][3]
                    offset = 8 * data.index(four[i][3]) + 8
                    result += [tab + "mov " + inx + str(offset) + "],rax"]
                    var_type[four[i][3]] = 'ptr'
                else:
                    # 普通整数运算
                    # 加载第一个操作数
                    if isinstance(four[i][1], (int, float)):
                        result += [tab + "mov eax," + str(int(four[i][1]))]
                    elif four[i][1] != '_':
                        result += [tab + "mov eax," + inx + str(8 * data.index(four[i][1]) + 8) + "]"]
                    
                    # 加载第二个操作数
                    if isinstance(four[i][2], (int, float)):
                        result += [tab + "mov ebx," + str(int(four[i][2]))]
                    elif four[i][2] != '_':
                        result += [tab + "mov ebx," + inx + str(8 * data.index(four[i][2]) + 8) + "]"]
                    
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
                    offset = 8 * data.index(four[i][3]) + 8
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
                offset1 = 8 * data.index(four[i][1]) + 8
                result += [tab + "movss xmm0," + inx + str(offset1) + "]  ; 加载float操作数1"]
            
            # 加载第二个操作数到xmm1
            if isinstance(four[i][2], float):
                result += [tab + f"mov eax, __float32__({four[i][2]})"]
                result += [tab + "movd xmm1, eax  ; 浮点立即数→xmm1"]
            elif isinstance(four[i][2], int):
                result += [tab + f"mov eax, {four[i][2]}"]
                result += [tab + "cvtsi2ss xmm1, eax  ; int→float"]
            elif four[i][2] != '_':
                offset2 = 8 * data.index(four[i][2]) + 8
                result += [tab + "movss xmm1," + inx + str(offset2) + "]  ; 加载float操作数2"]
            
            # 执行浮点运算（SSE指令）
            result += [tab + rule[four[i][0]][1] + " xmm0,xmm1  ; 浮点" + four[i][0][1:]]
            
            # 存储结果
            data[data.index(-1)] = four[i][3]
            offset = 8 * data.index(four[i][3]) + 8
            result += [tab + "movss " + inx + str(offset) + "],xmm0  ; 存储float结果"]
            var_type[four[i][3]] = 'float'
        
        # ============ 类型转换（新增）============
        elif op_type == "类型转换":
            data[data.index(-1)] = four[i][3]
            offset_src = 8 * data.index(four[i][1]) + 8
            offset_dst = 8 * data.index(four[i][3]) + 8
            
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
            target = four[i][3]
            if target not in four_in:
                print(f"⚠️ 警告: 跳转目标 {target} 不在标签映射中，使用 END_PROGRAM")
                target_label = "END_PROGRAM"
            else:
                target_label = four_in[target]
            result += [tab + "jmp " + target_label]
            result += [""]
        
        # ============ 返回语句 ============
        elif op_type == "返回":
            # 四元组格式: ('ret', '_', '_', val)，返回值在four[i][3]
            ret_val = four[i][3]
            if ret_val != '_' and ret_val is not None:
                if isinstance(ret_val, (int, float)):
                    result += [tab + f"mov eax, {int(ret_val)}  ; 返回值"]
                elif ret_val in data:
                    offset = 8 * data.index(ret_val) + 8
                    result += [tab + f"mov eax," + inx + str(offset) + "]  ; 返回值"]
            result += [tab + "leave"]
            result += [tab + "ret"]
        
        # ============ 函数调用 ============
        elif op_type == "函数":
            if four[i][0] == "arg":
                # 收集参数，在call时统一处理
                arg_value = four[i][3]
                
                # 跳过空参数
                if arg_value == '_' or arg_value is None:
                    continue
                
                # 将参数添加到待传递列表
                pending_args.append(arg_value)
            
            elif four[i][0] == "call":
                # 获取函数名（可能在 four[i][1] 或 four[i][3]）
                # 有返回值: ('call', func_name, '_', result_var)
                # 无返回值: ('call', '_', '_', func_name)
                func_name = four[i][1] if four[i][1] != '_' else four[i][3]
                
                # 检查参数类型（用于output函数）
                last_arg_is_float = False
                if pending_args:
                    arg_val = pending_args[-1]  # 最后一个参数
                    if arg_val in var_type and var_type[arg_val] == 'float':
                        last_arg_is_float = True
                    elif isinstance(arg_val, float):
                        last_arg_is_float = True
                
                if func_name == "output_float" or (func_name == "output" and last_arg_is_float):
                    # 输出浮点数
                    if pending_args:
                        arg_val = pending_args[0]
                        if isinstance(arg_val, float):
                            result += [tab + f"mov eax, __float32__({arg_val})"]
                            result += [tab + "movd xmm0, eax  ; float常量→xmm0"]
                        elif arg_val in data:
                            offset = 8 * data.index(arg_val) + 8
                            result += [tab + "movss xmm0," + inx + str(offset) + "]  ; float参数→xmm0"]
                    result += [tab + "cvtss2sd xmm0, xmm0  ; float→double（确保转换）"]
                    result += [tab + "mov rdi, out_format_float  ; %f格式"]
                    result += [tab + "mov rax, 1  ; 1个浮点参数在xmm0"]
                    result += [tab + "call printf wrt ..plt  ; 调用printf"]
                    pending_args.clear()  # 清空参数列表
                
                elif func_name == "output_int" or func_name == "output":
                    # 输出整数
                    if pending_args:
                        arg_val = pending_args[0]
                        if isinstance(arg_val, (int, float)):
                            result += [tab + f"mov rsi, {int(arg_val)}  ; 常量参数"]
                        elif arg_val in data:
                            offset = 8 * data.index(arg_val) + 8
                            result += [tab + "movsx rsi, dword " + inx + str(offset) + "]  ; int参数→rsi"]
                    result += [tab + "mov rdi, out_format_int  ; %d格式"]
                    result += [tab + "xor rax, rax  ; 0个浮点参数"]
                    result += [tab + "call printf wrt ..plt  ; 调用printf"]
                    pending_args.clear()  # 清空参数列表
                
                elif func_name == "input":
                    # 输入（整数）
                    result += [tab + 'mov rsi, number']
                    result += [tab + 'mov rdi, in_format_int']
                    result += [tab + "xor rax, rax"]
                    result += [tab + 'call scanf']
                    result += [tab + 'mov rbx, [number]']
                    result += [tab + 'add rsp, 8']
                    pending_args.clear()
                
                else:
                    # 自定义函数调用
                    # 格式: ('call', func_name, '_', result_var) 或 ('call', '_', '_', func_name)
                    # func_name 已在前面定义
                    if four[i][1] != '_' and four[i][3] != '_':
                        # 有返回值的函数调用
                        result_var = four[i][3]
                    else:
                        # 无返回值的函数调用
                        result_var = None
                    
                    # 按System V ABI顺序传递参数：edi, esi, edx, ecx, r8d, r9d
                    param_regs_32 = ['edi', 'esi', 'edx', 'ecx', 'r8d', 'r9d']
                    for idx, arg_val in enumerate(pending_args):
                        if idx >= 6:
                            # 超过6个参数需要通过栈传递（暂不支持）
                            result += [tab + f"; 警告：参数{idx+1}超出寄存器范围"]
                            break
                        
                        if isinstance(arg_val, (int, float)):
                            result += [tab + f"mov {param_regs_32[idx]}, {int(arg_val)}  ; 参数{idx+1}"]
                        elif arg_val in data:
                            offset = 8 * data.index(arg_val) + 8
                            result += [tab + f"mov {param_regs_32[idx]}," + inx + str(offset) + f"]  ; 参数{idx+1}"]
                        else:
                            result += [tab + f"; 警告：参数 {arg_val} 未定义"]
                    
                    pending_args.clear()  # 清空参数列表
                    
                    result += [tab + f"call {func_name}  ; 调用自定义函数"]
                    if result_var and result_var != '_':
                        # 保存返回值
                        if result_var not in data:
                            data[data.index(-1)] = result_var
                        offset = 8 * data.index(result_var) + 8
                        result += [tab + f"mov " + inx + str(offset) + "],eax  ; 保存返回值"]
                        var_type[result_var] = 'int'
        
        # ============ 函数定义 ============
        elif op_type == "函数开始":
            func_name = four[i][1] if len(four[i]) > 1 else "unknown"
            # 跳过已定义的函数（避免重复定义）
            if func_name in defined_functions:
                in_duplicate_func = True
                continue
            defined_functions.add(func_name)
            in_duplicate_func = False
            result += [""]
            result += [f"{func_name}:"]
            result += [tab + "push rbp"]
            result += [tab + "mov rbp, rsp"]
            result += [tab + "sub rsp, 64  ; 为局部变量分配空间"]
            # 重置局部变量表（每个函数有自己的栈帧）
            func_data = [-1] * 50
            func_param_count = 0
        
        elif op_type == "函数结束":
            # 跳过重复函数的结束
            if in_duplicate_func:
                in_duplicate_func = False
                continue
            result += [tab + "mov rsp, rbp"]
            result += [tab + "pop rbp"]
            result += [tab + "ret"]
            result += [""]
        
        elif op_type == "函数参数":
            # 跳过重复函数的参数
            if in_duplicate_func:
                continue
            # param x - 函数参数声明
            param_name = four[i][1] if len(four[i]) > 1 else four[i][3]
            if param_name not in data:
                data[data.index(-1)] = param_name
            offset = 8 * data.index(param_name) + 8
            # 从寄存器加载参数（System V ABI：rdi, rsi, rdx, rcx, r8, r9）
            param_regs = ['edi', 'esi', 'edx', 'ecx', 'r8d', 'r9d']
            if func_param_count < 6:
                result += [tab + f"mov " + inx + str(offset) + f"],{param_regs[func_param_count]}  ; 参数{func_param_count+1}"]
            func_param_count += 1
            var_type[param_name] = 'int'
        
        elif op_type == "函数返回":
            # return 或 return value
            # 四元组格式: ('ret', '_', '_', val)，返回值在four[i][3]
            ret_val = four[i][3]
            if ret_val != '_' and ret_val is not None:
                if isinstance(ret_val, (int, float)):
                    result += [tab + f"mov eax, {int(ret_val)}  ; 返回值"]
                elif ret_val in data:
                    offset = 8 * data.index(ret_val) + 8
                    result += [tab + f"mov eax," + inx + str(offset) + "]  ; 返回值"]
            result += [tab + "mov rsp, rbp"]
            result += [tab + "pop rbp"]
            result += [tab + "ret"]
        
        # ============ 内存操作（数组和指针）============
        elif op_type == "内存分配":
            # arr = alloc size
            arr_name = four[i][1] if len(four[i]) > 1 else "arr"
            size = four[i][2] if len(four[i]) > 2 and four[i][2] != '_' else 40
            size = int(size)
            
            # 为数组指针分配一个槽位
            if arr_name not in data:
                data[data.index(-1)] = arr_name
            offset = 8 * data.index(arr_name) + 8
            
            # 预留数组空间：为数组的每4字节预留一个槽位
            # 这样后续变量分配不会覆盖数组空间
            num_slots = (size + 3) // 4  # 向上取整
            for _ in range(num_slots):
                if -1 in data:
                    data[data.index(-1)] = f"__arr_reserved_{arr_name}_{_}"
            
            # 数组基址：在当前变量之后预留空间
            arr_base_offset = offset + size
            result += [tab + f"lea rax, [rbp-{arr_base_offset}]  ; 数组基址"]
            result += [tab + f"mov " + inx + str(offset) + "],rax  ; 保存数组指针"]
            var_type[arr_name] = 'ptr'
        
        elif op_type == "内存读取":
            # 四元组格式: ('load', addr_var, '_', result_var)
            addr_var = four[i][1]
            result_var = four[i][3]
            if result_var not in data:
                data[data.index(-1)] = result_var
            result_offset = 8 * data.index(result_var) + 8
            
            if addr_var in data:
                addr_offset = 8 * data.index(addr_var) + 8
                result += [tab + f"mov rax," + inx + str(addr_offset) + "]  ; 加载地址"]
                result += [tab + f"mov eax,[rax]  ; 解引用"]
                result += [tab + f"mov " + inx + str(result_offset) + "],eax"]
            var_type[result_var] = 'int'
        
        elif op_type == "内存写入":
            # store value addr
            value_var = four[i][1] if len(four[i]) > 1 else None
            addr_var = four[i][2] if len(four[i]) > 2 else None
            
            
            if value_var and addr_var:
                # 检查 value_var 是否是数字（整数或浮点数）
                is_num = isinstance(value_var, (int, float))
                if not is_num and isinstance(value_var, str):
                    try:
                        num_val = float(value_var) if '.' in value_var else int(value_var)
                        is_num = True
                    except ValueError:
                        is_num = False
                        num_val = None
                else:
                    num_val = value_var if is_num else None
                
                if is_num and num_val is not None:
                    result += [tab + f"mov ebx, {int(num_val)}  ; 存储立即数"]
                elif value_var in data:
                    val_offset = 8 * data.index(value_var) + 8
                    result += [tab + f"mov ebx," + inx + str(val_offset) + "]  ; 加载值"]
                else:
                    result += [tab + f"; 警告: store 值 {value_var} 未找到"]
                
                if addr_var in data:
                    addr_offset = 8 * data.index(addr_var) + 8
                    result += [tab + f"mov rax," + inx + str(addr_offset) + "]  ; 加载地址"]
                    result += [tab + f"mov [rax],ebx  ; 存储到内存"]
                else:
                    result += [tab + f"; 警告: store 地址 {addr_var} 未找到"]
        
        elif op_type == "取地址":
            # 四元组格式: ('addr', src_var, '_', result_var)
            src_var = four[i][1]
            result_var = four[i][3]
            
            if result_var not in data:
                data[data.index(-1)] = result_var
            result_offset = 8 * data.index(result_var) + 8
            
            
            if src_var in data:
                src_offset = 8 * data.index(src_var) + 8
                result += [tab + f"lea rax,[rbp-{src_offset}]  ; 取变量地址"]
                result += [tab + f"mov " + inx + str(result_offset) + "],rax"]
            var_type[result_var] = 'ptr'
        
        # ============ 结构体操作 ============
        elif op_type == "结构体定义":
            # STRUCT_DEF struct_name - 结构体定义（编译时处理，不生成运行时代码）
            struct_name = four[i][1] if len(four[i]) > 1 else "unknown"
            result += [f"; 结构体定义: {struct_name}"]
        
        elif op_type == "结构体成员":
            # MEMBER member_name - 结构体成员声明（编译时处理）
            member_name = four[i][1] if len(four[i]) > 1 else "unknown"
            result += [f"; 结构体成员: {member_name}"]

    except Exception as e:
        print(f"❌ 错误: 处理四元组 {i} 失败: {four[i]}")
        print(f"  异常: {e}")
        traceback.print_exc()
        raise

# 循环结束后，检查是否需要添加 END_PROGRAM 标签
if len(four) in four_in:
    result += [""]
    result += [four_in[len(four)] + ":"]

# 添加函数结束代码
result += [""]
result += [tab + "mov rsp, rbp"]
result += [tab + "pop rbp"]
result += [tab + "xor rax, rax  ; 返回0"]
result += [tab + "ret"]

# 数据段
result += [""]
result += ["section .data"]
result += [tab + 'out_format_int: db "%d", 10, 0  ; 整数输出格式']
result += [tab + 'out_format_float: db "%.2f", 10, 0  ; 浮点输出格式（保留2位小数）']
result += [tab + 'in_format_int: db "%d", 0  ; 整数输入格式']

# 添加浮点常量到数据段
float_const_idx = 0
for var_name, vtype in var_type.items():
    if vtype == 'float' and var_name.startswith('_fconst_'):
        # 这些是程序中使用的浮点常量
        pass

# BSS段（未初始化数据）
result += [""]
result += ["section .bss"]
result += [tab + 'number resb 4  ; 输入缓冲区']

# 写入文件
for line in result:
    output_file.write(f'{line}\n')

output_file.close()
print("✅ 汇编代码生成成功: assembly.asm")
print(f"  📊 总行数: {len(result)}")
print(f"  📦 变量数: {len([x for x in data if x != -1])}")
print(f"  🔢 整型变量: {len([k for k, v in var_type.items() if v == 'int'])}")
print(f"  🔣 浮点变量: {len([k for k, v in var_type.items() if v == 'float'])}")

