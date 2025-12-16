/**
 * Mini-C 编译器 - 基础功能集成测试
 * 
 * 测试场景：
 * - 变量声明和初始化
 * - 简单算术运算
 * - if 条件语句
 * - 输出函数
 */

void main() {
    int a = 10;
    int b = 20;
    int c = (a + b) * 2;
    
    if (c > 50) {
        output_int(c);  // 预期输出：60
    }
}

