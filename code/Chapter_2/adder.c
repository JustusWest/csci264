#include <stdio.h>

long assign(void) {
    long y = 40;
    return y;
}

long adder(void) {
    long a;
    return a + 2;
}

int main(void) {
    long x;
    assign();
    x = adder();
    printf("x is: %ld\n", x);
    return 0;
}
