---
title: "08 Arrays in C"
---

[*Reading: Dive into Systems §2.5*](https://diveintosystems.org/book/C2-C_depth/arrays.html)

## One-Dimensional Arrays

A **static** array is declared with a type and a capacity. A **dynamic** array is a pointer holding the address of a `malloc`'d block:

```c
int s_arr[20];                        // stack (or data, if global)
int *d_arr = malloc(sizeof(int) * 20);  // heap
```

Different declarations, but both names evaluate to the **base address** of the array — the address of element 0 — so the same `[i]` syntax works on both.

Elements are stored in **contiguous** memory. Element `i` sits `i` elements past the base, so the addresses differ by the size of one element:

```c
int  iarray[4];
char carray[4];
```

| | element 0 | element 1 | element 2 | element 3 |
|---|---|---|---|---|
| `iarray` (4 bytes each) | 1230 | 1234 | 1238 | 1242 |
| `carray` (1 byte each) | 1280 | 1281 | 1282 | 1283 |

Two separate arrays have no guaranteed relationship to each other, and two separate `malloc` calls are usually far apart. Only the elements *within* one array are guaranteed to be contiguous.

## Arrays and Functions

Passing an array passes its base address, for static and dynamic arrays alike:

```c
void init_array(int *arr, int size);   // int arr[] means the same thing
```

The function can change the caller's elements, because both names refer to the same memory. Since an array carries no size information, the size goes in a second parameter.

## Two-Dimensional Arrays

A static 2D array is declared with two capacities and accessed with two indices:

```c
int grid[3][4];        // 3 rows, 4 columns
grid[1][2] = 7;
```

In memory it is still one contiguous block, stored in **row-major order** — all of row 0, then all of row 1, then row 2:

![Showing the memory heirarchy of a statically declared two d array, it covers all the first row locations in order, then the second row and so on.]({{ "/figures/ch2/2Donemalloc.png" | relative_url }})


For an array with `M` columns, element `[i][j]` is `i * M + j` elements past the base. The compiler does that arithmetic for you every time you write `grid[i][j]`.


## Dynamically Allocated 2D Arrays

**Method 1: one `malloc`.** Ask for all `N * M` elements at once.

```c
int *grid;
grid = malloc(sizeof(int) * N * M);
```

This gives one contiguous block, exactly like the static case. But the type is `int *`, and the compiler has no idea you meant it as a grid, so `grid[i][j]` will not compile. You do the row-major arithmetic yourself:

```c
grid[i * M + j] = 7;      // row i, column j
```

Freeing is one call:

```c
free(grid);
```

**Method 2: an array of arrays.** Allocate an array of `N` pointers, then a row of `M` ints for each one.

```c
int **grid;
int i;

grid = malloc(sizeof(int *) * N);       // N pointers
for (i = 0; i < N; i++) {
    grid[i] = malloc(sizeof(int) * M);  // one row of M ints
}
```
![Showing the memory heirarchy of a dynamically declared two d array of arrays, first array stores a series of pointers, one for each row, which each point to another array on the heap.]({{ "/figures/ch2/2Dnmallocs.png" | relative_url }})

Here `grid[i][j]` does compile: `grid[i]` is a pointer to row `i`, and `[j]` indexes into it. The cost is `N + 1` separate allocations, rows that are not next to each other, and freeing in reverse:

```c
for (i = 0; i < N; i++) {
    free(grid[i]);
}
free(grid);
```

Method 1 uses less memory and keeps everything contiguous. Method 2 reads more naturally.

## Example - Dyanmically Allocated Array

The lab starts with a runtime-sized array of user input. We will write that part together in class.

<details class="code-example" markdown="1">
<summary>Show code: reading values and looking at addresses</summary>

```c
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
```

</details>

A run with three values:

```text
How many values? 3
Enter 3 values: 5 1 8
Values:   5 1 8
&arr  = 0x7ffd80d6e520
arr   = 0x56502824c2c0
&arr[0] = 0x56502824c2c0
&arr[1] = 0x56502824c2c4
&arr[2] = 0x56502824c2c8
```

- `&arr` and `arr` are far apart. `&arr` is where the pointer variable lives, on the stack. `arr` is what it holds: the address of the array, on the heap.
- `arr` and `&arr[0]` are the same address. The name of the array *is* the base address.
- The element addresses go up by 4. The elements are contiguous, and an `int` is 4 bytes.

## Looking Ahead

Tomorrow is Lab 3, the program above is a good starting point.

For some practice, try the [Exercises from the book for §2.5](https://diveintosystems.org/exercises/section-2_5.html).
