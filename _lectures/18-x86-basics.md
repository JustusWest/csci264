---
title: "18 x86-64 Basics"
published: true
---

[*Reading: Dive into Systems §7.1*](https://diveintosystems.org/book/C7-x86_64/basics.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we followed a program through `gcc` and got a first look at x86-64 assembly. Today we learn how to read it: registers, operands, and instruction suffixes.

To check what kind of processor a machine has, run `uname -m`. The lab machines print `x86_64`. My Mac prints `arm64`.

## A First Example
Let's start with a simple C program, `adder`:

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


Here is `adder2` as `objdump -d` prints it in the book ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG4vL2FkZHMgdHdvIHRvIGFuIGludGVnZXIgYW5kIHJldHVybnMgdGhlIHJlc3VsdFxuaW50IGFkZGVyMihpbnQgYSkge1xuICAgIHJldHVybiBhICsgMjtcbn1cblxuaW50IG1haW4odm9pZCl7XG4gICAgaW50IHggPSA0MDtcbiAgICB4ID0gYWRkZXIyKHgpO1xuICAgIHByaW50ZihcInggaXM6ICVkXFxuXCIsIHgpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

```text
0000000000400526 <adder2>:
  400526:  55          push   %rbp
  400527:  48 89 e5    mov    %rsp,%rbp
  40052a:  89 7d fc    mov    %edi,-0x4(%rbp)
  40052d:  8b 45 fc    mov    -0x4(%rbp),%eax
  400530:  83 c0 02    add    $0x2,%eax
  400533:  5d          pop    %rbp
  400534:  c3          retq
```

Each line has three parts:

- the **address** of the instruction in memory (`400526`)
- the instruction's **machine code**, as hex bytes (`55`)
- the **assembly** for those bytes (`push %rbp`)

Instructions are not all the same size. `push %rbp` is one byte and `mov %rsp,%rbp` is three, so the program counter will go up by different amounts.

As we saw last time, one line of C becomes several instructions. `a + 2` is `mov -0x4(%rbp),%eax` and `add $0x2,%eax`.

## Registers

A **register** is a word-sized storage location inside the CPU. x86-64 has 16 registers for 64-bit data:

`%rax`, `%rbx`, `%rcx`, `%rdx`, `%rdi`, `%rsi`, `%rsp`, `%rbp`, and `%r8` through `%r15`.

A register just holds bits. Whether those bits are an `int` or an address is up to the program.

Fourteen of them are general purpose. The compiler reserves two for managing the stack:

- **`%rsp`**, the **stack pointer**, always points to the top of the stack.
- **`%rbp`**, the **frame pointer** (or base pointer), points to the base of the current function's stack frame.

That is why every function starts with `push %rbp` and `mov %rsp,%rbp` (more detail in next lecture).

The compiler also uses registers to pass values between functions:

- The first six parameters go in `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, and `%r9`, in that order.
- The return value goes in `%rax`.

![The 16 x86-64 integer registers and their usage conventions, in two columns of labeled boxes. Left column: %rax, return value; %rbx, callee saved; %rcx, argument #4; %rdx, argument #3; %rsi, argument #2; %rdi, argument #1; %rsp, stack pointer; %rbp, callee saved. Right column: %r8, argument #5; %r9, argument #6; %r10 and %r11, caller saved; %r12 through %r15, callee saved. The argument and return-value registers are shaded yellow, the callee-saved registers green, and %rsp red.]({{ "/figures/ch7/registers.png" | relative_url }})

*Figure: Bryant and O'Hallaron, [Computer Systems: A Programmer's Perspective](https://csapp.cs.cmu.edu/) lecture slides, used with permission.*

The figure labels `%rbp` "callee saved" rather than frame pointer. Callee saved and caller saved are rules for which registers a function must put back before it returns. We'll get to them when we cover functions.

In `adder2`, `a` arrives in `%edi` (part of `%rdi`), and the result is left in `%eax` (part of `%rax`).

## Component Registers

x86-64 is an extension of a 32-bit architecture, which was an extension of a 16-bit one. So each register can also be used in smaller pieces. For `%rax`:

```text
 63                       32 31           16 15    8 7     0
+---------------------------+---------------+-------+-------+
|                           |               |  %ah  |  %al  |
+---------------------------+---------------+-------+-------+
                                            |<---- %ax ---->|
                            |<----------- %eax ------------>|
|<------------------------- %rax -------------------------->|
```

There are two naming patterns:

- **The first eight registers** (`%rax` to `%rbp`) were in the 32-bit version. Replace the `r` with `e` for the lower 32 bits (`%eax`), and drop the `r` for the lower 16 (`%ax`). The lowest byte is `%al`. `%rax`, `%rbx`, `%rcx`, and `%rdx` also have a name for the second byte: `%ah`.
- **`%r8` to `%r15`** are new in x86-64. Add `d` for 32 bits, `w` for 16, and `b` for 8: `%r9d`, `%r9w`, `%r9b`.

<details class="code-example" markdown="1">
<summary>Show table: every register's components</summary>

| 64-bit | Lower 32 bits | Lower 16 bits | Lower 8 bits |
|---|---|---|---|
| `%rax` | `%eax` | `%ax` | `%al` |
| `%rbx` | `%ebx` | `%bx` | `%bl` |
| `%rcx` | `%ecx` | `%cx` | `%cl` |
| `%rdx` | `%edx` | `%dx` | `%dl` |
| `%rdi` | `%edi` | `%di` | `%dil` |
| `%rsi` | `%esi` | `%si` | `%sil` |
| `%rsp` | `%esp` | `%sp` | `%spl` |
| `%rbp` | `%ebp` | `%bp` | `%bpl` |
| `%r8` | `%r8d` | `%r8w` | `%r8b` |
| `%r9` | `%r9d` | `%r9w` | `%r9b` |
| `%r10` | `%r10d` | `%r10w` | `%r10b` |
| `%r11` | `%r11d` | `%r11w` | `%r11b` |
| `%r12` | `%r12d` | `%r12w` | `%r12b` |
| `%r13` | `%r13d` | `%r13w` | `%r13b` |
| `%r14` | `%r14d` | `%r14w` | `%r14b` |
| `%r15` | `%r15d` | `%r15w` | `%r15b` |

</details>

The compiler picks the piece that matches the type. An `int` is 32 bits, so `adder2` uses `%edi` and `%eax`. If `a` were a `long`, it would use `%rdi` and `%rax`. Pointers are 64 bits, so they always use the full register.

Writing to a 32-bit piece also clears the upper 32 bits. After `mov $0x2,%eax`, all of `%rax` is 2. The 16-bit and 8-bit pieces leave the rest of the register alone.

### The Instruction Pointer

One more register: **`%rip`**, the **instruction pointer**. It holds the address of the next instruction to run. This is the **program counter (PC)**.

Programs cannot write to `%rip` directly. It moves forward after every instruction, and changes with jumps or function calls.


## Instruction Structure

Each instruction has an **opcode**, which says what to do, and one or more **operands**, which say what to do it to.

```text
add $0x2,%eax
```

The opcode is `add`, and the operands are `$0x2` and `%eax`.

```text
opcode source, destination
```

With two operands, the **source comes first and the destination second**. `mov %rsp,%rbp` copies `%rsp` into `%rbp`. `add $0x2,%eax` adds 2 to `%eax` and stores the result in `%eax`.

### AT&T vs. Intel Syntax

There are two ways to write x86 assembly. We use **AT&T syntax**, like the book and `gcc` on Linux. (UNIX was developed at AT&T Bell Labs.) Windows tools and many online references use **Intel syntax**, which puts the destination *first*:

| AT&T (ours) | Intel |
|---|---|
| `add $0x2,%eax` | `add eax, 0x2` |
| `mov %edi,-0x4(%rbp)` | `mov DWORD PTR [rbp-0x4], edi` |

Intel syntax has no `%` or `$`, and uses square brackets for memory. If an example you find online has the operands in the "wrong" order, check which syntax it is written in.

## Operands

There are three kinds of operands:

- **Constants** (literals) start with `$`. `$0x2` is the value 2.
- **Registers** start with `%`. `%eax` is the value in `%eax`.
- **Memory** operands perform address lookups in RAM.  Addresses can combine registers and constant values, for example `-0x4(%rbp)` means: take the value in `%rbp`, subtract 4, and look up what is stored at that address.

A memory operand is a pointer dereference. In C terms, `-0x4(%rbp)` is `*(%rbp - 4)`.

The `$` matters. `$0x808` is the number 0x808. `0x808` without the `$` is a memory operand: the value *stored at* address 0x808.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — the operands in adder2</p>

| Instruction | Source | Destination |
|---|---|---|
| `push %rbp` | register | the stack |
| `mov %rsp,%rbp` | register | register |
| `mov %edi,-0x4(%rbp)` | register | memory |
| `mov -0x4(%rbp),%eax` | memory | register |
| `add $0x2,%eax` | constant | register |
| `pop %rbp` | the stack | register |

</div>

### The General Memory Form

Every memory operand is some version of:

```text
D(Rb,Ri,S)   →   M[D + Rb + Ri*S]
```

- `D` is a constant **displacement**
- `Rb` is the **base** register
- `Ri` is the **index** register
- `S` is the **scale**: 1, 2, 4, or 8

If `%rax` holds the base address of an `int` array and `%rcx` holds `i`, then `(%rax,%rcx,4)` is `arr[i]`. We'll come back to this with arrays in §7.7.

There are two rules:

- **A constant can't be a destination.** You can't store a value into the number 2.
- **Memory can't be both the source and the destination** in one instruction. To copy from memory to memory, go through a register.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — evaluating operands</p>

Suppose memory and the registers hold:

| Address | Value |
|---|---|
| 0x804 | 0xCA |
| 0x808 | 0xFD |
| 0x80c | 0x12 |
| 0x810 | 0x1E |

| Register | Value |
|---|---|
| `%rax` | 0x804 |
| `%rbx` | 0x10 |
| `%rcx` | 0x4 |
| `%rdx` | 0x1 |

Then:

| Operand | Form | Translation | Value |
|---|---|---|---|
| `%rcx` | register | `%rcx` | 0x4 |
| `(%rax)` | memory | M[`%rax`] = M[0x804] | 0xCA |
| `$0x808` | constant | 0x808 | 0x808 |
| `0x808` | memory | M[0x808] | 0xFD |
| `0x8(%rax)` | memory | M[`%rax` + 8] = M[0x80c] | 0x12 |
| `(%rax,%rcx)` | memory | M[`%rax` + `%rcx`] = M[0x808] | 0xFD |
| `0x4(%rax,%rcx)` | memory | M[`%rax` + `%rcx` + 4] = M[0x80c] | 0x12 |
| `0x800(,%rdx,4)` | memory | M[0x800 + `%rdx`*4] = M[0x804] | 0xCA |
| `(%rax,%rdx,8)` | memory | M[`%rax` + `%rdx`*8] = M[0x80c] | 0x12 |

For every memory operand, the steps are the same: compute the address, then look up the value at that address. `%rcx` is 0x4, but `(%rcx)` would be whatever is stored at address 0x4.

</div>

{% comment %}
INSTRUCTOR: put the two value tables on the board and have them fill in the Value column before showing the translations.
{% endcomment %}

Try these yourself. Now `%rax` is 0x100 and `%rcx` is 0x4, and memory holds 0xAB at 0x100, 0xCD at 0x104, 0xEF at 0x108, and 0x10 at 0x10c.

| Operand | Value |
|---|---|
| `(%rax)` | |
| `(%rax,%rcx)` | |
| `(%rax,%rcx,2)` | |
| `0x104(,%rcx,2)` | |
| `0x108` | |
| `$0x4` | |

<details class="code-example" markdown="1">
<summary>Show answers</summary>

| Operand | Translation | Value |
|---|---|---|
| `(%rax)` | M[0x100] | 0xAB |
| `(%rax,%rcx)` | M[0x100 + 0x4] = M[0x104] | 0xCD |
| `(%rax,%rcx,2)` | M[0x100 + 0x4*2] = M[0x108] | 0xEF |
| `0x104(,%rcx,2)` | M[0x104 + 0x4*2] = M[0x10c] | 0x10 |
| `0x108` | M[0x108] | 0xEF |
| `$0x4` | the constant 4 | 0x4 |

</details>

## Instruction Suffixes

Many instructions have a one-letter suffix that gives the size of the data:

| Suffix | C type | Size (bytes) |
|---|---|---|
| `b` | `char` | 1 |
| `w` | `short` | 2 |
| `l` | `int` or `unsigned` | 4 |
| `s` | `float` | 4 |
| `q` | `long`, `unsigned long`, all pointers | 8 |
| `d` | `double` | 8 |

The `l` stands for "long," from when 32 bits was long. In C on our machines, a `long` is 8 bytes and uses `q`.

The compiler's output (`gcc -S`, and the code we paste into the ASM Visualizer) always includes the suffix: `movl`, `addl`, `pushq`. `objdump` and Compiler Explorer's binary view leave it off when a register already tells you the size. `mov %edi,-0x4(%rbp)` has to move 4 bytes, because `%edi` is 32 bits.

When no register is involved, the suffix stays. From last time's `assign`:

```text
movl   $0x28,-0x4(%rbp)
```

Neither a constant nor a memory address says how many bytes to write, so the `l` does: 4 bytes, one `int`.

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — which suffix?</p>

| Instruction | Suffix |
|---|---|
| `add $5,%rax` | `q`: `%rax` is 64 bits |
| `add $3,%al` | `b`: `%al` is 8 bits |
| `add $4,%r11w` | `w`: `%r11w` is 16 bits |
| `add $9,%edi` | `l`: `%edi` is 32 bits |

</div>

## Running adder2

[Open adder2 in the ASM Visualizer](https://asm.diveintosystems.org/fullprog/x86_64/%20%20%20%20%20%20%20%20.globl%20%20main%0Aadder2%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25edi%2C%20-4%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%242%2C%20%25eax%0A%20%20%20%20%20%20%20%20popq%20%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20ret%0A.LC0%3A%0A%20%20%20%20%20%20%20%20.string%20%22x%20is%3A%20%25d%5Cn%22%0Amain%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%2416%2C%20%25rsp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2440%2C%20-4%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25edi%0A%20%20%20%20%20%20%20%20call%20%20%20%20adder2%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20-4%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-4%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25esi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC0%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20leave%0A%20%20%20%20%20%20%20%20ret) and click **Run Code**. This is the compiler's version of the code, so every instruction has its suffix.

<details class="code-example" markdown="1">
<summary>Show code: adder2 in the ASM Visualizer</summary>

```text
        .globl  main
adder2:
        pushq   %rbp
        movq    %rsp, %rbp
        movl    %edi, -4(%rbp)
        movl    -4(%rbp), %eax
        addl    $2, %eax
        popq    %rbp
        ret
.LC0:
        .string "x is: %d\n"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        subq    $16, %rsp
        movl    $40, -4(%rbp)
        movl    -4(%rbp), %eax
        movl    %eax, %edi
        call    adder2
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

{% comment %}
INSTRUCTOR: before stepping into adder2, ask which register `a` will be in.
{% endcomment %}

Step through it with **Next** and watch the registers:

- **In `main`**, `movl $40, -4(%rbp)` stores `x` on the stack. Then `x` is copied into `%eax`, and from there into `%edi`: the first parameter.
- **In `adder2`**, `a` is copied from `%edi` to the stack and back into `%eax`. Then `addl $2, %eax` makes `%rax` `0x2a`, which is 42.
- **Back in `main`**, the return value in `%eax` is copied into `x`, then into `%esi` for `printf`. The program prints `x is: 42`.

## Looking Ahead

On Monday we start §7.2: the most common instructions (`mov`, `add`, `sub`), and how `push` and `pop` manage the stack. Tuesday's lab is on the lab machines, where we compile `adder2` and look at it with `objdump` ourselves.

For some practice, try the [Exercises from the book for §7.1](https://diveintosystems.org/exercises/section-7_1.html).
