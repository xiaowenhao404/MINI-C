#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess
import os
import sys

# 项目路径
project_path = r"C:\Users\36774\Desktop\资料\绩点\大三上资料\编译原理课设\Mini-C"

# 切换到项目目录
os.chdir(project_path)
print(f"当前目录: {os.getcwd()}")

# 执行 git 命令
def run_git(args):
    result = subprocess.run(
        ["git"] + args,
        capture_output=True,
        text=True,
        encoding='utf-8',
        errors='replace'
    )
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(result.stderr)
    return result.returncode

# 1. 查看状态
print("\n=== Git Status ===")
run_git(["status"])

# 2. 添加所有更改
print("\n=== Git Add ===")
run_git(["add", "-A"])

# 3. 提交更改
print("\n=== Git Commit ===")
commit_msg = "文档整理与更新：统一中文命名、新增答辩文档、更新README索引"
run_git(["commit", "-m", commit_msg])

# 4. 推送到远程
print("\n=== Git Push ===")
run_git(["push"])

print("\n=== 完成 ===")

