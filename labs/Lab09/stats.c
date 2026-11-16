#include <stdio.h>
#include <stdlib.h>

void min_max(long *arr, long size, long *min, long *max) {
    *min = 0;
    *max = 0;
    for (long i = 1; i < size; i++) {
        if (arr[i] < arr[*min]) {
            *min = i;
        }
        if (arr[i] > arr[*max]) {
            *max = i;
        }
    }
}

void swap(long *a, long *b) {
    long t = *a;
    *a = *b;
    *b = t;
}

int main(void) {
    long n;
    scanf("%ld", &n);
    long *arr = malloc(n * sizeof(long));
    if (arr == NULL) {
        printf("malloc failed\n");
        exit(1);
    }
    for (long i = 0; i < n; i++) {
        scanf("%ld", &arr[i]);
    }
    long min, max;
    min_max(arr, n, &min, &max);
    swap(&arr[min], &arr[max]);
    for (long i = 0; i < n; i++) {
        printf("%ld ", arr[i]);
    }
    printf("\n");
    free(arr);
    return 0;
}
