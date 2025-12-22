// ============================================
// Mini-C 编译器 v3.0 综合功能测试
// ============================================

// 自定义函数：加法
int add(int a, int b) {
    return a + b;
}

void main() {
    // ===== 1. 整数运算 =====
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);  // 30
    
    // ===== 2. 实型数据（浮点运算）=====
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output_float(sum);  // 5.14
    
    // ===== 3. 常量折叠优化 =====
    int result = 3 + 4 * 5;
    output_int(result);  // 23
    
    // ===== 4. 函数调用 =====
    int sum2 = add(15, 25);
    output_int(sum2);  // 40
    
    // ===== 5. if-else 控制流 =====
    if (1) {
        output_int(100);  // 100
    } else {
        output_int(0);
    }
    
    // ===== 6. while 循环 =====
    int i = 0;
    while (i < 3) {
        output_int(i);  // 0, 1, 2
        i = i + 1;
    }
    
    // ===== 7. 浮点乘法 =====
    float pi = 3.14;
    float doubled = pi * 2.0;
    output_float(doubled);  // 6.28
    
    // ===== 8. 数组和指针声明 =====
    int arr[5];
    int* ptr;
}

// 预期输出：30, 5.14, 23, 40, 100, 0, 1, 2, 6.28
// 
// 编译器支持的语法特性：
// ✓ 实型数据 float
// ✓ 函数定义与调用
// ✓ 一维/二维数组声明
// ✓ 指针声明
// ✓ 结构体定义
// ✓ if-else 控制流
// ✓ while 循环
// ✓ 常量折叠优化