/**
 * Mini-C 编译器 - 嵌套 if 语句测试
 * 
 * 测试场景：
 * - 嵌套的 if-else
 * - 多层条件判断
 */

void main() {
    int score = 85;
    
    if (score >= 90) {
        output_int(90);  // A
    } else {
        if (score >= 80) {
            output_int(80);  // B - 预期输出
        } else {
            if (score >= 70) {
                output_int(70);  // C
            } else {
                output_int(60);  // D
            }
        }
    }
}

