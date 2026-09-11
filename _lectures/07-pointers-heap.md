---
title: "07 Pointers, Functions, and the Heap"
---

[*Reading: Dive into Systems §2.3*](https://diveintosystems.org/book/C2-C_depth/pointers_functions.html) and [*§2.4*](https://diveintosystems.org/book/C2-C_depth/dynamic_memory.html)

Last time we introduced pointers. Today we look at the two most common reasons to use them: letting a function change its caller's variables, and requesting memory while the program is running.

## Why Pointers?

Suppose a program reads a million `int`s and then runs a bunch of different functions on them:

```c
#define N 1000000

int main(void) {
    int data[1000000];   // 4 MB
    read_data(data, N);
    print_stats(data, N);   // calls max(data, N), average(data, N), ...
    return 0;
}
```

What if every function got its own copy of the array, the way a function gets its own copy of an `int`? Each call would copy 4 MB into its stack frame. While `max` is running, the stack would hold three copies of the same data. 12 MB is more than the entire stack on a typical Linux system (8 MB), so the program would crash.

Instead, store the array once on the heap and give each function its address. Each frame holds an 8-byte pointer, and every function works on the same data.

![Two pictures of the stack while max is running, called from print_stats, called from main. Left: each of the three frames holds its own 4 MB copy of the array, 12 MB total. Right: each frame holds only an 8-byte address, and all three point to a single 4 MB array on the heap.]({{ "/figures/ch2/passbyvalue_vs_pointer.png" | relative_url }})

C already does the right-hand version for arrays: passing an array passes its address, not a copy. For anything else a function needs to share or change, you pass a pointer yourself.

## Pass by Value

All arguments in C are **passed by value**. The parameter gets a *copy* of its argument, so changing the parameter does not change the argument.

<details class="code-example" markdown="1">
<summary>Show code: change_value (pass by value)</summary>

```c
#include <stdio.h>

int change_value(int input);

int main(void) {
    int x;
    int y;

    x = 30;
    y = change_value(x);
    printf("x: %d y: %d\n", x, y);  // prints x: 30 y: 30

    return 0;
}

int change_value(int input) {
    int val;

    val = input;

    if (val < 100) {
        input = 100;
    } else {
        input = val * 2;
    }

    return val;
}
```

</details>

`input` lives in `change_value`'s stack frame. It starts as a copy of `x`, gets set to 100, and disappears when `change_value` returns. `x` is still 30.

## Pass by Pointer

To let a function modify a caller's variable, pass it the variable's **address**. The function can dereference that address to reach back into the caller's stack frame.

**1. Declare the parameter** as a pointer to the variable's type:

```c
int change_value(int *input) {
```

**2. Pass an address** in the call:

```c
int x;
change_value(&x);
```

**3. Dereference the parameter** in the function body:

```c
*input = 100;   // the location input points to (x) gets 100
```

<details class="code-example" markdown="1">
<summary>Show code: change_value (pass by pointer)</summary>

```c
#include <stdio.h>

int change_value(int *input);

int main(void) {
    int x;
    int y;

    x = 30;
    y = change_value(&x);
    printf("x: %d y: %d\n", x, y);  // prints x: 100 y: 30

    return 0;
}

/*
 * changes the value of the argument
 * input: a pointer to the value to change
 * returns: the original value of the argument
 */
int change_value(int *input) {
    int val;

    val = *input;       /* val gets the value input points to */

    if (val < 100) {
        *input = 100;   /* the value input points to gets 100 */
    } else {
        *input = val * 2;
    }

    return val;
}
```

</details>

Here is the stack right before `change_value` returns:

![The stack before change_value returns: change_value's frame holds input, which stores the address of x, and val, which holds 30. An arrow goes from input to x in main's frame, which now holds 100. main's y is still unset.]({{ "/figures/ch2/passbypointer.png" | relative_url }})

`input` holds the address of `x`, which lives in `main`'s frame. When `change_value` returns, its frame is gone, but the change to `x` stays.


This is still pass by value. The value being copied just happens to be an address. Assigning a new address to `input` would not change anything in `main`; only writing *through* `input` does.

Pointer parameters are also how a C function "returns" more than one value:

```c
void divide(int a, int b, int *quot, int *rem) {
    *quot = a / b;
    *rem = a % b;
}

int q, r;
divide(17, 5, &q, &r);   // q is 3, r is 2
```

## Arrays and Functions

When you pass an array to a function, C copies the array's **base address** (the address of element 0) into the parameter. The parameter and the argument refer to the same elements, so the function can change them.

```c
void zero_out(int arr[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = 0;
    }
}
```

There is no way to get the size of an array from the array itself, so functions that take an array almost always take its size too.

## Dynamic Memory Allocation

So far, every variable's size has been fixed when we wrote the program. **Dynamic memory allocation** lets a program request memory while it runs. This is useful when:

- the size isn't known until runtime (e.g. it depends on user input)
- you want exactly as much space as you need, not some fixed maximum
- the size needs to grow or shrink as the program runs

## Heap Memory

Dynamically allocated memory comes from the **heap**.

![Showing the heirarchy of memory, stacked on top of eachother: operating system, Code, data, heap, stack - with a pointer going from the stack to the heap.]({{ "/figures/ch2/program_memory.png" | relative_url }})

Heap memory is **anonymous**: it has no variable name. Declaring a variable puts it on the stack or in the data region. The only way to reach heap memory is through a pointer that stores its address, like `ptr` in the figure above.

## malloc and free

`malloc` and `free` are in the standard library, so include `<stdlib.h>`.

**`malloc(n)`** allocates `n` contiguous bytes on the heap and returns the base address. Use `sizeof` to compute `n`:

```c
int *p;
p = malloc(sizeof(int));   // enough heap space for one int
```

`malloc` returns a `void *`, a generic pointer to no type in particular. Assigning it to `p` treats the memory as an `int`. You will sometimes see the cast written out; it means the same thing:

```c
p = (int *) malloc(sizeof(int));
```

**`malloc` can fail.** If there isn't enough heap space, it returns `NULL`. Always check before dereferencing:

```c
p = malloc(sizeof(int));
if (p == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
}
*p = 6;   // the heap memory p points to gets 6
```

```text
       stack                      heap
    +-----------+           +-----------+
  p |     *-----|---------->|     6     |
    +-----------+           +-----------+
                              (no name)
```

**`free(p)`** gives the memory back when you are done with it. Heap memory is never released automatically; it stays allocated until you free it or the program ends.

```c
free(p);
p = NULL;
```

Setting `p` to `NULL` after `free` is a good habit. If a bug later dereferences `p`, the program crashes right away instead of silently writing into memory that `malloc` may have handed to someone else.

`free` doesn't need a size, because `malloc` stores the size in a small header right before the block it returns.

## Dynamically Allocated Arrays

One call to `malloc` returns one contiguous chunk, which is exactly what an array needs. Request the size of one element times the number of elements:

```c
int *arr;
char *c_arr;

arr = malloc(sizeof(int) * 20);     // an array of 20 ints
c_arr = malloc(sizeof(char) * 10);  // an array of 10 chars
```

![main's stack frame holds arr and c_arr, each storing an address in the heap. arr points to a 20-element array in the heap, and c_arr points to a separate 10-element array.]({{ "/figures/ch2/heaparray.png" | relative_url }})

The pointers are on the stack, and the arrays are on the heap. Each array is contiguous, but separate calls to `malloc` are not next to each other.

Use the same `[]` syntax as a static array:

```c
int i;
int s_array[20];
int *d_array;

d_array = malloc(sizeof(int) * 20);
if (d_array == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
}

for (i = 0; i < 20; i++) {
    s_array[i] = i;
    d_array[i] = i;
}
printf("%d %d\n", s_array[3], d_array[3]);   // prints 3 3
```

This works because both names evaluate to the base address of an array:

| Expression | Value | Type |
|---|---|---|
| `s_array` | base address of the array (stack) | array of `int` |
| `d_array` | base address of the array (heap) | `int *` |

Since `d_array` points to element 0, these two lines do the same thing:

```c
d_array[0] = 8;
*d_array = 8;
```

Free each array when you're done:

```c
free(d_array);
d_array = NULL;
```

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — an array sized at runtime</p>

Read `n` from the user, then build an array of the first `n` squares.

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n, i;
    int *arr;

    printf("How many values? ");
    scanf("%d", &n);

    arr = malloc(sizeof(int) * n);
    if (arr == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }

    for (i = 0; i < n; i++) {
        arr[i] = i * i;
    }
    for (i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    arr = NULL;
    return 0;
}
```

```text
How many values? 5
0 1 4 9 16
```

With a static array we would have had to pick a capacity when writing the program. Here, the size is whatever the user types.

</div>


## Pointers to Heap Memory and Functions

Passing a dynamically allocated array to a function works the same way as passing a static array: the parameter gets the base address. The same function works for both.

The parameter declarations `int *arr` and `int arr[]` are equivalent. By convention, the pointer form is used when the function may be called with a heap array.

```c
void init_array(int *arr, int size) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = i;
    }
}

int main(void) {
    int *arr1;

    arr1 = malloc(sizeof(int) * 10);
    if (arr1 == NULL) {
        printf("malloc error\n");
        exit(1);
    }

    init_array(arr1, 10);
    ...
}
```

Right before `init_array` returns:

![The stack before init_array returns: init_array's frame holds size and i (both 10) and arr, and main's frame holds arr1. Both arr and arr1 point to the same 10-element heap array holding 0 through 9.]({{ "/figures/ch2/heaparrayparam.png" | relative_url }})

Only the address is copied. The array itself stays on the heap, and both `arr` and `arr1` point to it.

A function can also `malloc` memory and return the pointer. The heap block outlives the function that allocated it; only the local pointer variable goes away.

```c
int *make_array(int size) {
    int *arr = malloc(sizeof(int) * size);
    return arr;   // fine: the caller gets the heap address
}
```

## Common Errors

**Asking for the wrong number of bytes.** `malloc` counts bytes, not elements.

```c
arr = malloc(20);                 // ERROR: 20 bytes, room for only 5 ints
arr = malloc(sizeof(int) * 20);   // CORRECT: room for 20 ints
```

**Forgetting to free.** Memory that is never freed stays allocated. If you lose the last pointer to it, you can never free it. This is a **memory leak**.

```c
arr = malloc(sizeof(int) * 20);
arr = malloc(sizeof(int) * 50);   // the first block is now unreachable
```

**Using memory after freeing it.** The pointer still holds the old address, but that memory no longer belongs to you.

```c
free(arr);
arr[0] = 5;   // ERROR: may corrupt memory malloc has given to something else
```

**Returning the address of a local variable.** Locals live in the function's stack frame, which is gone once the function returns.

```c
int *make_array(int size) {
    int arr[10];
    return arr;   // ERROR: arr's memory disappears when make_array returns
}
```

Use `malloc` when memory needs to outlive the function that created it.

## Looking Ahead

Next time we look at how C lays out structs and two-dimensional arrays in memory, and at pointer arithmetic.

Our next lab will put this into practice with pointers and dynamic memory allocation.

For some practice, try the Exercises from the book for [§2.3](https://diveintosystems.org/exercises/section-2_3.html) and [§2.4](https://diveintosystems.org/exercises/section-2_4.html).
