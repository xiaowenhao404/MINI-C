/**
 * Mini-C 编译器 - 类型转换集成测试
 * 
 * 测试场景：
 * - int 到 float 转换
 * - float 到 int 转换
 * - 混合类型运算
 */

void main() {
    int i = 42;
    float f = i;        // int → float
    output_float(f);    // 预期：42.00
    
    float x = 3.99;
    int j = x;          // float → int（截断）
    output_int(j);      // 预期：3
    
    int a = 10;
    float b = 3.14;
    float c = a + b;    // 混合运算
    output_float(c);    // 预期：13.14
}

