---
title: "02 Conditionals, Loops, Functions, and Arrays"
---

[*Reading: Dive into Systems §1.3–1.5*](https://diveintosystems.org/book/C1-C_intro/conditionals.html)

Building on last time, we move from C's basic syntax into its control structures — conditionals, loops, and functions — and then into C's most important compound data type, the array (and its close cousin, the string).

## Conditionals

Table 5 in the reading shows that `if`-`else` syntax in C and Python is almost identical. The main difference is that Python uses indentation to mark the body of a branch, while C uses curly braces (but keep indenting your C code anyway — it's still how humans read it).

<details class="code-example">
<summary>Show code: if-else example</summary>
<table>
<tr>
<th>ifelse.py</th>
<th>ifelse.c</th>
</tr>
<tr>
<td markdown="1">
```python
# Python if-else example
def main():
    num1 = input("Enter the 1st number:")
    num1 = int(num1)
    num2 = input("Enter the 2nd number:")
    num2 = int(num2)

    if num1 > num2:
        print("%d is biggest" % num1)
        num2 = num1
    else:
        print("%d is biggest" % num2)
        num1 = num2

# call the main function:
main()
```
</td>
<td markdown="1">
```c
/* C if-else example */
#include <stdio.h>

int main(void) {
    int num1, num2;

    printf("Enter the 1st number: ");
    scanf("%d", &num1);
    printf("Enter the 2nd number: ");
    scanf("%d", &num2);

    if (num1 > num2) {
        printf("%d is biggest\n", num1);
        num2 = num1;
    } else {
        printf("%d is biggest\n", num2);
        num1 = num2;
    }

    return 0;
}
```
</td>
</tr>
</table>
</details>

Both languages also support multiway branching by chaining `if` and `else if`:

```c
if ( <boolean expression> ) {
    <true body>
}
else if ( <boolean expression 2> ) {
    <true 2 body>
}
// ... more else if's ...
else { // the final else part is optional
    <false body>
}
```

**Boolean values in C.** C has no boolean type. Instead, integers are evaluated as booleans:

- **`0`** evaluates to **false**
- **any nonzero value** (positive *or* negative) evaluates to **true**

**Relational operators:** equality `==` and inequality `!=`; comparison `<`, `<=`, `>`, `>=`.

**Logical operators:** negation `!`, and `&&`, or `||`. `&&` and `||` are *short-circuiting* — `&&` stops evaluating as soon as it hits a false operand, `||` stops as soon as it hits a true one, so the remaining operand(s) aren't evaluated at all.

```c
// assume x and y are ints, already assigned values
if ( (x > 10) && (y >= x) ) {
    printf("y and x are both larger than 10\n");
    x = 13;
} else if ( ((-x) == 10) || (y > x) ) {
    printf("y might be bigger than x\n");
    x = y * x;
} else {
    printf("I have no idea what the relationship between x and y is\n");
}
```

## Loops

C supports `for` and `while` loops like Python, plus `do`-`while`, which Python doesn't have.

**`while` loops** — syntax and behavior are nearly identical between the two languages:

<details class="code-example">
<summary>Show code: while loop example</summary>
<table>
<tr>
<th>whileloop.py</th>
<th>whileloop.c</th>
</tr>
<tr>
<td markdown="1">
```python
# Python while loop example
def main():
    num = input("Enter a value: ")
    num = int(num)

    # make sure num is not negative
    if num < 0:
        num = -num

    val = 1
    while val < num:
        print("%d" % (val))
        val = val * 2

# call the main function:
main()
```
</td>
<td markdown="1">
```c
/* C while loop example */
#include <stdio.h>

int main(void) {
    int num, val;

    printf("Enter a value: ");
    scanf("%d", &num);

    // make sure num is not negative
    if (num < 0) {
        num = -num;
    }

    val = 1;
    while (val < num) {
        printf("%d\n", val);
        val = val * 2;
    }

    return 0;
}
```
</td>
</tr>
</table>
</details>

`while` checks the boolean expression *first*, and only runs the body if it's true — so the body might run zero times.

**`do`-`while` loops** run the body *first*, then check the condition, repeating for as long as it's true. That means the body always runs at least once. Here's the same loop as above, rewritten as a `do`-`while`:

```c
/* C do-while loop example */
#include <stdio.h>

int main(void) {
    int num, val;

    printf("Enter a value: ");
    scanf("%d", &num);

    // make sure num is not negative
    if (num < 0) {
        num = -num;
    }

    val = 1;
    do {
        printf("%d\n", val);
        val = val * 2;
    } while (val < num);

    return 0;
}
```

**`for` loops** — this is where C and Python really diverge. Python's `for` iterates over a sequence; C's `for` is a general-purpose loop built from three parts:

```c
for ( <initialization>; <boolean expression>; <step> ) {
    <body>
}
```

Evaluation order: (1) run `<initialization>` once, (2) check `<boolean expression>` — if false, exit the loop, (3) run the loop body, (4) run `<step>`, (5) go back to (2).

<details class="code-example">
<summary>Show code: for loop example</summary>
<table>
<tr>
<th>forloop.py</th>
<th>forloop.c</th>
</tr>
<tr>
<td markdown="1">
```python
# Python for loop example
def main():
    num = input("Enter a value: ")
    num = int(num)

    # make sure num is not negative
    if num < 0:
        num = -num

    for i in range(num):
        print("%d" % i)

# call the main function:
main()
```
</td>
<td markdown="1">
```c
/* C for loop example */
#include <stdio.h>

int main(void) {
    int num, i;

    printf("Enter a value: ");
    scanf("%d", &num);

    // make sure num is not negative
    if (num < 0) {
        num = -num;
    }

    for (i = 0; i < num; i++) {
        printf("%d\n", i);
    }

    return 0;
}
```
</td>
</tr>
</table>
</details>

`for` and `while` are equally powerful in C — anything you write with one, you can rewrite with the other. Python doesn't have this property, since its `for` is restricted to iterating over sequences.

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

## Arrays

An array is an ordered collection of data of the same type — "ordered" means each element has a specific position (position 0, position 1, ...), not that the values are sorted. Python's list gives you a high-level interface (it resizes itself, and gives you `len`, `append`, `insert`, and so on); C's array is a low-level, fixed-capacity block of storage, and it's up to you to track how much of it you're using.

<details class="code-example">
<summary>Show code: list vs. array example</summary>
<table>
<tr>
<th>list_ex.py</th>
<th>array_ex.c</th>
</tr>
<tr>
<td markdown="1">
```python
# An example Python program using a list.
def main():
    # create an empty list
    my_lst = []

    # add 10 integers to the list
    for i in range(10):
        my_lst.append(i)

    # set value at position 3 to 100
    my_lst[3] = 100

    # print the number of list items
    print("list %d items:" % len(my_lst))

    # print each element of the list
    for i in range(10):
        print("%d" % my_lst[i])

# call the main function:
main()
```
</td>
<td markdown="1">
```c
/* An example C program using an array. */
#include <stdio.h>

int main(void) {
    int i, size = 0;

    // declare array of 10 ints
    int my_arr[10];

    // set the value of each array element
    for (i = 0; i < 10; i++) {
        my_arr[i] = i;
        size++;
    }

    // set value at position 3 to 100
    my_arr[3] = 100;

    // print the number of array elements
    printf("array of %d items:\n", size);

    // print each element of the array
    for (i = 0; i < 10; i++) {
        printf("%d\n", my_arr[i]);
    }

    return 0;
}
```
</td>
</tr>
</table>
</details>

Two differences worth calling out: Python's `len()` tells you how many items are in a list; C has no equivalent, so *you* keep a `size` variable yourself. And in C, array elements are laid out in consecutive locations in memory — the third element sits immediately after the second and immediately before the fourth. Python hides how its lists are stored.

**Array access.** C only supports accessing elements by indexing (Python has slices and other tricks; C doesn't):

```c
my_arr[3] = 100; // C syntax to set the element in position 3 to 100.
my_arr[0] = 5;    // C syntax to set the first element to 5.
```

These are *static* arrays: their capacity is fixed at declaration and can't grow.

```c
int  arr[10];  // declare an array of 10 ints
char str[20];  // declare an array of 20 chars
```

It's common to declare more capacity than you use. When that happens, you need a separate variable to track how many elements are actually in use:

```c
int i, num;
int arr[10];  // declare an array of ints, with a capacity of 10

num = 6;      // keep track of how many elements of arr are used

// initialize first 5 elements of arr (at indices 0-4)
for (i=0; i < 5; i++) {
    arr[i] = i * 2;
}

arr[5] = 100; // assign the element at index 5 the value 100
```

**Out-of-bounds access.** Unlike Python (which raises an `IndexError`), C does **not** check array bounds, at compile time or at runtime:

```c
int array[10];   // an array of size 10 has valid indices 0 through 9

array[10] = 100;  // 10 is not a valid index into the array
```

The compiler is happy to compile this. Running it can crash your program, silently corrupt some other variable's value, or appear to do nothing at all — and the behavior can differ from run to run. It's entirely on you, the programmer, to keep array accesses within bounds.

## Strings

C has no string type. Recall that an array can be declared larger than what a program actually uses — which means a program can't assume a string's length equals the size of the array holding it. So C strings end with a special sentinel, the **null character** (`'\0'`), marking where the string actually ends. A string ending in `'\0'` is called *null-terminated*. Forgetting to account for the null character is one of the most common sources of bugs for new C programmers.

<details class="code-example" markdown="1">
<summary>Show code: strings and the C string library</summary>

```c
#include <stdio.h>
#include <string.h>   // include the C string library

int main(void) {
    char str1[10];
    char str2[10];
    int len;

    str1[0] = 'h';
    str1[1] = 'i';
    str1[2] = '\0';

    len = strlen(str1);

    printf("%s %d\n", str1, len);  // prints: hi 2

    strcpy(str2, str1);     // copies the contents of str1 to str2
    printf("%s\n", str2);   // prints:  hi

    strcpy(str2, "hello");  // copy the string "hello" to str2
    len = strlen(str2);
    printf("%s has %d chars\n", str2, len);   // prints: hello has 5 chars
}
```

</details>

`strlen` and `strcpy` come from `<string.h>`, C's string library — `strlen` returns a string's length (not counting the `'\0'`), and `strcpy` copies one string's contents into another. Both assume their arguments are properly null-terminated.
