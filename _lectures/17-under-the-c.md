---
title: "17 Under the C"
published: true
---

[*Reading: Dive into Systems Ch. 6*](https://diveintosystems.org/book/C6-asm_intro/index.html) and [*§2.9.7*](https://diveintosystems.org/book/C2-C_depth/advanced_assembly.html). Optionally, the *Compilation Steps* part of [*§2.9.5*](https://diveintosystems.org/book/C2-C_depth/advanced_libraries.html)

## Why Read Assembly?

Very few people write assembly by hand anymore. Compilers do it, and do it well. But reading it is still a useful skill:

- **High-level code hides details.** C does not tell you where a variable lives or what a function call costs. Assembly does.
- **Small devices.** Many embedded processors (in cars, appliances, sensors) are too small to run compiled code from a high-level language and are programmed in assembly directly.
- **Security.** Many attacks exploit how a program stores data at runtime. Understanding them, and reverse engineering malware, means reading assembly.
- **You often don't have the source.** A compiled program is just machine code. Assembly is the readable form of it.

## gcc 

So far we have typed `gcc` and gotten an executable. That one command runs four separate steps:

| Step | What it does | Stop here with | Output |
|---|---|---|---|
| 1. Preprocess | Handles `#` lines: pastes in `#include` files, replaces `#define`s | `gcc -E` | C code (printed to the screen) |
| 2. Compile | Translates C to assembly for *this* machine | `gcc -S` | `prog.s` (text) |
| 3. Assemble | Translates assembly to binary machine code | `gcc -c` | `prog.o` (binary) |
| 4. Link | Combines `.o` files and library code into one program | `gcc` | `a.out` (binary) |

Each flag tells `gcc` to stop after that step, so we can look at what it produced.

## Following a Program Through the Pipeline

Here is the example from the start of Ch. 6:

```c
#include <stdio.h>

int adder() {
    int a;
    return a + 2;
}

int assign() {
    int y = 40;
    return y;
}

int main(void) {
    int x;
    assign();
    x = adder();
    printf("x is: %d\n", x);
    return 0;
}
```

What does it print? `assign` returns 40, but `main` throws that value away. `adder` adds 2 to `a`, but `a` was never given a value, so `x` should be garbage. Yet on most 64-bit machines it prints:

```console
$ gcc -o adder adder.c
$ ./adder
x is: 42
```

By the end of Ch. 7 we will be able to explain why.

### 1. Preprocess

```console
$ wc -l adder.c
19 adder.c
$ gcc -E adder.c | wc -l
```

{% comment %}
INSTRUCTOR: paste the line count from the Mac here (it was 586 for lab1.c, almost all of it stdio.h).
{% endcomment %}

The `#include <stdio.h>` line was replaced by the full contents of that header file. That is where the prototype for `printf` comes from:

```console
$ gcc -E adder.c | grep "int printf"
extern int printf (const char *__restrict __format, ...);
```

The header has the *prototype*, not the code for `printf`.

### 2. Compile

```console
$ gcc -S adder.c
$ wc -l adder.s
```

{% comment %}
INSTRUCTOR: paste the line count from the Mac here.
{% endcomment %}

`adder.s` is a text file. Open it in an editor. It's the assembly version of our program.

### 3. Assemble

```console
$ gcc -c adder.c
$ file adder.o
adder.o: Mach-O 64-bit object arm64
```

`adder.o` is an **object file**: machine code, not text. We can't open it in an editor, but `objdump -d` will **disassemble** it, turning the bytes back into assembly:

```console
$ objdump -d adder.o
```

Each line shows an address, the instruction's bytes in hex, and the assembly for those bytes.

`adder.o` is not a complete program yet. `nm` lists the names it defines and the names it uses:

```console
$ nm adder.o
0000000000000000 T _adder
0000000000000014 T _assign
000000000000002c T _main
                 U _printf
```

{% comment %}
INSTRUCTOR: the nm addresses above are predicted from the ARM code sizes, not captured. Re-run on the Mac and paste.
{% endcomment %}

`T` means the function's code is in this file. `U` means **undefined**: we call it, but the code isn't here.

### 4. A First Look at Assembly

Everything so far ran on my Mac, which has an Apple processor that uses **ARM**. The lab machines use **x86-64**. The C is the same, but each CPU family has its own **instruction set**, so the `adder.s` my Mac made is not the assembly we study in this course.

<details class="code-example" markdown="1">
<summary>Show code: adder and assign on my Mac (ARM)</summary>

```text
_adder:
	sub	sp, sp, #16
	ldr	w8, [sp, #12]
	add	w0, w8, #2
	add	sp, sp, #16
	ret
_assign:
	sub	sp, sp, #16
	mov	w8, #40
	str	w8, [sp, #12]
	ldr	w0, [sp, #12]
	add	sp, sp, #16
	ret
```

</details>

{% comment %}
INSTRUCTOR: the ARM listing above is from Compiler Explorer's clang with --target=arm64-apple-macos, not from the Mac. Check it against `gcc -S` on the Mac.
{% endcomment %}

To see x86-64 assembly from any computer, we will use **Compiler Explorer**. [Set it up with these instructions]({{ "/godbolt/" | relative_url }}), or [open this program in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5pbnQgYWRkZXIoKSB7XG4gICAgaW50IGE7XG4gICAgcmV0dXJuIGEgKyAyO1xufVxuXG5pbnQgYXNzaWduKCkge1xuICAgIGludCB5ID0gNDA7XG4gICAgcmV0dXJuIHk7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBpbnQgeDtcbiAgICBhc3NpZ24oKTtcbiAgICB4ID0gYWRkZXIoKTtcbiAgICBwcmludGYoXCJ4IGlzOiAlZFxcblwiLCB4KTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D) with the settings already in place.

Here is the whole program, with the line of C each part comes from. (Compiler Explorer color-matches these for you.) The numbers on the left are addresses:

```text
adder:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  mov    -0x4(%rbp),%eax     # return a + 2;
   7:  add    $0x2,%eax
   a:  pop    %rbp
   b:  ret
assign:
   c:  push   %rbp
   d:  mov    %rsp,%rbp
  10:  movl   $0x28,-0x4(%rbp)    # int y = 40;
  17:  mov    -0x4(%rbp),%eax     # return y;
  1a:  pop    %rbp
  1b:  ret
main:
  1c:  push   %rbp
  1d:  mov    %rsp,%rbp
  20:  sub    $0x10,%rsp
  24:  call   29 <main+0xd>       # assign();
  29:  call   2e <main+0x12>      # x = adder();
  2e:  mov    %eax,-0x4(%rbp)
  31:  mov    -0x4(%rbp),%eax     # printf("x is: %d\n", x);
  34:  mov    %eax,%esi
  36:  mov    $0x0,%edi
  3b:  mov    $0x0,%eax
  40:  call   45 <main+0x29>
  45:  mov    $0x0,%eax           # return 0;
  4a:  leave
  4b:  ret
```

We are not learning to read this yet, but a few things are worth noticing:

- **One line of C can be several instructions.** `return a + 2;` is two: `mov` gets `a`, and `add` adds 2.
- Names starting with `%` are **registers**, the small, fast storage inside the CPU from Ch. 5. `%eax` holds 32 bits (an `int`). `%rbp` and `%rsp` hold 64 bits.
- **Parentheses mean "go to this address in memory"**, like `*` in C. `-0x4(%rbp)` means "the address in `%rbp`, minus 4". That is where `adder` keeps `a`, where `assign` keeps `y`, and where `main` keeps `x`.
- **`$` marks a constant**, shown in hex. `$0x28` is our 40 and `$0x2` is our 2.
- **Every function starts and ends the same way.** `push %rbp` and `mov %rsp,%rbp` set up the function's space on the stack, and `pop %rbp` (or `leave`) and `ret` clean it up and go back to the caller.
- **`call` jumps to another function** and `ret` comes back. A function's return value is left in `%eax`: `adder` puts `a + 2` there, and `main` copies it into `x`.
- **`call 29 <main+0xd>` doesn't name `assign` yet.** This is an object file, so the address is a placeholder. The linker fills in the real one. (Compiler Explorer shows the name on the line just under the `call`.)

Now look at where `assign` stores 40 and where `adder` reads `a`. Remember that, and we'll come back to it.

With optimization turned on, the code gets much shorter. Type `-O1` in the compiler options box, or [open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5pbnQgYWRkZXIoKSB7XG4gICAgaW50IGE7XG4gICAgcmV0dXJuIGEgKyAyO1xufVxuXG5pbnQgYXNzaWduKCkge1xuICAgIGludCB5ID0gNDA7XG4gICAgcmV0dXJuIHk7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBpbnQgeDtcbiAgICBhc3NpZ24oKTtcbiAgICB4ID0gYWRkZXIoKTtcbiAgICBwcmludGYoXCJ4IGlzOiAlZFxcblwiLCB4KTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D):

```text
adder:
   0:  mov    $0x2,%eax
   5:  ret
assign:
   6:  mov    $0x28,%eax
   b:  ret
main:
   c:  sub    $0x8,%rsp
  10:  mov    $0x2,%esi
  15:  mov    $0x0,%edi
  1a:  mov    $0x0,%eax
  1f:  call   24 <main+0x18>
  24:  mov    $0x0,%eax
  29:  add    $0x8,%rsp
  2d:  ret
```

- **`adder` and `assign` don't use the stack.** Each is one `mov` and a `ret`.
- **`adder` never reads `a`.** `a` has no value, so the compiler may assume anything. It just returns 2.
- **`main` never calls `assign` or `adder`.** It already knows the answer and hands 2 straight to `printf`.

So with `-O1`, this program prints `x is: 2`. The 42 was never guaranteed: it came from using a variable we never set. Optimization changed the answer.

{% comment %}
INSTRUCTOR: Before showing -O1, ask what they think it will print now. The answer changing from 42 to 2 is the payoff.
{% endcomment %}

### 5. Running It

Compiler Explorer shows us the assembly, but it doesn't let us watch it run. For that we use the **ASM Visualizer**, made by the authors of our textbook. [Here is how it works]({{ "/asm-visualizer/" | relative_url }}).

To move the unoptimized code over:

1. In Compiler Explorer, clear `-O1` and turn **off** **Compile to binary object** in the compiler output options menu (the gear icon). The visualizer needs the compiler's version of the code, which uses names like `call assign` instead of addresses.
2. Copy the assembly into the visualizer's **Full Program Mode** (x86_64).
3. Add `.globl main` as the first line. Compiler Explorer hides it.

Or [open the finished program in the ASM Visualizer](https://asm.diveintosystems.org/fullprog/x86_64/%20%20%20%20%20%20%20%20.globl%20%20main%0Aadder%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%242%2C%20%25eax%0A%20%20%20%20%20%20%20%20popq%20%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20ret%0Aassign%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2440%2C%20-4%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20popq%20%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20ret%0A.LC0%3A%0A%20%20%20%20%20%20%20%20.string%20%22x%20is%3A%20%25d%5Cn%22%0Amain%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%2416%2C%20%25rsp%0A%20%20%20%20%20%20%20%20call%20%20%20%20assign%0A%20%20%20%20%20%20%20%20call%20%20%20%20adder%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20-4%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25esi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC0%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20leave%0A%20%20%20%20%20%20%20%20ret) and click **Run Code**.

<details class="code-example" markdown="1">
<summary>Show code: the finished program</summary>

```text
        .globl  main
adder:
        pushq   %rbp
        movq    %rsp, %rbp
        movl    -4(%rbp), %eax
        addl    $2, %eax
        popq    %rbp
        ret
assign:
        pushq   %rbp
        movq    %rsp, %rbp
        movl    $40, -4(%rbp)
        movl    -4(%rbp), %eax
        popq    %rbp
        ret
.LC0:
        .string "x is: %d\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        call    assign
        call    adder
        movl    %eax, -4(%rbp)
        movl    -4(%rbp), %eax
        movl    %eax, %esi
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    $0, %eax
        leave
        ret
```

</details>

Step through it with **Next** and watch:

- **`call assign` jumps into `assign`**, and `ret` jumps back to `main`.
- **When `adder` reads `a`, `%rax` becomes `0x28`**: 40, the number `assign` stored. Then `add` makes it `0x2A`, which is 42.
- **The program prints `x is: 42`** under Program Output.

How did `adder` find the 40 that `assign` left behind? That is a question for Ch. 7.

**This course uses x86-64.** From now on, our assembly comes from Compiler Explorer, the ASM Visualizer, or the lab machines.
