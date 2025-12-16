# Mini-C 编译器 - 语义分析测试 Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC_DIR = ../../src/semantic
TARGET = test_type_system

# 源文件
SOURCES = test_type_system.c $(SRC_DIR)/type_system.c
HEADERS = $(SRC_DIR)/type_system.h

# 编译测试程序
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# 运行测试
test: $(TARGET)
	./$(TARGET)

# 清理
clean:
	rm -f $(TARGET) $(TARGET).exe *.o

.PHONY: test clean

