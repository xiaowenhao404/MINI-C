/**
 * Mini-C 编译器 - 浮点运算和类型转换测试
 * 
 * 测试场景：
 * 1. 浮点常量定义
 * 2. 整数到浮点转换（i2f）
 * 3. 浮点加减乘除运算
 * 4. 混合类型运算
 * 5. 浮点输出
 */

void main() {
    // 测试1：基本整数运算
    int a = 10;
    int b = 3;
    int c = a + b;
    output_int(c);  // 期望输出：13
    
    // 测试2：浮点常量
    float x = 3.14;
    float y = 2.5;
    
    // 测试3：浮点加法
    float sum = x + y;
    output_float(sum);  // 期望输出：5.64
    
    // 测试4：浮点减法
    float diff = x - y;
    output_float(diff);  // 期望输出：0.64
    
    // 测试5：浮点乘法
    float prod = x * y;
    output_float(prod);  // 期望输出：7.85
    
    // 测试6：浮点除法
    float quot = x / y;
    output_float(quot);  // 期望输出：1.26
    
    // 测试7：类型转换（int → float）
    float converted = a;  // 应生成i2f指令
    output_float(converted);  // 期望输出：10.00
    
    // 测试8：混合运算（需要类型提升）
    float mixed = a + x;  // a应被转换为float
    output_float(mixed);  // 期望输出：13.14
}

