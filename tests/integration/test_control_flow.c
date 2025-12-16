/**
 * Mini-C 编译器 - 控制流集成测试
 * 
 * 测试场景：
 * - if-else 语句
 * - while 循环
 * - 比较运算
 */

void main() {
    int a = 10;
    int b = 5;
    
    // 测试 if-else
    if (a > b) {
        output_int(1);  // 预期输出：1
    } else {
        output_int(0);
    }
    
    // 测试 while 循环
    int count = 0;
    while (count < 3) {
        output_int(count);  // 预期输出：0, 1, 2
        count = count + 1;
    }
}

