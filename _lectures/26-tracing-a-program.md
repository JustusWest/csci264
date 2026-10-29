---
title: "26 Tracing a Program"
published: true
---

[*Reading: Dive into Systems §7.5.2 and §7.5.3*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we wrote one function and followed a single call. Today we write a whole program, then trace it.

Here is the book's example from the start of Chapter 7. We use `long` instead of `int` so the assembly uses the 64-bit registers:

```c
#include <stdio.h>

long assign(void) {
    long y = 40;
    return y;
}

long adder(void) {
    long a;
    return a + 2;
}

int main(void) {
    long x;
    assign();
    x = adder();
    printf("x is: %ld\n", x);
    return 0;
}
```

```console
$ gcc -o prog prog.c
$ ./prog
x is: 42
```

`adder` never sets `a`. So where does 42 come from?

## Writing the Program

Let's write this program in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64), one function at a time, using the four parts from last time.

**Local variables live in the frame.** Each local gets an 8-byte slot below `%rbp`, starting at `-8(%rbp)`. Each of our three functions has one local, so each one uses `-8(%rbp)`.

| C | Assembly |
|---|---|
| `long y = 40;` | `movq $40, -8(%rbp)` |
| `return y;` | `mov -8(%rbp), %rax` |
| `long a;` | nothing |
| `return a + 2;` | `mov -8(%rbp), %rax`, then `add $2, %rax` |
| `assign();` | `call assign` |
| `x = adder();` | `call adder`, then `mov %rax, -8(%rbp)` |

- **`long a;` makes no instruction.** Declaring a variable without a value just gives it a slot. Nothing is written there.
- **`assign();` ignores the return value.** The 40 comes back in `%rax`, and `main` never uses it.
- **`main` makes room for `x` with `sub`, so it ends with `leave`.** `assign` and `adder` call nothing, so they never move `%rsp`.
- **We leave out the `printf`.** The visualizer doesn't show what it prints, and `x` will be on the stack where we can see it.

<details class="code-example" markdown="1">
<summary>Show code: the program</summary>

```text
  .text
  .globl	assign
  .type	assign, @function

assign:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start assign's frame
  movq $40, -8(%rbp)      # long y = 40;
  mov -8(%rbp), %rax      # return y;
  pop %rbp                # restore main's %rbp
  ret
  .size	assign, .-assign

  .globl	adder
  .type	adder, @function

adder:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start adder's frame
                          # long a;  (no instruction)
  mov -8(%rbp), %rax      # return a + 2;
  add $2, %rax
  pop %rbp                # restore main's %rbp
  ret
  .size	adder, .-adder

  .globl	main
  .type	main, @function

main:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start main's frame
  sub $16, %rsp           # room for x
  call assign             # assign();
  call adder              # x = adder();
  mov %rax, -8(%rbp)
  mov $0, %rax            # return 0;
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09assign%0A%20%20.type%09assign%2C%20%40function%0A%0Aassign%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20assign's%20frame%0A%20%20movq%20%2440%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20long%20y%20%3D%2040%3B%0A%20%20mov%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20return%20y%3B%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09assign%2C%20.-assign%0A%0A%20%20.globl%09adder%0A%20%20.type%09adder%2C%20%40function%0A%0Aadder%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20adder's%20frame%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20long%20a%3B%20%20(no%20instruction)%0A%20%20mov%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20return%20a%20%2B%202%3B%0A%20%20add%20%242%2C%20%25rax%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09adder%2C%20.-adder%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20main's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20x%0A%20%20call%20assign%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20assign()%3B%0A%20%20call%20adder%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%20adder()%3B%0A%20%20mov%20%25rax%2C%20-8(%25rbp)%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%200%3B%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

</details>

This is the same code `gcc` makes without optimization, apart from the `printf`.

## How to Trace a Program

1. **Write down the starting values** of `%rip`, `%rsp`, and `%rbp`.
2. **Follow `%rip`.** Add a row for each instruction in the order it runs. After a `call`, the next row is the first line of the function being called.
3. **Fill in only what changes.** Leave a blank if nothing changed.
4. **Draw the stack next to the table.** Update it on every `push`, `pop`, `call`, `ret`, and `mov` to memory. Never erase a value. Cross it out only when something overwrites it.
5. **Check at every `ret`.** `%rsp` and `%rbp` should be back to what they were before the `call`.

Here is what changes for each instruction:

| Instruction | What changes |
|---|---|
| `push %rbp` | `%rsp` goes down 8, then `%rbp` is copied to the new top |
| `pop %rbp` | the top is copied to `%rbp`, then `%rsp` goes up 8 |
| `sub $0x10,%rsp` | `%rsp` goes down 16. Nothing is written |
| `call F` | `%rsp` goes down 8, the return address is copied there, then `%rip` = F |
| `ret` | the top is copied to `%rip`, then `%rsp` goes up 8 |
| `leave` | `%rsp` = `%rbp`, then `pop %rbp` |

Every other instruction moves `%rip` to the next line.

We trace with the visualizer's own addresses, shortened to the last three hex digits as last time, so you can check every row by stepping through. `main` has already run `push %rbp`, `mov %rsp, %rbp`, and `sub $16, %rsp`, so `%rsp` is AE0 and `%rbp` is AF0.

## assign

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — main calls assign</p>

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 12E | AE0 | AF0 | |
| 12E | `call assign` | | | | |
| 106 | `push %rbp` | | | | |
| 107 | `mov %rsp,%rbp` | | | | |
| 10A | `movq $40,-8(%rbp)` | | | | |
| 112 | `mov -8(%rbp),%rax` | | | | |
| 116 | `pop %rbp` | | | | |
| 117 | `ret` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 12E | AE0 | AF0 | |
| 12E | `call assign` | **106** | AD8 |  | M[AD8] = 133 |
| 106 | `push %rbp` | 107 | AD0 |  | M[AD0] = AF0 |
| 107 | `mov %rsp,%rbp` | 10A |  | AD0 |  |
| 10A | `movq $40,-8(%rbp)` | 112 |  |  | M[AC8] = 0x28 (`y`) |
| 112 | `mov -8(%rbp),%rax` | 116 |  |  | `%rax` = 0x28 |
| 116 | `pop %rbp` | 117 | AD8 | AF0 |  |
| 117 | `ret` | **133** | AE0 |  |  |

</details>

</div>

Here is the stack right after `movq $40,-8(%rbp)`. Like the ASM Visualizer, the lowest address is at the top:

| Address | Value | |
|---|---|---|
| AC8 | 0x28 (`y`) | |
| AD0 | AF0 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| AD8 | 133 (return address) | |
| AE0 | (unused) | ← `main`'s `%rsp` |
| AE8 | (`x`, not set yet) | |
| AF0 | 0x1 (`main`'s caller's `%rbp`) | ← `main`'s `%rbp` |

- **`main`'s frame is AE0 to AF0.** `sub $16, %rsp` made room for `x` at `-8(%rbp)`, which is AE8.
- **`y` is past the top of the stack.** `%rsp` is AD0 and `y` is at AC8. `assign` calls nothing, so it doesn't bother moving `%rsp`: nothing will be pushed on top of `y`.
- **After `ret`, `%rsp` and `%rbp` are back to AE0 and AF0.** `main` is the active frame again.
- **0x28 is still at AC8.** `pop` and `ret` move `%rsp`. They don't erase anything.

## adder

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — main calls adder</p>

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 133 | AE0 | AF0 | |
| 133 | `call adder` | | | | |
| 118 | `push %rbp` | | | | |
| 119 | `mov %rsp,%rbp` | | | | |
| 11C | `mov -8(%rbp),%rax` | | | | |
| 120 | `add $2,%rax` | | | | |
| 124 | `pop %rbp` | | | | |
| 125 | `ret` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 133 | AE0 | AF0 | |
| 133 | `call adder` | **118** | AD8 |  | M[AD8] = 138 |
| 118 | `push %rbp` | 119 | AD0 |  | M[AD0] = AF0 |
| 119 | `mov %rsp,%rbp` | 11C |  | AD0 |  |
| 11C | `mov -8(%rbp),%rax` | 120 |  |  | `%rax` = **0x28** |
| 120 | `add $2,%rax` | 124 |  |  | `%rax` = 0x2a |
| 124 | `pop %rbp` | 125 | AD8 | AF0 |  |
| 125 | `ret` | **138** | AE0 |  |  |

</details>

</div>

Here is the stack right before `mov -8(%rbp),%rax`:

| Address | Value | |
|---|---|---|
| AC8 | 0x28 (left there by `assign`) | |
| AD0 | AF0 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| AD8 | 138 (return address, was 133) | |
| AE0 | (unused) | ← `main`'s `%rsp` |
| AE8 | (`x`, not set yet) | |
| AF0 | 0x1 | ← `main`'s `%rbp` |

- **`adder`'s frame starts at the same address as `assign`'s.** Both are called from `main` with `%rsp` at AE0, so both get `%rbp` = AD0.
- **`a` and `y` are the same 8 bytes.** Both are at `-8(%rbp)`, which is AC8. `assign` left 40 there and nothing erased it.
- **`call adder` overwrote the old return address.** 138 replaced 133 at AD8.
- **0x2a is 42.** `adder` returns it in `%rax`.

## Back to main

- **`mov %rax, -8(%rbp)` stores the 42 in `x`**, at AE8. In the C, `printf` would print it here.
- **`leave` sets `%rsp` to AF0, then pops `main`'s caller's `%rbp`.** `ret` pops the return address at AF8 and goes back into the code that called `main`.

## The Bug

- **Local variables are not set to 0.** An uninitialized local holds whatever was in its slot last.
- **The stack keeps old values.** `assign`'s `y` was still at AC8 when `adder` read `a` from the same place.
- **We can't predict it.** If `main` called `adder()` before `assign()`, `a` would hold whatever was at AC8 before `main` ran.
- **It only works by accident.** Reading an uninitialized variable is undefined behavior. With `-O1`, the compiler drops the stack entirely and the program prints `x is: 2` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFzc2lnbih2b2lkKSB7XG4gICAgbG9uZyB5ID0gNDA7XG4gICAgcmV0dXJuIHk7XG59XG5cbmxvbmcgYWRkZXIodm9pZCkge1xuICAgIGxvbmcgYTtcbiAgICByZXR1cm4gYSArIDI7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHg7XG4gICAgYXNzaWduKCk7XG4gICAgeCA9IGFkZGVyKCk7XG4gICAgcHJpbnRmKFwieCBpczogJWxkXFxuXCIsIHgpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)). `gcc` will warn you if you ask:

```console
$ gcc -Wall -o prog prog.c
prog.c: In function 'adder':
prog.c:10:14: warning: 'a' is used uninitialized [-Wuninitialized]
   10 |     return a + 2;
      |            ~~^~~
```

## Keeping a Value Across a Call

Last time we wrote `scale`. Now `main` calls it twice and adds the results:

```c
int main(void) {
    long total = scale(1, 2, 3) + scale(4, 5, 6);
    return total;
}
```

`scale(1, 2, 3)` is 5 and `scale(4, 5, 6)` is 26, so `total` should be 31.

Here is `scale` written a little differently. It does its work in `%rcx`, then copies the answer to `%rax`:

```text
scale:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start scale's frame
  mov %rdi, %rcx          # tmp = x
  imul %rsi, %rcx         # tmp = x * m
  add %rdx, %rcx          # tmp = x * m + b
  mov %rcx, %rax          # result = tmp
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax
```

**First try:** keep the first result in `%rcx` while `main` makes the second call.

<details class="code-example" markdown="1">
<summary>Show code: first try</summary>

```text
main:
  push %rbp
  mov %rsp, %rbp
  mov $1, %rdi            # x = 1
  mov $2, %rsi            # m = 2
  mov $3, %rdx            # b = 3
  call scale              # %rax = scale(1, 2, 3)
  mov %rax, %rcx          # keep the first result in %rcx
  mov $4, %rdi            # x = 4
  mov $5, %rsi            # m = 5
  mov $6, %rdx            # b = 6
  call scale              # %rax = scale(4, 5, 6)
  add %rcx, %rax          # total = first + second
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09scale%0A%20%20.type%09scale%2C%20%40function%0A%0Ascale%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20scale's%20frame%0A%20%20mov%20%25rdi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%0A%20%20imul%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%20*%20m%0A%20%20add%20%25rdx%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%20*%20m%20%2B%20b%0A%20%20mov%20%25rcx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20tmp%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09scale%2C%20.-scale%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%201%0A%20%20mov%20%242%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20m%20%3D%202%0A%20%20mov%20%243%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20b%20%3D%203%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20scale(1%2C%202%2C%203)%0A%20%20mov%20%25rax%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20keep%20the%20first%20result%20in%20%25rcx%0A%20%20mov%20%244%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%204%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20m%20%3D%205%0A%20%20mov%20%246%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20b%20%3D%206%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20scale(4%2C%205%2C%206)%0A%20%20add%20%25rcx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20total%20%3D%20first%20%2B%20second%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at `0x34`, which is 52, not 31.

</details>

- **`scale` changed `%rcx`.** The second call left 26 there, so `main` added 26 + 26.
- **`scale` is allowed to.** `%rcx` is caller saved: any call can change it. If `main` needs a value after a call, it has to keep it somewhere the call won't touch.

**Fix:** save the first result in `main`'s own frame.

<details class="code-example" markdown="1">
<summary>Show code: fixed</summary>

```text
main:
  push %rbp
  mov %rsp, %rbp
  sub $16, %rsp           # room to save a value
  mov $1, %rdi            # x = 1
  mov $2, %rsi            # m = 2
  mov $3, %rdx            # b = 3
  call scale              # %rax = scale(1, 2, 3)
  mov %rax, -8(%rbp)      # save the first result on the stack
  mov $4, %rdi            # x = 4
  mov $5, %rsi            # m = 5
  mov $6, %rdx            # b = 6
  call scale              # %rax = scale(4, 5, 6)
  add -8(%rbp), %rax      # total = first + second
  leave
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09scale%0A%20%20.type%09scale%2C%20%40function%0A%0Ascale%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20scale's%20frame%0A%20%20mov%20%25rdi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%0A%20%20imul%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%20*%20m%0A%20%20add%20%25rdx%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20tmp%20%3D%20x%20*%20m%20%2B%20b%0A%20%20mov%20%25rcx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20tmp%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09scale%2C%20.-scale%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20to%20save%20a%20value%0A%20%20mov%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%201%0A%20%20mov%20%242%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20m%20%3D%202%0A%20%20mov%20%243%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20b%20%3D%203%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20scale(1%2C%202%2C%203)%0A%20%20mov%20%25rax%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20save%20the%20first%20result%20on%20the%20stack%0A%20%20mov%20%244%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%204%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20m%20%3D%205%0A%20%20mov%20%246%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20b%20%3D%206%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20scale(4%2C%205%2C%206)%0A%20%20add%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20total%20%3D%20first%20%2B%20second%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at `0x1F`, which is 31.

</details>

- **`sub $16, %rsp` makes room in `main`'s frame.** Each `call` now pushes below the saved value instead of on top of it. Because `main` moved `%rsp`, it ends with `leave`.
- **The stack is the safe place.** `scale` only touches its own frame, so `-8(%rbp)` in `main`'s frame survives the call.

Next time you'll trace programs like this one in pairs.
