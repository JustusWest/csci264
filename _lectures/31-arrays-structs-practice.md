---
title: "31 Practice: Arrays and Structs"
published: true
---

[*No new reading. Review Dive into Systems §7.7 and §7.9*](https://diveintosystems.org/book/C7-x86_64/arrays.html)

To check your answers, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

In the next lab, you'll take a function that `gcc` compiled without optimization and rewrite it by hand so it runs fewer instructions. Today we practice the pieces: walking an array with an index in a register, counting instructions, and reaching into a struct.

We'll write each one together in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64). Check the Stack Content table after every run.

## Writing Array Code

1. **Pick a register for every variable.** The arguments arrive in `%rdi`, `%rsi`, and `%rdx`. They can stay there: there's no need to copy them to the stack.
2. **The base address stays put.** The index is the thing that moves.
3. **`arr[i]` is one operand:** `(%rdi,%rcx,8)` for a `long` array. `&arr[i]` is the same operand with `lea`.
4. **A field is a constant offset.** `s->field` is `D(%rdi)`, and an array inside a struct uses all of `D(Rb,Ri,S)`.

- **A function can use `%rax`, `%rcx`, `%rdx`, `%rsi`, `%rdi`, and `%r8` through `%r11` without saving them.** These are the same registers a call is allowed to change. The rest, like `%rbx`, belong to the caller.
- **`mov` can't go memory to memory.** To copy one element to another, go through a register.

## Counting Instructions

To compare two versions of a function, count the instructions in **one trip through the loop**. The setup before the loop runs once, but the loop body runs once per element. With 100,000 elements, one extra instruction in the loop is 100,000 extra instructions.

If the loop has an `if` in it, count both ways: when the jump skips the body, and when it doesn't.

## Problem 1

<div class="board-example" markdown="1">
<p class="board-title">Problem 1 — find</p>

<div class="board-setup" markdown="1">

```c
long find(long *arr, long n, long target) {
    for (long i = 0; i < n; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}
```

1. Write `find` in assembly, and a `main` that calls `find(arr, 5, 8)` on `long arr[5] = {2, 7, 1, 8, 2};`.
2. What does it return for `target` = 2? For `target` = 5?
3. Which register should hold `i`? Why?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
  .text
  .globl	find
  .type	find, @function

find:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start find's frame
  mov $0, %rax            # i = 0
  jmp find_test           # go to the test
find_loop:
  cmp %rdx, (%rdi,%rax,8) # compare arr[i] with target
  je find_done            # if they're equal, return i
  add $1, %rax            # i++
find_test:
  cmp %rsi, %rax          # compare i with n
  jl find_loop            # if i < n, go back
  mov $-1, %rax           # not found: return -1
find_done:
  pop %rbp                # restore main's %rbp
  ret                     # return i in %rax
  .size	find, .-find

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for arr
  movq $2, -40(%rbp)      # arr[0] = 2
  movq $7, -32(%rbp)      # arr[1] = 7
  movq $1, -24(%rbp)      # arr[2] = 1
  movq $8, -16(%rbp)      # arr[3] = 8
  movq $2, -8(%rbp)       # arr[4] = 2
  lea -40(%rbp), %rdi     # first argument: arr
  mov $5, %rsi            # second argument: n
  mov $8, %rdx            # third argument: target
  call find               # %rax = find(arr, 5, 8)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09find%0A%20%20.type%09find%2C%20%40function%0A%0Afind%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20find's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20jmp%20find_test%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Afind_loop%3A%0A%20%20cmp%20%25rdx%2C%20(%25rdi%2C%25rax%2C8)%20%23%20compare%20arr%5Bi%5D%20with%20target%0A%20%20je%20find_done%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20they're%20equal%2C%20return%20i%0A%20%20add%20%241%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Afind_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20n%0A%20%20jl%20find_loop%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20n%2C%20go%20back%0A%20%20mov%20%24-1%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%23%20not%20found%3A%20return%20-1%0Afind_done%3A%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20i%20in%20%25rax%0A%20%20.size%09find%2C%20.-find%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%242%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%202%0A%20%20movq%20%247%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%207%0A%20%20movq%20%241%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%201%0A%20%20movq%20%248%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%208%0A%20%20movq%20%242%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%202%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20n%0A%20%20mov%20%248%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20third%20argument%3A%20target%0A%20%20call%20find%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20find(arr%2C%205%2C%208)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 3.

2. For `target` = 2, it returns 0: the first match wins, because the loop stops as soon as it finds one. For `target` = 5, it returns -1. The visualizer shows it as 0xffffffffffffffff, which is -1 in two's complement.
3. `%rax`. When the match is found, `i` is already the return value, so `je` can jump straight to the end.

- **`cmp` can read memory.** `cmp %rdx, (%rdi,%rax,8)` compares `arr[i]` with `target` without loading `arr[i]` into a register first.
- **There are two ways out of the loop.** `je` leaves when it finds a match. Falling out the bottom means it never did.
- **`i` is an index, not a value.** `find` returns where the match is. `arr[i]` is the value there, and `&arr[i]` is its address. Keep track of which one each register holds.

</details>

</div>

## Problem 2

This time we start from what `gcc` writes. This is the output of `gcc -O0 -S`, the same kind of file you'll edit in lab, with `endbr64` and the lines that start with a dot taken out.

<div class="board-example" markdown="1">
<p class="board-title">Problem 2 — count_neg</p>

```c
long count_neg(long *arr, long n) {
    long count = 0;
    for (long i = 0; i < n; i++) {
        if (arr[i] < 0) {
            count++;
        }
    }
    return count;
}
```

```text
count_neg:
  pushq  %rbp
  movq   %rsp, %rbp
  movq   %rdi, -24(%rbp)
  movq   %rsi, -32(%rbp)
  movq   $0, -16(%rbp)
  movq   $0, -8(%rbp)
  jmp    .L2
.L4:
  movq   -8(%rbp), %rax
  leaq   0(,%rax,8), %rdx
  movq   -24(%rbp), %rax
  addq   %rdx, %rax
  movq   (%rax), %rax
  testq  %rax, %rax
  jns    .L3
  addq   $1, -16(%rbp)
.L3:
  addq   $1, -8(%rbp)
.L2:
  movq   -8(%rbp), %rax
  cmpq   -32(%rbp), %rax
  jl     .L4
  movq   -16(%rbp), %rax
  popq   %rbp
  ret
```

<div class="board-setup" markdown="1">

1. Annotate the listing. Where do `arr`, `n`, `count`, and `i` live?
2. How many instructions run in one trip through the loop when `arr[i]` is negative? When it isn't?
3. Write a faster `count_neg` that gives the same answer. Call it from a `main` with `long arr[5] = {4, -2, 0, -7, 9};`.
4. How many instructions run in one trip through your loop?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
count_neg:
  pushq  %rbp
  movq   %rsp, %rbp
  movq   %rdi, -24(%rbp)      # arr
  movq   %rsi, -32(%rbp)      # n
  movq   $0, -16(%rbp)        # count = 0
  movq   $0, -8(%rbp)         # i = 0
  jmp    .L2                  # go to the test
.L4:
  movq   -8(%rbp), %rax       # %rax = i
  leaq   0(,%rax,8), %rdx     # %rdx = i * 8
  movq   -24(%rbp), %rax      # %rax = arr
  addq   %rdx, %rax           # %rax = &arr[i]
  movq   (%rax), %rax         # %rax = arr[i]
  testq  %rax, %rax           # compare arr[i] with 0
  jns    .L3                  # if arr[i] >= 0, skip
  addq   $1, -16(%rbp)        # count++
.L3:
  addq   $1, -8(%rbp)         # i++
.L2:
  movq   -8(%rbp), %rax       # %rax = i
  cmpq   -32(%rbp), %rax      # compare i with n
  jl     .L4                  # if i < n, go back
  movq   -16(%rbp), %rax      # return count
  popq   %rbp
  ret
```

1. `arr` is at `-24(%rbp)`, `n` at `-32(%rbp)`, `count` at `-16(%rbp)`, and `i` at `-8(%rbp)`.
2. 12 when `arr[i]` is negative, and 11 when it isn't: the only difference is `count++`.

- **Five instructions to read `arr[i]`.** Load `i`, multiply by 8, load `arr`, add, then read. Last time `sumArray` did the same thing.
- **`testq %rax, %rax` compares `%rax` with 0.** It ANDs `%rax` with itself, so the result is `%rax`, and only the flags change. `jns` jumps if the sign flag is not set: if `arr[i]` is not negative.
- **`i` and `n` come back from the stack on every trip,** just to do the test at the bottom.

Now the faster version. The arguments stay in their registers, `count` goes in `%rax`, and `i` goes in `%rcx`:

```text
  .text
  .globl	count_neg
  .type	count_neg, @function

count_neg:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start count_neg's frame
  mov $0, %rax            # count = 0
  mov $0, %rcx            # i = 0
  jmp cn_test             # go to the test
cn_loop:
  cmpq $0, (%rdi,%rcx,8)  # compare arr[i] with 0
  jge cn_skip             # if arr[i] >= 0, skip
  add $1, %rax            # count++
cn_skip:
  add $1, %rcx            # i++
cn_test:
  cmp %rsi, %rcx          # compare i with n
  jl cn_loop              # if i < n, go back
  pop %rbp                # restore main's %rbp
  ret                     # return count in %rax
  .size	count_neg, .-count_neg

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for arr
  movq $4, -40(%rbp)      # arr[0] = 4
  movq $-2, -32(%rbp)     # arr[1] = -2
  movq $0, -24(%rbp)      # arr[2] = 0
  movq $-7, -16(%rbp)     # arr[3] = -7
  movq $9, -8(%rbp)       # arr[4] = 9
  lea -40(%rbp), %rdi     # first argument: arr
  mov $5, %rsi            # second argument: n
  call count_neg          # %rax = count_neg(arr, 5)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09count_neg%0A%20%20.type%09count_neg%2C%20%40function%0A%0Acount_neg%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20count_neg's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20count%20%3D%200%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20jmp%20cn_test%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Acn_loop%3A%0A%20%20cmpq%20%240%2C%20(%25rdi%2C%25rcx%2C8)%20%20%23%20compare%20arr%5Bi%5D%20with%200%0A%20%20jge%20cn_skip%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20arr%5Bi%5D%20%3E%3D%200%2C%20skip%0A%20%20add%20%241%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20count%2B%2B%0Acn_skip%3A%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Acn_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20n%0A%20%20jl%20cn_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20n%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20count%20in%20%25rax%0A%20%20.size%09count_neg%2C%20.-count_neg%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%244%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%204%0A%20%20movq%20%24-2%2C%20-32(%25rbp)%20%20%20%20%20%23%20arr%5B1%5D%20%3D%20-2%0A%20%20movq%20%240%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%200%0A%20%20movq%20%24-7%2C%20-16(%25rbp)%20%20%20%20%20%23%20arr%5B3%5D%20%3D%20-7%0A%20%20movq%20%249%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%209%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20n%0A%20%20call%20count_neg%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20count_neg(arr%2C%205)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 2.

4. 6 when `arr[i]` is negative, and 5 when it isn't.

- **`count` lives in `%rax` the whole time.** When the loop ends, the return value is already there, so there's nothing to copy, the same as `find`.
- **`cmpq $0, (%rdi,%rcx,8)` is gcc's five instructions plus its `testq`.** It reads `arr[i]` and compares it with 0 in one instruction.
- **`cmpq` needs the `q`.** Comparing memory with a constant gives the assembler no register to tell it the size.
- **`jge` is the C condition, flipped.** `if (arr[i] < 0)` skips `count++` when `arr[i] >= 0`.
- **On 100,000 numbers,** a third of them negative, Valgrind counts about 1.13 million instructions in `gcc`'s version and about 530,000 in ours. That's 2.1 times fewer, and almost all of it comes from the loop.

</details>

</div>

## Problem 3

<div class="board-example" markdown="1">
<p class="board-title">Problem 3 — an array inside a struct</p>

<div class="board-setup" markdown="1">

```c
struct scores {
    long n;
    long vals[4];
};

void add_score(struct scores *s, long x) {
    s->vals[s->n] = x;
    s->n = s->n + 1;
}

long total(struct scores *s) {
    long sum = 0;
    for (long i = 0; i < s->n; i++) {
        sum += s->vals[i];
    }
    return sum;
}
```

1. What is the offset of `n`? Of `vals[0]`? Of `vals[i]`? What is `sizeof(struct scores)`?
2. Write `add_score`, and a `main` with a local `struct scores s` that sets `s.n` to 0, then calls `add_score(&s, 90)` and `add_score(&s, 75)`.
3. Write `total`, and add a call to it at the end of `main`.
4. If `vals` came before `n` in the struct, what would change in your code?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

1. `n` is at 0, `vals[0]` at 8, and `vals[i]` at `8 + 8*i`. `sizeof(struct scores)` is 40: one `long` and four more.

```text
  .text
  .globl	add_score
  .type	add_score, @function

add_score:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start add_score's frame
  mov (%rdi), %rax        # %rax = s->n
  mov %rsi, 8(%rdi,%rax,8) # s->vals[s->n] = x
  addq $1, (%rdi)         # s->n = s->n + 1
  pop %rbp                # restore main's %rbp
  ret
  .size	add_score, .-add_score

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for s
  movq $0, -40(%rbp)      # s.n = 0
  lea -40(%rbp), %rdi     # first argument: &s
  mov $90, %rsi           # second argument: x
  call add_score          # add_score(&s, 90)
  lea -40(%rbp), %rdi     # &s again: a call can change %rdi
  mov $75, %rsi           # second argument: x
  call add_score          # add_score(&s, 75)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09add_score%0A%20%20.type%09add_score%2C%20%40function%0A%0Aadd_score%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20add_score's%20frame%0A%20%20mov%20(%25rdi)%2C%20%25rax%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20s-%3En%0A%20%20mov%20%25rsi%2C%208(%25rdi%2C%25rax%2C8)%20%23%20s-%3Evals%5Bs-%3En%5D%20%3D%20x%0A%20%20addq%20%241%2C%20(%25rdi)%20%20%20%20%20%20%20%20%20%23%20s-%3En%20%3D%20s-%3En%20%2B%201%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09add_score%2C%20.-add_score%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20s%0A%20%20movq%20%240%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20s.n%20%3D%200%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26s%0A%20%20mov%20%2490%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20x%0A%20%20call%20add_score%20%20%20%20%20%20%20%20%20%20%23%20add_score(%26s%2C%2090)%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20%26s%20again%3A%20a%20call%20can%20change%20%25rdi%0A%20%20mov%20%2475%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20x%0A%20%20call%20add_score%20%20%20%20%20%20%20%20%20%20%23%20add_score(%26s%2C%2075)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A).

| Address | Value | |
|---|---|---|
| `-40(%rbp)` | 2 | `s.n` |
| `-32(%rbp)` | 90 | `s.vals[0]` |
| `-24(%rbp)` | 75 | `s.vals[1]` |
| `-16(%rbp)` | | `s.vals[2]`, never set |
| `-8(%rbp)` | | `s.vals[3]`, never set |

- **`8(%rdi,%rax,8)` uses every part of `D(Rb,Ri,S)`.** `Rb` is the struct, `D` gets us to `vals`, and `Ri` and `S` get us to element `i`. Last time `rs[i].height` used the same form, but there the elements were 32 bytes, so we had to multiply `i` ourselves.
- **The index comes from memory, so load it first.** An operand can't read memory to find out where to read memory.
- **`main` sets `%rdi` again before the second call.** Our `add_score` happens not to change `%rdi`, but a call is allowed to.

```text
  .text
  .globl	add_score
  .type	add_score, @function

add_score:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start add_score's frame
  mov (%rdi), %rax        # %rax = s->n
  mov %rsi, 8(%rdi,%rax,8) # s->vals[s->n] = x
  addq $1, (%rdi)         # s->n = s->n + 1
  pop %rbp                # restore main's %rbp
  ret
  .size	add_score, .-add_score

  .globl	total
  .type	total, @function

total:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start total's frame
  mov $0, %rax            # sum = 0
  mov $0, %rcx            # i = 0
  mov (%rdi), %rdx        # %rdx = s->n
  jmp total_test          # go to the test
total_loop:
  add 8(%rdi,%rcx,8), %rax # sum += s->vals[i]
  add $1, %rcx            # i++
total_test:
  cmp %rdx, %rcx          # compare i with s->n
  jl total_loop           # if i < s->n, go back
  pop %rbp                # restore main's %rbp
  ret                     # return sum in %rax
  .size	total, .-total

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for s
  movq $0, -40(%rbp)      # s.n = 0
  lea -40(%rbp), %rdi     # first argument: &s
  mov $90, %rsi           # second argument: x
  call add_score          # add_score(&s, 90)
  lea -40(%rbp), %rdi     # &s again: a call can change %rdi
  mov $75, %rsi           # second argument: x
  call add_score          # add_score(&s, 75)
  lea -40(%rbp), %rdi     # first argument: &s
  call total              # %rax = total(&s)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09add_score%0A%20%20.type%09add_score%2C%20%40function%0A%0Aadd_score%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20add_score's%20frame%0A%20%20mov%20(%25rdi)%2C%20%25rax%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20s-%3En%0A%20%20mov%20%25rsi%2C%208(%25rdi%2C%25rax%2C8)%20%23%20s-%3Evals%5Bs-%3En%5D%20%3D%20x%0A%20%20addq%20%241%2C%20(%25rdi)%20%20%20%20%20%20%20%20%20%23%20s-%3En%20%3D%20s-%3En%20%2B%201%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09add_score%2C%20.-add_score%0A%0A%20%20.globl%09total%0A%20%20.type%09total%2C%20%40function%0A%0Atotal%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20total's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20sum%20%3D%200%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20mov%20(%25rdi)%2C%20%25rdx%20%20%20%20%20%20%20%20%23%20%25rdx%20%3D%20s-%3En%0A%20%20jmp%20total_test%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Atotal_loop%3A%0A%20%20add%208(%25rdi%2C%25rcx%2C8)%2C%20%25rax%20%23%20sum%20%2B%3D%20s-%3Evals%5Bi%5D%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Atotal_test%3A%0A%20%20cmp%20%25rdx%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20s-%3En%0A%20%20jl%20total_loop%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20s-%3En%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20sum%20in%20%25rax%0A%20%20.size%09total%2C%20.-total%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20s%0A%20%20movq%20%240%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20s.n%20%3D%200%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26s%0A%20%20mov%20%2490%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20x%0A%20%20call%20add_score%20%20%20%20%20%20%20%20%20%20%23%20add_score(%26s%2C%2090)%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20%26s%20again%3A%20a%20call%20can%20change%20%25rdi%0A%20%20mov%20%2475%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20x%0A%20%20call%20add_score%20%20%20%20%20%20%20%20%20%20%23%20add_score(%26s%2C%2075)%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26s%0A%20%20call%20total%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20total(%26s)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xa5, which is 165.

- **`s->n` is read once, before the loop.** `cmp (%rdi), %rcx` would also work, and it's the same number of instructions, but it reads memory on every trip.

4. `vals[i]` would be at offset `8*i`, so the operand becomes `(%rdi,%rax,8)`. `n` would be at 32, so every `(%rdi)` that means `s->n` becomes `32(%rdi)`. The C doesn't change at all.

</details>

</div>

## More Practice

<div class="board-example" markdown="1">
<p class="board-title">Problem 4 — reverse</p>

<div class="board-setup" markdown="1">

```c
void reverse(long *arr, long n) {
    long i = 0;
    long j = n - 1;
    while (i < j) {
        long t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
        i++;
        j--;
    }
}
```

1. Write `reverse` in assembly, and a `main` that calls it on `long arr[5] = {1, 2, 3, 4, 5};`.
2. The C swap uses one temporary, `t`. Why does yours need two registers?
3. How many trips through the loop for `n` = 5? For `n` = 4? What happens to the middle element when `n` is odd?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
  .text
  .globl	reverse
  .type	reverse, @function

reverse:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start reverse's frame
  mov $0, %rcx            # i = 0
  lea -1(%rsi), %rdx      # j = n - 1
  jmp rev_test            # go to the test
rev_loop:
  mov (%rdi,%rcx,8), %rax # t = arr[i]
  mov (%rdi,%rdx,8), %r8  # %r8 = arr[j]
  mov %r8, (%rdi,%rcx,8)  # arr[i] = arr[j]
  mov %rax, (%rdi,%rdx,8) # arr[j] = t
  add $1, %rcx            # i++
  sub $1, %rdx            # j--
rev_test:
  cmp %rdx, %rcx          # compare i with j
  jl rev_loop             # if i < j, go back
  pop %rbp                # restore main's %rbp
  ret
  .size	reverse, .-reverse

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for arr
  movq $1, -40(%rbp)      # arr[0] = 1
  movq $2, -32(%rbp)      # arr[1] = 2
  movq $3, -24(%rbp)      # arr[2] = 3
  movq $4, -16(%rbp)      # arr[3] = 4
  movq $5, -8(%rbp)       # arr[4] = 5
  lea -40(%rbp), %rdi     # first argument: arr
  mov $5, %rsi            # second argument: n
  call reverse            # reverse(arr, 5)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09reverse%0A%20%20.type%09reverse%2C%20%40function%0A%0Areverse%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20reverse's%20frame%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20lea%20-1(%25rsi)%2C%20%25rdx%20%20%20%20%20%20%23%20j%20%3D%20n%20-%201%0A%20%20jmp%20rev_test%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Arev_loop%3A%0A%20%20mov%20(%25rdi%2C%25rcx%2C8)%2C%20%25rax%20%23%20t%20%3D%20arr%5Bi%5D%0A%20%20mov%20(%25rdi%2C%25rdx%2C8)%2C%20%25r8%20%20%23%20%25r8%20%3D%20arr%5Bj%5D%0A%20%20mov%20%25r8%2C%20(%25rdi%2C%25rcx%2C8)%20%20%23%20arr%5Bi%5D%20%3D%20arr%5Bj%5D%0A%20%20mov%20%25rax%2C%20(%25rdi%2C%25rdx%2C8)%20%23%20arr%5Bj%5D%20%3D%20t%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0A%20%20sub%20%241%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%20%23%20j--%0Arev_test%3A%0A%20%20cmp%20%25rdx%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20j%0A%20%20jl%20rev_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20j%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09reverse%2C%20.-reverse%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%241%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%201%0A%20%20movq%20%242%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%202%0A%20%20movq%20%243%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%203%0A%20%20movq%20%244%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%204%0A%20%20movq%20%245%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%205%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20n%0A%20%20call%20reverse%20%20%20%20%20%20%20%20%20%20%20%20%23%20reverse(arr%2C%205)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). The Stack Content table ends with 5, 4, 3, 2, 1 from `-40(%rbp)` down to `-8(%rbp)`.

2. `mov` can't go memory to memory, so `arr[i] = arr[j]` has to pass through a register too. `%rax` is `t`, and `%r8` carries `arr[j]`.
3. Two trips for both. For `n` = 5, `i` and `j` meet at 2, so `arr[2]` is never touched: it's already in the right place.

- **Two indexes, one base.** `(%rdi,%rcx,8)` is `arr[i]` and `(%rdi,%rdx,8)` is `arr[j]`.
- **`lea -1(%rsi), %rdx` is `j = n - 1` in one instruction.** `mov %rsi, %rdx` and `sub $1, %rdx` work too.

</details>

</div>

