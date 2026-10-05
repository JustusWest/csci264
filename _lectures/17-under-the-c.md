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

Here is just the `do`–`while` loop from `lab1.s`, with the line of C each part comes from:

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

We are not learning to read this yet, but a few things are worth noticing:

- **One line of C is several instructions.** Each instruction does one small thing.
- Names like `w8`, `w9`, and `x11` are **registers**, the small, fast storage inside the CPU from Ch. 5. The `w` registers hold 32 bits (our `int`s) and the `x` registers hold 64 bits (our pointer `p`).
- **Square brackets mean "go to this address in memory"**, like `*` in C. Our variables are locations on the stack: `u` is `[sp, #44]`, `b` is `[sp, #40]`, `r` is `[sp, #36]`, and `p` is `[sp, #24]`.
- **`ldr` loads** a value from memory into a register and **`str` stores** it back. `strb` stores a single byte: a `char`. Most of the loop is moving data back and forth.
- **`#` marks a constant.** Our 10, 48, and 55 are right there.
- **There is no `%` instruction.** `sdiv` divides, then `mul` and `subs` compute the remainder as `u - (u / b) * b`.
- **There is no `if` and no loop.** Labels like `LBB0_1` mark places in the code, and `b` (branch) jumps to them. The loop is `cbnz` ("branch if not zero") back to `LBB0_1`. Notice the compiler flipped our test: instead of `r < 10`, it checks `r >= 10` (`b.ge`) and jumps to the `else`.

The compiler did exactly what we wrote: it even divides twice. With optimization turned on, it does better:

```console
$ gcc -S -O1 lab1.c
```

```text
LBB0_1:
	mov	x13, x10
	sdiv	w10, w10, w9
	msub	w13, w10, w9, w13
	cmp	w13, #10
	csel	w14, w12, w11, lt
	add	w13, w14, w13
	strb	w13, [x8, #-1]!
	cbnz	w10, LBB0_1
```

Eight lines instead of 27:

- **One division instead of two.** `msub` (multiply-subtract) gets the remainder in one step.
- **The variables stay in registers** instead of going back to memory every time.
- **`strb w13, [x8, #-1]!`** does `p--` and `*p = ...` in one instruction.
- **The `if`/`else` is gone.** 48 and 55 are already sitting in `w12` and `w11`, and `csel` (conditional select) picks the right one based on the compare. That's the eager execution trick from the pipelining lecture: no branch means no control hazard.

{% comment %}
INSTRUCTOR: Before showing -O1, ask how they would make the loop faster. Someone usually spots the double division. The csel callback to Fri 10/2 is the payoff.
{% endcomment %}

## Assembly Depends on the Machine

Everything above came from my Mac, which has an Apple processor that uses **ARM**. The lab machines use **x86-64**. The C is the same, but each CPU family has its own **instruction set**.

<details class="code-example" markdown="1">
<summary>Show code: the same loop on the lab machines (x86-64)</summary>

```text
.L4:
	movl	-132(%rbp), %eax      # r = u % b
	movl	-128(%rbp), %ecx
	cltd
	idivl	%ecx
	movl	%edx, -124(%rbp)
	movl	-132(%rbp), %eax      # u = u / b
	movl	-128(%rbp), %ecx
	cltd
	idivl	%ecx
	movl	%eax, -132(%rbp)
	subq	$1, -120(%rbp)        # p--
	cmpl	$9, -124(%rbp)        # if (r < 10)
	jg	.L2
	movl	-124(%rbp), %eax      # *p = r + 48
	addl	$48, %eax
	movl	%eax, %edx
	movq	-120(%rbp), %rax
	movb	%dl, (%rax)
	jmp	.L3
.L2:
	movl	-124(%rbp), %eax      # else *p = r + 55
	addl	$55, %eax
	movl	%eax, %edx
	movq	-120(%rbp), %rax
	movb	%dl, (%rax)
.L3:
	movl	-132(%rbp), %eax      # while (u != 0)
	testl	%eax, %eax
	jne	.L4
```

</details>

**This course uses x86-64**, so do your assembly work on the lab machines. If you compile on your own computer, you are likely to get different code.
