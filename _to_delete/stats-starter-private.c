/*
 * CSCI 264 — Lab 3: Pointers and Dynamic Memory
 *
 * Name:
 * Classmates who helped:
 * Generative AI use:
 */

#include <stdio.h>
#include <stdlib.h>

void read_values(int *arr, int n);
void min_max(int *arr, int n, int *min, int *max);
void swap(int *a, int *b);
void reverse(int *arr, int n);

int main(void) {
    int n, min, max, i;
    int *arr = NULL;

    printf("How many values? ");
    scanf("%d", &n);

    /* TODO: allocate an array of n ints on the heap and store its address
     * in arr. If malloc returns NULL, print an error message and exit. */


    printf("[main]    &n    = %p\n", &n);
    printf("[main]    &arr  = %p\n", &arr);
    printf("[main]    arr   = %p\n", arr);
    printf("[main]    &min  = %p\n", &min);

    printf("Enter %d values: ", n);
    read_values(arr, n);

    printf("Values:   ");
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    min_max(arr, n, &min, &max);
    printf("Min:      %d\n", min);
    printf("Max:      %d\n", max);

    reverse(arr, n);
    printf("Reversed: ");
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    /* TODO: free the array and set arr to NULL */


    return 0;
}

/* read n integers from the user into arr */
void read_values(int *arr, int n) {
    /* TODO: use a loop and scanf to read n integers into arr */

}

/* store the smallest element of arr in *min and the largest in *max */
void min_max(int *arr, int n, int *min, int *max) {
    printf("[min_max] &arr  = %p\n", &arr);
    printf("[min_max] arr   = %p\n", arr);
    printf("[min_max] min   = %p\n", min);
    printf("[min_max] &min  = %p\n", &min);

    /* TODO: find the smallest and largest elements of arr, and store them
     * in the variables that min and max point to */

}

/* exchange the values that a and b point to */
void swap(int *a, int *b) {
    int temp;

    /* TODO: save the value that a points to in temp */


    printf("[swap]    &temp = %p\n", &temp);
    printf("[swap]    a     = %p\n", a);
    printf("[swap]    b     = %p\n", b);

    /* TODO: finish exchanging the values that a and b point to */

}

/* reverse the order of the n elements of arr, in place */
void reverse(int *arr, int n) {
    printf("[reverse] &arr  = %p\n", &arr);
    printf("[reverse] arr   = %p\n", arr);

    /* TODO: reverse arr by calling swap. Do not use a second array. */

}
