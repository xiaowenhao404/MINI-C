/**
 * Mini-C 编译器 - 比较运算测试
 * 
 * 测试场景：
 * - 大于、小于
 * - 大于等于、小于等于
 * - 等于、不等于
 */

void main() {
    int a = 10;
    int b = 5;
    int c = 10;
    
    // 大于
    if (a > b) {
        output_int(1);  // 预期：1
    } else {
        output_int(0);
    }
    
    // 小于
    if (b < a) {
        output_int(1);  // 预期：1
    } else {
        output_int(0);
    }
    
    // 等于
    if (a == c) {
        output_int(1);  // 预期：1
    } else {
        output_int(0);
    }
    
    // 不等于
    if (a != b) {
        output_int(1);  // 预期：1
    } else {
        output_int(0);
    }
}

