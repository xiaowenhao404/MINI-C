#!/bin/bash
# Mini-C 编译器环境检查脚本
# 作者：Mini-C 开发团队
# 说明：检查编译Mini-C所需的所有工具是否正确安装

echo "=========================================="
echo "  Mini-C 编译器环境检查"
echo "=========================================="
echo ""

# 颜色定义（如果终端支持）
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查结果统计
PASS_COUNT=0
FAIL_COUNT=0
WARN_COUNT=0

# 检查单个工具
check_tool() {
    local tool_name=$1
    local tool_cmd=$2
    local required=$3  # true 或 false
    
    echo -n "检查 $tool_name ... "
    
    if command -v $tool_cmd &> /dev/null; then
        local version=$($tool_cmd --version 2>&1 | head -n 1)
        echo -e "${GREEN}✓ 已安装${NC}"
        echo "  版本: $version"
        PASS_COUNT=$((PASS_COUNT + 1))
        return 0
    else
        if [ "$required" = "true" ]; then
            echo -e "${RED}✗ 未安装 (必需)${NC}"
            FAIL_COUNT=$((FAIL_COUNT + 1))
        else
            echo -e "${YELLOW}⚠ 未安装 (可选)${NC}"
            WARN_COUNT=$((WARN_COUNT + 1))
        fi
        return 1
    fi
}

echo "1. 必需工具检查"
echo "----------------------------------------"

# 检查 flex（词法分析器生成器）
check_tool "Flex" "flex" true
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install flex"
fi

# 检查 bison（语法分析器生成器）
check_tool "Bison" "bison" true
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install bison"
fi

# 检查 gcc（C编译器）
check_tool "GCC" "gcc" true
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install gcc"
fi

# 检查 nasm（汇编器）
check_tool "NASM" "nasm" true
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install nasm"
fi

# 检查 python3
check_tool "Python3" "python3" true
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install python3"
fi

echo ""
echo "2. 可选工具检查"
echo "----------------------------------------"

# 检查 git
check_tool "Git" "git" false

# 检查 make
check_tool "Make" "make" false

# 检查 valgrind（内存检查工具）
check_tool "Valgrind" "valgrind" false
if [ $? -ne 0 ]; then
    echo "  安装方法: sudo apt install valgrind"
    echo "  用途: 检测内存泄漏"
fi

echo ""
echo "3. Python 依赖检查"
echo "----------------------------------------"

# 检查 Python 版本
echo -n "检查 Python 版本 ... "
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:3])))')
    PYTHON_MAJOR=$(python3 -c 'import sys; print(sys.version_info.major)')
    PYTHON_MINOR=$(python3 -c 'import sys; print(sys.version_info.minor)')
    
    if [ $PYTHON_MAJOR -ge 3 ] && [ $PYTHON_MINOR -ge 8 ]; then
        echo -e "${GREEN}✓ $PYTHON_VERSION (符合要求 >= 3.8)${NC}"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo -e "${YELLOW}⚠ $PYTHON_VERSION (推荐 >= 3.8)${NC}"
        WARN_COUNT=$((WARN_COUNT + 1))
    fi
else
    echo -e "${RED}✗ Python3 未安装${NC}"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

# 检查 pip
echo -n "检查 pip ... "
if command -v pip3 &> /dev/null; then
    PIP_VERSION=$(pip3 --version | awk '{print $2}')
    echo -e "${GREEN}✓ 已安装 (版本 $PIP_VERSION)${NC}"
else
    echo -e "${YELLOW}⚠ 未安装 (用于安装Python包)${NC}"
    echo "  安装方法: sudo apt install python3-pip"
fi

# 检查 Flask（用于Web界面，3.0版本需要）
echo -n "检查 Flask ... "
if python3 -c "import flask" 2> /dev/null; then
    FLASK_VERSION=$(python3 -c "from importlib.metadata import version; print(version('flask'))" 2>/dev/null)
    if [ -z "$FLASK_VERSION" ]; then
        FLASK_VERSION="未知"
    fi
    echo -e "${GREEN}✓ 已安装 (版本 $FLASK_VERSION)${NC}"
else
    echo -e "${YELLOW}⚠ 未安装 (3.0版本Web界面需要)${NC}"
    echo "  安装方法: pip3 install flask"
fi

echo ""
echo "4. 系统环境检查"
echo "----------------------------------------"

# 检查操作系统
echo -n "操作系统 ... "
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo -e "${GREEN}✓ Linux${NC}"
    OS_INFO=$(cat /etc/os-release 2>/dev/null | grep PRETTY_NAME | cut -d '"' -f 2)
    echo "  发行版: $OS_INFO"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo -e "${GREEN}✓ Windows (WSL/MSYS)${NC}"
else
    echo -e "${YELLOW}⚠ $OSTYPE (推荐使用 Linux 或 WSL)${NC}"
fi

# 检查架构
echo -n "CPU 架构 ... "
ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
    echo -e "${GREEN}✓ x86-64${NC}"
else
    echo -e "${YELLOW}⚠ $ARCH (编译器目标为 x86-64)${NC}"
fi

# 检查磁盘空间
echo -n "磁盘空间 ... "
AVAIL_SPACE=$(df -h . | awk 'NR==2 {print $4}')
echo "可用空间: $AVAIL_SPACE"

echo ""
echo "=========================================="
echo "  检查结果汇总"
echo "=========================================="
echo -e "${GREEN}通过: $PASS_COUNT${NC}"
echo -e "${YELLOW}警告: $WARN_COUNT${NC}"
echo -e "${RED}失败: $FAIL_COUNT${NC}"

echo ""
if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ 环境检查通过！可以开始编译。${NC}"
    echo ""
    echo "快速开始:"
    echo "  1. make                 # 编译编译器"
    echo "  2. make file=test       # 编译测试文件"
    echo "  3. ./test               # 运行测试"
    echo ""
    exit 0
else
    echo -e "${RED}✗ 环境检查失败！请安装缺失的工具。${NC}"
    echo ""
    echo "一键安装所有必需工具 (Ubuntu/Debian):"
    echo "  sudo apt update"
    echo "  sudo apt install flex bison gcc nasm python3 python3-pip make git"
    echo ""
    exit 1
fi

