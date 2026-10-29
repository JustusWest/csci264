---
title: "25 Functions in Assembly"
published: true
---

[*Reading: Dive into Systems §7.5 and §7.5.1*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Today we look at functions in Assembly: how one function calls another, how it gets back, and how arguments and return values are passed. We'll learn each piece by writing a function ourselves.

Three registers run a function call:

| Register | Holds |
|---|---|
| `%rip` | the address of the next instruction |
| `%rsp` | the top of the stack |
| `%rbp` | the base of the current function's frame |

Recall from earlier in the course that each function call gets its own **stack frame**:

![A stack with a main frame (a, b, res) below a max frame (x, y, bigger)]({{ "/figures/ch7/stackFrame.png" | relative_url }})

Now we can say exactly where a frame is: it is the addresses from `%rsp` to `%rbp`.

Here is the function we'll write ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFkZGVyMihsb25nIGEpIHtcbiAgICByZXR1cm4gYSArIDI7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHggPSA0MDtcbiAgICB4ID0gYWRkZXIyKHgpO1xuICAgIHByaW50ZihcInggaXM6ICVsZFxcblwiLCB4KTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

```c
long adder2(long a) {
    return a + 2;
}

int main(void) {
    long x = 40;
    x = adder2(x);
    printf("x is: %ld\n", x);
    return 0;
}
```

We'll leave out the `printf`, since the visualizer doesn't show what it prints.

So far we've used the ASM Visualizer's Arithmetic Mode, which writes the function setup for you and hides it. Today we use **Function Mode**, which hides nothing. You write each function yourself, from its label to its `ret`, plus a `main` that calls it. The lines that start with `.` (`.globl`, `.type`, `.size`) tell the assembler where each function begins and ends. They don't run. Open a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64) to follow along.

## call, ret, and leave

Programs can't write to `%rip` directly. `call` and `ret` change it for us:

| Instruction | Translation | Same as |
|---|---|---|
| `call F` | Save where to come back to, then jump to F | `push` the address of the next instruction<br>`%rip` = F |
| `ret` | Go back to the saved address | `pop` into `%rip` |
| `leave` | Throw away this function's frame | `mov %rbp,%rsp`<br>`pop %rbp` |

- **The saved address is the return address,** the instruction right after the `call`.
- **The return address lives on the stack.** That is how `ret` knows where to go, and why every function has to leave `%rsp` where it found it. If `%rsp` is off by 8 when `ret` runs, it pops the wrong value into `%rip`.
- **`leave` undoes `push %rbp` and `mov %rsp,%rbp`.** A function that moved `%rsp` with `sub` needs `mov %rbp,%rsp` to throw that space away before it pops. A function that never moved `%rsp` can just `pop %rbp`.

So every function has the same four parts:

1. **A label** with the function's name. `call` jumps there.
2. **Set up the frame:** `push %rbp`, then `mov %rsp, %rbp`.
3. **The body.**
4. **Clean up and return:** `pop %rbp` and `ret`, or `leave` and `ret` if the function moved `%rsp`.

Let's write those parts first, with no body yet. `main` makes room for `x` with `sub`, so it ends with `leave`.

<details class="code-example" markdown="1">
<summary>Show code: step 1, the skeleton</summary>

```text
  .text
  .globl	adder2
  .type	adder2, @function

adder2:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start adder2's frame
                          # (the body goes here)
  pop %rbp                # restore main's %rbp
  ret                     # go back to main
  .size	adder2, .-adder2

  .globl	main
  .type	main, @function

main:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start main's frame
  sub $16, %rsp           # room for x
  call adder2             # adder2();
  leave                   # throw away main's frame
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09adder2%0A%20%20.type%09adder2%2C%20%40function%0A%0Aadder2%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20adder2's%20frame%0A%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20(the%20body%20goes%20here)%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20back%20to%20main%0A%20%20.size%09adder2%2C%20.-adder2%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20main's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20x%0A%20%20call%20adder2%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20adder2()%3B%0A%20%20leave%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20throw%20away%20main's%20frame%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

</details>

## How %rip Moves

Run the skeleton and step through it with **Next**. The Address column shows where each instruction is in memory. The register table doesn't show `%rip`, but the orange arrow marks the instruction it points to.

- **x86-64 instructions are different sizes, from 1 to 15 bytes.** After each instruction, `%rip` goes up by that instruction's length. `main`'s `push %rbp` at 0x40110C is 1 byte, so the next instruction is at 0x40110D.
- **`call adder2` is at 0x401114 and is 5 bytes long,** so the instruction after it, `leave`, is at 0x401119. `call` pushes 0x401119 and sets `%rip` to 0x401106, the first instruction of `adder2`. Watch 0x401119 appear at the top of the stack.
- **`ret` pops 0x401119 into `%rip`,** so `main` picks up at `leave`.
- **`main`'s `leave` and `ret` work the same way.** `leave` throws away `main`'s frame, and `ret` returns to the code that called `main`.

## Parameters and Return Values

The first six arguments go in registers, in this order:

| Parameter | Register |
|---|---|
| 1 | `%rdi` |
| 2 | `%rsi` |
| 3 | `%rdx` |
| 4 | `%rcx` |
| 5 | `%r8` |
| 6 | `%r9` |
| 7 and on | on the stack |

The return value goes in `%rax`.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — which register?</p>

Assume `x`, `y`, and `z` are `long` variables. At the `call`, where is each argument?

<div class="board-setup" markdown="1">

| Call | Where are the arguments? |
|---|---|
| `f(x)` | ? |
| `g(y, x)` | ? |
| `h(1, 2, 3, 4, 5, 6, 7)` | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Call | Where are the arguments? |
|---|---|
| `f(x)` | `%rdi` = `x` |
| `g(y, x)` | `%rdi` = `y`, `%rsi` = `x` |
| `h(1, 2, 3, 4, 5, 6, 7)` | `%rdi` = 1, `%rsi` = 2, `%rdx` = 3, `%rcx` = 4, `%r8` = 5, `%r9` = 6, and 7 on the stack |

The register goes by the argument's position, not by its name. In `g(y, x)`, `y` goes in `%rdi` because it is first.

</details>

</div>

Now we can write the body. `a` arrives in `%rdi`, and the result goes back in `%rax`. `main` loads `x` into `%rdi` before the `call`, and stores `%rax` back into `x` after it.

- **The caller loads the registers before the `call`.**
- **The compiler copies each parameter to the stack right away** when it isn't optimizing. That is the `mov %rdi, -8(%rbp)` at the top of the functions we read last time. We don't need to: `a` can stay in `%rdi`.
- **An `int` parameter uses part of the register**, like `%edi` for the first one.

<details class="code-example" markdown="1">
<summary>Show code: step 2, the body</summary>

```text
  .text
  .globl	adder2
  .type	adder2, @function

adder2:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start adder2's frame
  mov %rdi, %rax          # result = a
  add $2, %rax            # result = a + 2
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax
  .size	adder2, .-adder2

  .globl	main
  .type	main, @function

main:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start main's frame
  sub $16, %rsp           # room for x
  movq $40, -8(%rbp)      # long x = 40;
  mov -8(%rbp), %rdi      # first argument: x
  call adder2             # %rax = adder2(x)
  mov %rax, -8(%rbp)      # x = adder2(x);
  mov $0, %rax            # return 0;
  leave                   # throw away main's frame
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09adder2%0A%20%20.type%09adder2%2C%20%40function%0A%0Aadder2%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20adder2's%20frame%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20a%0A%20%20add%20%242%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20a%20%2B%202%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09adder2%2C%20.-adder2%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20main's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20x%0A%20%20movq%20%2440%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20long%20x%20%3D%2040%3B%0A%20%20mov%20-8(%25rbp)%2C%20%25rdi%20%20%20%20%20%20%23%20first%20argument%3A%20x%0A%20%20call%20adder2%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20adder2(x)%0A%20%20mov%20%25rax%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20x%20%3D%20adder2(x)%3B%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%200%3B%0A%20%20leave%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20throw%20away%20main's%20frame%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). When it finishes, `%rax` is 0, `main`'s return value, and `x` on the stack is `0x2A`, which is 42.

</details>

### Saving Registers

There is only one set of registers, and every function uses them. So x86-64 has rules for who has to put a register back:

- **Callee saved:** `%rbx`, `%rbp`, and `%r12` through `%r15`. A function that changes one must restore it before it returns. That is why every function pushes `%rbp` at the start and pops it at the end.
- **Caller saved:** all the others, including `%rax` and the six parameter registers. Any `call` can change them. If the caller still needs a value in one, it has to save it first.

## The Stack During a Call

Here is the stack in the visualizer at the deepest point, right after `adder2`'s `mov %rsp, %rbp`. We write just the last three hex digits of each address, so 0x1FFF000AD0 is AD0 and 0x40112C is 12C. Like the visualizer's Stack Content table, the lowest address is at the top:

| Address | Value | |
|---|---|---|
| AD0 | AF0 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| AD8 | 12C (return address) | |
| AE0 | (unused) | ← `main`'s `%rsp` |
| AE8 | 0x28 (`x`) | |
| AF0 | 0x1 (`main`'s caller's `%rbp`) | ← `main`'s `%rbp` |

- **`adder2`'s frame sits right on top of `main`'s.** Each call adds a frame at lower addresses.
- **The return address is always at `8(%rbp)`**, and the caller's `%rbp` is at `0(%rbp)`.
- **`a` is not on the stack.** C passes arguments by value: `adder2` gets a copy of `x` in `%rdi`, and our `adder2` never stores it.
- **After `ret`, `%rsp` and `%rbp` are back to AE0 and AF0**, exactly what `main` had before the call.
- **Nothing was erased.** AF0 and 12C are still at AD0 and AD8 after `ret`. They're just past the top of the stack now, and the next call will overwrite them.

## Try It

Write this function in Function Mode, with a `main` that calls `scale(5, 3, 1)`:

```c
long scale(long x, long m, long b) {
    return x * m + b;
}
```

Use the four parts from above. When it finishes, `%rax` should be 16.

<details class="code-example" markdown="1">
<summary>Show code: scale</summary>

```text
  .text
  .globl	scale
  .type	scale, @function

scale:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start scale's frame
  mov %rdi, %rax          # result = x
  imul %rsi, %rax         # result = x * m
  add %rdx, %rax          # result = x * m + b
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax
  .size	scale, .-scale

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  mov $5, %rdi            # x = 5
  mov $3, %rsi            # m = 3
  mov $1, %rdx            # b = 1
  call scale              # %rax = scale(5, 3, 1)
  pop %rbp
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09scale%0A%20%20.type%09scale%2C%20%40function%0A%0Ascale%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20scale's%20frame%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20x%0A%20%20imul%20%25rsi%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20x%20*%20m%0A%20%20add%20%25rdx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20x%20*%20m%20%2B%20b%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09scale%2C%20.-scale%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%245%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%205%0A%20%20mov%20%243%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20m%20%3D%203%0A%20%20mov%20%241%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20b%20%3D%201%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20scale(5%2C%203%2C%201)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

- **`x`, `m`, and `b` arrive in `%rdi`, `%rsi`, and `%rdx`,** the first three parameter registers.
- **`main` can load the three registers in any order.** Only which register each value ends up in matters.

</details>

Next time we trace the book's `assign` and `adder` program, one frame at a time.
