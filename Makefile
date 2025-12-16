# Mini-C 编译器 Makefile
# 作者：Mini-C 开发团队
# 说明：支持编译整个项目、指定文件编译、测试运行和清理

# ==================== 编译工具配置 ====================
LEX = flex
YACC = bison
CC = gcc
NASM = nasm
PYTHON = python3

# ==================== 编译选项 ====================
# 通用编译选项
CFLAGS = -Wall -std=c11
# 调试版本选项
DEBUG_FLAGS = -g -O0 -DDEBUG
# 发布版本选项
RELEASE_FLAGS = -O2

# 当前使用的编译选项（默认发布版本）
CURRENT_FLAGS = $(RELEASE_FLAGS)

# ==================== 目录配置 ====================
SRC_DIR = src
FRONTEND_DIR = $(SRC_DIR)/frontend
SEMANTIC_DIR = $(SRC_DIR)/semantic
IR_DIR = $(SRC_DIR)/ir
OPT_DIR = $(SRC_DIR)/optimization
CODEGEN_DIR = $(SRC_DIR)/codegen
UTILS_DIR = $(SRC_DIR)/utils

OUTPUT_DIR = output
TEST_DIR = tests
SCRIPT_DIR = scripts

# 引用现有代码
REF_DIR = c-complier-master

# ==================== 源文件配置 ====================
# 词法和语法分析文件（使用现有的）
LEX_FILE = $(REF_DIR)/lex.l
YACC_FILE = $(REF_DIR)/yacc.y

# 生成的词法和语法分析器
LEX_OUTPUT = lex.yy.c
YACC_OUTPUT = yacc.tab.c
YACC_HEADER = yacc.tab.h

# 现有的辅助文件
UTIL_SOURCES = $(REF_DIR)/tree.c $(REF_DIR)/hashMap.c $(REF_DIR)/stack.c \
               $(REF_DIR)/inner.c $(REF_DIR)/linkList.c

# 编译器可执行文件名
COMPILER = compiler

# ==================== 测试文件配置 ====================
# 默认测试文件
file = test
TEST_FILE = $(file).c

# ==================== 主要目标 ====================

# 默认目标：编译整个项目
all: $(COMPILER)
	@echo "=== 编译完成 ==="
	@echo "运行方式: make file=<文件名> （不含.c扩展名）"

# 编译器主程序
$(COMPILER): $(LEX_OUTPUT) $(YACC_OUTPUT) $(UTIL_SOURCES)
	@echo "=== 编译编译器核心 ==="
	$(CC) $(CFLAGS) $(CURRENT_FLAGS) -o $(COMPILER) \
		$(YACC_OUTPUT) $(LEX_OUTPUT) $(UTIL_SOURCES) -lfl
	@echo "编译器生成成功: $(COMPILER)"

# 生成词法分析器
$(LEX_OUTPUT): $(LEX_FILE)
	@echo "=== 生成词法分析器 ==="
	$(LEX) $(LEX_FILE)

# 生成语法分析器
$(YACC_OUTPUT) $(YACC_HEADER): $(YACC_FILE)
	@echo "=== 生成语法分析器 ==="
	$(YACC) -d $(YACC_FILE)

# ==================== 编译C源文件目标 ====================

# 编译指定的C源文件
compile: $(COMPILER)
	@echo "=== 开始编译 $(TEST_FILE) ==="
	@if not exist "$(TEST_FILE)" ( \
		echo 错误: 文件 $(TEST_FILE) 不存在 & exit /b 1 \
	)
	@echo "--- 步骤 1/5: 词法和语法分析 ---"
	./$(COMPILER) $(TEST_FILE)
	@echo "--- 步骤 2/5: 生成中间代码 ---"
	@if exist Innercode (echo 中间代码已生成) else (echo 警告: 中间代码生成失败)
	@echo "--- 步骤 3/5: 生成汇编代码 ---"
	$(PYTHON) $(REF_DIR)/assembly.py
	@echo "--- 步骤 4/5: 汇编 ---"
	$(NASM) -f elf64 assembly.asm -o $(file).o
	@echo "--- 步骤 5/5: 链接 ---"
	$(CC) -no-pie -o $(file) $(file).o
	@echo "=== 编译成功！可执行文件: $(file) ==="
	@echo "运行方式: ./$(file)"

# 编译并运行
run: compile
	@echo "=== 运行程序 ==="
	./$(file)

# ==================== 调试版本 ====================

# 编译调试版本
debug: CURRENT_FLAGS = $(DEBUG_FLAGS)
debug: clean $(COMPILER)
	@echo "=== 调试版本编译完成（包含调试符号）==="

# ==================== 测试目标 ====================

# 运行测试套件
test: $(COMPILER)
	@echo "=== 运行测试套件 ==="
	@if exist "$(SCRIPT_DIR)\test_runner.py" ( \
		$(PYTHON) $(SCRIPT_DIR)\test_runner.py \
	) else ( \
		echo 警告: 测试运行器尚未实现 \
	)

# 测试词法分析
test-lexer: $(COMPILER)
	@echo "=== 测试词法分析 ==="
	@for %%f in ($(TEST_DIR)\lexer\*.c) do ( \
		echo 测试: %%f & \
		./$(COMPILER) %%f \
	)

# 测试语法分析
test-parser: $(COMPILER)
	@echo "=== 测试语法分析 ==="
	@for %%f in ($(TEST_DIR)\parser\*.c) do ( \
		echo 测试: %%f & \
		./$(COMPILER) %%f \
	)

# ==================== 环境检查 ====================

# 检查开发环境
check-env:
	@echo "=== 检查开发环境 ==="
	@if exist "$(SCRIPT_DIR)\check_env.sh" ( \
		bash $(SCRIPT_DIR)\check_env.sh \
	) else ( \
		echo 正在手动检查环境... & \
		where flex >nul 2>&1 && echo [OK] flex 已安装 || echo [ERROR] flex 未安装 & \
		where bison >nul 2>&1 && echo [OK] bison 已安装 || echo [ERROR] bison 未安装 & \
		where gcc >nul 2>&1 && echo [OK] gcc 已安装 || echo [ERROR] gcc 未安装 & \
		where nasm >nul 2>&1 && echo [OK] nasm 已安装 || echo [ERROR] nasm 未安装 & \
		where python >nul 2>&1 && echo [OK] python 已安装 || echo [ERROR] python 未安装 \
	)

# ==================== 查看结果 ====================

# 查看词法分析结果
show-lexical:
	@if exist Lexical (type Lexical) else (echo 词法分析结果文件不存在)

# 查看语法分析结果
show-grammar:
	@if exist Grammatical (type Grammatical) else (echo 语法分析结果文件不存在)

# 查看中间代码
show-ir:
	@if exist Innercode (type Innercode) else (echo 中间代码文件不存在)

# 查看汇编代码
show-asm:
	@if exist assembly.asm (type assembly.asm) else (echo 汇编代码文件不存在)

# 查看所有中间结果
show-all: show-lexical show-grammar show-ir show-asm

# ==================== 清理目标 ====================

# 清理生成文件
clean:
	@echo "=== 清理生成文件 ==="
	-del /Q $(LEX_OUTPUT) $(YACC_OUTPUT) $(YACC_HEADER) 2>nul
	-del /Q $(COMPILER) $(COMPILER).exe 2>nul
	-del /Q *.o *.out test test.exe 2>nul
	-del /Q Lexical Grammatical Innercode assembly.asm 2>nul
	-if exist $(OUTPUT_DIR) rmdir /S /Q $(OUTPUT_DIR) 2>nul
	-mkdir $(OUTPUT_DIR) 2>nul
	@echo "清理完成"

# 深度清理（包括备份文件）
clean-all: clean
	@echo "=== 深度清理 ==="
	-del /Q *.bak *.orig *.log *.tmp 2>nul
	-del /Q *~ 2>nul

# ==================== 帮助信息 ====================

help:
	@echo ========================================
	@echo   Mini-C 编译器 Makefile 使用帮助
	@echo ========================================
	@echo.
	@echo 常用命令:
	@echo   make                 - 编译编译器
	@echo   make file=^<name^>     - 编译指定的C文件（不含.c后缀）
	@echo   make run file=^<name^> - 编译并运行指定的C文件
	@echo   make debug           - 编译调试版本
	@echo   make test            - 运行测试套件
	@echo   make clean           - 清理生成文件
	@echo   make check-env       - 检查开发环境
	@echo.
	@echo 查看中间结果:
	@echo   make show-lexical    - 查看词法分析结果
	@echo   make show-grammar    - 查看语法分析结果
	@echo   make show-ir         - 查看中间代码
	@echo   make show-asm        - 查看汇编代码
	@echo   make show-all        - 查看所有中间结果
	@echo.
	@echo 示例:
	@echo   make file=test       - 编译 test.c
	@echo   make run file=test   - 编译并运行 test.c
	@echo ========================================

# ==================== 伪目标声明 ====================

.PHONY: all compile run debug test test-lexer test-parser check-env \
        show-lexical show-grammar show-ir show-asm show-all \
        clean clean-all help

# ==================== 开发说明 ====================
# 
# 目录结构:
#   src/              - 源代码目录
#     frontend/       - 前端（词法、语法分析）
#     semantic/       - 语义分析
#     ir/             - 中间代码生成
#     optimization/   - 代码优化
#     codegen/        - 代码生成
#     utils/          - 工具函数
#   tests/            - 测试用例
#   scripts/          - 辅助脚本
#   docs/             - 文档
#   c-complier-master/ - 参考实现
#
# 开发流程:
#   1. 编写.c测试文件
#   2. make file=<测试文件名>
#   3. 查看中间结果: make show-all
#   4. 运行: ./<测试文件名>
#

