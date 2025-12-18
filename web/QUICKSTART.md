# Mini-C Web IDE 快速开始指南

## 5分钟快速上手

### 步骤1：准备编译器（1分钟）

```bash
# 进入项目根目录
cd Mini-C

# 编译编译器
make

# 验证
./compiler --help  # 或 ls -l compiler
```

### 步骤2：启动 Web 服务器（30秒）

```bash
# 进入 web 目录
cd web

# 安装依赖（首次运行）
pip install flask

# 启动服务器
python app.py
```

看到以下输出表示成功：
```
✓ Flask 服务器
✓ 端口: 5000
✓ 访问地址: http://127.0.0.1:5000
```

### 步骤3：打开浏览器（10秒）

访问：`http://127.0.0.1:5000`

### 步骤4：编译第一个程序（1分钟）

1. 界面已经有示例代码
2. 点击右上角 "编译与运行" 按钮
3. 等待几秒
4. 查看右侧标签页的结果

### 步骤5：查看中间结果（2分钟）

点击右侧的4个标签页：
- **Token流**：词法分析结果
- **语法树AST**：语法结构
- **中间代码IR**：四元式
- **汇编代码ASM**：x86-64汇编

## 尝试不同的代码

### 示例1：浮点运算

```c
void main() {
    float pi = 3.14;
    float r = 5.0;
    float area = pi * r * r;
    output_float(area);
}
```

### 示例2：常量折叠优化

```c
void main() {
    int result = 3 + 4 * 5;  // 编译期计算为 23
    output_int(result);
}
```

### 示例3：控制流

```c
void main() {
    int i = 0;
    while (i < 5) {
        output_int(i);
        i = i + 1;
    }
}
```

## 快捷键

- `Ctrl+Enter` (Win/Linux) 或 `Cmd+Enter` (Mac)：快速编译

## 常见问题

### Q: 找不到编译器？

**A**: 
```bash
cd ..  # 回到项目根目录
make   # 编译编译器
cd web # 回到web目录
python app.py  # 重启服务器
```

### Q: Flask 未安装？

**A**:
```bash
pip install flask
# 或
pip install -r requirements.txt
```

### Q: 端口5000被占用？

**A**: 修改 `app.py` 最后一行：
```python
app.run(debug=True, host='127.0.0.1', port=8000)  # 改为8000或其他端口
```

## 下一步

- 查看 `README.md` 了解更多功能
- 查看 `TESTING.md` 进行完整测试
- 尝试编写自己的C程序

---

**祝使用愉快！** 🎉

