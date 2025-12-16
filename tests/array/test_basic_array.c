// Mini-C 编译器 - 一维数组基础测试
// 测试 TASK201：一维数组支持

// 测试1: 数组声明和初始化
void test_array_init() {
    int arr[5] = {10, 20, 30, 40, 50};
    output_int(arr[0]);  // 应输出 10
    output_int(arr[4]);  // 应输出 50
}

// 测试2: 数组元素访问和修改
void test_array_modify() {
    int data[3];
    data[0] = 100;
    data[1] = 200;
    data[2] = 300;
    
    output_int(data[0]);  // 应输出 100
    output_int(data[1]);  // 应输出 200
    output_int(data[2]);  // 应输出 300
}

// 测试3: 数组遍历和求和
void test_array_sum() {
    int numbers[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    
    for (int i = 0; i < 5; i = i + 1) {
        sum = sum + numbers[i];
    }
    
    output_int(sum);  // 应输出 15
}

// 测试4: float 类型数组
void test_float_array() {
    float values[3] = {1.5, 2.5, 3.5};
    // 注意：当前可能还不支持 output_float，用 int 类型测试
}

// 测试5: char 类型数组
void test_char_array() {
    char letters[3] = {'A', 'B', 'C'};
    // 注意：char 需要特殊处理
}

// 测试6: 二维场景模拟（使用一维数组）
void test_matrix_simulation() {
    int matrix[9];  // 3x3 矩阵
    matrix[0] = 1;
    matrix[1] = 2;
    matrix[2] = 3;
    matrix[3] = 4;
    matrix[4] = 5;
    matrix[5] = 6;
    matrix[6] = 7;
    matrix[7] = 8;
    matrix[8] = 9;
    
    // 访问 matrix[1][1] (index 4)
    output_int(matrix[4]);  // 应输出 5
}

// main 函数
void main() {
    test_array_init();
    test_array_modify();
    test_array_sum();
}

