#include <stdio.h>
#include <stdlib.h>

#define N 1000

long mat[N][N];

long sum_v1(long n) {
    long total = 0;
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            total += mat[i][j];
        }
    }
    return total;
}

long sum_v2(long n) {
    long total = 0;
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            total += mat[j][i];
        }
    }
    return total;
}

int main(void) {
    for (long i = 0; i < N; i++) {
        for (long j = 0; j < N; j++) {
            mat[i][j] = rand() % 100;
        }
    }
    printf("%ld\n", sum_v1(N));
    printf("%ld\n", sum_v2(N));
    return 0;
}
