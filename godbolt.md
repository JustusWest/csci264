---
layout: page
title: Compiler Explorer Setup
permalink: /godbolt/
---

The assembly you get depends on the machine. My Mac gives ARM; this course uses x86-64. Lecture examples are shown in **Compiler Explorer** ([godbolt.org](https://godbolt.org)). It compiles your code on x86-64 servers and shows the assembly in your browser, so everyone sees the same output no matter what computer they use. It is free and needs no account. To run assembly and watch it step by step, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Links in the lecture notes open Compiler Explorer with these settings already set. To try your own code, set it up yourself.

## Setup

You only need to do this once per browser. Godbolt remembers your settings.

1. Go to [godbolt.org](https://godbolt.org). The left pane is your C code and the right pane is the assembly.
2. **Set the language to C.** The dropdown at the top of the left pane says **C++** by default. Change it to **C**.
3. **Check the compiler.** The dropdown at the top of the right pane should now say **x86-64 gcc** and a version number. If it doesn't, pick the newest **x86-64 gcc**.
4. **Leave the compiler options box empty.** No options means no optimization, which is how the book compiles its examples (`gcc -o adder adder.c`).
5. **Open the Compiler output options menu** (the gear icon in the right pane):
    - Turn **on** **Compile to binary object**. Godbolt will then disassemble the compiled code the way `objdump -d` does, which is how the book shows its examples.
    - Turn **off** **Intel asm syntax**. The book (and `gcc` on Linux) uses AT&T syntax.
6. Leave the **Filter...** menu as it is.

## Check Your Setup

Paste in the book's first example from §7.1:

```c
#include <stdio.h>

//adds two to an integer and returns the result
int adder2(int a) {
    return a + 2;
}

int main(void){
    int x = 40;
    x = adder2(x);
    printf("x is: %d\n", x);
    return 0;
}
```

The assembly for `adder2` should look like this:

```text
push   %rbp
mov    %rsp,%rbp
mov    %edi,-0x4(%rbp)
mov    -0x4(%rbp),%eax
add    $0x2,%eax
pop    %rbp
ret
```

Two small differences from the book are expected:

- **The addresses start at 0.** The book's addresses (`400526`, ...) come from a complete program. Godbolt shows an object file that hasn't been linked yet.
- **The book prints `retq`; Godbolt prints `ret`.** They are the same instruction. Newer tools just drop the `q`.

If your output looks different, check this table:

| You see | Cause | Fix |
|---|---|---|
| `movl` instead of `mov`, offsets like `-4(%rbp)` instead of `-0x4(%rbp)`, lots of lines starting with `.` | Showing compiler output, not disassembly | Output... → turn on **Compile to binary object** |
| `mov DWORD PTR [rbp-4], edi` (no `%` signs, operands reversed) | Intel syntax | Output... → turn off **Intel asm syntax** |
| `ldr`, `str`, `w8`, `x9` | ARM compiler selected | Choose an **x86-64 gcc** compiler |
| Many fewer instructions, no `-0x4(%rbp)` | Optimization is on | Clear the compiler options box |
| Errors about `#include` or C++ | Language is C++ | Set the left pane's language to **C** |

## Reading the Output

- **The number on the left is the address** of each instruction. The gray bytes under each instruction are its machine code.
- **Colors match C to assembly.** Each line of C and the instructions it produced share a background color. Hover over a line on either side to highlight its partners.

