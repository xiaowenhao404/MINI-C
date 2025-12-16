// Mini-C 编译器 - 结构体基础测试
// 测试 TASK207-208：结构体支持

// 测试1: 结构体定义和变量声明
void test_struct_definition() {
    struct Point {
        int x;
        int y;
    };
    
    struct Point p;
    p.x = 10;
    p.y = 20;
    
    output_int(p.x);  // 应输出 10
    output_int(p.y);  // 应输出 20
}

// 测试2: 结构体成员访问
void test_member_access() {
    struct Student {
        int id;
        int age;
    };
    
    struct Student s;
    s.id = 1001;
    s.age = 20;
    
    output_int(s.id);   // 应输出 1001
    output_int(s.age);   // 应输出 20
}

// 测试3: 结构体指针和 -> 运算符
void test_pointer_member_access() {
    struct Point {
        int x;
        int y;
    };
    
    struct Point p;
    p.x = 5;
    p.y = 15;
    
    struct Point *ptr = &p;
    
    output_int(ptr->x);  // 应输出 5
    output_int(ptr->y);  // 应输出 15
}

// 测试4: 结构体作为函数参数
void modify_point(struct Point *p, int new_x, int new_y) {
    p->x = new_x;
    p->y = new_y;
}

void test_struct_parameter() {
    struct Point {
        int x;
        int y;
    };
    
    struct Point p;
    p.x = 1;
    p.y = 2;
    
    modify_point(&p, 100, 200);
    
    output_int(p.x);  // 应输出 100
    output_int(p.y);  // 应输出 200
}

void main() {
    test_struct_definition();
    test_member_access();
    test_pointer_member_access();
    test_struct_parameter();
}

