/**
 * Mini-C 编译器 - 类型转换专项测试
 * 
 * 测试场景：
 * 1. int → float 转换（i2f）
 * 2. float → int 转换（f2i）
 * 3. 转换后的运算
 */

void main() {
    // 测试1：int → float
    int i1 = 42;
    float f1 = i1;  // i2f
    output_float(f1);  // 期望：42.00
    
    // 测试2：float → int（截断）
    float f2 = 3.99;
    int i2 = f2;  // f2i（截断）
    output_int(i2);  // 期望：3
    
    // 测试3：负数转换
    int i3 = -10;
    float f3 = i3;  // i2f
    output_float(f3);  // 期望：-10.00
    
    // 测试4：转换后参与运算
    int base = 100;
    float rate = 0.05;
    float result = base * rate;  // base应转为float
    output_float(result);  // 期望：5.00
}

