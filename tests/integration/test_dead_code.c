/**
 * Mini-C 编译器 - 死代码消除优化测试
 * 
 * 测试场景：
 * - if(0) 应被删除
 * - if(1) 的 else 应被删除
 * - while(0) 应被删除
 */

void main() {
    int a = 10;
    
    // 死代码，应被删除
    if (0) {
        output_int(999);
    }
    
    // else 分支应被删除
    if (1) {
        output_int(a);  // 预期：10
    } else {
        output_int(888);
    }
    
    // 死循环，应被删除
    while (0) {
        a = 777;
    }
    
    output_int(a);  // 预期：10
}

