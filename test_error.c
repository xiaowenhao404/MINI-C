// Mini-C 编译器错误测试文件
// 将需要测试的代码粘贴到这里，然后运行: ./compiler test_error.c
void main()
{
    int a = 10;
    int b = c + 5;
    output_int(b);
    int x = 10;
    int x = 20;
    output_int(x);
    {
        int local = 10;
    }
     a = local + 5;
    output_int(a);
}
