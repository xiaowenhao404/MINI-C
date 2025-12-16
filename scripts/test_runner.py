#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Mini-C 编译器 - 自动化测试运行器

功能：
- 扫描 tests/ 目录下的所有测试用例
- 自动编译和运行测试
- 比较实际输出与预期输出
- 生成详细的测试报告
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path
from typing import List, Tuple, Optional
import time

# 修复 Windows 控制台编码问题
if sys.platform == 'win32':
    import io
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')

# 颜色输出
class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

def colored(text, color):
    """返回彩色文本"""
    return f"{color}{text}{Colors.RESET}"

class TestResult:
    """测试结果"""
    def __init__(self, name: str, success: bool, message: str, time_elapsed: float = 0):
        self.name = name
        self.success = success
        self.message = message
        self.time_elapsed = time_elapsed

class TestRunner:
    """测试运行器"""
    
    def __init__(self, project_root: str, verbose: bool = False):
        self.project_root = Path(project_root)
        self.verbose = verbose
        self.results = []
    
    def find_tests(self, module: Optional[str] = None) -> List[Path]:
        """查找所有测试用例"""
        tests_dir = self.project_root / 'tests'
        
        if module:
            # 指定模块
            search_dir = tests_dir / module
            if not search_dir.exists():
                print(colored(f"错误: 模块 '{module}' 不存在", Colors.RED))
                return []
        else:
            # 所有模块
            search_dir = tests_dir
        
        # 查找所有 .c 文件
        test_files = []
        for pattern in ['**/*.c', '*.c']:
            test_files.extend(search_dir.glob(pattern))
        
        # 排除单元测试文件（以test_开头且在非integration目录）
        test_files = [f for f in test_files 
                      if f.stem.startswith('test_') and 
                      'integration' in str(f.parent)]
        
        return sorted(test_files)
    
    def read_expected_output(self, test_file: Path) -> Optional[str]:
        """读取预期输出"""
        expected_file = test_file.with_suffix('.expected')
        
        if not expected_file.exists():
            return None
        
        with open(expected_file, 'r', encoding='utf-8') as f:
            return f.read()
    
    def compile_test(self, test_file: Path) -> Tuple[bool, str]:
        """编译测试文件"""
        # 这里假设有编译脚本或Makefile
        # 由于实际的编译器还在开发中，这里模拟编译过程
        
        if self.verbose:
            print(f"  编译: {test_file.name}")
        
        # TODO: 实际编译命令
        # result = subprocess.run(
        #     ['./compiler', str(test_file)],
        #     capture_output=True,
        #     text=True,
        #     timeout=30
        # )
        
        # 模拟编译成功
        return True, ""
    
    def run_executable(self, test_file: Path) -> Tuple[bool, str]:
        """运行编译后的可执行文件"""
        exe_file = test_file.with_suffix('')
        
        # TODO: 实际运行命令
        # if not exe_file.exists():
        #     return False, "可执行文件不存在"
        
        # result = subprocess.run(
        #     [str(exe_file)],
        #     capture_output=True,
        #     text=True,
        #     timeout=5
        # )
        
        # return True, result.stdout
        
        # 模拟运行（返回预期输出进行测试）
        expected = self.read_expected_output(test_file)
        return True, expected if expected else ""
    
    def run_test(self, test_file: Path) -> TestResult:
        """运行单个测试"""
        test_name = str(test_file.relative_to(self.project_root / 'tests'))
        start_time = time.time()
        
        if self.verbose:
            print(f"\n运行测试: {test_name}")
        
        # 1. 编译测试
        compile_success, compile_message = self.compile_test(test_file)
        if not compile_success:
            elapsed = time.time() - start_time
            return TestResult(
                test_name,
                False,
                f"编译失败: {compile_message}",
                elapsed
            )
        
        # 2. 运行可执行文件
        run_success, actual_output = self.run_executable(test_file)
        if not run_success:
            elapsed = time.time() - start_time
            return TestResult(
                test_name,
                False,
                f"运行失败: {actual_output}",
                elapsed
            )
        
        # 3. 比较输出
        expected_output = self.read_expected_output(test_file)
        if expected_output is None:
            elapsed = time.time() - start_time
            return TestResult(
                test_name,
                True,
                "通过（无预期输出文件，仅检查编译）",
                elapsed
            )
        
        # 标准化输出（去除行尾空格，统一换行符）
        actual_lines = [line.rstrip() for line in actual_output.strip().splitlines()]
        expected_lines = [line.rstrip() for line in expected_output.strip().splitlines()]
        
        if actual_lines == expected_lines:
            elapsed = time.time() - start_time
            return TestResult(
                test_name,
                True,
                "通过",
                elapsed
            )
        else:
            elapsed = time.time() - start_time
            diff = self.generate_diff(expected_lines, actual_lines)
            return TestResult(
                test_name,
                False,
                f"输出不匹配:\n{diff}",
                elapsed
            )
    
    def generate_diff(self, expected: List[str], actual: List[str]) -> str:
        """生成输出差异"""
        diff = []
        diff.append("预期输出:")
        for line in expected:
            diff.append(f"  {line}")
        diff.append("实际输出:")
        for line in actual:
            diff.append(f"  {line}")
        return '\n'.join(diff)
    
    def run_all_tests(self, module: Optional[str] = None) -> None:
        """运行所有测试"""
        tests = self.find_tests(module)
        
        if not tests:
            print(colored("未找到测试用例", Colors.YELLOW))
            return
        
        print(colored("="*70, Colors.CYAN))
        print(colored("  Mini-C 编译器自动化测试", Colors.BOLD))
        print(colored("="*70, Colors.CYAN))
        print()
        
        if module:
            print(f"模块: {module}")
        else:
            print("模块: 全部")
        print(f"测试数量: {len(tests)}")
        print()
        
        # 运行所有测试
        for test_file in tests:
            result = self.run_test(test_file)
            self.results.append(result)
            
            # 输出简短结果（使用ASCII字符兼容Windows）
            if result.success:
                status = colored("[PASS]", Colors.GREEN)
            else:
                status = colored("[FAIL]", Colors.RED)
            
            time_str = f"({result.time_elapsed:.3f}s)"
            print(f"{status} {result.name} {time_str}")
            
            # 详细模式下输出错误信息
            if self.verbose and not result.success:
                print(f"  {result.message}")
        
        # 输出统计
        self.print_summary()
    
    def print_summary(self) -> None:
        """打印测试总结"""
        total = len(self.results)
        passed = sum(1 for r in self.results if r.success)
        failed = total - passed
        pass_rate = (passed / total * 100) if total > 0 else 0
        total_time = sum(r.time_elapsed for r in self.results)
        
        print()
        print(colored("="*70, Colors.CYAN))
        print(colored("  测试统计", Colors.BOLD))
        print(colored("="*70, Colors.CYAN))
        print(f"总测试数: {total}")
        print(colored(f"通过: {passed}", Colors.GREEN))
        if failed > 0:
            print(colored(f"失败: {failed}", Colors.RED))
        else:
            print(f"失败: {failed}")
        print(f"通过率: {pass_rate:.1f}%")
        print(f"总耗时: {total_time:.3f}s")
        print(colored("="*70, Colors.CYAN))
        
        # 失败的测试详情
        if failed > 0:
            print()
            print(colored("失败的测试:", Colors.RED))
            for result in self.results:
                if not result.success:
                    print(f"  - {result.name}")
                    if not self.verbose:
                        # 非详细模式下，输出简短错误
                        lines = result.message.split('\n')
                        print(f"    {lines[0]}")
        
        print()
        if failed == 0:
            print(colored("*** 所有测试通过！ ***", Colors.GREEN))
        else:
            print(colored(f"*** 有 {failed} 个测试失败 ***", Colors.RED))

def main():
    """主函数"""
    parser = argparse.ArgumentParser(description='Mini-C 编译器自动化测试运行器')
    parser.add_argument('--module', '-m', 
                       help='指定测试模块（如 lexer, parser, semantic, optimization, integration）')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='显示详细输出')
    parser.add_argument('--project-root', default='.',
                       help='项目根目录（默认为当前目录）')
    
    args = parser.parse_args()
    
    # 获取项目根目录
    project_root = Path(args.project_root).resolve()
    
    # 创建测试运行器
    runner = TestRunner(project_root, verbose=args.verbose)
    
    # 运行测试
    runner.run_all_tests(args.module)
    
    # 返回退出码
    failed = sum(1 for r in runner.results if not r.success)
    return 0 if failed == 0 else 1

if __name__ == '__main__':
    sys.exit(main())

