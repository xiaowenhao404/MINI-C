// Mini-C 编译器 - 数组参数测试
// 测试 TASK206：函数参数和返回值优化

// 测试1: 数组参数退化为指针
void process_array(int arr[], int len) {
    for (int i = 0; i < len; i = i + 1) {
        arr[i] = arr[i] * 2;
    }
}

void test_array_parameter() {
    int data[5] = {1, 2, 3, 4, 5};
    process_array(data, 5);
    
    output_int(data[0]);  // 应输出 2
    output_int(data[1]);  // 应输出 4
    output_int(data[2]);  // 应输出 6
}

// 测试2: 指针参数
void modify_through_pointer(int *ptr, int value) {
    *ptr = value;
}

void test_pointer_parameter() {
    int x = 10;
    modify_through_pointer(&x, 20);
    
    output_int(x);  // 应输出 20
}

// 测试3: 数组和指针混合参数
void process_mixed(int arr[], int *count, int multiplier) {
    for (int i = 0; i < *count; i = i + 1) {
        arr[i] = arr[i] * multiplier;
    }
}

void test_mixed_parameters() {
    int numbers[3] = {1, 2, 3};
    int count = 3;
    
    process_mixed(numbers, &count, 3);
    
    output_int(numbers[0]);  // 应输出 3
    output_int(numbers[1]);  // 应输出 6
    output_int(numbers[2]);  // 应输出 9
}

// 测试4: 数组参数在函数内部使用
void print_array(int arr[], int len) {
    for (int i = 0; i < len; i = i + 1) {
        output_int(arr[i]);
    }
}

void test_array_usage() {
    int values[3] = {100, 200, 300};
    print_array(values, 3);
    // 应输出: 100, 200, 300
}

void main() {
    test_array_parameter();
    test_pointer_parameter();
    test_mixed_parameters();
    test_array_usage();
}

