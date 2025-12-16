/**
 * Mini-C 编译器 - 算术运算测试
 * 
 * 测试场景：
 * - 整数加减乘除
 * - 取模运算
 * - 运算优先级
 */

void main() {
    int a = 20;
    int b = 3;
    
    // 加法
    int sum = a + b;
    output_int(sum);  // 预期：23
    
    // 减法
    int diff = a - b;
    output_int(diff);  // 预期：17
    
    // 乘法
    int prod = a * b;
    output_int(prod);  // 预期：60
    
    // 除法
    int quot = a / b;
    output_int(quot);  // 预期：6
    
    // 取模
    int mod = a % b;
    output_int(mod);  // 预期：2
}

