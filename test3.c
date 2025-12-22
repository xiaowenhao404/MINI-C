int add(int a, int b) {
    return a + b;
}

void main() {
    int a = 10;
    int b = 20;
    int c = a + b;
    output_int(c);
    
    float x = 3.14;
    float y = 2.0;
    float sum = x + y;
    output_float(sum);
    
    int sum2 = add(15, 25);
    output_int(sum2);
}

