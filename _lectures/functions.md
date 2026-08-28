---
title: "Functions"
published: false
---


## Functions

Functions break code into manageable pieces and cut down on duplication. Some key terms:

- **parameter** — input to the function
- **return** — output of the function
- **declaration** (or **prototype**) — specifies the function's name, parameter list, and return type
- **definition** — the code that's executed when the function is called

```c
// function definition format:
// ---------------------------
<return type> <function name> (<parameter list>)
{
    <function body>
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

    bigger = x;
    if (y > x) {
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

### The Stack

The execution stack keeps track of the state of active functions. Each function call creates a new **stack frame** holding that call's parameters and local variables, stacked on top of whichever function called it:

![A stack with a main frame (a, b, res) below a max frame (x, y, bigger)]({{ "/figures/ch1/function_simple.png" | relative_url }})

Here's a fuller example that declares `max` with a prototype, calls it from `main`, and adds a second function, `print_table`:

<details class="code-example" markdown="1">
<summary>Show code: functions, prototypes, and scanf</summary>

```c
/* This file shows examples of defining and calling C functions.
 * It also demonstrates using scanf().
 */

#include <stdio.h>

/* This is an example of a FUNCTION PROTOTYPE.  It declares just the type
 * information for a function (the function's name, return type, and parameter
 * list). A prototype is used when code in main wants to call the function
 * before its full definition appears in the file.
 */
int max(int n1, int n2);

/* A prototype for another function.  void is the return type of a function
 * that does not return a value
 */
void print_table(int start, int stop);

/* All C programs must have a main function.  This function defines what the
 * program does when it begins executing, and it's typically used to organize
 * the big-picture behavior of the program.
 */
int main(void) {
    int x, y, larger;

    printf("This program will operate over two int values.\n");

    printf("Enter the first value: ");
    scanf("%d", &x);

    printf("Enter the second value: ");
    scanf("%d", &y);

    larger = max(x, y);

    printf("The larger of %d and %d is %d\n", x, y, larger);

    print_table(x, larger);

    return 0;
}

/* This is an example of a FUNCTION DEFINITION.  It specifies not only the
 * function name and type, but it also fully defines the code of its body.
 * (Notice, and emulate, the complete function comment!)
 */
/* Computes the max of two integer values.
 *   n1: the first value
 *   n2: the other value
 *   returns: the larger of n1 and n2
 */
int max(int n1, int n2)  {
    int result;

    result = n1;

    if (n2 > n1) {
        result = n2;
    }

    return result;
}

/* prints out the squares from start to stop
 *   start: the beginning of the range
 *   stop: the end of the range
 */
void print_table(int start, int stop) {
    int i;

    for (i = start; i <= stop; i++) {
        printf("%d\t", i*i);
    }

    printf("\n");
}
```

</details>

Full runnable version: [`code/Chapter_1/stack.c`]({{ "/code/Chapter_1/stack.c" | relative_url }})
