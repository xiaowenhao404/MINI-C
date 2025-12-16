#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Mini-C 编译器 - 增强型代码生成器 v2.0
支持函数定义、调用和 System V x86-64 调用约定

功能：
- 多函数支持
- System V ABI 调用约定
- 函数序言和尾声
- 参数传递（寄存器和栈）
- 返回值处理
"""

import sys

# ==================== 全局配置 ====================

# System V x86-64 调用约定：整数参数寄存器
PARAM_REGS_INT = ['rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9']

# 浮点参数寄存器
PARAM_REGS_FLOAT = ['xmm0', 'xmm1', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7']

# ==================== 读取 IR 指令 ====================

def read_ir_from_file(filename):
    """读取中间代码文件"""
    with open(filename, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    instructions = []
    for line in lines:
        line = line.strip()
        if line:
            # 移除行号
            parts = line.split(maxsplit=1)
            if len(parts) > 1:
                instructions.append(parts[1])
            else:
                instructions.append(line)
    
    return instructions

# ==================== 函数分组 ====================

def group_functions(instructions):
    """
    将 IR 指令按函数分组
    
    Returns:
        list of (func_name, [instructions])
    """
    functions = []
    current_func = None
    current_insts = []
    
    for inst in instructions:
        if inst.startswith("FUNC_BEGIN"):
            # 函数开始
            func_name = inst.split()[1]
            current_func = func_name
            current_insts = []
        
        elif inst.startswith("FUNC_END"):
            # 函数结束
            if current_func:
                functions.append((current_func, current_insts))
                current_func = None
                current_insts = []
        
        else:
            # 函数内的指令
            if current_func:
                current_insts.append(inst)
    
    return functions

# ==================== 栈空间计算 ====================

def analyze_variables(instructions):
    """分析函数中使用的变量"""
    variables = set()
    
    for inst in instructions:
        parts = inst.split()
        
        # 查找所有变量名（不是数字）
        for part in parts:
            if part and not part.isdigit() and part not in ['=', '+', '-', '*', '/', 'return', 'arg', 'call', 'goto', 'if']:
                if part.startswith('t') or part.isidentifier():
                    variables.add(part)
    
    return list(variables)

def calculate_stack_size(variables):
    """计算需要的栈空间（16字节对齐）"""
    size = len(variables) * 8  # 每个变量8字节
    # 对齐到16字节
    return ((size + 15) // 16) * 16

# ==================== 汇编代码生成 ====================

def generate_function_prologue(func_name, stack_size):
    """生成函数序言"""
    code = []
    code.append(f"{func_name}:")
    code.append("    push rbp")
    code.append("    mov rbp, rsp")
    if stack_size > 0:
        code.append(f"    sub rsp, {stack_size}")
    return code

def generate_function_epilogue(func_name):
    """生成函数尾声"""
    code = []
    code.append(f".{func_name}_exit:")
    code.append("    leave")
    code.append("    ret")
    return code

def get_variable_offset(var_name, variables):
    """获取变量的栈偏移量"""
    if var_name in variables:
        index = variables.index(var_name)
        return (index + 1) * 8
    return 0

def generate_function_body(instructions, func_name, variables):
    """生成函数体汇编代码"""
    code = []
    param_stack = []  # 用于收集 PARAM 指令的参数
    
    for inst in instructions:
        parts = inst.split()
        
        if not parts:
            continue
        
        # ============ 参数传递 ============
        if inst.startswith("arg "):
            # 收集参数
            param_value = parts[1]
            param_stack.append(param_value)
        
        # ============ 函数调用 ============
        elif inst.startswith("call ") or "= call" in inst:
            if '=' in inst:
                # 有返回值: t0 = call func 2
                result_var = parts[0]
                func_name_call = parts[3]
                arg_count = int(parts[4]) if len(parts) > 4 else len(param_stack)
            else:
                # 无返回值: call func 2
                result_var = None
                func_name_call = parts[1]
                arg_count = int(parts[2]) if len(parts) > 2 else len(param_stack)
            
            # 准备参数（前6个用寄存器）
            for i in range(min(arg_count, 6)):
                if i < len(param_stack):
                    param = param_stack[i]
                    if param.isdigit():
                        code.append(f"    mov {PARAM_REGS_INT[i]}, {param}")
                    else:
                        offset = get_variable_offset(param, variables)
                        code.append(f"    mov {PARAM_REGS_INT[i]}, [rbp-{offset}]")
            
            # 超过6个参数压栈（从右到左）
            for i in range(arg_count - 1, 5, -1):
                if i < len(param_stack):
                    param = param_stack[i]
                    if param.isdigit():
                        code.append(f"    push {param}")
                    else:
                        offset = get_variable_offset(param, variables)
                        code.append(f"    push qword [rbp-{offset}]")
            
            # 栈对齐
            if arg_count > 6:
                extra_args = arg_count - 6
                if extra_args % 2 == 1:
                    code.append("    sub rsp, 8  ; 栈对齐")
            
            # 调用函数
            code.append(f"    call {func_name_call}")
            
            # 清理栈
            if arg_count > 6:
                extra_args = arg_count - 6
                bytes_to_clean = extra_args * 8
                if extra_args % 2 == 1:
                    bytes_to_clean += 8
                code.append(f"    add rsp, {bytes_to_clean}")
            
            # 保存返回值
            if result_var:
                offset = get_variable_offset(result_var, variables)
                code.append(f"    mov [rbp-{offset}], rax  ; 保存返回值")
            
            # 清空参数栈
            param_stack = []
        
        # ============ 返回语句 ============
        elif inst.startswith("return"):
            if len(parts) > 1:
                # 有返回值
                return_value = parts[1]
                if return_value.isdigit():
                    code.append(f"    mov rax, {return_value}")
                else:
                    offset = get_variable_offset(return_value, variables)
                    code.append(f"    mov rax, [rbp-{offset}]")
            # 跳转到函数出口
            code.append(f"    jmp .{func_name}_exit")
        
        # ============ 赋值 ============
        elif '=' in inst and 'call' not in inst:
            # 简单赋值: a = b 或 a = 10
            parts = inst.split('=')
            lhs = parts[0].strip()
            rhs = parts[1].strip()
            
            # 确保变量在变量表中
            if lhs not in variables:
                variables.append(lhs)
            
            lhs_offset = get_variable_offset(lhs, variables)
            
            if rhs.isdigit():
                # 立即数
                code.append(f"    mov qword [rbp-{lhs_offset}], {rhs}")
            else:
                # 变量或表达式结果
                if rhs not in variables:
                    variables.append(rhs)
                rhs_offset = get_variable_offset(rhs, variables)
                code.append(f"    mov rax, [rbp-{rhs_offset}]")
                code.append(f"    mov [rbp-{lhs_offset}], rax")
        
        # ============ 算术运算 ============
        elif any(op in inst for op in [' + ', ' - ', ' * ', ' / ']):
            # 格式: t0 = a + b
            parts = inst.split('=')
            if len(parts) != 2:
                continue
            
            result_var = parts[0].strip()
            expr = parts[1].strip()
            
            # 确保结果变量在变量表中
            if result_var not in variables:
                variables.append(result_var)
            
            # 解析表达式
            for op in ['+', '-', '*', '/']:
                if f' {op} ' in expr:
                    operands = expr.split(op)
                    if len(operands) == 2:
                        left = operands[0].strip()
                        right = operands[1].strip()
                        
                        # 加载左操作数
                        if left.isdigit():
                            code.append(f"    mov rax, {left}")
                        else:
                            if left not in variables:
                                variables.append(left)
                            left_offset = get_variable_offset(left, variables)
                            code.append(f"    mov rax, [rbp-{left_offset}]")
                        
                        # 加载右操作数
                        if right.isdigit():
                            code.append(f"    mov rbx, {right}")
                        else:
                            if right not in variables:
                                variables.append(right)
                            right_offset = get_variable_offset(right, variables)
                            code.append(f"    mov rbx, [rbp-{right_offset}]")
                        
                        # 执行运算
                        if op == '+':
                            code.append("    add rax, rbx")
                        elif op == '-':
                            code.append("    sub rax, rbx")
                        elif op == '*':
                            code.append("    imul rax, rbx")
                        elif op == '/':
                            code.append("    xor rdx, rdx")
                            code.append("    idiv rbx")
                        
                        # 保存结果
                        result_offset = get_variable_offset(result_var, variables)
                        code.append(f"    mov [rbp-{result_offset}], rax")
                        break
    
    return code

def generate_function(func_name, instructions):
    """生成单个函数的汇编代码"""
    code = []
    
    # 分析变量
    variables = analyze_variables(instructions)
    stack_size = calculate_stack_size(variables)
    
    # 函数序言
    code.extend(generate_function_prologue(func_name, stack_size))
    
    # 保存参数（根据调用约定）
    # 假设参数已经通过 IR 分析确定（这里简化处理）
    
    # 函数体
    code.extend(generate_function_body(instructions, func_name, variables))
    
    # 函数尾声
    code.extend(generate_function_epilogue(func_name))
    
    return code

# ==================== 主函数 ====================

def main():
    """主函数：读取 IR 并生成汇编"""
    
    # 读取 IR 指令
    print("读取中间代码...")
    instructions = read_ir_from_file('Innercode')
    print(f"✓ 读取了 {len(instructions)} 条 IR 指令")
    
    # 按函数分组
    print("\n分析函数结构...")
    functions = group_functions(instructions)
    print(f"✓ 发现 {len(functions)} 个函数")
    for func_name, _ in functions:
        print(f"  - {func_name}")
    
    # 生成汇编代码
    print("\n生成汇编代码...")
    assembly = []
    
    # 数据段
    assembly.append("section .data")
    assembly.append("    out_format_int: db '%d', 10, 0")
    assembly.append("    out_format_float: db '%.2f', 10, 0")
    assembly.append("    in_format_int: db '%d', 0")
    assembly.append("")
    
    # 代码段
    assembly.append("section .text")
    assembly.append("    global main")
    assembly.append("    extern printf, scanf")
    assembly.append("")
    
    # 生成每个函数的代码
    for func_name, func_instructions in functions:
        print(f"  生成函数: {func_name}")
        func_code = generate_function(func_name, func_instructions)
        assembly.extend(func_code)
        assembly.append("")
    
    # BSS段
    assembly.append("section .bss")
    assembly.append("    number resb 8")
    
    # 写入文件
    with open('assembly.asm', 'w', encoding='utf-8') as f:
        for line in assembly:
            f.write(line + '\n')
    
    print(f"\n✓ 汇编代码生成成功！")
    print(f"  - 文件: assembly.asm")
    print(f"  - 共生成 {len(assembly)} 行汇编代码")
    print(f"  - 函数数量: {len(functions)}")
    
    return 0

if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as e:
        print(f"✗ 错误: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)

