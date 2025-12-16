/**
 * Mini-C 编译器 - 浮点运算集成测试
 * 
 * 测试场景：
 * - 浮点变量声明
 * - 浮点四则运算
 * - 浮点输出
 */

void main() {
    float x = 3.5;
    float y = 2.0;
    float sum = x + y;
    float diff = x - y;
    float prod = x * y;
    
    output_float(sum);   // 预期：5.50
    output_float(diff);  // 预期：1.50
    output_float(prod);  // 预期：7.00
}

