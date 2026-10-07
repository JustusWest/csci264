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

## Following Lab 1 Through the Pipeline

Here is a solution to Lab 1, base conversion of integers:

<details class="code-example" markdown="1">
<summary>Show code: lab1.c</summary>

```c
#include <stdio.h>

int main(void) {
    int u, b, r;
    char y[100];
    char *p;

    printf("Enter a number and a base: ");
    scanf("%d %d", &u, &b);

    p = &y[99];
    *p = 0;
    p--;
    *p = '\n';

    do {
        r = u % b;
        u = u / b;
        p--;
        if (r < 10) {
            *p = r + 48;
        } else {
            *p = r + 55;
        }
    } while (u != 0);

    printf("%s", p);
    return 0;
}
```

</details>


### 1. Preprocess

```console
$ wc -l lab1.c
34 lab1.c
$ gcc -E lab1.c | wc -l
586
```

The `#include <stdio.h>` line was replaced by the full contents of that header file. That is where the prototype for `printf` comes from:

```console
$ gcc -E lab1.c | grep "int printf"
extern int printf (const char *__restrict __format, ...);
```

The header has the *prototype*, not the code for `printf`.

### 2. Compile

```console
$ gcc -S lab1.c
$ wc -l lab1.s
106 lab1.s
```

`lab1.s` is a text file. Open it in an editor. It's the assembly version of our program.
### 3. Assemble

```console
$ gcc -c lab1.c
$ file lab1.o
lab1.o: Mach-O 64-bit object arm64
```

`lab1.o` is an **object file**: machine code, not text. We can't open it in an editor, but `objdump -d` will **disassemble** it, turning the bytes back into assembly:

```console
$ objdump -d lab1.o
```

Each line shows an address, the instruction's bytes in hex, and the assembly for those bytes.

`lab1.o` is not a complete program yet. `nm` lists the names it defines and the names it uses:

```console
$ nm lab1.o
                 U ___stack_chk_fail
                 U ___stack_chk_guard
0000000000000000 T _main
                 U _printf
                 U _scanf
```

`T` means the function's code is in this file. `U` means **undefined**: we call it, but the code isn't here. (`__stack_chk_fail` is a safety check the compiler adds. We'll see why when we get to buffer overflows.)

### 4. A First Look at Assembly

Everything so far ran on my Mac, which has an Apple processor that uses **ARM**. The lab machines use **x86-64**. The C is the same, but each CPU family has its own **instruction set**, so the `lab1.s` my Mac made is not the assembly we study in this course.

<details class="code-example" markdown="1">
<summary>Show code: the loop from my Mac (ARM)</summary>

```text
LBB0_1:
	ldr	w8, [sp, #44]       ; r = u % b
	ldr	w9, [sp, #40]
	sdiv	w10, w8, w9
	mul	w9, w10, w9
	subs	w8, w8, w9
	str	w8, [sp, #36]
	ldr	w8, [sp, #44]       ; u = u / b
	ldr	w9, [sp, #40]
	sdiv	w8, w8, w9
	str	w8, [sp, #44]
	ldr	x11, [sp, #24]      ; p--
	subs	x11, x11, #1
	str	x11, [sp, #24]
	ldr	w8, [sp, #36]       ; if (r < 10)
	cmp	w8, #10
	b.ge	LBB0_3
	ldr	w8, [sp, #36]       ; *p = r + 48
	add	w8, w8, #48
	ldr	x9, [sp, #24]
	strb	w8, [x9]
	b	LBB0_4
LBB0_3:
	ldr	w8, [sp, #36]       ; else *p = r + 55
	add	w8, w8, #55
	ldr	x9, [sp, #24]
	strb	w8, [x9]
LBB0_4:
	ldr	w8, [sp, #44]       ; while (u != 0)
	cbnz	w8, LBB0_1
```

</details>

To see x86-64 assembly from any computer, we will use **Compiler Explorer**. [Set it up with these instructions]({{ "/godbolt/" | relative_url }}), or [open Lab 1 in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5pbnQgbWFpbih2b2lkKSB7XG4gICAgaW50IHUsIGIsIHI7XG4gICAgY2hhciB5WzEwMF07XG4gICAgY2hhciAqcDtcblxuICAgIHByaW50ZihcIkVudGVyIGEgbnVtYmVyIGFuZCBhIGJhc2U6IFwiKTtcbiAgICBzY2FuZihcIiVkICVkXCIsICZ1LCAmYik7XG5cbiAgICBwID0gJnlbOTldO1xuICAgICpwID0gMDtcbiAgICBwLS07XG4gICAgKnAgPSAnXFxuJztcblxuICAgIGRvIHtcbiAgICAgICAgciA9IHUgJSBiO1xuICAgICAgICB1ID0gdSAvIGI7XG4gICAgICAgIHAtLTtcbiAgICAgICAgaWYgKHIgPCAxMCkge1xuICAgICAgICAgICAgKnAgPSByICsgNDg7XG4gICAgICAgIH0gZWxzZSB7XG4gICAgICAgICAgICAqcCA9IHIgKyA1NTtcbiAgICAgICAgfVxuICAgIH0gd2hpbGUgKHUgIT0gMCk7XG5cbiAgICBwcmludGYoXCIlc1wiLCBwKTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D) with the settings already in place.

Here is just the `do`–`while` loop, with the line of C each part comes from. (Compiler Explorer color-matches these for you.) The numbers on the left are addresses:

```text
  50:  mov    -0x10(%rbp),%eax    # r = u % b
  53:  mov    -0x14(%rbp),%ecx
  56:  cltd
  57:  idiv   %ecx
  59:  mov    %edx,-0xc(%rbp)
  5c:  mov    -0x10(%rbp),%eax    # u = u / b
  5f:  mov    -0x14(%rbp),%ecx
  62:  cltd
  63:  idiv   %ecx
  65:  mov    %eax,-0x10(%rbp)
  68:  subq   $0x1,-0x8(%rbp)     # p--
  6d:  cmpl   $0x9,-0xc(%rbp)     # if (r < 10)
  71:  jg     83 <main+0x83>
  73:  mov    -0xc(%rbp),%eax     # *p = r + 48
  76:  add    $0x30,%eax
  79:  mov    %eax,%edx
  7b:  mov    -0x8(%rbp),%rax
  7f:  mov    %dl,(%rax)
  81:  jmp    91 <main+0x91>
  83:  mov    -0xc(%rbp),%eax     # else *p = r + 55
  86:  add    $0x37,%eax
  89:  mov    %eax,%edx
  8b:  mov    -0x8(%rbp),%rax
  8f:  mov    %dl,(%rax)
  91:  mov    -0x10(%rbp),%eax    # while (u != 0)
  94:  test   %eax,%eax
  96:  jne    50 <main+0x50>
```

We are not learning to read this yet, but a few things are worth noticing:

- **One line of C is several instructions.** Each instruction does one small thing.
- Names starting with `%` are **registers**, the small, fast storage inside the CPU from Ch. 5. `%eax`, `%ecx`, and `%edx` hold 32 bits (our `int`s), `%rax` holds 64 bits (our pointer `p`), and `%dl` holds 8 bits (a `char`).
- **Parentheses mean "go to this address in memory"**, like `*` in C. `-0x10(%rbp)` means "the address in `%rbp`, minus 0x10". Our variables are locations on the stack: `u` is `-0x10(%rbp)`, `b` is `-0x14(%rbp)`, `r` is `-0xc(%rbp)`, and `p` is `-0x8(%rbp)`.
- **`mov` copies a value** from memory to a register, or back. Most of the loop is moving data back and forth. `mov %dl,(%rax)` stores one byte at the address in `p`: that's `*p = ...`.
- **`$` marks a constant**, shown in hex. Our 10, 48, and 55 are there as `$0x9`, `$0x30`, and `$0x37`.
- **There is no `%` instruction.** `idiv` divides and leaves *both* answers: the quotient in `%eax` and the remainder in `%edx`. (`cltd` gets `%edx` ready for the divide.)
- **There is no `if` and no loop.** Jumps go to addresses: `jmp` always jumps, `jg` jumps if greater, and `jne` jumps if not equal. The loop is `jne` back to address `50`. Notice the compiler flipped our test: instead of `r < 10`, it checks `r > 9` (`jg`) and jumps to the `else`.

The compiler did exactly what we wrote: it even divides twice, though one `idiv` already gives both answers. With optimization turned on, it does better. Type `-O1` in the compiler options box, or [open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5pbnQgbWFpbih2b2lkKSB7XG4gICAgaW50IHUsIGIsIHI7XG4gICAgY2hhciB5WzEwMF07XG4gICAgY2hhciAqcDtcblxuICAgIHByaW50ZihcIkVudGVyIGEgbnVtYmVyIGFuZCBhIGJhc2U6IFwiKTtcbiAgICBzY2FuZihcIiVkICVkXCIsICZ1LCAmYik7XG5cbiAgICBwID0gJnlbOTldO1xuICAgICpwID0gMDtcbiAgICBwLS07XG4gICAgKnAgPSAnXFxuJztcblxuICAgIGRvIHtcbiAgICAgICAgciA9IHUgJSBiO1xuICAgICAgICB1ID0gdSAvIGI7XG4gICAgICAgIHAtLTtcbiAgICAgICAgaWYgKHIgPCAxMCkge1xuICAgICAgICAgICAgKnAgPSByICsgNDg7XG4gICAgICAgIH0gZWxzZSB7XG4gICAgICAgICAgICAqcCA9IHIgKyA1NTtcbiAgICAgICAgfVxuICAgIH0gd2hpbGUgKHUgIT0gMCk7XG5cbiAgICBwcmludGYoXCIlc1wiLCBwKTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D):

```text
  80:  mov    %edi,%eax           # r = u % b and u = u / b
  82:  cltd
  83:  idiv   %r8d
  86:  mov    %eax,%edi
  88:  sub    $0x1,%rsi           # p--
  8c:  lea    0x30(%rdx),%r9d     # *p = r + 48 or r + 55
  90:  lea    0x37(%rdx),%ecx
  93:  cmp    $0x9,%edx
  96:  mov    %r9d,%edx
  99:  cmovg  %ecx,%edx
  9c:  mov    %dl,(%rsi)
  9e:  test   %eax,%eax           # while (u != 0)
  a0:  jne    80 <main+0x80>
```

13 lines instead of 27:

- **One division instead of two.** A single `idiv` gives `u / b` in `%eax` and `u % b` in `%edx`.
- **The variables stay in registers** instead of going back to memory every time: `u` is in `%edi`, `b` in `%r8d`, and `p` in `%rsi`.
- **The `if`/`else` is gone.** The two `lea` lines compute both `r + 48` and `r + 55`, and `cmovg` (conditional move) picks the right one based on the compare. That's the eager execution trick from the pipelining lecture: no branch means no control hazard.

{% comment %}
INSTRUCTOR: Before showing -O1, ask how they would make the loop faster. Someone usually spots the double division. The cmovg callback to Fri 10/2 is the payoff.
{% endcomment %}

### 5. Running It

Compiler Explorer shows us the assembly, but it doesn't let us watch it run. For that we use the **ASM Visualizer**, made by the authors of our textbook. [Here is how it works]({{ "/asm-visualizer/" | relative_url }}).

To move the unoptimized code over:

1. In Compiler Explorer, clear `-O1` and turn **off** **Compile to binary object** in the compiler output options menu (the gear icon). The visualizer needs the compiler's version of the code, with labels like `.L4` instead of addresses.
2. Copy the assembly into the visualizer's **Full Program Mode** (x86_64).
3. Add `.globl main` as the first line. Compiler Explorer hides it.
4. The visualizer can't read from the keyboard, so replace the six lines that call `scanf` with two lines that set `u` and `b`:

```text
        movl    $26, -16(%rbp)      # u = 26
        movl    $16, -20(%rbp)      # b = 16
```

Or [open the finished program in the ASM Visualizer](https://asm.diveintosystems.org/fullprog/x86_64/%20%20%20%20%20%20%20%20.globl%20%20main%0A.LC0%3A%0A%20%20%20%20%20%20%20%20.string%20%22Enter%20a%20number%20and%20a%20base%3A%20%22%0A.LC1%3A%0A%20%20%20%20%20%20%20%20.string%20%22%25d%20%25d%22%0A.LC2%3A%0A%20%20%20%20%20%20%20%20.string%20%22%25s%22%0Amain%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20addq%20%20%20%20%24-128%2C%20%25rsp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC0%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2426%2C%20-16%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2416%2C%20-20%28%25rbp%29%0A%20%20%20%20%20%20%20%20leaq%20%20%20%20-128%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20addq%20%20%20%20%2499%2C%20%25rax%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rax%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%240%2C%20%28%25rax%29%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%241%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%2410%2C%20%28%25rax%29%0A.L4%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-20%28%25rbp%29%2C%20%25ecx%0A%20%20%20%20%20%20%20%20cltd%0A%20%20%20%20%20%20%20%20idivl%20%20%20%25ecx%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25edx%2C%20-12%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-20%28%25rbp%29%2C%20%25ecx%0A%20%20%20%20%20%20%20%20cltd%0A%20%20%20%20%20%20%20%20idivl%20%20%20%25ecx%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20-16%28%25rbp%29%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%241%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20cmpl%20%20%20%20%249%2C%20-12%28%25rbp%29%0A%20%20%20%20%20%20%20%20jg%20%20%20%20%20%20.L2%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-12%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%2448%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25edx%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%25dl%2C%20%28%25rax%29%0A%20%20%20%20%20%20%20%20jmp%20%20%20%20%20.L3%0A.L2%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-12%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%2455%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25edx%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%25dl%2C%20%28%25rax%29%0A.L3%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20testl%20%20%20%25eax%2C%20%25eax%0A%20%20%20%20%20%20%20%20jne%20%20%20%20%20.L4%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rax%2C%20%25rsi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC2%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20leave%0A%20%20%20%20%20%20%20%20ret) and click **Run Code**.

<details class="code-example" markdown="1">
<summary>Show code: the finished program</summary>

```text
        .globl  main
.LC0:
        .string "Enter a number and a base: "
.LC1:
        .string "%d %d"
.LC2:
        .string "%s"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        addq    $-128, %rsp
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    $26, -16(%rbp)
        movl    $16, -20(%rbp)
        leaq    -128(%rbp), %rax
        addq    $99, %rax
        movq    %rax, -8(%rbp)
        movq    -8(%rbp), %rax
        movb    $0, (%rax)
        subq    $1, -8(%rbp)
        movq    -8(%rbp), %rax
        movb    $10, (%rax)
.L4:
        movl    -16(%rbp), %eax
        movl    -20(%rbp), %ecx
        cltd
        idivl   %ecx
        movl    %edx, -12(%rbp)
        movl    -16(%rbp), %eax
        movl    -20(%rbp), %ecx
        cltd
        idivl   %ecx
        movl    %eax, -16(%rbp)
        subq    $1, -8(%rbp)
        cmpl    $9, -12(%rbp)
        jg      .L2
        movl    -12(%rbp), %eax
        addl    $48, %eax
        movl    %eax, %edx
        movq    -8(%rbp), %rax
        movb    %dl, (%rax)
        jmp     .L3
.L2:
        movl    -12(%rbp), %eax
        addl    $55, %eax
        movl    %eax, %edx
        movq    -8(%rbp), %rax
        movb    %dl, (%rax)
.L3:
        movl    -16(%rbp), %eax
        testl   %eax, %eax
        jne     .L4
        movq    -8(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC2, %edi
        movl    $0, %eax
        call    printf
        movl    $0, %eax
        leave
        ret
```

</details>

Step through it with **Next** and watch:

- **The loop runs twice**, once for each digit of 26 in base 16: `1A`.
- **The first time, `r` is 10**, so `jg` jumps to `.L2`, the `else`.
- **The second time, `r` is 1**, so the code falls through to the `if` part, and `jmp` skips over the `else`.
- **The program prints `1A`** under Program Output at the end.

**This course uses x86-64.** From now on, our assembly comes from Compiler Explorer, the ASM Visualizer, or the lab machines.
