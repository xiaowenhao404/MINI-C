# Mini-C 编译器 - 语义分析测试 Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC_DIR = ../../src/semantic

# 测试目标
TESTS = test_type_system test_function_type test_symbol_table test_function_symbol

# 编译所有测试
all: $(TESTS)

# 类型系统测试
test_type_system: test_type_system.c $(SRC_DIR)/type_system.c $(SRC_DIR)/type_system.h
	$(CC) $(CFLAGS) -o $@ test_type_system.c $(SRC_DIR)/type_system.c

# 函数类型测试
test_function_type: test_function_type.c $(SRC_DIR)/type_system.c $(SRC_DIR)/type_system.h
	$(CC) $(CFLAGS) -o $@ test_function_type.c $(SRC_DIR)/type_system.c

# 符号表测试
test_symbol_table: test_symbol_table.c $(SRC_DIR)/symbol_table.c $(SRC_DIR)/type_system.c
	$(CC) $(CFLAGS) -o $@ test_symbol_table.c $(SRC_DIR)/symbol_table.c $(SRC_DIR)/type_system.c

# 函数符号测试
test_function_symbol: test_function_symbol.c $(SRC_DIR)/symbol_table.c $(SRC_DIR)/type_system.c
	$(CC) $(CFLAGS) -o $@ test_function_symbol.c $(SRC_DIR)/symbol_table.c $(SRC_DIR)/type_system.c

# 运行所有测试
test: $(TESTS)
	@echo "运行类型系统测试..."
	./test_type_system
	@echo ""
	@echo "运行函数类型测试..."
	./test_function_type
	@echo ""
	@echo "运行符号表测试..."
	./test_symbol_table
	@echo ""
	@echo "运行函数符号测试..."
	./test_function_symbol

# 清理
clean:
	rm -f $(TESTS) $(addsuffix .exe,$(TESTS)) *.o

.PHONY: all test clean

