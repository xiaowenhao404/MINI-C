#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试语法分析器 - TASK205.3
"""

import subprocess
import os
import sys

project_root = r"C:\Users\36774\Desktop\资料\绩点\大三上资料\编译原理课设\Mini-C"
os.chdir(project_root)

print("=" * 60)
print("测试 TASK205 子任务 5.3：语法分析器扩展")
print("=" * 60)
print()

# 1. 重新生成词法分析器
print("步骤 1/4: 生成词法分析器...")
result = subprocess.run(["flex", "c-complier-master/lex.l"],
                       capture_output=True, text=True)
if result.returncode != 0:
    print(f"✗ 词法分析器生成失败:\n{result.stderr}")
    sys.exit(1)
print("✓ 词法分析器生成成功")

# 2. 重新生成语法分析器
print("\n步骤 2/4: 生成语法分析器...")
result = subprocess.run(["bison", "-d", "c-complier-master/yacc.y"],
                       capture_output=True, text=True)
if result.returncode != 0:
    print(f"✗ 语法分析器生成失败:\n{result.stderr}")
    sys.exit(1)
print("✓ 语法分析器生成成功")

# 3. 编译编译器
print("\n步骤 3/4: 编译编译器...")
compile_cmd = [
    "gcc",
    "-o", "compiler.exe",
    "yacc.tab.c",
    "lex.yy.c",
    "c-complier-master/tree.c",
    "c-complier-master/hashMap.c",
    "c-complier-master/stack.c",
    "c-complier-master/inner.c",
    "c-complier-master/linkList.c",
    "-std=c11",
    "-Wall"
]

result = subprocess.run(compile_cmd, capture_output=True, text=True)
if result.returncode != 0:
    print(f"✗ 编译失败:\n{result.stderr}")
    sys.exit(1)
print("✓ 编译器编译成功")

# 4. 测试解析函数语法
print("\n步骤 4/4: 测试解析函数语法...")
test_file = "tests/parser/test_function_syntax.c"

result = subprocess.run(["./compiler.exe", test_file],
                       capture_output=True, text=True)

if result.returncode != 0:
    print(f"✗ 语法解析失败:\n{result.stdout}\n{result.stderr}")
    sys.exit(1)

print("✓ 函数语法解析成功！")
print()

# 检查生成的文件
if os.path.exists("Grammatical"):
    print("✓ 语法树文件生成成功（Grammatical）")
    with open("Grammatical", 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read(500)  # 读取前500字符
        print(f"\n语法树预览:\n{content}...")

print("\n" + "=" * 60)
print("语法分析器测试完成！✓")
print("=" * 60)


