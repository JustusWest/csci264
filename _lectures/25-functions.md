---
title: "25 Functions in Assembly"
published: true
---

[*Reading: Dive into Systems §7.5 and §7.5.1*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Today we look at functions in Assembly: how one function calls another, how it gets back, and how arguments and return values are passed.

Three registers run a function call:

| Register | Holds |
|---|---|
| `%rip` | the address of the next instruction |
| `%rsp` | the top of the stack |
| `%rbp` | the base of the current function's frame |

Recall from earlier in the course that each function call gets its own **stack frame**:

![A stack with a main frame (a, b, res) below a max frame (x, y, bigger)]({{ "/figures/ch7/stackFrame.png" | relative_url }})

Now we can say exactly where a frame is: it is the addresses from `%rsp` to `%rbp`.

## How %rip Moves

Here is `adder2` again ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFkZGVyMihsb25nIGEpIHtcbiAgICByZXR1cm4gYSArIDI7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHggPSA0MDtcbiAgICB4ID0gYWRkZXIyKHgpO1xuICAgIHByaW50ZihcInggaXM6ICVsZFxcblwiLCB4KTtcbiAgICByZXR1cm4gMDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

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

```text
adder2:
  401126:  55                      push   %rbp
  401127:  48 89 e5                mov    %rsp,%rbp
  40112a:  48 89 7d f8             mov    %rdi,-0x8(%rbp)
  40112e:  48 8b 45 f8             mov    -0x8(%rbp),%rax
  401132:  48 83 c0 02             add    $0x2,%rax
  401136:  5d                      pop    %rbp
  401137:  c3                      ret
```

Recall that x86-64 instructions are different sizes, from 1 to 15 bytes. After each instruction, `%rip` goes up by that instruction's length. The byte column tells you how far.

## call, ret, and leave

Here is `main`, up to the call and back:

```text
main:
  401138:  55                      push   %rbp
  401139:  48 89 e5                mov    %rsp,%rbp
  40113c:  48 83 ec 10             sub    $0x10,%rsp
  401140:  48 c7 45 f8 28 00 00 00 movq   $0x28,-0x8(%rbp)    # long x = 40;
  401148:  48 8b 45 f8             mov    -0x8(%rbp),%rax     # x = adder2(x);
  40114c:  48 89 c7                mov    %rax,%rdi
  40114f:  e8 d2 ff ff ff          call   adder2
  401154:  48 89 45 f8             mov    %rax,-0x8(%rbp)
  ...
  401178:  c9                      leave
  401179:  c3                      ret
```

Programs can't write to `%rip` directly. `call` and `ret` change it for us:

| Instruction | Translation | Same as |
|---|---|---|
| `call F` | Save where to come back to, then jump to F | `push` the address of the next instruction<br>`%rip` = F |
| `ret` | Go back to the saved address | `pop` into `%rip` |
| `leave` | Throw away this function's frame | `mov %rbp,%rsp`<br>`pop %rbp` |

- **The saved address is the return address.** For this call it is 0x401154, the instruction right after the `call`.
- **The return address lives on the stack.** That is how `ret` knows where to go, and why every function has to leave `%rsp` where it found it. If `%rsp` is off by 8 when `ret` runs, it pops the wrong value into `%rip`.
- **`leave` undoes `push %rbp` and `mov %rsp,%rbp`.** `main` moved `%rsp` with `sub $0x10,%rsp`, so it needs `mov %rbp,%rsp` to throw that space away before it pops. `adder2` never moved `%rsp`, so `pop %rbp` is enough.

## Tracing a Call

Like the book, we drop the leading `0x401` from code addresses, so 0x401154 is just 0x154. `main` has already set up its frame: `%rbp` is 0xd40 and `%rsp` is 0xd30 (16 bytes reserved by `sub $0x10,%rsp`). Each row shows the values *after* that instruction runs. A blank means no change.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — main calls adder2</p>

<div class="board-setup" markdown="1">

| Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|
| (start) | 0x140 | 0xd30 | 0xd40 | |
| `movq $0x28,-0x8(%rbp)` | | | | |
| `mov -0x8(%rbp),%rax` | | | | |
| `mov %rax,%rdi` | | | | |
| `call adder2` | | | | |
| `push %rbp` | | | | |
| `mov %rsp,%rbp` | | | | |
| `mov %rdi,-0x8(%rbp)` | | | | |
| `mov -0x8(%rbp),%rax` | | | | |
| `add $0x2,%rax` | | | | |
| `pop %rbp` | | | | |
| `ret` | | | | |
| `mov %rax,-0x8(%rbp)` | | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|
| (start) | 0x140 | 0xd30 | 0xd40 | |
| `movq $0x28,-0x8(%rbp)` | 0x148 | | | M[0xd38] = 0x28 (`x`) |
| `mov -0x8(%rbp),%rax` | 0x14c | | | `%rax` = 0x28 |
| `mov %rax,%rdi` | 0x14f | | | `%rdi` = 0x28 |
| `call adder2` | **0x126** | 0xd28 | | M[0xd28] = 0x154 |
| `push %rbp` | 0x127 | 0xd20 | | M[0xd20] = 0xd40 |
| `mov %rsp,%rbp` | 0x12a | | 0xd20 | |
| `mov %rdi,-0x8(%rbp)` | 0x12e | | | M[0xd18] = 0x28 (`a`) |
| `mov -0x8(%rbp),%rax` | 0x132 | | | `%rax` = 0x28 |
| `add $0x2,%rax` | 0x136 | | | `%rax` = 0x2a |
| `pop %rbp` | 0x137 | 0xd28 | 0xd40 | |
| `ret` | **0x154** | 0xd30 | | |
| `mov %rax,-0x8(%rbp)` | 0x158 | | | M[0xd38] = 0x2a (`x`) |

</details>

</div>

Here is the stack right after `mov %rdi,-0x8(%rbp)`, the deepest point. Like the Stack Content table in the ASM Visualizer, the lowest address is at the top:

| Address | Value | |
|---|---|---|
| 0xd18 | 0x28 (`a`) | |
| 0xd20 | 0xd40 (`main`'s `%rbp`) | ← `%rsp`, `%rbp` |
| 0xd28 | 0x154 (return address) | |
| 0xd30 | | ← `main`'s `%rsp` |
| 0xd38 | 0x28 (`x`) | |
| 0xd40 | (saved by `main`) | ← `main`'s `%rbp` |

- **`adder2`'s frame sits right on top of `main`'s.** Each call adds a frame at lower addresses.
- **The return address is always at `8(%rbp)`**, and the caller's `%rbp` is at `0(%rbp)`. Every function compiled without optimization has this same layout.
- **`a` and `x` are both 40, in two different places.** C passes arguments by value: `adder2` gets a copy of `x` in `%rdi`, and puts that copy in its own frame.
- **After `ret`, `%rsp` and `%rbp` are back to 0xd30 and 0xd40**, exactly what `main` had before the call.
- **Nothing was erased.** 0xd40, 0x154, and 0x28 are still at 0xd20, 0xd28, and 0xd18. They're just past the top of the stack now, and the next call will overwrite them.

`main` ends the same way, with `leave` and `ret`. `leave` sets `%rsp` to 0xd40 and pops `main`'s caller's `%rbp`, so `%rsp` ends at 0xd48. `ret` pops the return address at 0xd48, which goes back into the C library code that called `main`.



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

- **The caller loads the registers before the `call`.** In `main` above, `mov %rax,%rdi` puts `x` in the first parameter register.
- **Without optimization, the callee copies them to its frame right away.** That is the `mov %rdi,-0x8(%rbp)` at the top of `adder2`, and the parameter copies we read last time.
- **An `int` parameter uses part of the register**, like `%edi` for the first one.

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — which register?</p>

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

### Saving Registers

There is only one set of registers, and every function uses them. So x86-64 has rules for who has to put a register back:

- **Callee saved:** `%rbx`, `%rbp`, and `%r12` through `%r15`. A function that changes one must restore it before it returns. That is why every function pushes `%rbp` at the start and pops it at the end.
- **Caller saved:** all the others, including `%rax` and the six parameter registers. Any `call` can change them. If the caller still needs a value in one, it has to save it first.


## In the ASM Visualizer

So far we've used the ASM Visualizer's Arithmetic Mode, which writes the function setup for you and hides it. **Function Mode** hides nothing. You write each function yourself, from its label to its `ret`, plus a `main` that calls them. It starts you with an empty `main` to fill in. The lines that start with `.` (`.globl`, `.type`, `.size`) are part of that starting code: they tell the assembler where each function begins and ends, and they don't run.

[Open adder2 in Function Mode](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09adder2%0A%20%20.type%09adder2%2C%20%40function%0A%0Aadder2%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%25rdi%2C%20-8(%25rbp)%0A%20%20mov%20-8(%25rbp)%2C%20%25rax%0A%20%20add%20%242%2C%20%25rax%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09adder2%2C%20.-adder2%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2416%2C%20%25rsp%0A%20%20movq%20%2440%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20long%20x%20%3D%2040%3B%0A%20%20mov%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20x%20%3D%20adder2(x)%3B%0A%20%20mov%20%25rax%2C%20%25rdi%0A%20%20call%20adder2%0A%20%20mov%20%25rax%2C%20-8(%25rbp)%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%200%3B%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A) and click **Run Code**. It is the code from above, without the `printf`. Step with **Next**:

- **The Address column works like a listing.** The register table doesn't show `%rip`, but the orange arrow marks the next instruction, and its address is what `%rip` holds.
- **At `call adder2`, the return address appears on the stack.** It is 0x401134, the address of the `mov` right after the `call`.
- **`RSP` and `RBP` move by the same amounts as in our trace:** down 8 on `call`, down 8 on `push %rbp`, and back up 8 each on `pop` and `ret`. The addresses themselves are different from ours.



## Try It

```c
long scale(long x, long m, long b) {
    return x * m + b;
}

int main(void) {
    long y = 5;
    long z = scale(y, 3, 1);
    printf("z is: %ld\n", z);
    return 0;
}
```

Here is the start of `main` and all of `scale`, without optimization:

```text
main:
    push %rbp
    mov %rsp, %rbp
    sub $0x10, %rsp
    movq $0x5, -16(%rbp)
    mov -16(%rbp), %rax
    mov $0x1, %rdx
    mov $0x3, %rsi
    mov %rax, %rdi
    call scale
    mov %rax, -8(%rbp)
    ...

scale:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    mov %rdx, -24(%rbp)
    mov -8(%rbp), %rax
    imul -16(%rbp), %rax
    mov %rax, %rdx
    mov -24(%rbp), %rax
    add %rdx, %rax
    pop %rbp
    ret
```

Just before the `call`, `%rsp` is 0xd30 and `%rbp` is 0xd40. The `mov` right after the `call` is at address 0x171.

1. At the `call`, what is in `%rdi`, `%rsi`, and `%rdx`? Which one did the compiler load first?
2. When `scale`'s first instruction runs, what is `%rsp`, and what is stored there?
3. After `mov %rsp, %rbp` in `scale`, where are `x`, `m`, and `b`? Where is the return address?
4. What does `scale` leave in `%rax`, and where does `main` put it?

<details class="code-example" markdown="1">
<summary>Show answers</summary>

1. `%rdi` = 5 (`y`), `%rsi` = 3, `%rdx` = 1. The compiler loaded the third argument first. The order doesn't matter, only which register each one ends up in.
2. `%rsp` is 0xd28, and 0x171 is stored there. `call` pushed the return address.
3. `%rbp` is 0xd20. `x` is at `-8(%rbp)` (0xd18), `m` at `-16(%rbp)` (0xd10), and `b` at `-24(%rbp)` (0xd08). The return address is at `8(%rbp)` (0xd28), as in every frame.
4. `%rax` = 5 × 3 + 1 = 16. `main` stores it in `z`, at `-8(%rbp)` in `main`'s frame.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIHNjYWxlKGxvbmcgeCwgbG9uZyBtLCBsb25nIGIpIHtcbiAgICByZXR1cm4geCAqIG0gKyBiO1xufVxuXG5pbnQgbWFpbih2b2lkKSB7XG4gICAgbG9uZyB5ID0gNTtcbiAgICBsb25nIHogPSBzY2FsZSh5LCAzLCAxKTtcbiAgICBwcmludGYoXCJ6IGlzOiAlbGRcXG5cIiwgeik7XG4gICAgcmV0dXJuIDA7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19). The compiler writes `mov $0x1,%edx` and `mov $0x3,%esi`. Writing the 32-bit half of a register clears the top half, so it is the same as using `%rdx` and `%rsi`, and one byte shorter.

</details>



Next time we trace the book's `assign` and `adder` program, one frame at a time.
