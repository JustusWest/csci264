/*
 * CSCI 264 — Lab 3 SOLUTION (instructor copy — do not post)
 *
 * Same layout as the starter code (stats.c); each TODO is filled in.
 * Compile:  gcc -Wall -o stats stats_solution.c
 *
 * ---------------------------------------------------------------------------
 * READING THE ADDRESSES (x86-64 Linux)
 * ---------------------------------------------------------------------------
 * Addresses change every run (ASLR), so only compare them within one run.
 *   stack addresses start 0x7ff...            (long, high)
 *   heap addresses  start 0x55... or 0x56...  (short, low)
 * Tip: have the student run with 5 values, e.g. 4 -2 9 0 7, so reverse
 * calls swap twice.
 *
 * Real run of this file, gcc 13, x86-64 Linux (addresses will differ):
 *
 *   How many values? 5
 *   [main]    &n    = 0x7ffe46559280     Q1
 *   [main]    &arr  = 0x7ffe46559290
 *   [main]    arr   = 0x55f4783da2c0     Q1  <- heap
 *   [main]    &min  = 0x7ffe46559284
 *   Enter 5 values: 4 -2 9 0 7
 *   Values:   4 -2 9 0 7
 *   [min_max] &arr  = 0x7ffe46559258     Q2  (differs from main's &arr)
 *   [min_max] arr   = 0x55f4783da2c0     Q2  (same as main's arr)
 *   [min_max] min   = 0x7ffe46559284     Q3  (same as main's &min)
 *   [min_max] &min  = 0x7ffe46559248     Q4  (lower than main's)
 *   Min:      -2
 *   Max:      9
 *   [reverse] &arr  = 0x7ffe46559258
 *   [reverse] arr   = 0x55f4783da2c0
 *   [swap]    &temp = 0x7ffe46559234
 *   [swap]    a     = 0x55f4783da2c0     Q5  (= main's arr)
 *   [swap]    b     = 0x55f4783da2d0     Q5  (a + 0x10 -> arr[4])
 *   [swap]    &temp = 0x7ffe46559234
 *   [swap]    a     = 0x55f4783da2c4
 *   [swap]    b     = 0x55f4783da2cc
 *   Reversed: 7 0 9 -2 4
 *
 * Five DEMO QUESTIONS are marked  "DEMO Qn"  with the expected answer
 * after "EXPECT:", in the order the lines print.
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

    arr = malloc(sizeof(int) * n);
    if (arr == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }

    printf("[main]    &n    = %p\n", &n);
    printf("[main]    &arr  = %p\n", &arr);
    printf("[main]    arr   = %p\n", arr);
    printf("[main]    &min  = %p\n", &min);
    /*
     * DEMO Q1: One of main's four addresses is very different from the
     * others. Which one, and where does each of them live?
     *   EXPECT: arr. &n, &arr, &min are main's local variables on the
     *   stack (close together, 0x7ff...). arr is the address malloc
     *   returned, on the heap (0x55.../0x56...). &arr is where the pointer
     *   is stored; arr is what it holds.
     */

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

    free(arr);
    arr = NULL;

    return 0;
}

/* read n integers from the user into arr */
void read_values(int *arr, int n) {
    int i;
    for (i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
}

/* store the smallest element of arr in *min and the largest in *max */
void min_max(int *arr, int n, int *min, int *max) {
    int i;

    printf("[min_max] &arr  = %p\n", &arr);
    printf("[min_max] arr   = %p\n", arr);
    printf("[min_max] min   = %p\n", min);
    printf("[min_max] &min  = %p\n", &min);
    /*
     * DEMO Q2: Compare [min_max] arr and &arr to main's. What matches, what
     * doesn't, and why?
     *   EXPECT: arr is the SAME -- only the address of the array was copied
     *   into the parameter, not the array. &arr is DIFFERENT -- the
     *   parameter is its own variable in min_max's stack frame.
     *
     * DEMO Q3: [min_max] min matches one of main's lines. Which one, and
     * what does that let min_max do?
     *   EXPECT: [main] &min. The parameter holds the address of main's min,
     *   so  *min = ...  writes straight into main's variable.
     *
     * DEMO Q4: Which are lower, min_max's stack addresses or main's? Why?
     *   EXPECT: min_max's. It was called by main, and each new frame goes
     *   below its caller's -- the stack grows toward lower addresses.
     */

    *min = arr[0];
    *max = arr[0];
    for (i = 1; i < n; i++) {
        if (arr[i] < *min) {
            *min = arr[i];
        }
        if (arr[i] > *max) {
            *max = arr[i];
        }
    }
}

/* exchange the values that a and b point to */
void swap(int *a, int *b) {
    int temp;

    temp = *a;

    printf("[swap]    &temp = %p\n", &temp);
    printf("[swap]    a     = %p\n", a);
    printf("[swap]    b     = %p\n", b);
    /*
     * DEMO Q5: On the first call, how far apart are a and b (in hex)? Which
     * element of the array does each point to?
     *   EXPECT: a is &arr[0] (same as [main] arr). For n = 5, b - a = 0x10
     *   = 16 bytes = 4 ints, so b is &arr[4], the last element.
     */

    *a = *b;
    *b = temp;
}

/* reverse the order of the n elements of arr, in place */
void reverse(int *arr, int n) {
    int i;

    printf("[reverse] &arr  = %p\n", &arr);
    printf("[reverse] arr   = %p\n", arr);

    for (i = 0; i < n / 2; i++) {
        swap(&arr[i], &arr[n - 1 - i]);
    }
}
