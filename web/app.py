"""
Mini-C 编译器 Web 可视化界面 - Flask 后端
作者：Mini-C 开发团队
版本：3.0

功能：
- 接收用户代码
- 调用编译器生成中间结果
- 返回各阶段输出文件内容
"""

import os
import subprocess
import sys
import platform
from flask import Flask, render_template, request, jsonify
import re

app = Flask(__name__)

def get_short_path_name(long_path):
    """
    获取Windows短路径（8.3格式），用于解决WSL中文路径问题
    """
    if platform.system() != 'Windows':
        return long_path
    try:
        import ctypes
        from ctypes import wintypes
        
        GetShortPathNameW = ctypes.windll.kernel32.GetShortPathNameW
        GetShortPathNameW.argtypes = [wintypes.LPCWSTR, wintypes.LPWSTR, wintypes.DWORD]
        GetShortPathNameW.restype = wintypes.DWORD
        
        # 首先获取需要的缓冲区大小
        length = GetShortPathNameW(long_path, None, 0)
        if length == 0:
            return long_path
        
        # 分配缓冲区并获取短路径
        buffer = ctypes.create_unicode_buffer(length)
        GetShortPathNameW(long_path, buffer, length)
        return buffer.value
    except Exception:
        return long_path

def filter_wsl_warnings(text):
    """过滤 WSL 网络警告等乱码信息"""
    if not text:
        return ""
    lines = text.split('\n')
    filtered_lines = []
    for line in lines:
        # 跳过 WSL 网络警告行（各种关键词）
        line_lower = line.lower()
        if 'wsl:' in line_lower or 'wsl ' in line_lower:
            continue
        if 'localhost' in line_lower:
            continue
        if 'nat' in line_lower or 'proxy' in line_lower:
            continue
        # 跳过包含Unicode替换字符的行（乱码标志）
        if '\ufffd' in line or '�' in line:
            continue
        # 跳过包含明显乱码模式的行
        # 检测连续的非常见字符组合
        if any(pattern in line for pattern in ['hKm', 'NtM', 'FO*', 'j_N', 'N/ec']):
            continue
        # 跳过明显乱码行（包含大量非打印或非ASCII字符）
        non_ascii_count = sum(1 for c in line if ord(c) > 127 or (ord(c) < 32 and c not in '\n\r\t'))
        if len(line) > 3 and non_ascii_count > len(line) * 0.15:
            continue
        # 跳过只包含空白或控制字符的行
        if line.strip() and all(ord(c) < 32 or ord(c) > 126 for c in line.strip()):
            continue
        filtered_lines.append(line)
    result = '\n'.join(filtered_lines)
    # 清理多余空行
    while '\n\n\n' in result:
        result = result.replace('\n\n\n', '\n\n')
    return result.strip()

# ==================== 配置 ====================

# 获取项目根目录（web目录的父目录）
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# 检测操作系统
IS_WINDOWS = platform.system() == 'Windows'

# 编译器路径和中间文件名称
if IS_WINDOWS:
    # Windows环境：使用WSL路径
    # 先获取短路径以避免中文路径问题
    SHORT_BASE_DIR = get_short_path_name(BASE_DIR)
    
    # 将短路径转换为WSL路径格式
    wsl_base = SHORT_BASE_DIR.replace('\\', '/')
    # 处理盘符 (C: -> /mnt/c)
    if len(wsl_base) > 1 and wsl_base[1] == ':':
        drive = wsl_base[0].lower()
        wsl_base = f'/mnt/{drive}' + wsl_base[2:]
    COMPILER_EXEC = './compiler'  # WSL中的相对路径
    COMPILER_EXEC_WIN = os.path.join(BASE_DIR, 'compiler')  # Windows路径（用于检查文件是否存在）
    ASSEMBLY_SCRIPT = 'scripts/asm_generator.py'
    TEMP_SOURCE = 'temp.c'
    WSL_BASE_DIR = wsl_base  # 短路径不需要额外转义
else:
    # Linux环境：直接使用路径
    COMPILER_EXEC = os.path.join(BASE_DIR, 'compiler')
    COMPILER_EXEC_WIN = COMPILER_EXEC  # Linux下与COMPILER_EXEC相同
    ASSEMBLY_SCRIPT = os.path.join(BASE_DIR, 'scripts', 'asm_generator.py')
    TEMP_SOURCE = os.path.join(BASE_DIR, 'temp.c')
    WSL_BASE_DIR = BASE_DIR

# 中间结果文件映射（使用绝对路径）
FILES_MAP = {
    'tokens': os.path.join(BASE_DIR, 'Lexical'),       # 词法分析结果
    'ast': os.path.join(BASE_DIR, 'Grammatical'),      # 语法树
    'ir': os.path.join(BASE_DIR, 'Innercode'),         # 中间代码（四元式）
    'asm': os.path.join(BASE_DIR, 'assembly.asm')      # 汇编代码
}

# ==================== 工具函数 ====================

def read_file_content(filename):
    """
    安全读取文件内容
    
    如果文件不存在或读取失败，返回提示信息
    """
    # 文件类型描述映射
    file_descriptions = {
        'Lexical': ('词法分析结果', '编译器在词法分析阶段遇到问题'),
        'Grammatical': ('语法分析结果（AST）', '源代码存在语法错误，导致语法分析失败'),
        'Innercode': ('中间代码（四元式）', '语法分析失败，无法生成中间代码'),
        'assembly.asm': ('汇编代码', '中间代码生成失败，无法生成汇编代码')
    }
    
    # 获取文件名（不含路径）
    base_name = os.path.basename(filename)
    desc = file_descriptions.get(base_name, ('未知文件', '未知原因'))
    
    if not os.path.exists(filename):
        return (f"[❌ 文件未生成] {desc[0]}\n\n"
                f"📍 文件路径: {filename}\n\n"
                f"💡 可能原因: {desc[1]}\n\n"
                f"🔧 解决方法: 请检查控制台输出中的错误信息，修正源代码后重新编译。")
    
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()
            if not content.strip():
                return (f"[⚠️ 文件为空] {desc[0]}\n\n"
                        f"📍 文件路径: {filename}\n\n"
                        f"💡 可能原因: {desc[1]}\n\n"
                        f"🔧 解决方法: 请检查控制台输出中的错误信息，修正源代码后重新编译。")
            return content
    except UnicodeDecodeError:
        # 尝试使用 latin-1 编码读取
        try:
            with open(filename, 'r', encoding='latin-1') as f:
                return f.read()
        except Exception as e:
            return f"[错误] 读取文件 {filename} 失败: {str(e)}"
    except Exception as e:
        return f"[错误] 读取文件 {filename} 时发生异常: {str(e)}"

def cleanup_temp_files():
    """清理临时文件"""
    temp_files = [TEMP_SOURCE] + list(FILES_MAP.values())
    for f in temp_files:
        # 文件已经是绝对路径
        if os.path.exists(f):
            try:
                os.remove(f)
            except:
                pass

# ==================== 路由 ====================

@app.route('/')
def index():
    """主页面"""
    return render_template('index.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    """
    编译接口
    
    接收用户代码，调用编译器，返回中间结果
    """
    code = request.json.get('code', '')
    
    if not code.strip():
        return jsonify({
            'success': False,
            'console': '错误: 代码为空，请输入C代码后再编译。',
            'results': {}
        })
    
    output_data = {}
    console_output = ""
    
    try:
        # 清理旧文件
        cleanup_temp_files()
        
        # 1. 保存代码到临时文件
        with open(TEMP_SOURCE, 'w', encoding='utf-8') as f:
            f.write(code)
        
        console_output += f"🚀 === Mini-C 编译器 v3.0 === 🚀\n\n"
        console_output += f"📄 源文件: {TEMP_SOURCE}\n"
        console_output += f"📝 代码行数: {len(code.splitlines())}\n\n"
        
        # 2. 检查编译器是否存在
        compiler_path_to_check = COMPILER_EXEC_WIN if IS_WINDOWS else COMPILER_EXEC
        if not os.path.exists(compiler_path_to_check):
            return jsonify({
                'success': False,
                'console': f"错误: 找不到编译器程序\n\n"
                          f"检查路径: {compiler_path_to_check}\n\n"
                          f"请先在WSL中执行以下命令：\n"
                          f"  cd {WSL_BASE_DIR if IS_WINDOWS else BASE_DIR}\n"
                          f"  make\n\n"
                          f"这将生成 compiler 可执行文件。",
                'results': {}
            })
        
        # 3. 调用编译器核心（生成 Lexical, Grammatical, Innercode）
        console_output += "📦 步骤 1/5: 运行编译器...\n"
        
        if IS_WINDOWS:
            # Windows环境：使用WSL运行编译器
            # 使用引号包围路径以处理中文和空格
            wsl_cmd = f'cd "{wsl_base}" && {COMPILER_EXEC} {TEMP_SOURCE}'
            console_output += f"$ wsl bash -c '{wsl_cmd}'\n"
            compiler_process = subprocess.run(
                ['wsl', 'bash', '-c', wsl_cmd],
                capture_output=True,
                text=True,
                timeout=10,
                encoding='utf-8',
                errors='replace'  # 处理无法解码的字符
            )
        else:
            # Linux环境：直接运行
            console_output += f"$ {COMPILER_EXEC} {TEMP_SOURCE}\n"
            compiler_process = subprocess.run(
                [COMPILER_EXEC, TEMP_SOURCE],
                capture_output=True,
                text=True,
                timeout=10,
                cwd=BASE_DIR,
                encoding='utf-8',
                errors='replace'
            )
        
        stdout_content = compiler_process.stdout if compiler_process.stdout else ""
        stderr_content = compiler_process.stderr if compiler_process.stderr else ""
        
        # 过滤掉 WSL 的网络警告（不是编译错误）
        wsl_warning_patterns = [
            "wsl:", "wsl ", "localhost", "NAT", "代理", "proxy",
            "检测到", "未镜像", "不支持"
        ]
        # 检测乱码模式（WSL警告在Windows上可能显示为乱码）
        garbled_patterns = ['\ufffd', '�', 'hKm', 'NtM', 'FO*', 'j_N']
        is_wsl_warning = (
            any(p in stderr_content for p in wsl_warning_patterns) or
            any(p in stderr_content for p in garbled_patterns)
        )
        
        # 显示输出
        if stdout_content:
            console_output += stdout_content
        else:
            console_output += "(编译器无输出)\n"
        
        # 显示返回码信息
        console_output += f"\n[返回码: {compiler_process.returncode}]\n"
        
        if stderr_content:
            if is_wsl_warning:
                pass  # 完全忽略 WSL 网络警告
            else:
                console_output += "\n[错误输出]\n" + filter_wsl_warnings(stderr_content)
        
        # 检查编译是否成功
        # 明确的错误模式
        error_patterns = ["Error:", "syntax error", "undefined", "already defined"]
        has_explicit_error = any(p.lower() in stdout_content.lower() for p in error_patterns)
        
        # 编译成功的标志（支持中英文）
        compile_success_marker = "read successfully" in stdout_content or "编译成功" in stdout_content
        
        # 判断编译是否成功
        if compile_success_marker and not has_explicit_error:
            # 有成功标志且无错误 -> 成功
            compiler_has_error = False
        elif has_explicit_error:
            # 有明确错误 -> 失败
            compiler_has_error = True
        elif compiler_process.returncode != 0 and not is_wsl_warning and stdout_content.strip():
            # 返回码非零、不是WSL警告、且有输出 -> 可能失败
            compiler_has_error = True
        elif not stdout_content.strip() and compiler_process.returncode != 0:
            # 无输出且返回码非零 -> 编译器可能没有正确执行
            # 检查 Innercode 文件是否存在且非空作为备选判断
            if os.path.exists(FILES_MAP['ir']) and os.path.getsize(FILES_MAP['ir']) > 0:
                compiler_has_error = False  # 文件存在，可能编译成功了
            else:
                compiler_has_error = True
        else:
            # 默认成功（可能是WSL警告导致的returncode非零）
            compiler_has_error = False
        
        if compiler_has_error:
            console_output += f"\n❌ [失败] 检测到编译错误\n"
            console_output += "📋 请检查上方错误信息并修复代码\n"
            compile_success = False
        else:
            console_output += "\n✅ [成功] 编译通过\n"
            compile_success = True
        
        # 4. 调用汇编生成脚本（仅在编译成功时）
        if compile_success and os.path.exists(FILES_MAP['ir']):
            console_output += "\n⚙️ 步骤 2/5: 生成汇编代码...\n"
            
            if IS_WINDOWS:
                # Windows环境：使用WSL运行Python脚本
                wsl_cmd = f'cd "{wsl_base}" && python3 {ASSEMBLY_SCRIPT}'
                console_output += f"$ wsl bash -c '{wsl_cmd}'\n"
                asm_process = subprocess.run(
                    ['wsl', 'bash', '-c', wsl_cmd],
                    capture_output=True,
                    text=True,
                    timeout=10,
                    encoding='utf-8',
                    errors='replace'
                )
            else:
                # Linux环境：直接运行
                console_output += f"$ python3 {ASSEMBLY_SCRIPT}\n"
                asm_process = subprocess.run(
                    ['python3', ASSEMBLY_SCRIPT],
                    capture_output=True,
                    text=True,
                    timeout=10,
                    cwd=BASE_DIR,
                    encoding='utf-8',
                    errors='replace'
                )
            
            if asm_process.stdout:
                console_output += asm_process.stdout
            if asm_process.stderr:
                # 过滤WSL警告，只显示真正的错误
                filtered_stderr = filter_wsl_warnings(asm_process.stderr)
                if filtered_stderr.strip():
                    console_output += "\n❌ [汇编错误]\n" + filtered_stderr
            
            # 如果 stdout 和 stderr 都为空，说明有问题
            if not asm_process.stdout and not asm_process.stderr and asm_process.returncode != 0:
                console_output += "\n⚠️ [警告] 汇编生成器无输出\n"
            
            if asm_process.returncode == 0:
                console_output += "\n✅ [成功] 汇编代码已生成\n"
                
                # 步骤 3: 汇编和链接
                console_output += "\n🔧 步骤 3/5: 汇编和链接...\n"
                
                if IS_WINDOWS:
                    # Windows环境：使用WSL进行汇编和链接（使用引号包围中文路径）
                    nasm_cmd = f'cd "{wsl_base}" && nasm -f elf64 assembly.asm -o temp.o 2>&1'
                    link_cmd = f'cd "{wsl_base}" && gcc -no-pie -o temp_exe temp.o 2>&1'
                    
                    console_output += f"$ nasm -f elf64 assembly.asm -o temp.o\n"
                    nasm_process = subprocess.run(
                        ['wsl', 'bash', '-c', nasm_cmd],
                        capture_output=True,
                        text=True,
                        timeout=10,
                        encoding='utf-8',
                        errors='replace'
                    )
                    
                    if nasm_process.returncode == 0:
                        console_output += "✅ 汇编完成\n"
                        console_output += f"$ gcc -no-pie -o temp_exe temp.o\n"
                        
                        link_process = subprocess.run(
                            ['wsl', 'bash', '-c', link_cmd],
                            capture_output=True,
                            text=True,
                            timeout=10,
                            encoding='utf-8',
                            errors='replace'
                        )
                        
                        if link_process.returncode == 0:
                            console_output += "✅ 链接完成\n"
                            
                            # 步骤 4: 运行程序
                            console_output += "\n▶️ 步骤 4/5: 运行程序...\n"
                            run_cmd = f'cd "{wsl_base}" && ./temp_exe 2>&1'
                            
                            run_process = subprocess.run(
                                ['wsl', 'bash', '-c', run_cmd],
                                capture_output=True,
                                text=True,
                                timeout=5,
                                encoding='utf-8',
                                errors='replace'
                            )
                            
                            console_output += "\n" + "="*40 + "\n"
                            console_output += "🖥️ 程序输出:\n"
                            console_output += "="*40 + "\n"
                            stdout_filtered = filter_wsl_warnings(run_process.stdout) if run_process.stdout else ""
                            stderr_filtered = filter_wsl_warnings(run_process.stderr) if run_process.stderr else ""
                            if stdout_filtered.strip():
                                # 确保输出以换行符结尾
                                console_output += stdout_filtered if stdout_filtered.endswith('\n') else stdout_filtered + '\n'
                            if stderr_filtered.strip():
                                console_output += stderr_filtered if stderr_filtered.endswith('\n') else stderr_filtered + '\n'
                            if not stdout_filtered.strip() and not stderr_filtered.strip():
                                console_output += "(无输出)\n"
                            console_output += "="*40 + "\n"
                            
                            if run_process.returncode == 0:
                                console_output += "✅ 程序运行完成\n"
                            else:
                                console_output += f"⚠️ 退出码: {run_process.returncode}\n"
                        else:
                            console_output += f"❌ 链接失败: {link_process.stderr}\n"
                    else:
                        console_output += f"❌ 汇编失败: {nasm_process.stdout}{nasm_process.stderr}\n"
                else:
                    # Linux环境
                    console_output += f"$ nasm -f elf64 assembly.asm -o temp.o\n"
                    nasm_process = subprocess.run(
                        ['nasm', '-f', 'elf64', 'assembly.asm', '-o', 'temp.o'],
                        capture_output=True,
                        text=True,
                        timeout=10,
                        cwd=BASE_DIR,
                        encoding='utf-8',
                        errors='replace'
                    )
                    
                    if nasm_process.returncode == 0:
                        console_output += "✅ 汇编完成\n"
                        console_output += f"$ gcc -no-pie -o temp_exe temp.o\n"
                        
                        link_process = subprocess.run(
                            ['gcc', '-no-pie', '-o', 'temp_exe', 'temp.o'],
                            capture_output=True,
                            text=True,
                            timeout=10,
                            cwd=BASE_DIR,
                            encoding='utf-8',
                            errors='replace'
                        )
                        
                        if link_process.returncode == 0:
                            console_output += "✅ 链接完成\n"
                            
                            # 步骤 4: 运行程序
                            console_output += "\n▶️ 步骤 4/5: 运行程序...\n"
                            
                            run_process = subprocess.run(
                                ['./temp_exe'],
                                capture_output=True,
                                text=True,
                                timeout=5,
                                cwd=BASE_DIR,
                                encoding='utf-8',
                                errors='replace'
                            )
                            
                            console_output += "\n" + "="*40 + "\n"
                            console_output += "🖥️ 程序输出:\n"
                            console_output += "="*40 + "\n"
                            if run_process.stdout:
                                # 确保输出以换行符结尾
                                console_output += run_process.stdout if run_process.stdout.endswith('\n') else run_process.stdout + '\n'
                            if run_process.stderr:
                                console_output += run_process.stderr if run_process.stderr.endswith('\n') else run_process.stderr + '\n'
                            if not run_process.stdout and not run_process.stderr:
                                console_output += "(无输出)\n"
                            console_output += "="*40 + "\n"
                            
                            if run_process.returncode == 0:
                                console_output += "✅ 程序运行完成\n"
                            else:
                                console_output += f"⚠️ 退出码: {run_process.returncode}\n"
                        else:
                            console_output += f"❌ 链接失败: {link_process.stderr}\n"
                    else:
                        console_output += f"❌ 汇编失败: {nasm_process.stderr}\n"
            else:
                console_output += f"\n❌ 汇编生成失败 (退出码: {asm_process.returncode})\n"
        else:
            if not compile_success:
                console_output += "\n⏭️ 步骤 2/5: [跳过] 编译失败\n"
                console_output += "⏭️ 步骤 3/5: [跳过] 编译失败\n"
                console_output += "⏭️ 步骤 4/5: [跳过] 编译失败\n"
            else:
                console_output += "\n⏭️ 步骤 2/5: [跳过] 无中间代码\n"
        
        # 5. 读取所有中间结果文件
        console_output += "\n📂 步骤 5/5: 读取输出文件...\n"
        for key, filename in FILES_MAP.items():
            content = read_file_content(filename)
            output_data[key] = content
            
            # 统计信息
            if "[系统提示]" not in content and "[错误]" not in content:
                lines = content.count('\n')
                console_output += f"✅ {filename}: {lines} 行\n"
            else:
                console_output += f"➖ {filename}: 未生成\n"
        
        console_output += "\n🎉 === 编译完成 === 🎉\n"
        
        # 6. 返回结果
        return jsonify({
            'success': True,
            'results': output_data,
            'console': console_output
        })
        
    except subprocess.TimeoutExpired:
        return jsonify({
            'success': False,
            'console': console_output + "\n\n⏰ 错误: 编译超时 (>10秒)\n💡 可能是代码中存在死循环或编译器卡住了。",
            'results': output_data
        })
    
    except Exception as e:
        return jsonify({
            'success': False,
            'console': console_output + f"\n\n💥 系统错误: {str(e)}\n\n🔧 请检查编译器安装情况。",
            'results': output_data
        })

@app.route('/about')
def about():
    """关于页面"""
    return jsonify({
        'name': 'Mini-C Compiler IDE',
        'version': '3.0',
        'description': '基于 Flask + CodeMirror 的编译器可视化界面',
        'features': [
            '词法分析展示',
            '语法树展示',
            '中间代码展示',
            '汇编代码展示',
            'CodeMirror 语法高亮'
        ]
    })

# ==================== 主程序 ====================

if __name__ == '__main__':
    print("="*50)
    print("  🚀 Mini-C 编译器 Web 界面启动中...")
    print("="*50)
    print()
    print("✅ Flask 服务器已启动")
    print("✅ 端口: 5000")
    print("✅ 访问地址: http://127.0.0.1:5000")
    print()
    print("💡 提示:")
    print("  - 请确保编译器已构建 (在 WSL 中运行 'make')")
    print("  - 在浏览器中打开上面的地址")
    print("  - 按 Ctrl+C 停止服务器")
    print()
    print("="*50)
    
    # 启动Flask服务器
    # debug=True 开启调试模式（代码修改自动重载）
    app.run(debug=True, host='127.0.0.1', port=5000)

