// Mini-C 编译器 2.0 版本 - 综合功能测试
// 测试所有 2.0 版本的新功能

// ==================== 结构体定义 ====================
struct Point {
    int x;
    int y;
};

struct Rectangle {
    int width;
    int height;
};

// ==================== 函数定义 ====================

// 测试1: 函数定义和调用
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

// 测试2: 数组参数
void process_array(int arr[], int len) {
    for (int i = 0; i < len; i = i + 1) {
        arr[i] = arr[i] * 2;
    }
}

// 测试3: 指针参数
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 测试4: 递归函数
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// 测试5: 结构体参数
void init_point(struct Point *p, int x, int y) {
    p->x = x;
    p->y = y;
}

int get_area(struct Rectangle *rect) {
    return rect->width * rect->height;
}

// 测试6: 指针算术
int array_sum_with_pointer(int *ptr, int len) {
    int sum = 0;
    for (int i = 0; i < len; i = i + 1) {
        sum = sum + *(ptr + i);
    }
    return sum;
}

// 测试7: 数组和指针结合
void test_array_pointer() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *p = &arr[0];
    
    output_int(*p);           // 应输出 1
    output_int(*(p + 2));     // 应输出 3
    output_int(arr[3]);       // 应输出 4
}

// 测试8: 多维数组
void test_matrix() {
    int matrix[2][3];
    
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[0][2] = 3;
    matrix[1][0] = 4;
    matrix[1][1] = 5;
    matrix[1][2] = 6;
    
    int sum = 0;
    for (int i = 0; i < 2; i = i + 1) {
        for (int j = 0; j < 3; j = j + 1) {
            sum = sum + matrix[i][j];
        }
    }
    
    output_int(sum);  // 应输出 21
}

// 测试9: 结构体和指针
void test_struct_pointer() {
    struct Point p1;
    p1.x = 10;
    p1.y = 20;
    
    struct Point *ptr = &p1;
    
    output_int(ptr->x);  // 应输出 10
    output_int(ptr->y);  // 应输出 20
    
    init_point(ptr, 100, 200);
    
    output_int(p1.x);  // 应输出 100
    output_int(p1.y);  // 应输出 200
}

// 测试10: 复杂表达式
int complex_expression() {
    int a = 10;
    int b = 20;
    int c = 30;
    
    int result = (a + b) * c - (a * b);
    return result;  // (10+20)*30 - (10*20) = 900 - 200 = 700
}

// 测试11: 函数调用链
int compute(int x) {
    int a = add(x, 10);
    int b = multiply(a, 2);
    return b;
}

// ==================== 主函数 ====================

void main() {
    // 基础测试
    output_int(add(10, 20));           // 应输出 30
    output_int(multiply(5, 6));        // 应输出 30
    
    // 数组测试
    int arr[3] = {1, 2, 3};
    process_array(arr, 3);
    output_int(arr[0]);                // 应输出 2
    output_int(arr[1]);                // 应输出 4
    
    // 指针测试
    int x = 10;
    int y = 20;
    swap(&x, &y);
    output_int(x);                     // 应输出 20
    output_int(y);                     // 应输出 10
    
    // 递归测试
    output_int(factorial(5));          // 应输出 120
    
    // 结构体测试
    struct Rectangle rect;
    rect.width = 10;
    rect.height = 5;
    output_int(get_area(&rect));       // 应输出 50
    
    // 综合测试
    test_array_pointer();
    test_matrix();
    test_struct_pointer();
    output_int(complex_expression());  // 应输出 700
    output_int(compute(5));            // 应输出 30
}

