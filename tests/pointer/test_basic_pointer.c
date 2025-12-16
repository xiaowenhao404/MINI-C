// Mini-C 编译器 - 指针基础测试
// 测试 TASK203：指针基础支持

// 测试1: 指针声明和取地址
void test_pointer_declaration() {
    int a = 100;
    int *p = &a;  // 指针声明和初始化
    
    output_int(a);  // 应输出 100
}

// 测试2: 解引用读取值
void test_deref_read() {
    int x = 42;
    int *ptr = &x;
    int y = *ptr;  // 通过指针读取值
    
    output_int(y);  // 应输出 42
}

// 测试3: 通过指针修改变量值
void test_pointer_modify() {
    int x = 10;
    int *ptr = &x;
    *ptr = 20;  // 通过指针修改值
    
    output_int(x);  // 应输出 20
}

// 测试4: 指针赋值
void test_pointer_assignment() {
    int a = 100;
    int b = 200;
    int *p1 = &a;
    int *p2 = &b;
    
    p1 = p2;  // 指针赋值
    output_int(*p1);  // 应输出 200
}

// 测试5: 多级指针（基础）
void test_pointer_chain() {
    int value = 999;
    int *p1 = &value;
    int **p2 = &p1;  // 指向指针的指针（如果支持）
    
    // 注意：多重指针需要 TASK204 支持
    output_int(value);
}

// 测试6: 指针和数组结合
void test_pointer_array() {
    int arr[3] = {1, 2, 3};
    int *p = &arr[0];  // 指向数组首元素
    
    output_int(*p);  // 应输出 1
    output_int(arr[0]);  // 应输出 1
}

// main 函数
void main() {
    test_pointer_declaration();
    test_deref_read();
    test_pointer_modify();
    test_pointer_assignment();
    test_pointer_array();
}

