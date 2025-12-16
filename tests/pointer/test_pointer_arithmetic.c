// Mini-C 编译器 - 指针运算测试
// 测试 TASK204：指针运算和解引用

// 测试1: 指针加整数
void test_pointer_add() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = &arr[0];
    
    int *p2 = p + 2;  // 指针加整数
    output_int(*p2);  // 应输出 30
}

// 测试2: 指针减整数
void test_pointer_sub() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = &arr[4];
    
    int *p2 = p - 2;  // 指针减整数
    output_int(*p2);  // 应输出 30
}

// 测试3: 指针相减
void test_pointer_diff() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p1 = &arr[0];
    int *p2 = &arr[3];
    
    int diff = p2 - p1;  // 指针相减
    output_int(diff);  // 应输出 3
}

// 测试4: 指针比较
void test_pointer_compare() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p1 = &arr[0];
    int *p2 = &arr[2];
    
    int result = (p1 < p2);  // 指针比较
    output_int(result);  // 应输出 1 (true)
}

// 测试5: 指针和数组结合
void test_pointer_array_access() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;  // 数组名退化为指针
    
    int value = *(p + 2);  // 等价于 arr[2]
    output_int(value);  // 应输出 30
}

// 测试6: 多重指针（如果支持）
void test_multi_level_pointer() {
    int value = 999;
    int *p1 = &value;
    int **p2 = &p1;  // 二级指针
    
    // 注意：多重指针需要完整的语法支持
    output_int(value);
}

void main() {
    test_pointer_add();
    test_pointer_sub();
    test_pointer_diff();
    test_pointer_compare();
    test_pointer_array_access();
}

