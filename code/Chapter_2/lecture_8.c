#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n, i;
    int *arr = NULL;

    printf("How many values? ");
    scanf("%d", &n);

    arr = malloc(sizeof(int) * n);
    if (arr == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }

    printf("Enter %d values: ", n);
    for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    printf("Values:   ");
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("&arr  = %p\n", &arr);
    printf("arr   = %p\n", arr);
    for (i = 0; i < n; i++) {
        printf("&arr[%d] = %p\n", i, &arr[i]);
    }

    free(arr);
    arr = NULL;

    return 0;
}
