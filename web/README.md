# Mini-C 编译器 Web 可视化界面

基于 Flask + CodeMirror 的在线编译器 IDE。

## 功能特性

- 在线代码编辑器（CodeMirror 语法高亮）
- 一键编译和运行
- 编译中间结果展示：
  - Token 流（词法分析）
  - 语法树（AST）
  - 中间代码（四元式 IR）
  - 汇编代码（x86-64 NASM）
- 控制台输出
- macOS 风格界面设计

## 快速开始

### 1. 安装依赖

```bash
cd web
pip install -r requirements.txt
```

### 2. 确保编译器已编译

```bash
# 回到项目根目录
cd ..
make

# 应该生成 ./compiler 文件
```

### 3. 启动 Web 服务器

**在 WSL/Linux 环境中**:
```bash
cd web
python3 app.py
```

**在 Windows 环境中**（需要 WSL）:
```bash
cd web
python app.py
```

注意：如果在 Windows 上运行 Flask，编译器会自动通过 WSL 调用。

### 4. 访问界面

在浏览器中打开：
```
http://127.0.0.1:5000
```

## 使用说明

1. 在左侧编辑器中编写或修改 C 代码
2. 点击右上角 "编译与运行" 按钮（或按 Ctrl+Enter）
3. 在右侧标签页查看编译各阶段结果
4. 底部控制台显示编译过程信息和错误

## 项目结构

```
web/
├── app.py              # Flask 后端服务器
├── requirements.txt    # Python 依赖
├── templates/
│   └── index.html     # 主界面 HTML
├── static/
│   ├── style.css      # 界面样式
│   └── script.js      # 前端交互逻辑
└── README.md          # 本文件
```

## 技术栈

- **后端**: Flask 2.3.0
- **前端**: HTML5 + CSS3 + JavaScript (ES6+)
- **编辑器**: CodeMirror 5.65.2
- **主题**: Nord（深色舒适）
- **样式**: macOS Big Sur 风格

## 支持的编译器功能

- 数据类型：int, float, char
- 运算符：+, -, *, /, %, <, >, ==, !=, &&, ||
- 控制流：if-else, while, for
- 函数：output_int(), output_float()
- 优化：常量折叠、死代码消除

## 快捷键

- `Ctrl+Enter` (Windows/Linux) 或 `Cmd+Enter` (Mac)：编译代码

## 常见问题

### Q: 编译按钮点击无反应？

**A**: 
1. 检查浏览器控制台是否有JavaScript错误
2. 确认Flask服务器正常运行
3. 刷新页面重试

### Q: 显示"找不到编译器程序"？

**A**: 
```bash
# 在项目根目录执行
make
# 确保生成了 compiler 文件
```

### Q: 中间结果文件显示"未生成"？

**A**: 
1. 检查代码是否有语法错误
2. 查看控制台输出的错误信息
3. 确认编译器正确执行

### Q: 如何修改默认示例代码？

**A**: 编辑 `static/script.js` 中 `initCodeMirror()` 函数的 `value` 字段。

## 自定义

### 修改主题

在 `templates/index.html` 中修改 CodeMirror 主题：

```html
<!-- 可选主题 -->
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.2/theme/monokai.min.css">
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.2/theme/dracula.min.css">
```

在 `static/script.js` 中修改：

```javascript
editor = CodeMirror(editorWrapper, {
    theme: 'monokai',  // 或 'dracula'
    // ...
});
```

### 修改界面颜色

编辑 `static/style.css` 中的 `:root` 变量：

```css
:root {
    --accent-color: #007aff;  /* 主题色 */
    --bg-color: #f5f5f7;      /* 背景色 */
    /* ... */
}
```

## 部署

### 开发环境

```bash
python app.py
```

### 生产环境（使用 Gunicorn）

```bash
# 安装 Gunicorn
pip install gunicorn

# 启动服务器
gunicorn -w 4 -b 0.0.0.0:5000 app:app
```

## 扩展功能（TODO）

- [ ] FIRST/FOLLOW 集计算器
- [ ] AST 树形图可视化（D3.js）
- [ ] IR 控制流图可视化
- [ ] 代码自动补全
- [ ] 多文件编译支持
- [ ] 代码示例库
- [ ] 编译历史记录
- [ ] 主题切换功能

## 参考资料

- [Flask 文档](https://flask.palletsprojects.com/)
- [CodeMirror 文档](https://codemirror.net/5/doc/manual.html)
- [Nord 主题](https://www.nordtheme.com/)

---

**版本**: 3.0  
**最后更新**: 2025-12-16  
**作者**: Mini-C 开发团队

