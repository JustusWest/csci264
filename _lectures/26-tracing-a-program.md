---
title: "26 Tracing a Program"
published: true
---

[*Reading: Dive into Systems §7.5.2 and §7.5.3*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we traced one function call. Today we trace a whole program, from `main` through function executions.

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

## A Full Assembly Program

Here is the assembly ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFzc2lnbih2b2lkKSB7XG4gICAgbG9uZyB5ID0gNDA7XG4gICAgcmV0dXJuIHk7XG59XG5cbmxvbmcgYWRkZXIodm9pZCkge1xuICAgIGxvbmcgYTtcbiAgICByZXR1cm4gYSArIDI7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHg7XG4gICAgYXNzaWduKCk7XG4gICAgeCA9IGFkZGVyKCk7XG4gICAgcHJpbnRmKFwieCBpczogJWxkXFxuXCIsIHgpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

```text
assign:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  movq   $0x28,-0x8(%rbp)
  401132:  mov    -0x8(%rbp),%rax
  401136:  pop    %rbp
  401137:  ret

adder:
  401138:  push   %rbp
  401139:  mov    %rsp,%rbp
  40113c:  mov    -0x8(%rbp),%rax
  401140:  add    $0x2,%rax
  401144:  pop    %rbp
  401145:  ret

main:
  401146:  push   %rbp
  401147:  mov    %rsp,%rbp
  40114a:  sub    $0x10,%rsp
  40114e:  call   assign                # assign();
  401153:  call   adder                 # x = adder();
  401158:  mov    %rax,-0x8(%rbp)
  40115c:  mov    -0x8(%rbp),%rax       # printf("x is: %ld\n", x);
  401160:  mov    %rax,%rsi
  401163:  mov    $0x402004,%rdi
  401168:  mov    $0x0,%rax
  40116d:  call   printf
  401172:  mov    $0x0,%rax             # return 0;
  401177:  leave
  401178:  ret
```

- **`assign` and `adder` take no arguments**, so nothing is loaded into `%rdi` before the calls. The trace is only about frames.
- **`adder` never writes to `-0x8(%rbp)`.** It only reads from it. That is the `a` that was never set.

As before, we drop the leading `0x401` from code addresses, so 0x401146 is just 0x146. The stack addresses are arbitrary: `%rsp` starts at 0xd48 and `%rbp` at 0x830.

## assign

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — main starts and calls assign</p>

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x146 | 0xd48 | 0x830 | |
| 0x146 | `push %rbp` | | | | |
| 0x147 | `mov %rsp,%rbp` | | | | |
| 0x14a | `sub $0x10,%rsp` | | | | |
| 0x14e | `call assign` | | | | |
| 0x126 | `push %rbp` | | | | |
| 0x127 | `mov %rsp,%rbp` | | | | |
| 0x12a | `movq $0x28,-0x8(%rbp)` | | | | |
| 0x132 | `mov -0x8(%rbp),%rax` | | | | |
| 0x136 | `pop %rbp` | | | | |
| 0x137 | `ret` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x146 | 0xd48 | 0x830 | |
| 0x146 | `push %rbp` | 0x147 | 0xd40 | | M[0xd40] = 0x830 |
| 0x147 | `mov %rsp,%rbp` | 0x14a | | 0xd40 | |
| 0x14a | `sub $0x10,%rsp` | 0x14e | 0xd30 | | |
| 0x14e | `call assign` | **0x126** | 0xd28 | | M[0xd28] = 0x153 |
| 0x126 | `push %rbp` | 0x127 | 0xd20 | | M[0xd20] = 0xd40 |
| 0x127 | `mov %rsp,%rbp` | 0x12a | | 0xd20 | |
| 0x12a | `movq $0x28,-0x8(%rbp)` | 0x132 | | | M[0xd18] = 0x28 (`y`) |
| 0x132 | `mov -0x8(%rbp),%rax` | 0x136 | | | `%rax` = 0x28 |
| 0x136 | `pop %rbp` | 0x137 | 0xd28 | 0xd40 | |
| 0x137 | `ret` | **0x153** | 0xd30 | | |

</details>

</div>

Here is the stack right after `movq $0x28,-0x8(%rbp)`. Like the ASM Visualizer, the lowest address is at the top:

| Address | Value | |
|---|---|---|
| 0xd18 | 0x28 (`y`) | |
| 0xd20 | 0xd40 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| 0xd28 | 0x153 (return address) | |
| 0xd30 | | ← `main`'s `%rsp` |
| 0xd38 | (`x`, not set yet) | |
| 0xd40 | 0x830 (`main`'s caller's `%rbp`) | ← `main`'s `%rbp` |
| 0xd48 | (return address into the C library) | |

- **`main`'s frame is 0xd30 to 0xd40.** `sub $0x10,%rsp` made room for `x` at `-0x8(%rbp)`, which is 0xd38.
- **`y` is past the top of the stack.** `%rsp` is 0xd20 and `y` is at 0xd18. `assign` calls nothing, so it doesn't bother moving `%rsp`: nothing will be pushed on top of `y`.
- **After `ret`, `%rsp` and `%rbp` are back to 0xd30 and 0xd40.** `main` is the active frame again.
- **0x28 is still at 0xd18.** `pop` and `ret` move `%rsp`. They don't erase anything.

## adder

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — main calls adder</p>

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x153 | 0xd30 | 0xd40 | |
| 0x153 | `call adder` | | | | |
| 0x138 | `push %rbp` | | | | |
| 0x139 | `mov %rsp,%rbp` | | | | |
| 0x13c | `mov -0x8(%rbp),%rax` | | | | |
| 0x140 | `add $0x2,%rax` | | | | |
| 0x144 | `pop %rbp` | | | | |
| 0x145 | `ret` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x153 | 0xd30 | 0xd40 | |
| 0x153 | `call adder` | **0x138** | 0xd28 | | M[0xd28] = 0x158 |
| 0x138 | `push %rbp` | 0x139 | 0xd20 | | M[0xd20] = 0xd40 |
| 0x139 | `mov %rsp,%rbp` | 0x13c | | 0xd20 | |
| 0x13c | `mov -0x8(%rbp),%rax` | 0x140 | | | `%rax` = **0x28** |
| 0x140 | `add $0x2,%rax` | 0x144 | | | `%rax` = 0x2a |
| 0x144 | `pop %rbp` | 0x145 | 0xd28 | 0xd40 | |
| 0x145 | `ret` | **0x158** | 0xd30 | | |

</details>

</div>

Here is the stack right before `mov -0x8(%rbp),%rax`:

| Address | Value | |
|---|---|---|
| 0xd18 | 0x28 (left there by `assign`) | |
| 0xd20 | 0xd40 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| 0xd28 | 0x158 (return address, was 0x153) | |
| 0xd30 | | ← `main`'s `%rsp` |
| 0xd38 | (`x`, not set yet) | |
| 0xd40 | 0x830 | ← `main`'s `%rbp` |

- **`adder`'s frame starts at the same address as `assign`'s.** Both are called from `main` with `%rsp` at 0xd30, so both get `%rbp` = 0xd20.
- **`a` and `y` are the same 8 bytes.** Both are at `-0x8(%rbp)`, which is 0xd18. `assign` left 40 there and nothing erased it.
- **`call adder` overwrote the old return address.** 0x158 replaced 0x153 at 0xd28.
- **0x2a is 42.** `adder` returns it in `%rax`.

## Back to main

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — main prints and returns</p>

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x158 | 0xd30 | 0xd40 | |
| 0x158 | `mov %rax,-0x8(%rbp)` | | | | |
| 0x15c | `mov -0x8(%rbp),%rax` | | | | |
| 0x160 | `mov %rax,%rsi` | | | | |
| 0x163 | `mov $0x402004,%rdi` | | | | |
| 0x168 | `mov $0x0,%rax` | | | | |
| 0x16d | `call printf` | | | | |
| 0x172 | `mov $0x0,%rax` | | | | |
| 0x177 | `leave` | | | | |
| 0x178 | `ret` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x158 | 0xd30 | 0xd40 | |
| 0x158 | `mov %rax,-0x8(%rbp)` | 0x15c | | | M[0xd38] = 0x2a (`x`) |
| 0x15c | `mov -0x8(%rbp),%rax` | 0x160 | | | `%rax` = 0x2a |
| 0x160 | `mov %rax,%rsi` | 0x163 | | | `%rsi` = 0x2a |
| 0x163 | `mov $0x402004,%rdi` | 0x168 | | | `%rdi` = 0x402004 |
| 0x168 | `mov $0x0,%rax` | 0x16d | | | `%rax` = 0 |
| 0x16d | `call printf` | 0x172 | | | prints `x is: 42`, `%rax` = 9 |
| 0x172 | `mov $0x0,%rax` | 0x177 | | | `%rax` = 0 |
| 0x177 | `leave` | 0x178 | 0xd48 | 0x830 | |
| 0x178 | `ret` | (C library) | 0xd50 | | |

</details>

</div>

- **`printf` takes two arguments.** The first is the format string, so `%rdi` gets its address, 0x402004. The second is `x`, so `%rsi` gets 0x2a.
- **`%rax` = 0 tells `printf` there are no floating-point arguments.** `printf` can take any number of arguments, so it needs to be told.
- **We don't trace inside `printf`.** The `call printf` row is the whole call. `printf` pushes its own frame below `main`'s, prints, and returns to 0x172. It returns the number of characters it printed (9) in `%rax`, and `main` ignores it.
- **`printf`'s code isn't in our file.** Earlier in the course, `nm` listed `printf` as `U`, for undefined. The linker connects the call to the C library. That is the `@plt` you see after `printf` in `objdump`.
- **`leave` sets `%rsp` to 0xd40, then pops 0x830 into `%rbp`.** `%rbp` is back where it started. `ret` pops the return address at 0xd48, which goes back into the C library code that called `main`.

## The Bug

- **Local variables are not set to 0.** An uninitialized local holds whatever was in its slot last.
- **The stack keeps old values.** `assign`'s `y` was still at 0xd18 when `adder` read `a` from the same place.
- **It only works by accident.** Reading an uninitialized variable is undefined behavior. With `-O1`, the compiler drops the stack entirely and the program prints `x is: 2` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFzc2lnbih2b2lkKSB7XG4gICAgbG9uZyB5ID0gNDA7XG4gICAgcmV0dXJuIHk7XG59XG5cbmxvbmcgYWRkZXIodm9pZCkge1xuICAgIGxvbmcgYTtcbiAgICByZXR1cm4gYSArIDI7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHg7XG4gICAgYXNzaWduKCk7XG4gICAgeCA9IGFkZGVyKCk7XG4gICAgcHJpbnRmKFwieCBpczogJWxkXFxuXCIsIHgpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)). `gcc` will warn you if you ask:

```console
$ gcc -Wall -o prog prog.c
prog.c: In function 'adder':
prog.c:10:14: warning: 'a' is used uninitialized [-Wuninitialized]
   10 |     return a + 2;
      |            ~~^~~
```

## In the ASM Visualizer

[Open the program in the ASM Visualizer's Function Mode](https://asm.diveintosystems.org/function/x86_64/%20%20.section%20.rodata%0A.LC0%3A%0A%20%20.string%20%22x%20is%3A%20%25ld%5Cn%22%0A%0A%20%20.text%0A%20%20.globl%09assign%0A%20%20.type%09assign%2C%20%40function%0A%0Aassign%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%2440%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09assign%2C%20.-assign%0A%0A%20%20.globl%09adder%0A%20%20.type%09adder%2C%20%40function%0A%0Aadder%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20addq%20%242%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09adder%2C%20.-adder%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20subq%20%2416%2C%20%25rsp%0A%20%20call%20assign%0A%20%20call%20adder%0A%20%20movq%20%25rax%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rsi%0A%20%20movq%20%24.LC0%2C%20%25rdi%0A%20%20movq%20%240%2C%20%25rax%0A%20%20call%20printf%0A%20%20movq%20%240%2C%20%25rax%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A) and click **Run Code**. Step with **Next** and watch `RSP` and `RBP` in the register table and the arrows in the Stack Content table.

- **The code addresses match ours** up to the `printf` setup, so `call assign` pushes 0x401153 and `call adder` pushes 0x401158, just like our trace. The stack addresses are different, but they change by the same amounts.
- **At `adder`'s `movq -8(%rbp), %rax`,** find `%rbp - 8` in the Stack Content table: it still holds `0x28`.
- **`call printf` runs as one step.** The visualizer doesn't show what `printf` prints, so look for `x` on the stack instead: it holds `0x2A`.


## Try It

Use the trace above.

1. If `assign` set `y = 100` instead, what would the program print?
2. Right after `call printf` returns, what is at 0xd28? Is 0x28 still at 0xd18?
3. Suppose `main` called `adder()` before `assign()`. What would it print?

<details class="code-example" markdown="1">
<summary>Show answers</summary>

1. `x is: 102`. `adder` reads whatever `assign` left at 0xd18.
2. 0xd28 holds 0x172, the return address `call printf` pushed. 0xd18 is inside `printf`'s frame, so it was probably overwritten. We can't tell without tracing `printf`.
3. We can't tell. `adder` would read whatever was at 0xd18 before `main` ran, plus 2. That is why the compiler warns about it.

</details>



Next time you'll trace programs like this one in pairs.
