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
    
    int result = 3 + 4 * 5;
    output_int(result);
    
    int sum2 = add(15, 25);
    output_int(sum2);
    
    if (1) {
        output_int(100);
    } else {
        output_int(0);
    }
    
    int i = 0;
    while (i < 3) {
        output_int(i);
        i = i + 1;
    }
}

