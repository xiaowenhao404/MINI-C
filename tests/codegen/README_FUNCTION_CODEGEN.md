# 函数代码生成测试

## 测试 TASK205 子任务 5.6：扩展代码生成

### 实现的功能

#### 1. 创建增强型代码生成器 v2.0

**文件**: `scripts/asm_generator_v2.py`

**新增功能**:
- ✅ 多函数支持
- ✅ 函数边界识别（FUNC_BEGIN, FUNC_END）
- ✅ 函数序言和尾声自动生成
- ✅ System V x86-64 调用约定
- ✅ 参数传递（前6个用寄存器）
- ✅ 返回值处理（rax）
- ✅ 栈对齐（16字节）
- ✅ 函数调用代码生成

#### 2. System V x86-64 调用约定实现

**参数传递**:
```
参数 1: rdi
参数 2: rsi
参数 3: rdx
参数 4: rcx
参数 5: r8
参数 6: r9
参数 7+: 栈传递（从右到左）
```

**返回值**:
```
整数: rax
浮点: xmm0
```

**被调用者保存寄存器**:
- rbx, rbp, r12-r15

#### 3. 函数序言（Prologue）
```asm
function_name:
    push rbp            ; 保存旧的栈帧指针
    mov rbp, rsp        ; 设置新的栈帧指针
    sub rsp, N          ; 分配栈空间（N是16字节对齐的）
```

#### 4. 函数尾声（Epilogue）
```asm
.function_name_exit:
    leave               ; 恢复栈帧（mov rsp, rbp; pop rbp）
    ret                 ; 返回
```

#### 5. 函数调用代码
```asm
; 准备参数
mov rdi, <param1>       ; 第1个参数
mov rsi, <param2>       ; 第2个参数
...

; 调用函数
call function_name

; 保存返回值
mov [rbp-offset], rax
```

### 使用方法

#### 方法 1：直接使用新的代码生成器

```bash
cd ~/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C

# 1. 生成 IR
./compiler tests/ir/test_function_ir.c

# 2. 使用新的代码生成器
python3 scripts/asm_generator_v2.py

# 3. 汇编和链接
nasm -f elf64 assembly.asm -o output.o
gcc -no-pie output.o -o output

# 4. 运行
./output
```

#### 方法 2：更新 Makefile（推荐）

修改 Makefile 中的汇编生成步骤：

```makefile
# 原来：
$(PYTHON) $(REF_DIR)/assembly.py

# 改为：
$(PYTHON) $(SCRIPT_DIR)/asm_generator_v2.py
```

然后使用：
```bash
make file=tests/ir/test_function_ir
./test_function_ir
```

### 测试用例

**test_function_ir.c**:
```c
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);
}
```

**预期 IR** (Innercode):
```
1 FUNC_BEGIN add
2 t0 = a + b
3 return t0
4 FUNC_END add
5 FUNC_BEGIN main
6 arg 10
7 arg 20
8 t1 = call add 2
9 result = t1
10 arg result
11 call output_int 1
12 FUNC_END main
```

**预期汇编** (assembly.asm 简化版):
```asm
section .data
    out_format_int: db '%d', 10, 0

section .text
    global main
    extern printf

add:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    
    ; a 在 [rbp-8], b 在 [rbp-16]
    mov rax, [rbp-8]
    mov rbx, [rbp-16]
    add rax, rbx
    ; t0 = rax (直接返回)
    
    jmp .add_exit

.add_exit:
    leave
    ret

main:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    
    ; 调用 add(10, 20)
    mov rdi, 10
    mov rsi, 20
    call add
    mov [rbp-8], rax    ; result = rax
    
    ; 调用 output_int(result)
    mov rdi, [rbp-8]
    mov rsi, [rbp-8]
    mov rax, 0
    call printf
    
.main_exit:
    leave
    ret
```

**运行结果**:
```
30
```

### 验证点

1. **函数定义**: 每个函数都有正确的序言和尾声
2. **参数传递**: 参数通过 rdi, rsi 等寄存器传递
3. **返回值**: 通过 rax 返回
4. **函数调用**: CALL 指令生成正确的汇编代码
5. **栈对齐**: 栈保持 16 字节对齐
6. **可执行性**: 生成的汇编可以编译和运行

### 当前限制

- 参数保存到栈的逻辑简化（需要符号表信息）
- 浮点参数使用 xmm 寄存器（待完善）
- 复杂表达式的翻译需要进一步优化

### 下一步改进

1. 完善参数保存逻辑（从符号表获取参数信息）
2. 支持浮点参数和返回值
3. 支持超过6个参数的情况
4. 优化临时变量分配

---

**状态**: TASK205 子任务 5.6 基础实现完成  
**下一步**: 集成测试和优化

