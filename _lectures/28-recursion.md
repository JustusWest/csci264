---
title: "28 Recursion"
published: true
---

[*Reading: Dive into Systems §7.6*](https://diveintosystems.org/book/C7-x86_64/recursion.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

**Recursion is not on Midterm 2.**

Last time we wrote functions that call other functions.


A **recursive function** is a function that calls itself. Every recursive function has two parts:

- **base case** — an input small enough to answer right away, with no call
- **recursive step** — call the function on a smaller input, then use the result

Each call has to get closer to the base case. If it never gets there, the calls never stop.

Here is the book's example. Both functions add up the numbers from `n` down to 1. `sumDown` uses a loop, and `sumr` uses recursion. We use `long` instead of `int` so the assembly uses the 64-bit registers:

```c
long sumDown(long n) {
    long total = 0;
    long i = n;
    while (i > 0) {
        total += i;
        i--;
    }
    return total;
}

long sumr(long n) {
    if (n <= 0) {
        return 0;
    }
    return n + sumr(n - 1);
}
```

- **Base case:** `n <= 0`. The sum of no numbers is 0.
- **Recursive step:** the sum from `n` down to 1 is `n` plus the sum from `n - 1` down to 1.

Here is `sumr(3)`, one call at a time:

```text
sumr(3) = 3 + sumr(2)
        = 3 + (2 + sumr(1))
        = 3 + (2 + (1 + sumr(0)))
        = 3 + (2 + (1 + 0))
        = 6
```

`sumr(3)` can't add anything until `sumr(2)` returns, and `sumr(2)` has to wait for `sumr(1)`. The calls go all the way down to the base case first. The additions happen on the way back.


## The Loop Version

Let's write `sumDown` first, in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64). It's a loop like the ones from earlier in the course, counting down. `n` arrives in `%rdi`. We keep `total` in `%rax`, since it's the return value, and `i` in `%rcx`.

<details class="code-example" markdown="1">
<summary>Show code: step 1, sumDown</summary>

```text
  .text
  .globl	sumDown
  .type	sumDown, @function

sumDown:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start sumDown's frame
  mov $0, %rax            # total = 0
  mov %rdi, %rcx          # i = n
  jmp down_test           # go to the test
down_loop:
  add %rcx, %rax          # total += i
  sub $1, %rcx            # i--
down_test:
  cmp $0, %rcx            # compare i with 0
  jg down_loop            # if i > 0, go back
  pop %rbp                # restore main's %rbp
  ret                     # return total in %rax
  .size	sumDown, .-sumDown

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  mov $3, %rdi            # n = 3
  call sumDown            # %rax = sumDown(3)
  pop %rbp
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumDown%0A%20%20.type%09sumDown%2C%20%40function%0A%0AsumDown%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sumDown's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20total%20%3D%200%0A%20%20mov%20%25rdi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%20n%0A%20%20jmp%20down_test%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Adown_loop%3A%0A%20%20add%20%25rcx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20total%20%2B%3D%20i%0A%20%20sub%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i--%0Adown_test%3A%0A%20%20cmp%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%200%0A%20%20jg%20down_loop%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3E%200%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20total%20in%20%25rax%0A%20%20.size%09sumDown%2C%20.-sumDown%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%243%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n%20%3D%203%0A%20%20call%20sumDown%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumDown(3)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 6.

</details>

- **The test is at the bottom,** so `jg` jumps back while the C condition, `i > 0`, is true.
- **One frame, whatever `n` is.** The loop reuses the same two registers on every pass.

## The Recursive Version

`sumr` has the same four parts as any function. The body is an `if`: either the base case, or the recursive step. The recursive step puts `n - 1` in `%rdi`, calls `sumr`, then adds `n` to the result.

### First Try

`n` arrives in `%rdi`, but the call needs `%rdi` for `n - 1`. So let's keep `n` in `%rcx`:

<details class="code-example" markdown="1">
<summary>Show code: step 2, first try</summary>

```text
  .text
  .globl	sumr
  .type	sumr, @function

sumr:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start this call's frame
  cmp $0, %rdi            # compare n with 0
  jg sumr_step            # if n > 0, go to the recursive step
  mov $0, %rax            # base case: return 0;
  jmp sumr_done
sumr_step:
  mov %rdi, %rcx          # keep n in %rcx
  sub $1, %rdi            # argument: n - 1
  call sumr               # %rax = sumr(n - 1)
  add %rcx, %rax          # result = n + sumr(n - 1)
sumr_done:
  pop %rbp                # restore the caller's %rbp
  ret                     # return result in %rax
  .size	sumr, .-sumr

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  mov $3, %rdi            # n = 3
  call sumr               # %rax = sumr(3)
  pop %rbp
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumr%0A%20%20.type%09sumr%2C%20%40function%0A%0Asumr%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20this%20call's%20frame%0A%20%20cmp%20%240%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20compare%20n%20with%200%0A%20%20jg%20sumr_step%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20n%20%3E%200%2C%20go%20to%20the%20recursive%20step%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20base%20case%3A%20return%200%3B%0A%20%20jmp%20sumr_done%0Asumr_step%3A%0A%20%20mov%20%25rdi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20keep%20n%20in%20%25rcx%0A%20%20sub%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20argument%3A%20n%20-%201%0A%20%20call%20sumr%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumr(n%20-%201)%0A%20%20add%20%25rcx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20n%20%2B%20sumr(n%20-%201)%0Asumr_done%3A%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20the%20caller's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09sumr%2C%20.-sumr%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%243%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n%20%3D%203%0A%20%20call%20sumr%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumr(3)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

</details>

Run it. `%rax` ends at 3, not 6. Try `sumr(5)`: it returns 5.

- **There is only one `%rcx`.** Every call runs `mov %rdi, %rcx`, so each call overwrites the `n` of the call before it. The last call to reach the recursive step is `sumr(1)`, so by the time the calls return, `%rcx` is 1, and every `add` adds 1.
- **This is the caller-saved rule again.** Last time, in `dist2`, `sq` *might* have changed `%rsi`. Here the callee is `sumr` itself, so it *will* change `%rcx` and `%rdi`.

### Saving n

Each call has to save its own `n` in its own frame. Make room with `sub`, store `n` at `-8(%rbp)`, add it back after the call, and end with `leave`:

<details class="code-example" markdown="1">
<summary>Show code: step 3, saving n</summary>

```text
  .text
  .globl	sumr
  .type	sumr, @function

sumr:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start this call's frame
  sub $16, %rsp           # room for n
  mov %rdi, -8(%rbp)      # save n: the call will change %rdi
  cmp $0, %rdi            # compare n with 0
  jg sumr_step            # if n > 0, go to the recursive step
  mov $0, %rax            # base case: return 0;
  jmp sumr_done
sumr_step:
  sub $1, %rdi            # argument: n - 1
  call sumr               # %rax = sumr(n - 1)
  add -8(%rbp), %rax      # result = n + sumr(n - 1)
sumr_done:
  leave                   # throw away this call's frame
  ret                     # return result in %rax
  .size	sumr, .-sumr

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  mov $3, %rdi            # n = 3
  call sumr               # %rax = sumr(3)
  pop %rbp
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumr%0A%20%20.type%09sumr%2C%20%40function%0A%0Asumr%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20this%20call's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20n%0A%20%20mov%20%25rdi%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20save%20n%3A%20the%20call%20will%20change%20%25rdi%0A%20%20cmp%20%240%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20compare%20n%20with%200%0A%20%20jg%20sumr_step%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20n%20%3E%200%2C%20go%20to%20the%20recursive%20step%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20base%20case%3A%20return%200%3B%0A%20%20jmp%20sumr_done%0Asumr_step%3A%0A%20%20sub%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20argument%3A%20n%20-%201%0A%20%20call%20sumr%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumr(n%20-%201)%0A%20%20add%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20result%20%3D%20n%20%2B%20sumr(n%20-%201)%0Asumr_done%3A%0A%20%20leave%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20throw%20away%20this%20call's%20frame%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09sumr%2C%20.-sumr%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%243%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n%20%3D%203%0A%20%20call%20sumr%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumr(3)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 6.

</details>

- **Every call keeps its `n` at `-8(%rbp)`.** Same offset, but each call has its own `%rbp`, so each `n` is at a different address.
- **This is what `gcc` makes without optimization** ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6Imxvbmcgc3Vtcihsb25nIG4pIHtcbiAgICBpZiAobiA8PSAwKSB7XG4gICAgICAgIHJldHVybiAwO1xuICAgIH1cbiAgICByZXR1cm4gbiArIHN1bXIobiAtIDEpO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)).
## The Stack During sumr(3)

Here is the stack at the deepest point: inside `sumr(0)`, right after it saves `n`. See [the book](https://diveintosystems.org/book/C7-x86_64/recursion.html#_animation_observing_how_the_call_stack_changes) for a nice animation of the stack as `sumr` progresses.

| Address | Value | |
|---|---|---|
| A70 | (unused) | ← `%rsp` |
| A78 | 0 (`n`) | |
| A80 | AA0 (`sumr(1)`'s `%rbp`) | ← `%rbp`, `sumr(0)`'s frame |
| A88 | 12A (return address) | |
| A90 | (unused) | |
| A98 | 1 (`n`) | |
| AA0 | AC0 (`sumr(2)`'s `%rbp`) | ← `sumr(1)`'s `%rbp` |
| AA8 | 12A (return address) | |
| AB0 | (unused) | |
| AB8 | 2 (`n`) | |
| AC0 | AE0 (`sumr(3)`'s `%rbp`) | ← `sumr(2)`'s `%rbp` |
| AC8 | 12A (return address) | |
| AD0 | (unused) | |
| AD8 | 3 (`n`) | |
| AE0 | AF0 (`main`'s `%rbp`) | ← `sumr(3)`'s `%rbp` |
| AE8 | 140 (return address) | |
| AF0 | 0x1 (`main`'s caller's `%rbp`) | ← `main`'s `%rbp` |

- **Four calls, four frames.** Each one has its own saved `%rbp`, return address, and `n`.
- **Each frame is 32 bytes:** the return address, the saved `%rbp`, and the 16 bytes from `sub`.
- **Three of the return addresses are 12A.** That is `add -8(%rbp), %rax`, right after `call sumr`. Only `sumr(3)` returns to `main`, at 140.

### On the Way Back

| Call | `n` | `%rax` at its `ret` | Returns to |
|---|---|---|---|
| `sumr(0)` | 0 | 0 | 12A, in `sumr(1)` |
| `sumr(1)` | 1 | 0 + 1 = 1 | 12A, in `sumr(2)` |
| `sumr(2)` | 2 | 1 + 2 = 3 | 12A, in `sumr(3)` |
| `sumr(3)` | 3 | 3 + 3 = 6 | 140, in `main` |

- **The same `add` runs three times,** each time with a different `%rbp`, so `-8(%rbp)` is a different `n` each time: 1, then 2, then 3.
- **Each `leave` and `ret` throws away one frame.** After `sumr(3)` returns, `%rsp` and `%rbp` are both AF0 again, just as `main` had them.

## Loop or Recursion?

Both give the same answer. The difference is the stack. `sumDown` uses one frame. `sumr(n)` uses `n + 1` frames, and the stack is only so big (8 MB by default on Linux):

```console
$ ./sum
n: 1000000
sumDown: 500000500000
Segmentation fault (core dumped)
```

- **`sumr` ran out of stack.** A million frames at 32 bytes each is 32 MB. This is a **stack overflow**.
- **This is also what happens with no base case.** The calls never stop, and the program crashes the same way.
- **With `-O2`, `gcc` turns `sumr` back into a loop** ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6Imxvbmcgc3Vtcihsb25nIG4pIHtcbiAgICBpZiAobiA8PSAwKSB7XG4gICAgICAgIHJldHVybiAwO1xuICAgIH1cbiAgICByZXR1cm4gbiArIHN1bXIobiAtIDEpO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)): add `%rdi` to `%rax`, subtract 1, jump back. That's our `sumDown`, and the million works.
- **So why use recursion?** Some problems are much easier to write that way, like searching folders inside folders. You'll see more of them in later courses.

## Try It

The factorial of `n` is `n × (n - 1) × … × 1`, so 5! = 120. Here it is as a loop and with recursion:

```c
long factLoop(long n) {
    long result = 1;
    while (n > 1) {
        result *= n;
        n--;
    }
    return result;
}

long fact(long n) {
    if (n <= 1) {
        return 1;
    }
    return n * fact(n - 1);
}
```

1. Write `factLoop` in Function Mode, with a `main` that calls `factLoop(5)`.
2. Write `fact` the same way. Which value has to be saved across the call?
3. Both should leave 0x78 in `%rax`, which is 120. At the deepest point of `fact(5)`, how many frames are on the stack, counting `main`'s?

<details class="code-example" markdown="1">
<summary>Show code: factLoop</summary>

```text
factLoop:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start factLoop's frame
  mov $1, %rax            # result = 1
  jmp fl_test             # go to the test
fl_loop:
  imul %rdi, %rax         # result *= n
  sub $1, %rdi            # n--
fl_test:
  cmp $1, %rdi            # compare n with 1
  jg fl_loop              # if n > 1, go back
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax

main:
  push %rbp
  mov %rsp, %rbp
  mov $5, %rdi            # n = 5
  call factLoop           # %rax = factLoop(5)
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09factLoop%0A%20%20.type%09factLoop%2C%20%40function%0A%0AfactLoop%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20factLoop's%20frame%0A%20%20mov%20%241%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%201%0A%20%20jmp%20fl_test%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Afl_loop%3A%0A%20%20imul%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20result%20*%3D%20n%0A%20%20sub%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n--%0Afl_test%3A%0A%20%20cmp%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20compare%20n%20with%201%0A%20%20jg%20fl_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20n%20%3E%201%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09factLoop%2C%20.-factLoop%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%245%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n%20%3D%205%0A%20%20call%20factLoop%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20factLoop(5)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

- **`factLoop` can change `n` directly.** `%rdi` is a copy of the argument, and there's no call to save it from.

</details>

<details class="code-example" markdown="1">
<summary>Show code: fact</summary>

```text
fact:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start this call's frame
  sub $16, %rsp           # room for n
  mov %rdi, -8(%rbp)      # save n: the call will change %rdi
  cmp $1, %rdi            # compare n with 1
  jg fact_step            # if n > 1, go to the recursive step
  mov $1, %rax            # base case: return 1;
  jmp fact_done
fact_step:
  sub $1, %rdi            # argument: n - 1
  call fact               # %rax = fact(n - 1)
  imul -8(%rbp), %rax     # result = n * fact(n - 1)
fact_done:
  leave                   # throw away this call's frame
  ret                     # return result in %rax

main:
  push %rbp
  mov %rsp, %rbp
  mov $5, %rdi            # n = 5
  call fact               # %rax = fact(5)
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09fact%0A%20%20.type%09fact%2C%20%40function%0A%0Afact%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20this%20call's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20n%0A%20%20mov%20%25rdi%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20save%20n%3A%20the%20call%20will%20change%20%25rdi%0A%20%20cmp%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20compare%20n%20with%201%0A%20%20jg%20fact_step%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20n%20%3E%201%2C%20go%20to%20the%20recursive%20step%0A%20%20mov%20%241%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20base%20case%3A%20return%201%3B%0A%20%20jmp%20fact_done%0Afact_step%3A%0A%20%20sub%20%241%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20argument%3A%20n%20-%201%0A%20%20call%20fact%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20fact(n%20-%201)%0A%20%20imul%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%23%20result%20%3D%20n%20*%20fact(n%20-%201)%0Afact_done%3A%0A%20%20leave%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20throw%20away%20this%20call's%20frame%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09fact%2C%20.-fact%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%245%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20n%20%3D%205%0A%20%20call%20fact%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20fact(5)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A)

- **`n` has to be saved,** just like in `sumr`. The call to `fact` will change `%rdi`.
- **`imul` can read `n` straight from the stack,** the same way `add` did in `sumr`.
- **Six frames:** `main`, then `fact(5)`, `fact(4)`, `fact(3)`, `fact(2)`, and `fact(1)`. `fact(1)` is the base case, so it makes no call.

</details>

Next time we review for Midterm 2.
