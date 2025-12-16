"""
Mini-C 编译器 - 代码生成器测试脚本

测试策略：
1. 生成模拟的Innercode（四元式中间代码）
2. 运行asm_generator.py生成汇编
3. 验证生成的汇编代码是否包含期望的指令
"""

import os
import sys
import subprocess

# 测试用例：包含整数、浮点和类型转换的四元式
test_cases = [
    {
        "name": "整数运算测试",
        "innercode": """
1 a = 10
2 b = 3
3 t1 = a + b
4 arg t1
5 call output_int
6 return
""",
        "expected_asm": ["add eax,ebx", "mov rdi, out_format_int", "call printf"],
        "description": "测试整数加法和输出"
    },
    {
        "name": "浮点运算测试",
        "innercode": """
1 x = 3.14
2 y = 2.5
3 t1 = x f+ y
4 arg t1
5 call output_float
6 return
""",
        "expected_asm": ["movss xmm0", "addss xmm0,xmm1", "mov rdi, out_format_float"],
        "description": "测试浮点加法和输出"
    },
    {
        "name": "类型转换测试（i2f）",
        "innercode": """
1 a = 42
2 t1 i2f a
3 arg t1
4 call output_float
5 return
""",
        "expected_asm": ["cvtsi2ss xmm0, eax", "movss", "mov rdi, out_format_float"],
        "description": "测试整数到浮点转换"
    },
    {
        "name": "类型转换测试（f2i）",
        "innercode": """
1 x = 3.99
2 t1 f2i x
3 arg t1
4 call output_int
5 return
""",
        "expected_asm": ["movss xmm0", "cvttss2si eax, xmm0", "mov rdi, out_format_int"],
        "description": "测试浮点到整数转换"
    },
    {
        "name": "浮点四则运算",
        "innercode": """
1 a = 10.0
2 b = 2.0
3 t1 = a f+ b
4 t2 = a f- b
5 t3 = a f* b
6 t4 = a f/ b
7 return
""",
        "expected_asm": ["addss xmm0,xmm1", "subss xmm0,xmm1", "mulss xmm0,xmm1", "divss xmm0,xmm1"],
        "description": "测试浮点加减乘除"
    },
    {
        "name": "控制流测试",
        "innercode": """
1 a = 10
2 b = 5
3 t1 = a > b
4 if t1 goto 6
5 goto 7
6 arg a
7 return
""",
        "expected_asm": ["cmp eax,ebx", "ja CODE", "jmp CODE"],
        "description": "测试条件跳转"
    }
]

def run_test(test_case):
    """运行单个测试用例"""
    print(f"\n{'='*60}")
    print(f"测试: {test_case['name']}")
    print(f"描述: {test_case['description']}")
    print(f"{'='*60}")
    
    # 1. 写入测试的Innercode
    with open('Innercode', 'w', encoding='utf-8') as f:
        f.write(test_case['innercode'].strip())
    print("✓ 已生成测试用Innercode")
    
    # 2. 运行代码生成器
    try:
        result = subprocess.run(
            [sys.executable, '../../scripts/asm_generator.py'],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        if result.returncode != 0:
            print(f"✗ 代码生成器运行失败")
            print(f"错误信息: {result.stderr}")
            return False
        
        print(f"✓ 代码生成器运行成功")
        if result.stdout:
            print(f"输出: {result.stdout.strip()}")
        
    except subprocess.TimeoutExpired:
        print("✗ 代码生成器超时")
        return False
    except Exception as e:
        print(f"✗ 运行出错: {e}")
        return False
    
    # 3. 读取生成的汇编代码
    if not os.path.exists('assembly.asm'):
        print("✗ 未生成assembly.asm文件")
        return False
    
    with open('assembly.asm', 'r', encoding='utf-8') as f:
        asm_content = f.read()
    
    print(f"✓ 已读取生成的汇编代码（{len(asm_content.splitlines())} 行）")
    
    # 4. 验证期望的汇编指令
    all_found = True
    for expected in test_case['expected_asm']:
        if expected in asm_content:
            print(f"  ✓ 找到期望指令: {expected}")
        else:
            print(f"  ✗ 缺少期望指令: {expected}")
            all_found = False
    
    # 5. 显示关键汇编片段
    print("\n关键汇编代码片段:")
    print("-" * 60)
    lines = asm_content.splitlines()
    for i, line in enumerate(lines):
        # 显示包含关键指令的行
        if any(keyword in line for keyword in ['movss', 'addss', 'subss', 'mulss', 'divss', 
                                                  'cvt', 'printf', 'mov eax', 'add eax', 'cmp']):
            print(f"{i+1:3d}: {line}")
    
    return all_found

def main():
    """主测试函数"""
    print("="*70)
    print(" Mini-C 代码生成器测试套件")
    print("="*70)
    
    # 切换到测试目录
    test_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(test_dir)
    print(f"工作目录: {os.getcwd()}")
    
    # 运行所有测试
    passed = 0
    failed = 0
    
    for i, test_case in enumerate(test_cases, 1):
        try:
            if run_test(test_case):
                passed += 1
                print(f"\n✓ 测试 {i}/{len(test_cases)} 通过")
            else:
                failed += 1
                print(f"\n✗ 测试 {i}/{len(test_cases)} 失败")
        except Exception as e:
            failed += 1
            print(f"\n✗ 测试 {i}/{len(test_cases)} 异常: {e}")
    
    # 清理临时文件
    for filename in ['Innercode', 'assembly.asm']:
        if os.path.exists(filename):
            os.remove(filename)
            print(f"✓ 已清理临时文件: {filename}")
    
    # 输出测试总结
    print("\n" + "="*70)
    print(" 测试总结")
    print("="*70)
    print(f"总测试数: {len(test_cases)}")
    print(f"通过: {passed}")
    print(f"失败: {failed}")
    print(f"通过率: {passed/len(test_cases)*100:.1f}%")
    
    if failed == 0:
        print("\n🎉 所有测试通过！")
        return 0
    else:
        print(f"\n❌ 有 {failed} 个测试失败")
        return 1

if __name__ == "__main__":
    sys.exit(main())

