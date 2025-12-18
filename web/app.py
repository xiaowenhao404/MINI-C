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
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# ==================== 配置 ====================

# 编译器路径和中间文件名称
COMPILER_EXEC = '../compiler'  # 编译器可执行文件
ASSEMBLY_SCRIPT = '../scripts/asm_generator.py'  # 汇编生成脚本
TEMP_SOURCE = 'temp.c'  # 临时源文件

# 中间结果文件映射
FILES_MAP = {
    'tokens': 'Lexical',       # 词法分析结果
    'ast': 'Grammatical',      # 语法树
    'ir': 'Innercode',         # 中间代码（四元式）
    'asm': 'assembly.asm'      # 汇编代码
}

# ==================== 工具函数 ====================

def read_file_content(filename):
    """
    安全读取文件内容
    
    如果文件不存在或读取失败，返回提示信息
    """
    if not os.path.exists(filename):
        return f"[系统提示] 文件 {filename} 未生成。\n可能是编译失败或此阶段未执行。"
    
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()
            if not content.strip():
                return f"[系统提示] 文件 {filename} 为空。"
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
        
        console_output += f"=== Mini-C 编译器 v1.0 ===\n\n"
        console_output += f"源文件: {TEMP_SOURCE}\n"
        console_output += f"代码行数: {len(code.splitlines())}\n\n"
        
        # 2. 检查编译器是否存在
        if not os.path.exists(COMPILER_EXEC):
            return jsonify({
                'success': False,
                'console': f"错误: 找不到编译器程序 {COMPILER_EXEC}\n\n"
                          f"请先在项目根目录执行以下命令：\n"
                          f"  make\n\n"
                          f"这将生成 compiler 可执行文件。",
                'results': {}
            })
        
        # 3. 调用编译器核心（生成 Lexical, Grammatical, Innercode）
        console_output += "步骤 1/3: 调用编译器核心...\n"
        console_output += f"$ {COMPILER_EXEC} {TEMP_SOURCE}\n"
        
        compiler_process = subprocess.run(
            [COMPILER_EXEC, TEMP_SOURCE],
            capture_output=True,
            text=True,
            timeout=10  # 10秒超时
        )
        
        console_output += compiler_process.stdout
        if compiler_process.stderr:
            console_output += "\n[标准错误输出]\n" + compiler_process.stderr
        
        # 检查编译是否成功
        if compiler_process.returncode != 0:
            console_output += f"\n编译失败（退出码: {compiler_process.returncode}）\n"
        else:
            console_output += "\n✓ 编译器执行成功\n"
        
        # 4. 调用汇编生成脚本（如果中间代码生成成功）
        if os.path.exists('Innercode') and compiler_process.returncode == 0:
            console_output += "\n步骤 2/3: 生成汇编代码...\n"
            console_output += f"$ python {ASSEMBLY_SCRIPT}\n"
            
            asm_process = subprocess.run(
                ['python', ASSEMBLY_SCRIPT],
                capture_output=True,
                text=True,
                timeout=10
            )
            
            console_output += asm_process.stdout
            if asm_process.stderr:
                console_output += "\n[汇编生成错误]\n" + asm_process.stderr
            
            if asm_process.returncode == 0:
                console_output += "\n✓ 汇编代码生成成功\n"
            else:
                console_output += f"\n汇编生成失败（退出码: {asm_process.returncode}）\n"
        else:
            console_output += "\n步骤 2/3: 跳过（中间代码未生成）\n"
        
        # 5. 读取所有中间结果文件
        console_output += "\n步骤 3/3: 读取中间结果文件...\n"
        for key, filename in FILES_MAP.items():
            content = read_file_content(filename)
            output_data[key] = content
            
            # 统计信息
            if "[系统提示]" not in content and "[错误]" not in content:
                lines = content.count('\n')
                console_output += f"✓ {filename}: {lines} 行\n"
            else:
                console_output += f"⚠ {filename}: 未生成\n"
        
        console_output += "\n=== 编译完成 ===\n"
        
        # 6. 返回结果
        return jsonify({
            'success': True,
            'results': output_data,
            'console': console_output
        })
        
    except subprocess.TimeoutExpired:
        return jsonify({
            'success': False,
            'console': console_output + "\n\n错误: 编译超时（超过10秒）\n可能是代码存在无限循环或编译器卡住。",
            'results': output_data
        })
    
    except Exception as e:
        return jsonify({
            'success': False,
            'console': console_output + f"\n\n系统异常: {str(e)}\n\n请检查编译器是否正确安装。",
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
    print("  Mini-C 编译器可视化界面正在启动...")
    print("="*50)
    print()
    print("✓ Flask 服务器")
    print("✓ 端口: 5000")
    print("✓ 访问地址: http://127.0.0.1:5000")
    print()
    print("提示:")
    print("  - 确保已编译编译器（make）")
    print("  - 在浏览器中访问上述地址")
    print("  - 按 Ctrl+C 停止服务器")
    print()
    print("="*50)
    
    # 启动Flask服务器
    # debug=True 开启调试模式（代码修改自动重载）
    app.run(debug=True, host='127.0.0.1', port=5000)

