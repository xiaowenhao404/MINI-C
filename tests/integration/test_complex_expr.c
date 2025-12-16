/**
 * Mini-C 编译器 - 复杂表达式测试
 * 
 * 测试场景：
 * - 复杂的算术表达式
 * - 括号优先级
 * - 混合运算
 */

void main() {
    int a = 2;
    int b = 3;
    int c = 4;
    
    // 复杂表达式1: (2 + 3) * 4 - 10 / 2 = 20 - 5 = 15
    int result1 = (a + b) * c - 10 / 2;
    output_int(result1);  // 预期：15
    
    // 复杂表达式2: 2 * 3 + 4 * 5 = 6 + 20 = 26
    int result2 = a * b + c * 5;
    output_int(result2);  // 预期：26
    
    // 复杂表达式3: (10 - 2) / 4 + 3 = 8 / 4 + 3 = 2 + 3 = 5
    int result3 = (10 - a) / c + b;
    output_int(result3);  // 预期：5
}

