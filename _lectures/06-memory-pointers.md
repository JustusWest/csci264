---
title: "06 Functions, Program Memory, Scope, and Pointers"
---

[*Reading: Dive into Systems §1.4*](https://diveintosystems.org/book/C1-C_intro/functions.html) and [*§2.1–2.2*](https://diveintosystems.org/book/C2-C_depth/scope_memory.html)

## Functions

Functions break code into manageable pieces and cut down on duplication. Some key terms:

- **parameter** — input to the function
- **return** — output of the function
- **declaration** (or **prototype**) — specifies the function's name, parameter list, and return type
- **definition** — the code that's executed when the function is called

```c
// function format:
// ---------------------------
<return type> <function name> (<parameter list>) // declaration
{
    <function body> // definition
}

// parameter list format:
// ---------------------
<type> <param1 name>, <type> <param2 name>, ..., <type> <last param name>
```

For example, here's a function that computes the larger of two values:

<details class="code-example" markdown="1">
<summary>Show code: max (short version)</summary>

```c
/* This program computes the larger of two
 * values entered by the user.
 */
#include <stdio.h>

/* max: computes the larger of two integer values
 *   x: one integer value
 *   y: the other integer value
 *   returns: the larger of x and y
 */
int max(int x, int y) {
    int bigger;

    if (x > y) {
        bigger = x;
    } else {
        bigger = y;
    }
    printf("  in max, before return x: %d y: %d\n", x, y);
    return bigger;
}
```

</details>

A function call invokes a specific instance of the function, passing specific argument values:

```c
// function call format:
// ---------------------
function_name(<argument list>);

// argument list format:
// ---------------------
<argument 1 expression>, <argument 2 expression>, ..., <last argument expression>
```

For a full example, see [`code/Chapter_1/stack.c`]({{ "/code/Chapter_1/stack.c" | relative_url }}). Read it on your own; we will not walk through it in class.

### The Stack

The execution stack keeps track of the state of active functions. Each function call creates a new **stack frame** holding that call's parameters and local variables, stacked on top of whichever function called it:

![A stack with a main frame (a, b, res) below a max frame (x, y, bigger)]({{ "/figures/ch1/function_simple.png" | relative_url }})

When `max` returns, its frame is discarded and `x`, `y`, and `bigger` cease to exist. That is *why* a local variable is usable only inside its own function, and *why* each call to a function gets its own private copies.

## Scope

A variable's **scope** is the set of code blocks in which its name has meaning and can be used.

**Global variables** are declared *outside* of any function body. They are permanently in scope and can be used by any code in the program. Every global must have a unique name.

**Local variables and parameters** are declared *inside* a function body, and are in scope only within that function. Space for them is allocated when the function is called and deallocated when it returns.

<details class="code-example" markdown="1">
<summary>Show code: globals, locals, and parameters</summary>

```c
/* An example C program with local and global variables */
#include <stdio.h>

int max(int n1, int n2);   /* function prototypes */
int change(int amt);

int g_x;                   /* global variable: declared outside function bodies */

int main(void) {
    int x, result;         /* local variables: declared inside a function body */

    printf("Enter a value: ");
    scanf("%d", &x);

    g_x = 10;              /* globals can be accessed in any function */
    result = max(g_x, x);
    printf("%d is the largest of %d and %d\n", result, g_x, x);

    result = change(10);
    printf("g_x's value was %d and now is %d\n", result, g_x);

    return 0;
}

int max(int n1, int n2) {  /* function with two parameters */
    int val;               /* local variable */
    val = n1;
    if (n2 > n1) {
        val = n2;
    }
    return val;
}

int change(int amt) {
    int val;
    val = g_x;
    g_x += amt;
    return val;
}
```

</details>

Notice that `max` and `change` both declare a local variable named `val`. The two names refer to two different storage locations:  when code in `max` refers to `val`, it means *its own* `val`.

The same holds for each *activation* of a function. Each call gets its own bindings for its parameters and locals.

As a general rule, **avoid global variables.** There are occasional legitimate uses, but code written with only locals and parameters is more modular, easier to test, and easier to debug.

## The Parts of Program Memory

When you launch a program, the operating system allocates its **address space** — storage for everything the program needs to execute, both its instructions and its data. Think of the address space as one enormous array of addressable bytes. Each used address holds all or part of an instruction or a data value.

That space is divided into regions, each holding a different kind of thing:

![Showing the heirarchy of memory, stacked on top of eachother: operating system, Code, data, heap, stack - with a pointer going from the stack to the heap.]({{ "/figures/ch2/program_memory.png" | relative_url }})



**Code.** The program's instructions. `main`, `max`, and `change` all live here.

**Data.** Global variables. Unlike the stack, the data region never grows or shrinks — storage for a global persists for the entire run of the program.

**Stack.** Local variables and parameters. The stack grows and shrinks as functions are called and return, so it is placed at the high-address end of memory with room to expand. This is the execution stack from the first section: each active call gets a **stack frame** holding its parameters and locals, and the frame exists only while that call is active.

**Heap.** Dynamically allocated memory, starting at the top of the addresses so that both have room to grow.

The stack and the heap are the two regions that change size at runtime, which is why they are placed at opposite ends with the empty middle between them.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — where does each variable live?</p>

For the program above, place each name in its region:

| Name | Region | Lifetime |
|---|---|---|
| `main`, `max`, `change` (the instructions) | code | entire run |
| `g_x` | data | entire run |
| `x`, `result` (locals of `main`) | stack | while `main` is active |
| `n1`, `n2` (parameters of `max`) | stack | while that call to `max` is active |
| `val` in `max` | stack | while that call to `max` is active |
| `amt`, `val` in `change` | stack | while that call to `change` is active |

The two `val` variables are in the same *region* but not the same *location*, and they are never both live at the same time in this program.

</div>

## Addresses and Pointers

Every byte in the address space has an address. The **address operator** `&` applied to a variable evaluates to the address of that variable's storage location.

We have already used this without much explanation:

```c
scanf("%d", &num1);
```

`scanf` needs to *store* a value into `num1`, so it cannot be handed `num1`'s value — it must be told where `num1` lives. `&num1` is that location.

You can print an address with the `%p` placeholder:

```c
int x = 12;
printf("x holds %d and lives at %p\n", x, &x);
```

```text
x holds 12 and lives at 0x7ffee3b4c8ac
```

Addresses are conventionally printed in hexadecimal. Do not expect to see the same address twice across runs; modern systems deliberately randomize layout.

## Pointer Variables

A **pointer variable** stores the address of a memory location in which a value of a specific type can be stored. The pointer *points to* (refers to) that value, and gives you a level of indirection for reaching it.

```text
        ptr                          x
    +-----------+              +-----------+
    | 0x7ffe...-|------------->|    12     |
    +-----------+              +-----------+
      address of x               an int
```

The rules for using a pointer are like the rules for any other variable, except that you have to keep *two* types straight: the type of the pointer variable itself, and the type of the thing it points to.

**1. Declare it**, with `type_name *var_name`:

```c
int  *ptr;    // stores the memory address of an int  (ptr "points to" an int)
char *cptr;   // stores the memory address of a char  (cptr "points to" a char)
```

The type of `ptr` is "pointer to int" (`int *`). The type of `cptr` is "pointer to char" (`char *`). These are *different types*, and they are not interchangeable.

**2. Initialize it** — make it point at something. The usual way is `&` on an existing variable of the matching type:

```c
int x;
char ch;

ptr  = &x;    // ptr gets the address of x
cptr = &ch;   // cptr gets the address of ch
```

Mismatched types are a mistake even where the compiler lets them through with a warning:

```c
cptr = &x;    // ERROR: cptr holds a char address, but &x is the address of an int
```

Any pointer can also be given the special value `NULL`, which represents an invalid address. It is common to check if a pointer is `NULL` before using it to access memory.

```c
ptr = NULL;
```

**3. Use it.** The **dereference operator** `*` follows a pointer to the location it points to, and accesses the value stored there:

```c
ptr = &x;     // ptr points to x
*ptr = 8;     // the memory location ptr points to gets 8  (so x is now 8)
```

Read `*ptr` out loud as "the thing ptr points to." Note that `*` in a *declaration* (`int *ptr;`) and `*` in an *expression* (`*ptr = 8;`) are doing two different things — the first says "this variable is a pointer," the second says "go where this points."

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — tracing two pointers</p>

```c
int *ptr1, *ptr2, x, y;

x = 8;
ptr2 = &x;      // ptr2 is assigned the address of x
ptr1 = NULL;
```

| | `ptr1` | `ptr2` | `x` | `y` |
|---|---|---|---|---|
| after these three lines | NULL | &x | 8 | ? |

```c
*ptr2 = 10;     // the location ptr2 points to gets 10
y = *ptr2 + 3;  // y gets (what ptr2 points to) plus 3
```

| | `ptr1` | `ptr2` | `x` | `y` |
|---|---|---|---|---|
| | NULL | &x | 10 | 13 |

Note that `x` changed even though the statement never mentions `x`. That is the whole point of indirection — and the reason pointer bugs are hard to find.

```c
ptr1 = ptr2;    // ptr1 gets the address stored in ptr2 (both now point to x)
*ptr1 = 80;
```

| | `ptr1` | `ptr2` | `x` | `y` |
|---|---|---|---|---|
| | &x | &x | 80 | 13 |

Two pointers pointing at the same location is called **aliasing**. Writing through either one changes what the other sees.

```c
ptr1 = &y;      // change ptr1's value: change what it points to
*ptr1 = 80;     // now this writes to y, not x
```

| | `ptr1` | `ptr2` | `x` | `y` |
|---|---|---|---|---|
| | &y | &x | 80 | 80 |

The last two steps are the pair to watch: assigning *through* a pointer (`*ptr1 = 80`) moves data, while assigning *to* a pointer (`ptr1 = &y`) moves the arrow. They look nearly identical on the page and do completely different things.

</div>

## Common Errors



**Assigning to the pointer when you meant to assign through it.**

```c
int x, *ptr;

ptr = 20;     // ERROR: this points ptr at address 20
ptr = &x;
*ptr = 20;    // CORRECT: this stores 20 in the location ptr points to
```

Both lines compile. The first one aims the pointer at address 20, which your program almost certainly does not own, and the crash arrives later at whatever line finally dereferences it.

**Dereferencing a pointer that holds no valid address.** An uninitialized pointer holds whatever garbage was in that stack slot:

```c
int *ptr1;
*ptr1 = 100;   // crash, or worse, silent corruption
```

This is the reason to initialize pointers to `NULL` — a program can then check before dereferencing:

```c
if (ptr1 != NULL) {
    *ptr1 = 100;
}
```

Dereferencing `NULL` still crashes, but it crashes *immediately and reliably*, at the line responsible.

## Looking Ahead
Next time we look at §2.3 using pointer parameters to write functions that can modify the caller's variables and §2.4 introduces the heap, `malloc`, and `free`, so a program can request memory at runtime instead of being stuck with what it declared.

For some practice, [try the Exercises from the book](https://diveintosystems.org/exercises/section-2_2.html)
