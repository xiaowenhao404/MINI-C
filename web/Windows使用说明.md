# Mini-C Web界面 Windows 使用说明

## 📋 目录

- [环境说明](#环境说明)
- [前提条件](#前提条件)
- [启动方式](#启动方式)
- [常见问题](#常见问题)
- [技术说明](#技术说明)
- [推荐配置](#推荐配置)

---

## 环境说明

Mini-C 编译器在 WSL/Linux 中编译和运行，但 Web 界面可以在 Windows 上启动。

## 前提条件

1. ✅ WSL 已安装并配置
2. ✅ 在 WSL 中已编译编译器（`make`）
3. ✅ Python 3 已安装（Windows 或 WSL 中）

## 启动方式

### 方式1: 在 Windows 中启动 Flask（推荐）

```powershell
# 1. 确保在 WSL 中编译了编译器
wsl
cd /mnt/c/Users/36774/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C
make
exit

# 2. 在 Windows 中启动 Flask
cd C:\Users\36774\Desktop\资料\绩点\大三上资料\编译原理课设\Mini-C\web
pip install flask
python app.py
```

访问: `http://127.0.0.1:5000`

**工作原理**:
- Flask 在 Windows 中运行
- 编译器通过 WSL 调用（自动）
- 中间文件在项目根目录生成
- Web 界面读取中间文件并显示

### 方式2: 在 WSL 中启动 Flask

```bash
# 在 WSL 中
cd /mnt/c/Users/36774/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C
make

cd web
pip3 install flask
python3 app.py
```

访问: `http://127.0.0.1:5000`

## 常见问题

### Q: 提示"找不到编译器程序"？

**A**: 确保在 WSL 中编译了编译器
```bash
wsl
cd /mnt/c/Users/36774/Desktop/资料/绩点/大三上资料/编译原理课设/Mini-C
make
ls -la compiler  # 应该看到 compiler 文件
```

### Q: 编译按钮点击后无响应？

**A**: 检查：
1. WSL 是否正常运行：`wsl echo test`
2. 编译器是否存在：`wsl ls /mnt/c/.../Mini-C/compiler`
3. 浏览器控制台是否有错误

### Q: 显示"系统异常"？

**A**: 
1. 确保 WSL 已安装：`wsl --version`
2. 确保编译器在 WSL 中编译（不是在 Windows 中）
3. 检查路径是否正确

## 技术说明

### 跨平台实现

Web 应用会自动检测操作系统：
- **Windows**: 通过 WSL 调用编译器
- **Linux**: 直接调用编译器

### 路径转换

Windows 路径自动转换为 WSL 路径：
```
C:\Users\... → /mnt/c/Users/...
D:\Data\...  → /mnt/d/Data/...
```

### 文件访问

- 源文件写入 Windows 文件系统
- 编译器在 WSL 中访问（通过 /mnt/c）
- 中间文件生成在项目根目录
- Web 应用读取中间文件

## 推荐配置

### 开发环境
- **编译器**: 在 WSL 中编译和测试
- **Web 服务器**: 在 Windows 中运行（方便调试）
- **浏览器**: Windows 中访问

### 演示环境
- **编译器**: 在 WSL 中运行
- **Web 服务器**: 在 WSL 中运行
- **浏览器**: Windows 中访问 `http://127.0.0.1:5000`

---

**更新时间**: 2024-12-28  
**状态**: ✅ 跨平台支持完成

