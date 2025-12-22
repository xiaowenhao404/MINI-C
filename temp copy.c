// Mini-C 编译器示例程序
// 
// 功能：演示基础功能、浮点运算和优化
// 提示：点击右上角 "编译与运行" 按钮

void main() {
    // 测试1：基本变量和算术运算
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);  // 输出: 30
    
    // 测试2：常量折叠优化
    int result = 3 + 4 * 5;  // 编译期优化为 23
    output_int(result);  // 输出: 23
    
    // 测试3：浮点运算和类型转换
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output_float(sum);  // 输出: 5.14
    
    // 测试4：控制流和死代码消除
    if (1) {
        output_int(100);  // 保留
    } else {
        output_int(0);    // 死代码，会被优化删除
    }
    
    // 测试5：循环
    int i = 0;
    while (i < 3) {
        output_int(i);
        i = i + 1;
    }
}