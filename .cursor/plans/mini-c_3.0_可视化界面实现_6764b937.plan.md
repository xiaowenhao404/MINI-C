---
name: Mini-C 3.0 可视化界面实现
overview: 基于 Flask + CodeMirror 实现 Mini-C 编译器的 Web 可视化界面。用户在网页中编写代码，点击编译后在右侧标签页展示编译中间结果（Tokens、AST、IR、Assembly），底部显示控制台输出。可选实现 FIRST/FOLLOW 集合展示。
todos:
  - id: task301
    content: 创建Flask后端(app.py)
    status: completed
  - id: task302
    content: 创建前端界面(index.html)
    status: completed
    dependencies:
      - task301
  - id: task303
    content: 实现样式设计(style.css)
    status: completed
    dependencies:
      - task302
  - id: task304
    content: 实现交互逻辑(script.js)
    status: completed
    dependencies:
      - task302
  - id: task305
    content: FIRST/FOLLOW集计算(可选)
    status: cancelled
    dependencies:
      - task304
  - id: task306
    content: 集成测试与优化
    status: completed
    dependencies:
      - task304
---

# Mini-C 编译器 3.0 可视化界面实现方案

## 项目概述

基于现有的1.0版本编译器，开发Web可视化界面，提供：

1. 在线代码编辑器（CodeMirror语法高亮）
2. 一键编译功能
3. 编译中间结果展示（标签页切换）
4. 控制台输出
5. FIRST/FOLLOW集计算（可选）

## 技术栈

- **后端**: Python Flask（轻量级Web框架）
- **前端**: HTML5 + CSS3 + JavaScript
- **代码编辑器**: CodeMirror 5.x（C语言语法高亮）
- **样式**: macOS风格界面设计
- **编译器**: 使用1.0版本的compiler可执行文件

## 项目结构

```javascript
Mini-C/
├── web/
│   ├── app.py                  # Flask后端服务器
│   ├── requirements.txt        # Python依赖列表
│   ├── templates/
│   │   └── index.html         # 主界面HTML
│   ├── static/
│   │   ├── style.css          # 界面样式（macOS风格）
│   │   └── script.js          # 前端交互逻辑
│   └── README.md              # 使用说明
├── compiler                    # 编译器可执行文件（make生成）
└── scripts/
    └── asm_generator.py       # 汇编生成脚本
```



## 实现任务（按顺序）

### TASK301: Flask后端搭建

**文件**: `web/app.py`（约200行）**核心功能**：

1. 路由定义：

- `/` - 主页面
- `/compile` - 编译接口（POST）

2. 编译流程：
   ```python
               用户代码 → 保存temp.c → 调用./compiler → 读取中间文件 → 返回JSON
   ```




3. 文件读取：

- Lexical（词法分析）
- Grammatical（语法树）
- Innercode（中间代码）
- assembly.asm（汇编代码）

4. 错误处理：

- 捕获编译器输出
- 处理文件不存在
- 返回控制台信息

**工作量**: 1天---

### TASK302: 前端界面设计

**文件**: `web/templates/index.html`（约300行）**界面布局**：

```javascript
+------------------------------------------+
|  [●●●]  Mini-C Compiler    [▶编译与运行]  |
+------------------------------------------+
|  代码编辑器(左)   |   结果展示(右)          |
|  - CodeMirror    |   - Token流/AST/IR/ASM |
|  - 语法高亮       |   - 标签页切换          |
|  - 行号显示       |   - 文本展示            |
+------------------------------------------+
|  控制台输出 (Console)                     |
|  - 编译信息、错误提示                     |
+------------------------------------------+
```

**核心功能**：

1. 引入CodeMirror库（CDN）
2. 创建代码编辑器实例
3. 标签页切换逻辑
4. 编译按钮事件
5. 结果展示区域

**工作量**: 1-2天---

### TASK303: 样式设计

**文件**: `web/static/style.css`（约150行）**设计风格**: macOS Big Sur风格**核心样式**：

1. 窗口容器：圆角、阴影、边框
2. 顶部栏：红黄绿三点、标题、按钮
3. 三栏布局：编辑器、结果、控制台
4. 标签页：激活状态、悬停效果
5. 滚动条美化
6. 响应式布局

**工作量**: 0.5天---

### TASK304: 前端交互逻辑

**文件**: `web/static/script.js`（约150行）**核心功能**：

1. 初始化CodeMirror：
   ```javascript
               editor = CodeMirror(element, {
                   mode: 'text/x-csrc',  // C语言模式
                   theme: 'nord',         // 主题
                   lineNumbers: true      // 行号
               });
   ```




2. 编译请求：
   ```javascript
               async function runCompile() {
                   const code = editor.getValue();
                   const response = await fetch('/compile', {
                       method: 'POST',
                       body: JSON.stringify({code})
                   });
                   const data = await response.json();
                   updateResults(data);
               }
   ```




3. 标签页切换：
   ```javascript
               function switchTab(key) {
                   // 切换active类
                   // 更新显示内容
               }
   ```




4. 结果展示：
   ```javascript
               function updateDisplay(key) {
                   display.textContent = currentResults[key];
               }
   ```


**工作量**: 0.5天---

### TASK305: FIRST/FOLLOW集计算（可选）

**文件**:

- `web/first_follow.py`（算法实现，约300行）
- 在`index.html`添加新标签页

**功能**：

1. FIRST集算法实现
2. FOLLOW集算法实现
3. 文法解析（读取yacc.y或用户输入）
4. 表格展示结果

**工作量**: 2天（如果实现）---

### TASK306: 集成测试与优化

**任务**：

1. 测试编译流程
2. 测试文件读取
3. 测试标签页切换
4. 优化加载速度
5. 错误处理完善

**工作量**: 0.5天---

## 快速开始

### 步骤1: 安装依赖

```bash
cd web
pip install flask
```



### 步骤2: 确保编译器可用

```bash
# 在项目根目录
make
# 应该生成 ./compiler 文件
```



### 步骤3: 启动服务器

```bash
cd web
python app.py
# 访问 http://127.0.0.1:5000
```



### 步骤4: 测试功能

1. 在编辑器输入代码
2. 点击"编译与运行"
3. 查看右侧标签页（Tokens、AST、IR、ASM）
4. 查看底部控制台输出

## 关键技术点

### 1. CodeMirror集成

**作用**: 提供语法高亮的代码编辑器

```javascript
editor = CodeMirror(element, {
    mode: 'text/x-csrc',  // C语言模式
    theme: 'nord',         // 主题颜色
    lineNumbers: true,     // 显示行号
    indentUnit: 4,         // 缩进4空格
    smartIndent: true      // 智能缩进
});
```



### 2. Flask后端通信

**流程**:

```javascript
浏览器 --[POST /compile]--> Flask --[调用]--> ./compiler
                                      ↓
                            读取Lexical/Innercode等
                                      ↓
浏览器 <--[JSON结果]-------- Flask
```



### 3. 中间文件映射

```python
FILES_MAP = {
    'tokens': 'Lexical',      # 词法分析
    'ast': 'Grammatical',     # 语法树
    'ir': 'Innercode',        # 中间代码
    'asm': 'assembly.asm'     # 汇编代码
}
```



### 4. 标签页切换

```javascript
function switchTab(key) {
    // 1. 更新按钮active状态
    // 2. 从currentResults获取对应内容
    // 3. 更新显示区域
}
```



## 预期效果

### 界面展示

```javascript
┌─────────────────────────────────────────┐
│ ●●●  Mini-C Compiler    [▶ 编译与运行]  │
├──────────────────┬──────────────────────┤
│ void main() {    │ [Token流][AST][IR]   │
│   int a = 10;    │                      │
│   output_int(a); │ 1 a = 10             │
│ }                │ 2 arg a              │
│                  │ 3 call output_int    │
│                  │ 4 return             │
├──────────────────┴──────────────────────┤
│ Console: Compilation successful.        │
└─────────────────────────────────────────┘
```



### 功能特性

- 语法高亮：关键字蓝色、字符串绿色、注释灰色
- 行号显示：便于定位代码
- 一键编译：自动调用编译器
- 多标签展示：切换查看不同阶段结果
- 控制台输出：显示编译信息和错误

## 总工作量估算

| 任务 | 工作量 ||------|--------|| Flask后端 | 1天 || 前端界面 | 1-2天 || 样式设计 | 0.5天 || 交互逻辑 | 0.5天 || FIRST/FOLLOW | 2天（可选） || 测试优化 | 0.5天 || **总计** | **3.5-5.5天** |（如果不实现FIRST/FOLLOW，只需**1.5-3.5天**）

## 验收标准

- [ ] Web服务器正常启动
- [ ] 代码编辑器语法高亮正常
- [ ] 点击编译能调用编译器
- [ ] 标签页能正确展示Tokens、AST、IR、Assembly
- [ ] 控制台能显示编译信息
- [ ] 界面美观、响应流畅