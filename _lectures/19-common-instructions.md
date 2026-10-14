---
title: "19 Common Assembly Instructions"
published: true
---

[*Reading: Dive into Systems §7.2*](https://diveintosystems.org/book/C7-x86_64/common.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we learned the structure of an instruction (opcode operand, operand) and the types of operands. Today we look common instructions for arithmetic: `mov`, `add`, and `sub`, and managing the stack, `push` and `pop`.
## mov, add, and sub

| Instruction | In C |
|---|---|
| `mov S, D` | `D = S` |
| `add S, D` | `D += S` |
| `sub S, D` | `D -= S` |

A few things to notice:

- **`mov` copies.** The source keeps its value. Nothing is actually moved.
- **The destination is also an input.** `add` and `sub` have no separate place for the result, so they overwrite D.
- **`sub` is D minus S**, not S minus D. Read `sub %rcx,%rax` as "subtract `%rcx` from `%rax`."

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — the order of operands</p>

Start with `%rax` = 10 and `%rcx` = 3 each time:

| Instruction | Computes | Result |
|---|---|---|
| `sub %rcx,%rax` | `%rax` - `%rcx` | `%rax` = 7 |
| `sub %rax,%rcx` | `%rcx` - `%rax` | `%rcx` = -7 |
| `add %rcx,%rax` | `%rax` + `%rcx` | `%rax` = 13 |
| `mov %rcx,%rax` | | `%rax` = 3, and `%rcx` is still 3 |

</div>

## From C to mov, add, and sub

Here is a function with three local variables ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBjb21iaW5lKHZvaWQpIHtcbiAgICBpbnQgeCA9IDEwO1xuICAgIGludCB5ID0gMztcbiAgICBpbnQgeiA9IHggKyB5O1xuICAgIHogPSB6IC0gMTtcbiAgICByZXR1cm4gejtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

```c
int combine(void) {
    int x = 10;
    int y = 3;
    int z = x + y;
    z = z - 1;
    return z;
}
```

```text
combine:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  movl   $0xa,-0xc(%rbp)     # int x = 10;
   b:  movl   $0x3,-0x8(%rbp)     # int y = 3;
  12:  mov    -0xc(%rbp),%edx     # int z = x + y;
  15:  mov    -0x8(%rbp),%eax
  18:  add    %edx,%eax
  1a:  mov    %eax,-0x4(%rbp)
  1d:  subl   $0x1,-0x4(%rbp)     # z = z - 1;
  21:  mov    -0x4(%rbp),%eax     # return z;
  24:  pop    %rbp
  25:  ret
```

- **Each local gets a slot on the stack.** `x` is at `-0xc(%rbp)`, `y` at `-0x8(%rbp)`, and `z` at `-0x4(%rbp)`. They are 4 bytes apart because each is an `int`.
- **`z = x + y` takes four instructions.** Memory can't be both the source and the destination, so `x` and `y` are copied into registers, added, and then the result is copied to `z`.
- **`z = z - 1` takes one.** `sub` can work on memory directly, as long as the other operand isn't memory too. It needs the `l` suffix, since neither `$0x1` nor `-0x4(%rbp)` says how many bytes to change.
- **The return value is left in `%eax`.**

Notice that `z` is written to the stack at `1a`, changed in memory at `1d`, and read back at `21`. Without optimization (`-O0`, the default), the compiler does not keep values in registers from one line of C to the next. Every line reads its variables from the stack and writes them back. With `-O1`, this whole function becomes `mov $0xc,%eax` and `ret`.

{% comment %}
INSTRUCTOR: before showing the assembly, ask how many instructions they expect for z = x + y. Then ask why it can't be done in one.
{% endcomment %}

## The Stack

Recall from Lecture 6 that the stack holds local variables and parameters, and that each function call gets its own **stack frame**.

![The parts of program memory, drawn as a tall box with memory address 0 at the top and the maximum address at the bottom. From top to bottom: Operating system, Code, Data, and Heap, then an empty gap, then the Stack at the bottom. An arrow from the heap points down into the gap, and an arrow from the stack points up into it. A pointer variable ptr on the stack points to the value 6 in the heap.]({{ "/figures/ch2/program_memory.png" | relative_url }})

On x86-64, **the stack grows toward lower addresses**. The "top" of the stack, where values are added and removed, is its lowest address.

Two registers keep track of the stack:

- **`%rsp`** (stack pointer) points to the top of the stack.
- **`%rbp`** (frame pointer) points to the base of the current function's frame.

So subtracting from `%rsp` grows the stack, and adding to it shrinks the stack.

## push and pop

x86-64 has two instructions just for the stack:

| Instruction | Translation | Same as |
|---|---|---|
| `push S` | Put a copy of S on top of the stack | `sub $0x8,%rsp`<br>`mov S,(%rsp)` |
| `pop D` | Take the top value off the stack and put it in D | `mov (%rsp),D`<br>`add $0x8,%rsp` |

- **They take one operand**, unlike `mov`, `add`, and `sub`.
- **`push` makes room, then writes. `pop` reads, then shrinks.**
- **They always move 8 bytes.** In `gcc -S` output you will see them as `pushq` and `popq`.
- **`pop` doesn't erase anything.** The value is still in memory, it's just no longer part of the stack. The next `push` will overwrite it.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — push and pop</p>

Start with `%rsp` = 0x110, `%rax` = 7, `%rdx` = 2, and `%rcx` = 0:

```text
push %rax
push %rdx
pop  %rcx
```

| Instruction | `%rsp` | M[0x108] | M[0x100] | `%rcx` |
|---|---|---|---|---|
| (start) | 0x110 | | | 0 |
| `push %rax` | 0x108 | 7 | | 0 |
| `push %rdx` | 0x100 | 7 | 2 | 0 |
| `pop %rcx` | 0x108 | 7 | 2 | 2 |

`%rcx` gets 2, the value pushed last: last in, first out. After the `pop`, 2 is still at 0x100, but it's past the top of the stack, so it no longer counts.

</div>

{% comment %}
INSTRUCTOR: draw the stack on the board and ask what ends up in %rcx before filling in the last row.
{% endcomment %}

[Run Example 2 in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/push%20%25rax%0Apush%20%25rdx%0Apop%20%25rcx/0/0/0/0/7/0/2). `%rsp` starts at a different address there, but it moves the same way: down by 8 on each `push`, up by 8 on the `pop`.

{% comment %}
INSTRUCTOR: the two ASM Visualizer links in these notes were built by hand from the Arithmetic Mode URL format and not opened. Click through both before class.
{% endcomment %}

## Setting Up a Stack Frame

Here is `adder2` again:

```text
push   %rbp               # save main's frame pointer
mov    %rsp,%rbp          # adder2's frame starts here
mov    %edi,-0x4(%rbp)
mov    -0x4(%rbp),%eax
add    $0x2,%eax
pop    %rbp               # put main's frame pointer back
retq
```

When `adder2` starts, `%rbp` still points to `main`'s frame. `adder2` needs `%rbp` for its own frame, but `main` will want its value back. So:

- **`push %rbp`** saves `main`'s frame pointer on the stack.
- **`mov %rsp,%rbp`** sets `%rbp` to the current top of the stack. That is the base of `adder2`'s frame, and its local variables are found at negative offsets from it, like `-0x4(%rbp)`.
- **`pop %rbp`** at the end restores `main`'s frame pointer.

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — the start and end of adder2</p>

Using the values from the book, `%rsp` is 0xd28 and `%rbp` is 0xd40 when `adder2` starts:

| Instruction | `%rsp` | `%rbp` | Stack |
|---|---|---|---|
| (start) | 0xd28 | 0xd40 | |
| `push %rbp` | 0xd20 | 0xd40 | M[0xd20] = 0xd40 |
| `mov %rsp,%rbp` | 0xd20 | 0xd20 | |
| ... | | | |
| `pop %rbp` | 0xd28 | 0xd40 | |

After the `pop`, `%rsp` and `%rbp` are back where they started. A function leaves the stack the way it found it.

</div>

Every function compiled without optimization starts with these two lines, and ends with `pop %rbp` (or `leave`) and `ret`. You will see them in every function in tomorrow's lab.

`adder2` stores `a` at `-0x4(%rbp)`, which is 0xd1c: past the top of the stack. It never moves `%rsp` to make room. A function that doesn't call any other function is allowed to use the 128 bytes just past `%rsp` without reserving them. `main` calls functions, so it has to reserve its space with `sub`.

## Try It

Assume three `long` variables: `x` is at `-24(%rbp)`, `y` at `-16(%rbp)`, and `z` at `-8(%rbp)`. Write each line as C-like pseudocode, then write the whole thing as C.

```text
mov  -16(%rbp), %rax
add  -24(%rbp), %rax
mov  %rax, -8(%rbp)
subq $5, -8(%rbp)
mov  -8(%rbp), %rax
```

If `x` is 10 and `y` is 4, what is in `%rax` at the end? Why does the fourth line need a suffix when the others don't?

Check your answer in the [ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-16%28%25rbp%29%2C%20%25rax%0Aadd%20-24%28%25rbp%29%2C%20%25rax%0Amov%20%25rax%2C%20-8%28%25rbp%29%0Asubq%20%245%2C%20-8%28%25rbp%29%0Amov%20-8%28%25rbp%29%2C%20%25rax/0/10/4/0/0/0/0), which starts with `x` = 10 and `y` = 4.

<details class="code-example" markdown="1">
<summary>Show answers</summary>

| Instruction | Pseudocode |
|---|---|
| `mov -16(%rbp), %rax` | `rax = y` |
| `add -24(%rbp), %rax` | `rax = rax + x` |
| `mov %rax, -8(%rbp)` | `z = rax` |
| `subq $5, -8(%rbp)` | `z = z - 5` |
| `mov -8(%rbp), %rax` | `rax = z` |

In C:

```c
z = x + y;
z = z - 5;
return z;
```

The last line puts `z` in `%rax`, the return value. With `x` = 10 and `y` = 4, `%rax` ends at 9.

`subq $5, -8(%rbp)` has a constant and a memory operand, and neither one says how many bytes to change. The other lines all use `%rax`, which is 64 bits.

</details>

{% comment %}
INSTRUCTOR: have them work it in pairs for a few minutes before running it in the visualizer.
{% endcomment %}

For some practice, try the [Exercises from the book for §7.2](https://diveintosystems.org/exercises/section-7_2.html).
