# Mini-C Web 可视化总结

> 本文档总结 Web 端可视化的实现功能、思路，以及遇到的困难和解决方法。

## 📋 目录

- [概述](#概述)
- [实现功能](#实现功能)
- [技术架构](#技术架构)
- [编译流程](#编译流程)
- [遇到的困难与解决方法](#遇到的困难与解决方法)
- [对应文件](#对应文件)

---

## 概述

**对应文件**: `web/app.py`, `web/static/script.js`, `web/templates/index.html`

### 项目目标

提供一个基于 Flask 的 Web IDE，可以在浏览器中编写、编译、运行 Mini-C 程序，并实时查看编译各阶段结果。

### 主要特点

1. **在线编辑**：CodeMirror 编辑器，支持 C 语法高亮
2. **一键编译运行**：完整的 5 步编译流程
3. **多阶段展示**：词法分析、语法分析、中间代码、汇编代码
4. **跨平台支持**：Windows + WSL / Linux 双端运行

---

## 实现功能

### 代码编辑

| 功能 | 说明 |
|------|------|
| 语法高亮 | C 语言关键字着色 |
| 行号显示 | 左侧行号栏 |
| 自动缩进 | 智能缩进 |
| 字体缩放 | Ctrl + 滚轮调整字体大小 |

### 编译输出

| 标签页 | 内容 |
|--------|------|
| Token 流 | 词法分析结果（Lexical 文件） |
| 语法树 | 语法分析结果（Grammatical 文件） |
| 中间代码 | 三地址码（Innercode 文件） |
| 汇编代码 | x86-64 NASM 汇编（assembly.asm） |
| 程序输出 | 运行结果 |

### 错误显示

- 词法错误、语法错误、语义错误分类显示
- 错误信息包含行号和详细描述
- 编译失败时显示在程序输出区域

---

## 技术架构

### 技术栈

| 组件 | 技术 |
|------|------|
| 后端 | Flask (Python) |
| 前端 | HTML + CSS + JavaScript |
| 编辑器 | CodeMirror |
| 样式 | 自定义 CSS |
| 执行环境 | WSL / Linux |

### 目录结构

```
web/
├── app.py              # Flask 后端
├── static/
│   ├── script.js       # 前端 JavaScript
│   └── style.css       # 样式表
├── templates/
│   └── index.html      # HTML 模板
└── Windows使用说明.md  # Windows 使用文档
```

### 前后端交互

```
浏览器
    ↓ POST /compile
    ↓ {code: "源代码"}
Flask 后端
    ↓ 保存代码
    ↓ 调用编译器
    ↓ 生成汇编
    ↓ 汇编链接
    ↓ 运行程序
    ↓ 返回 JSON
浏览器
    ← {lexical, grammatical, innercode, assembly, output}
```

---

## 编译流程

### 5 步流程

```python
def compile_code(code):
    # 步骤 1: 保存源代码
    with open('temp.c', 'w', encoding='utf-8') as f:
        f.write(code)
    
    # 步骤 2: 调用编译器
    result = run_command('./compiler temp.c')
    if result.returncode != 0:
        return error_response(result.stderr)
    
    # 步骤 3: 生成汇编代码
    result = run_command('python3 scripts/asm_generator.py')
    
    # 步骤 4: 汇编和链接
    run_command('nasm -f elf64 assembly.asm -o temp.o')
    run_command('gcc -no-pie -o temp_exe temp.o')
    
    # 步骤 5: 运行程序
    result = run_command('./temp_exe')
    
    # 读取中间文件
    return {
        'lexical': read_file('Lexical'),
        'grammatical': read_file('Grammatical'),
        'innercode': read_file('Innercode'),
        'assembly': read_file('assembly.asm'),
        'output': result.stdout
    }
```

### 跨平台支持

```python
import platform

def run_command(cmd):
    if platform.system() == 'Windows':
        # 通过 WSL 执行
        wsl_path = win_to_wsl_path(os.getcwd())
        cmd = f'wsl bash -c "cd {wsl_path} && {cmd}"'
    
    return subprocess.run(
        cmd,
        shell=True,
        capture_output=True,
        encoding='utf-8',
        errors='replace'
    )

def win_to_wsl_path(win_path):
    """将 Windows 路径转换为 WSL 路径"""
    # C:\Users\... → /mnt/c/Users/...
    if ':' in win_path:
        drive, rest = win_path.split(':', 1)
        return f"/mnt/{drive.lower()}{rest.replace(chr(92), '/')}"
    return win_path
```

---

## 遇到的困难与解决方法

### 问题 1：WSL 路径转换

**问题描述**：
Windows 路径和 WSL 路径格式不同，导致文件找不到。

**解决方案**：

```python
def win_to_wsl_path(win_path):
    """C:\Users\... → /mnt/c/Users/..."""
    if ':' in win_path:
        drive, rest = win_path.split(':', 1)
        return f"/mnt/{drive.lower()}{rest.replace(chr(92), '/')}"
    return win_path
```

### 问题 2：编码问题

**问题描述**：
中文路径和中文输出导致乱码或错误。

**解决方案**：

```python
# 统一使用 UTF-8 编码
result = subprocess.run(
    cmd,
    encoding='utf-8',
    errors='replace',  # 替换无法解码的字符
    capture_output=True
)
```

### 问题 3：WSL 噪音过滤

**问题描述**：
WSL 输出包含网络警告等无关信息。

**解决方案**：

```python
def filter_wsl_warnings(output):
    """过滤 WSL 的网络警告等"""
    filtered = []
    for line in output.split('\n'):
        if 'wsl:' in line.lower():
            continue
        if '\ufffd' in line:  # 乱码字符
            continue
        filtered.append(line)
    return '\n'.join(filtered)
```

### 问题 4：CodeMirror 集成

**问题描述**：
需要支持 C 语言语法高亮和舒适的编辑体验。

**解决方案**：

```html
<!-- 引入 CodeMirror -->
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.2/codemirror.min.css">
<script src="https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.2/codemirror.min.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.2/mode/clike/clike.min.js"></script>
```

```javascript
// 初始化编辑器
var editor = CodeMirror.fromTextArea(document.getElementById("code"), {
    mode: "text/x-csrc",
    lineNumbers: true,
    theme: "default",
    indentUnit: 4,
    tabSize: 4
});
```

### 问题 5：异步编译

**问题描述**：
编译过程较长，需要提供反馈。

**解决方案**：

```javascript
async function compileCode() {
    // 显示加载状态
    document.getElementById('compile-btn').disabled = true;
    document.getElementById('status').textContent = '编译中...';
    
    try {
        const response = await fetch('/compile', {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({code: editor.getValue()})
        });
        
        const result = await response.json();
        displayResults(result);
    } finally {
        document.getElementById('compile-btn').disabled = false;
        document.getElementById('status').textContent = '';
    }
}
```

---

## 对应文件

| 文件 | 说明 |
|------|------|
| `web/app.py` | Flask 后端，处理编译请求 |
| `web/static/script.js` | 前端 JavaScript，编辑器和交互 |
| `web/static/style.css` | 样式表 |
| `web/templates/index.html` | HTML 模板 |
| `web/Windows使用说明.md` | Windows 环境使用文档 |

---

**文档版本**: v1.0  
**最后更新**: 2024-12-28  
**维护者**: Mini-C 开发团队

