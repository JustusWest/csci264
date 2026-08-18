---
title: "01 C Basics: Compiling, Variables, and I/O"
---

[*Reading: Dive into Systems §1.1–1.2*](https://diveintosystems.org/book/C1-C_intro/getting_started.html)

C is a high-level, imperative, procedural programming language. It's often considered the "lowest level" of the high-level languages: it gives you less built-in abstraction than something like Python, which makes it more labor-intensive to write, but puts you much closer to the hardware.

## From Python to C: A First Program

Here's the classic "Hello World" program, written first in Python and then in C, so we can compare syntax directly.

<details class="code-example">
<summary>Show code: hello.py / hello.c</summary>
<table>
<tr>
<th>hello.py</th>
<th>hello.c</th>
</tr>
<tr>
<td markdown="1">
```python
'''
    The Hello World Program in Python
'''

# Python math library
from math import *

# main function definition:
def main():
    # statements on their own line
    print("Hello World")
    print("sqrt(4) is %f" % (sqrt(4)))

# call the main function:
main()
```
</td>
<td markdown="1">
```c
/*
    The Hello World Program in C
*/

/* C math and I/O libraries */
#include <math.h>
#include <stdio.h>

/* main function definition: */
int main(void) {
    // statements end in a semicolon (;)
    printf("Hello World\n");
    printf("sqrt(4) is %f\n", sqrt(4));

    return 0;  // main returns value 0
}
```
</td>
</tr>
</table>
</details>

A few notable differences between the two:

| | Python | C |
|---|---|---|
| Comments | `#` | `//` (single line), `/* ... */` (multi-line) |
| Importing libraries | `import` | `#include` (always at the top of the file, outside any function) |
| Blocks | indentation | `{ }` to start/end a block, `;` to end a statement |

**The `main` function.** Every C program must have a function named `main`, and its return type must be `int`. `main` is called automatically when the program executes, and by convention it returns `0` to signal that the program ran with no errors. The `void` between the parentheses means this version of `main` doesn't take any parameters (later we'll see `main` accept command-line arguments).

## Compiling and Running C Programs

Python programs are **interpreted**: the Python interpreter reads and executes your source code directly.

![A Python program is directly executed by the Python interpreter]({{ "/figures/ch1/interpreted.png" | relative_url }})

C programs must be **compiled** before they can run. A C compiler is a program that translates C source code into a binary executable — a file of 0s and 1s that the underlying hardware can run directly. We'll use the GNU C compiler, `gcc`.

```bash
$ gcc hello.c
$ ./a.out
```

![gcc translates C source code into a binary executable, a.out, which the system then runs directly]({{ "/figures/ch1/compile.png" | relative_url }})

**Steps to write and run a C program:**

1. Write the file with a text editor (e.g. `nano` or `vim`):
   ```bash
   $ nano hello.c
   ```
2. Compile it. By default, `gcc` names the executable `a.out`:
   ```bash
   $ gcc hello.c
   $ ./a.out
   ```
3. To choose the executable's name yourself, use the `-o` flag:
   ```bash
   $ gcc -o <output_executable_file> <input_source_file>
   ```
   For example:
   ```bash
   $ gcc -o hello hello.c
   $ ./hello
   ```

Any time you edit the source file, you have to recompile it — `gcc` does not do this automatically, and an old executable can silently linger if a later compile fails.

## Variables and C Numeric Types

C uses **variables** as named storage locations. Every variable has:

- a **scope** — where in the program it can be used
- a **type** — what kind of value it can store

All variables must be declared before use, with the syntax `type_name variable_name;`. Each variable has exactly one type. The basic numeric types are `char`, `int`, `float`, and `double`.

```c
{
    /* 1. Define variables in this block's scope at the top of the block. */

    int x;            // declares x to be an int and allocates space for it
    int i, j, k;      // multiple variables of the same type can share a line

    char letter;      // a char stores a single-byte integer value
                       // often used to store a single ASCII character
                       // a char in C is a different type than a string in C

    float winpct;      // winpct is declared to be a float
    double pi;          // double is more precise than float

    /* 2. After defining all variables, you can use them in C statements. */

    x = 7;             // initialize variables before using their value
    k = x + 2;         // use x's value in an expression

    letter = 'A';         // single quotes for a single character value
    letter = letter + 1;  // letter now stores 'B' (one more than 'A' in ASCII)

    pi = 3.1415926;

    winpct = 11 / 2.0;    // winpct gets 5.5 -- winpct is a float
    j = 11 / 2;           // j gets 5: integer division truncates the decimal
    x = k % 2;            // % is C's mod operator: x gets 9 mod 2 (1)
}
```

**Numeric literals:**

```c
8     // the int value 8
3.4   // the double value 3.4
'h'   // the char value 'h' (its ASCII value is 104)
```

Note: `'h'` (single quotes) is a `char` literal — the value 104. `"h"` (double quotes) is a *string* literal, which is a different type entirely and is not the value 104.

**Numeric types:**

| Type | Usual size | Stores | Declaration |
|---|---|---|---|
| `char` | 1 byte | integers | `char x;` |
| `short` | 2 bytes | signed integers | `short x;` |
| `int` | 4 bytes | signed integers | `int x;` |
| `long` | 4 or 8 bytes | signed integers | `long x;` |
| `long long` | 8 bytes | signed integers | `long long x;` |
| `float` | 4 bytes | signed real numbers | `float x;` |
| `double` | 8 bytes | signed real numbers | `double x;` |

Any integer type can be made unsigned by adding the `unsigned` keyword, e.g. `unsigned int x;`. These sizes are typical, not guaranteed by the language — the exact size depends on the hardware architecture. You can check the size on a given machine with the `sizeof` operator, e.g. `sizeof(int)`.

**Arithmetic operators:** `+`, `-`, `*`, `/`, `%`

- C performs automatic type conversion when an operator combines operands of different types (e.g. combining an `int` with a `float` converts the `int` to `float` first).
- `%` (mod) only works on integer operands.
- **Integer division truncates.** If both operands of `/` are integer types, the result is an integer, and anything past the decimal point is discarded (`11 / 2` evaluates to `5`). If either operand is a `float`/`double`, `/` performs real division (`11 / 2.0` evaluates to `5.5`). This distinction comes up constantly — watch for it.
- Assignment: `=`. Assignment with update: `+=`, `-=`, `*=`, `/=`, `%=`. Increment/decrement: `++`, `--`.

## Input and Output: `printf` and `scanf`

`printf` prints a formatted string, similar to Python's formatted `print`. Both `printf` and `scanf` come from C's standard I/O library, so any file that uses them needs `#include <stdio.h>` at the top.

<details class="code-example" markdown="1">
<summary>Show code: printf example</summary>

**Python:**
```python
def main():
    print("Name: %s,  Info:" % "Vijay")
    print("\tAge: %d \t Ht: %g" %(20,5.9))
    print("\tYear: %d \t Dorm: %s" %(3, "Alice Paul"))

# call the main function:
main()
```

**C:**
```c
/* C printf example */
#include <stdio.h> // needed for printf

int main(void) {

    printf("Name: %s,  Info:\n", "Vijay");
    printf("\tAge: %d \t Ht: %g\n",20,5.9);
    printf("\tYear: %d \t Dorm: %s\n",
            3,"Alice Paul");

    return 0;
}
```

</details>

Both versions print identically formatted output. The main difference: Python's `print` adds a trailing newline automatically, but C's `printf` does not — so C format strings need an explicit `\n` wherever a newline is wanted. For each placeholder in the format string, `printf` expects one additional argument.

**Formatting placeholders:**

| Placeholder | Meaning |
|---|---|
| `%d` | decimal value (`int`, `short`, `char`) |
| `%g` | float or double value |
| `%s` | string value |
| `%c` | char value — prints the ASCII character |
| `%x` | value in hexadecimal |
| `%o` | value in octal |
| `%u` | unsigned decimal value |
| `%e` | float/double in scientific notation |

There is no placeholder for printing a value directly in binary.

**`char` values — numeric vs. character:**

```c
char ch;

ch = 'A';
printf("ch value is %d which is the ASCII value of  %c\n", ch, ch);

ch = 99;
printf("ch value is %d which is the ASCII value of  %c\n", ch, ch);
```
```text
ch value is 65 which is the ASCII value of  A
ch value is 99 which is the ASCII value of  c
```

**`scanf`** reads values typed in by the user (from stdin) and stores them in program variables. It's picky about the exact format of the input — if your program seems to hang waiting on badly-formed input, `Ctrl-C` will terminate it. (Chapter 2 covers more robust ways to read input.)

<details class="code-example" markdown="1">
<summary>Show code: scanf example</summary>

**Python:**
```python
def main():
    num1 = input("Enter a number:")
    num1 = int(num1)
    num2 = input("Enter another:")
    num2 = int(num2)

    print("%d + %d = %d" % (num1, num2, (num1+num2)))

# call the main function:
main()
```

**C:**
```c
/* C input (scanf) example */
#include <stdio.h>

int main(void) {
    int num1, num2;

    printf("Enter a number: ");
    scanf("%d", &num1);
    printf("Enter another: ");
    scanf("%d", &num2);

    printf("%d + %d = %d\n", num1, num2, (num1+num2));

    return 0;
}
```

</details>

Note the `&` before each variable name in `scanf`: `scanf` needs the *address* of the variable so it knows where in memory to store the value it reads, rather than the variable's current value. We'll come back to what `&` really means when we get to pointers.
