---
title: "08 C Data in Memory: Pointer Arithmetic and Structs"
---

[*Reading: Dive into Systems §1.6*](https://diveintosystems.org/book/C1-C_intro/structs.html), [*§2.5.2*](https://diveintosystems.org/book/C2-C_depth/arrays.html), [*§2.7.2*](https://diveintosystems.org/book/C2-C_depth/structs.html), and [*§2.9.4*](https://diveintosystems.org/book/C2-C_depth/advanced.html)

Three things today, all about where data sits in memory: what it means to add 1 to a pointer, how a struct groups values, and how a struct pointer gets those values back to a caller.

## Pointer Arithmetic

Adding to a pointer moves it by **elements**, not by bytes. If `p` points to an `int`, `p + 1` is 4 bytes further along, because that is where the next `int` starts.

```c
int arr[5] = {4, -2, 9, 0, 7};
int *p = arr;      // the name of an array is the address of element 0
```

```text
p          = 0x7fffebfc6760
p + 1      = 0x7fffebfc6764      4 bytes later
&arr[1]    = 0x7fffebfc6764      the same location
```

The compiler scales the arithmetic by the size of the type pointed to:

| Pointer type | `p + 1` moves |
|---|---|
| `char *` | 1 byte |
| `int *` | 4 bytes |
| `double *` | 8 bytes |

Two equivalences follow, and both are worth memorizing:

```c
arr[i]   is   *(arr + i)     // indexing IS pointer arithmetic
&arr[i]  is   arr + i        // the address of element i
```

This is why `arr[i]` works the same on a static array and on a `malloc`'d one. Both names evaluate to a base address, and `[i]` means "go i elements past it, then dereference."

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — reading the addresses from tomorrow's lab</p>

In the lab, `swap` prints the two pointers it was passed. A run with five values gives:

```text
[main]    arr   = 0x5652821e42c0
[swap]    a     = 0x5652821e42c0
[swap]    b     = 0x5652821e42d0
```

`a` matches `arr`, so `a` points at element 0. For `b`:

```text
0x...2d0 - 0x...2c0 = 0x10 = 16 bytes = 4 ints
```

So `b` is `arr + 4`, which is `&arr[4]` — the last element of a five-element array.

Three ways to write the same call:

```c
swap(&arr[0], &arr[4]);
swap(arr, arr + 4);
swap(arr + 0, &arr[n - 1]);
```

</div>

## Structs

An array holds many values **of one type**. A **struct** holds several values **of different types** under one name.

```c
struct studentT {
    char name[64];
    int age;
    float gpa;
    int grad_yr;
};
```

That defines a new type, `struct studentT`. It does not allocate anything. Declaring a variable does:

```c
struct studentT s;

strcpy(s.name, "Freya");   // name is an array of char, so use strcpy
s.age = 18;
s.gpa = 4.0;
s.grad_yr = 2030;
```

The `.` operator accesses a field. Think carefully about each field's type: `s.age` is an `int`, `s.name` is an array.

A struct's fields are laid out in memory in the order you declared them, in one contiguous block:

```text
   s
 +----------------+----------+-----------+-------------+
 | name[64]       | age      | gpa       | grad_yr     |
 +----------------+----------+-----------+-------------+
   +0               +64        +68         +72
```

Each field is at a fixed **offset** from the start of the struct. That is all a struct is to the machine — a base address plus offsets. In Chapter 7 you will see the compiler emitting exactly these offsets.

Unlike an array, a struct **is** copied when you pass it to a function — all 76 bytes of this one. Changes to the parameter do not reach the caller.

## Pointers to Structs

Declaring a pointer to a struct works like any other pointer:

```c
struct studentT *sptr;

sptr = malloc(sizeof(struct studentT));
if (sptr == NULL) {
    printf("Error: malloc failed\n");
    exit(1);
}
```

`sizeof(struct studentT)` gets the size of the whole struct, so one `malloc` covers every field.

To reach a field through a pointer you must dereference first, which would be `(*sptr).age`. Because this is so common, C gives you the arrow operator:

```c
sptr->age = 18;        // same as (*sptr).age = 18
sptr->gpa = 4.0;
```

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — returning several values at once</p>

Tomorrow's lab has `min_max` hand back two answers through two pointer parameters. A struct collects those answers into one value, so one pointer parameter does the whole job.

<details class="code-example" markdown="1">
<summary>Show code: stats struct</summary>

```c
#include <stdio.h>
#include <stdlib.h>

struct stats {
    int min;
    int max;
    double avg;
};

void compute_stats(int *arr, int n, struct stats *s) {
    int i, sum = 0;

    s->min = arr[0];
    s->max = arr[0];

    for (i = 0; i < n; i++) {
        if (arr[i] < s->min) {
            s->min = arr[i];
        }
        if (arr[i] > s->max) {
            s->max = arr[i];
        }
        sum += arr[i];
    }

    s->avg = (double) sum / n;
}

int main(void) {
    int arr[5] = {4, -2, 9, 0, 7};
    struct stats s;

    compute_stats(arr, 5, &s);

    printf("min %d  max %d  avg %.2f\n", s.min, s.max, s.avg);

    return 0;
}
```

</details>

```text
min -2  max 9  avg 3.60
```

`compute_stats` never mentions `s` in `main` by name. It writes through the pointer it was given, exactly like `*min = ...` in the lab.

Printing the addresses shows the fields sitting next to each other:

```text
&s      = 0x7fffebfc6750
&s.min  = 0x7fffebfc6750      offset 0
&s.max  = 0x7fffebfc6754      offset 4
&s.avg  = 0x7fffebfc6758      offset 8
```

The struct's address and its first field's address are the same location.

Move the struct to the heap and only two lines change:

```c
struct stats *sp = malloc(sizeof(struct stats));
compute_stats(arr, 5, sp);        // sp is already a pointer, so no &
printf("min %d  avg %.2f\n", sp->min, sp->avg);
free(sp);
```

`compute_stats` cannot tell the difference. It gets an address either way.

</div>

## Two-Dimensional Arrays

A static 2D array is declared with two sets of brackets and accessed with two indices:

```c
int grid[3][4];      // 3 rows, 4 columns
grid[1][2] = 7;
```

In memory it is one contiguous block in **row-major order** — all of row 0, then all of row 1, and so on:

```text
 row 0            row 1            row 2
[00][01][02][03] [10][11][12][13] [20][21][22][23]
```

To allocate one on the heap, ask for all `N * M` elements at once:

```c
int *grid;
grid = malloc(sizeof(int) * N * M);
```

That call returns a plain `int *`, and the compiler has no idea you meant it as a grid, so `grid[i][j]` will not compile. You do the row-major arithmetic yourself:

```c
grid[i * M + j] = 7;      // row i, column j
```

Skip `i` whole rows of `M` elements, then move `j` more. This is the same arithmetic the compiler was doing for you in the static case.

## Looking Ahead

Tomorrow is Lab 3: pointers and dynamic memory. Everything it needs is from last week, plus `&arr[i]` from today.

Next time we look at C strings and the string library.

For some practice, try the Exercises from the book for [§2.5](https://diveintosystems.org/exercises/section-2_5.html) and [§2.7](https://diveintosystems.org/exercises/section-2_7.html).
