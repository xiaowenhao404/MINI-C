// Mini-C 编译器 - 活性分析测试
// 测试 TASK209-211：活性分析优化

// 这个测试程序用于验证活性分析和栈槽复用优化
// 注意：这是一个示例程序，实际测试需要通过编译器生成IR后进行分析

void test_liveness_example() {
    int a = 10;
    int b = 20;
    int c = a + b;      // c 使用 a 和 b
    
    int d = c * 2;      // d 使用 c，此时 a 和 b 已不再活跃
    
    output_int(d);      // 使用 d
    
    // 在这个例子中：
    // - a 和 b 在定义 c 后就不再活跃，可以复用栈槽
    // - c 在定义 d 后就不再活跃，可以复用栈槽
    // - 理论上只需要 2 个栈槽（而不是 4 个）
}

void test_loop_liveness() {
    int sum = 0;
    int i;
    
    for (i = 0; i < 10; i = i + 1) {
        int temp = i * 2;    // temp 在循环内定义和使用
        sum = sum + temp;   // sum 在循环外仍活跃
    }
    
    output_int(sum);
    
    // 在这个例子中：
    // - temp 在每次循环迭代后都不再活跃，可以复用栈槽
    // - sum 在整个函数中活跃
    // - i 在循环中活跃
}

void main() {
    test_liveness_example();
    test_loop_liveness();
}

