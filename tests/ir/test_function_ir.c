// Mini-C 编译器 - 函数 IR 生成测试
// 测试 TASK205 子任务 5.5：扩展 IR 指令集

// 测试1: 简单函数定义和调用
int add(int a, int b) {
    return a + b;
}

void main() {
    int result = add(10, 20);
    output_int(result);
}

// 预期的 IR：
// FUNC_BEGIN add
// t0 = a + b
// return t0
// FUNC_END add
// 
// FUNC_BEGIN main
// arg 10
// arg 20
// t1 = call add 2
// result = t1
// arg result
// call output_int 1
// FUNC_END main

