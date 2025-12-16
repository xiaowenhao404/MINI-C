// Mini-C 编译器 - 函数语法测试
// 测试 TASK205 子任务 5.3：语法分析器扩展

// 测试1: 简单函数定义
int add(int a, int b) {
    return a + b;
}

// 测试2: 无参数函数
int get_value() {
    return 42;
}

// 测试3: void 返回类型
void print_message() {
    output_int(100);
    return;
}

// 测试4: 多参数函数
int calculate(int x, int y, int z) {
    int result = x + y + z;
    return result;
}

// 测试5: float 类型函数
float multiply_float(float a, float b) {
    return a * b;
}

// 测试6: char 类型函数
char get_char(char c) {
    return c;
}

// 测试7: 函数调用
int test_calls() {
    int a = add(10, 20);
    int b = get_value();
    int c = calculate(1, 2, 3);
    return a + b + c;
}

// 测试8: 递归函数
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// 测试9: main 函数调用其他函数
void main() {
    int result1 = add(10, 20);
    output_int(result1);
    
    int result2 = factorial(5);
    output_int(result2);
    
    int result3 = test_calls();
    output_int(result3);
}


