// Mini-C 编译器 - 二维数组测试
// 测试 TASK202：多维数组支持

// 测试1: 二维数组声明
void test_2d_declaration() {
    int matrix[3][4];  // 3行4列
    
    // 初始化
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[1][0] = 3;
    matrix[2][3] = 12;
    
    output_int(matrix[0][0]);  // 应输出 1
    output_int(matrix[2][3]);  // 应输出 12
}

// 测试2: 二维数组遍历
void test_2d_iteration() {
    int grid[2][3];
    
    // 使用行优先顺序初始化
    int value = 1;
    for (int i = 0; i < 2; i = i + 1) {
        for (int j = 0; j < 3; j = j + 1) {
            grid[i][j] = value;
            value = value + 1;
        }
    }
    
    // 验证
    output_int(grid[0][0]);  // 应输出 1
    output_int(grid[0][2]);  // 应输出 3
    output_int(grid[1][1]);  // 应输出 5
}

// 测试3: 矩阵求和
void test_matrix_sum() {
    int data[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    
    int sum = 0;
    for (int i = 0; i < 2; i = i + 1) {
        for (int j = 0; j < 3; j = j + 1) {
            sum = sum + data[i][j];
        }
    }
    
    output_int(sum);  // 应输出 21
}

void main() {
    test_2d_declaration();
    test_2d_iteration();
    test_matrix_sum();
}

