// Mini-C 编译器 - 函数语义分析集成测试
// 测试 TASK205 子任务 5.4：函数语义分析

// ========== 正确的函数定义 ==========

// 测试1: 简单函数
int add(int a, int b) {
    return a + b;
}

// 测试2: 无参数函数
int get_value() {
    return 42;
}

// 测试3: void 返回类型  
void print_number(int x) {
    output_int(x);
    return;
}

// 测试4: 多参数函数
int sum_three(int x, int y, int z) {
    int result = x + y + z;
    return result;
}

// 测试5: float 类型函数
float multiply(float a, float b) {
    return a * b;
}

// 测试6: char 类型函数
char get_first_char(char c) {
    return c;
}

// 测试7: 嵌套函数调用
int complex_calc(int a, int b) {
    int sum = add(a, b);
    int triple = sum_three(sum, a, b);
    return triple;
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
    int x = add(10, 20);
    output_int(x);
    
    int y = get_value();
    output_int(y);
    
    print_number(100);
    
    int z = factorial(5);
    output_int(z);
}

// ========== 预期会产生错误的测试（用于验证错误检测）==========

// 下面的代码应该产生语义错误（注释掉以避免编译失败）

/*
// 错误1: 函数重定义
int add(int x, int y) {  // 错误：函数 'add' 重定义
    return x + y;
}

// 错误2: 返回类型不匹配
int wrong_return() {
    return 3.14;  // 错误：返回 float 但声明为 int
}

// 错误3: 参数数量不匹配
void test_wrong_args() {
    int result = add(10);  // 错误：缺少参数
}

// 错误4: 参数类型不兼容（应该警告或错误）
void test_type_mismatch() {
    int result = add(10, 3.14);  // 可能警告：第二个参数类型转换
}

// 错误5: 调用未定义的函数
void test_undefined() {
    int x = undefined_function(10);  // 错误：未定义的函数
}

// 错误6: void 函数缺少返回值
int missing_return() {
    int a = 10;
    // 错误：缺少 return 语句（这个检查是可选的）
}

// 错误7: return 语句在函数外
// return 100;  // 错误：return 语句只能在函数内部使用
*/

