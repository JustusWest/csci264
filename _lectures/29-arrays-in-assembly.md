---
title: "29 Arrays in Assembly"
published: true
---

[*Reading: Dive into Systems §7.7*](https://diveintosystems.org/book/C7-x86_64/arrays.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).


In C, we saw that an array is one contiguous block of memory, and that `ptr + 1` moves forward by the size of the type it points to, not by one byte. In assembly there are no types. Every one of those iterations has to be in the instructions.

## Where Is arr[i]?

Element `i` of an array of type `T` is at:

```text
arr + i * sizeof(T)
```

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — element addresses</p>

<div class="board-setup" markdown="1">

`arr` starts at address 0x100. Fill in the address of each element:

| | `arr[0]` | `arr[1]` | `arr[2]` | `arr[3]` | `arr[4]` |
|---|---|---|---|---|---|
| `long arr[5]` | ? | ? | ? | ? | ? |
| `int arr[5]` | ? | ? | ? | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| | `arr[0]` | `arr[1]` | `arr[2]` | `arr[3]` | `arr[4]` |
|---|---|---|---|---|---|
| `long arr[5]` | 0x100 | 0x108 | 0x110 | 0x118 | 0x120 |
| `int arr[5]` | 0x100 | 0x104 | 0x108 | 0x10c | 0x110 |

- **Five `long`s take 40 bytes. Five `int`s take 20.**
- **In C, `arr + 3` means three elements.** In assembly it has to be 24 bytes for a `long` array, or 12 for an `int` array.

</details>

</div>

As before, we use `long` so the assembly uses the 64-bit registers.

## The Scaled-Index Operand

Recall the general memory operand:

```text
D(Rb,Ri,S)   →   M[D + Rb + Ri*S]
```
- `D` is a constant **displacement**
- `Rb` is the **base** register
- `Ri` is the **index** register
- `S` is the **scale**: 1, 2, 4, or 8


This is the operand arrays need. If `%rdi` holds the base address of a `long` array and `%rcx` holds `i`, then:

```text
(%rdi,%rcx,8)   →   M[%rdi + %rcx*8]   →   arr[i]
```

- **The scale is `sizeof` the element:** 1 for `char`, 4 for `int`, 8 for `long` and pointers. That's why the scale can only be 1, 2, 4, or 8.
- **The hardware does the multiply.** We don't have to write `imul` to index an array.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — array operations</p>

<div class="board-setup" markdown="1">

`long arr[10]`. `%rdi` holds `arr`, `%rcx` holds `i`, and `x` goes in `%rax`. Write one instruction for each line:

| C | Type of `x` | Assembly |
|---|---|---|
| `x = arr;` | `long *` | ? |
| `x = arr[0];` | `long` | ? |
| `x = arr[i];` | `long` | ? |
| `x = arr[5];` | `long` | ? |
| `x = &arr[3];` | `long *` | ? |
| `x = &arr[i];` | `long *` | ? |
| `arr[i] = x;` | — | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| C | Type of `x` | Assembly |
|---|---|---|
| `x = arr;` | `long *` | `mov %rdi, %rax` |
| `x = arr[0];` | `long` | `mov (%rdi), %rax` |
| `x = arr[i];` | `long` | `mov (%rdi,%rcx,8), %rax` |
| `x = arr[5];` | `long` | `mov 40(%rdi), %rax` |
| `x = &arr[3];` | `long *` | `lea 24(%rdi), %rax` |
| `x = &arr[i];` | `long *` | `lea (%rdi,%rcx,8), %rax` |
| `arr[i] = x;` | — | `mov %rax, (%rdi,%rcx,8)` |

- **Look at the type.** A `long` is read from memory with `mov`. A `long *` is just an address, so it comes from `lea`. 
- Recall that `lea` stands for *load effective address*, it is equivalent to the `&` operator in C.
- **A constant index needs no index register.** The compiler multiplies it out: 5 × 8 = 40.

</details>

</div>

## An Array on the Stack

The ASM Visualizer doesn't show the heap, so our arrays live on the stack, as a local in `main`:

```c
long arr[5] = {3, 1, 4, 1, 5};
```

`main` makes room with `sub`, then stores each element. `gcc` always moves `%rsp` by a multiple of 16, so 40 bytes becomes 48.

```text
  sub $48, %rsp           # room for arr
  movq $3, -40(%rbp)      # arr[0] = 3
  movq $1, -32(%rbp)      # arr[1] = 1
  movq $4, -24(%rbp)      # arr[2] = 4
  movq $1, -16(%rbp)      # arr[3] = 1
  movq $5, -8(%rbp)       # arr[4] = 5
```

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — arr on the stack</p>

<div class="board-setup" markdown="1">

Suppose `main`'s `%rbp` is AF0. Fill in the stack, lowest address at the top, like the ASM Visualizer:

| Address | Value |
|---|---|
| ? | ? |
| ? | ? |
| ? | ? |
| ? | ? |
| ? | ? |
| AF0 | `main`'s caller's `%rbp` |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Value | |
|---|---|---|
| AC8 | 3 | `arr[0]` |
| AD0 | 1 | `arr[1]` |
| AD8 | 4 | `arr[2]` |
| AE0 | 1 | `arr[3]` |
| AE8 | 5 | `arr[4]` |
| AF0 | `main`'s caller's `%rbp` | ← `%rbp` |

- **`arr[0]` is at the lowest address.** The stack grows down, but an array always goes up from its base.
- **The base address is AC8.** That's `-40(%rbp)`.

</details>

</div>

To pass the array to a function, pass its base address. Earlier in the course we saw that an array argument is really a pointer to element 0. In assembly that pointer comes from `lea`:

```text
  lea -40(%rbp), %rdi     # first argument: arr, the address AC8
```

## sumArray

Here is the book's example, with `long`:

```c
long sumArray(long *arr, long length) {
    long i, total = 0;
    for (i = 0; i < length; i++) {
        total += arr[i];
    }
    return total;
}
```

`arr` arrives in `%rdi` and `length` in `%rsi`. Let's write it in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64), starting with `main` and a `sumArray` that only returns `arr[0]`:

<details class="code-example" markdown="1">
<summary>Show code: step 1, arr[0]</summary>

```text
  .text
  .globl	sumArray
  .type	sumArray, @function

sumArray:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start sumArray's frame
  mov (%rdi), %rax        # for now, return arr[0]
  pop %rbp                # restore main's %rbp
  ret
  .size	sumArray, .-sumArray

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for arr
  movq $3, -40(%rbp)      # arr[0] = 3
  movq $1, -32(%rbp)      # arr[1] = 1
  movq $4, -24(%rbp)      # arr[2] = 4
  movq $1, -16(%rbp)      # arr[3] = 1
  movq $5, -8(%rbp)       # arr[4] = 5
  lea -40(%rbp), %rdi     # first argument: arr
  mov $5, %rsi            # second argument: length
  call sumArray           # %rax = sumArray(arr, 5)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumArray%0A%20%20.type%09sumArray%2C%20%40function%0A%0AsumArray%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sumArray's%20frame%0A%20%20mov%20(%25rdi)%2C%20%25rax%20%20%20%20%20%20%20%20%23%20for%20now%2C%20return%20arr%5B0%5D%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09sumArray%2C%20.-sumArray%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%243%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%203%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%201%0A%20%20movq%20%244%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%204%0A%20%20movq%20%241%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%201%0A%20%20movq%20%245%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%205%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20length%0A%20%20call%20sumArray%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumArray(arr%2C%205)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 3.

</details>

- **Check the Stack Content table.** The five elements should be at the addresses from Example 3.
- **Try `mov 16(%rdi), %rax`.** That's `arr[2]`, so `%rax` ends at 4.

Now the loop. It's a `for` loop like the ones from earlier in the course. We keep `total` in `%rax`, since it's the return value, and `i` in `%rcx`:

<details class="code-example" markdown="1">
<summary>Show code: step 2, the loop</summary>

```text
  .text
  .globl	sumArray
  .type	sumArray, @function

sumArray:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start sumArray's frame
  mov $0, %rax            # total = 0
  mov $0, %rcx            # i = 0
  jmp sum_test            # go to the test
sum_loop:
  add (%rdi,%rcx,8), %rax # total += arr[i]
  add $1, %rcx            # i++
sum_test:
  cmp %rsi, %rcx          # compare i with length
  jl sum_loop             # if i < length, go back
  pop %rbp                # restore main's %rbp
  ret                     # return total in %rax
  .size	sumArray, .-sumArray

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $48, %rsp           # room for arr
  movq $3, -40(%rbp)      # arr[0] = 3
  movq $1, -32(%rbp)      # arr[1] = 1
  movq $4, -24(%rbp)      # arr[2] = 4
  movq $1, -16(%rbp)      # arr[3] = 1
  movq $5, -8(%rbp)       # arr[4] = 5
  lea -40(%rbp), %rdi     # first argument: arr
  mov $5, %rsi            # second argument: length
  call sumArray           # %rax = sumArray(arr, 5)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumArray%0A%20%20.type%09sumArray%2C%20%40function%0A%0AsumArray%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sumArray's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20total%20%3D%200%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20jmp%20sum_test%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Asum_loop%3A%0A%20%20add%20(%25rdi%2C%25rcx%2C8)%2C%20%25rax%20%23%20total%20%2B%3D%20arr%5Bi%5D%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Asum_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20length%0A%20%20jl%20sum_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20length%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20total%20in%20%25rax%0A%20%20.size%09sumArray%2C%20.-sumArray%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%243%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%203%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%201%0A%20%20movq%20%244%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%204%0A%20%20movq%20%241%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%201%0A%20%20movq%20%245%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%205%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20length%0A%20%20call%20sumArray%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumArray(arr%2C%205)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xe, which is 14.

</details>

- **`add (%rdi,%rcx,8), %rax` is the whole loop body.** It reads `arr[i]` and adds it to `total` in one instruction.
- **`%rdi` never changes.** The base stays put, and `i` moves.
- **With `%rcx` = 3, the operand is AC8 + 3 × 8 = AE0.** That's `arr[3]` in Example 3.

## What gcc Makes

Here is `gcc`'s version, without optimization ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6Imxvbmcgc3VtQXJyYXkobG9uZyAqYXJyLCBsb25nIGxlbmd0aCkge1xuICAgIGxvbmcgaSwgdG90YWwgPSAwO1xuICAgIGZvciAoaSA9IDA7IGkgPCBsZW5ndGg7IGkrKykge1xuICAgICAgICB0b3RhbCArPSBhcnJbaV07XG4gICAgfVxuICAgIHJldHVybiB0b3RhbDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)). It's cleaned up a little: labels instead of addresses.

<div class="board-example" markdown="1">
<p class="board-title">Example 4 — gcc's sumArray</p>

```text
sumArray:
  push   %rbp
  mov    %rsp,%rbp
  mov    %rdi,-0x18(%rbp)
  mov    %rsi,-0x20(%rbp)
  movq   $0x0,-0x8(%rbp)
  movq   $0x0,-0x10(%rbp)
  jmp    test
loop:
  mov    -0x10(%rbp),%rax
  lea    0x0(,%rax,8),%rdx
  mov    -0x18(%rbp),%rax
  add    %rdx,%rax
  mov    (%rax),%rax
  add    %rax,-0x8(%rbp)
  addq   $0x1,-0x10(%rbp)
test:
  mov    -0x10(%rbp),%rax
  cmp    -0x20(%rbp),%rax
  jl     loop
  mov    -0x8(%rbp),%rax
  pop    %rbp
  ret
```

<div class="board-setup" markdown="1">

1. Where are `arr`, `length`, `total`, and `i`?
2. Which instructions compute `&arr[i]`?
3. Which instruction reads `arr[i]`?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
sumArray:
  push   %rbp
  mov    %rsp,%rbp
  mov    %rdi,-0x18(%rbp)      # arr
  mov    %rsi,-0x20(%rbp)      # length
  movq   $0x0,-0x8(%rbp)       # total = 0
  movq   $0x0,-0x10(%rbp)      # i = 0
  jmp    test
loop:
  mov    -0x10(%rbp),%rax      # %rax = i
  lea    0x0(,%rax,8),%rdx     # %rdx = i * 8
  mov    -0x18(%rbp),%rax      # %rax = arr
  add    %rdx,%rax             # %rax = arr + i*8, which is &arr[i]
  mov    (%rax),%rax           # %rax = arr[i]
  add    %rax,-0x8(%rbp)       # total += arr[i]
  addq   $0x1,-0x10(%rbp)      # i++
test:
  mov    -0x10(%rbp),%rax      # %rax = i
  cmp    -0x20(%rbp),%rax      # compare i with length
  jl     loop                  # if i < length, go back
  mov    -0x8(%rbp),%rax       # return total
  pop    %rbp
  ret
```

- **`lea` does the multiply here.** `lea 0x0(,%rax,8)` is `%rax * 8`, not a memory read.
- **Four instructions to read `arr[i]`.** Our version used one operand: `(%rdi,%rcx,8)`.
- **Every variable lives on the stack,** so every use is a trip to memory.

</details>

</div>

With `-O1` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6Imxvbmcgc3VtQXJyYXkobG9uZyAqYXJyLCBsb25nIGxlbmd0aCkge1xuICAgIGxvbmcgaSwgdG90YWwgPSAwO1xuICAgIGZvciAoaSA9IDA7IGkgPCBsZW5ndGg7IGkrKykge1xuICAgICAgICB0b3RhbCArPSBhcnJbaV07XG4gICAgfVxuICAgIHJldHVybiB0b3RhbDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)), `gcc` throws out `i` completely and walks a pointer through the array instead:

```text
sumArray:
  test   %rsi,%rsi             # if length <= 0,
  jle    empty                 #   return 0
  mov    %rdi,%rax             # p = arr
  lea    (%rdi,%rsi,8),%rcx    # end = &arr[length]
  mov    $0x0,%rdx             # total = 0
loop:
  add    (%rax),%rdx           # total += *p
  add    $0x8,%rax             # p++
  cmp    %rcx,%rax             # compare p with end
  jne    loop                  # if p != end, go back
done:
  mov    %rdx,%rax             # return total
  ret
empty:
  mov    $0x0,%rdx
  jmp    done
```

- **`p++` is `add $0x8`.** That's pointer arithmetic done by hand.
- **The loop is four instructions, and none of them touch the stack.**

## Try It

Use the same `main` for both, with a different array:

```c
long arr[5] = {2, 7, 1, 8, 2};
```

```c
long maxArray(long *arr, long length) {
    long max = arr[0];
    for (long i = 1; i < length; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

void scale(long *arr, long length, long k) {
    for (long i = 0; i < length; i++) {
        arr[i] = arr[i] * k;
    }
}
```

1. Write `maxArray`. It returns the largest element. You can assume `length` is at least 1.
2. Write `scale`. It multiplies every element by `k`, in place. `k` arrives in `%rdx`. Check the Stack Content table when it's done.
3. If `arr` held `int`s instead, what would change in your code?

<details class="code-example" markdown="1">
<summary>Show code: maxArray</summary>

```text
maxArray:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start maxArray's frame
  mov (%rdi), %rax        # max = arr[0]
  mov $1, %rcx            # i = 1
  jmp max_test            # go to the test
max_loop:
  mov (%rdi,%rcx,8), %rdx # %rdx = arr[i]
  cmp %rax, %rdx          # compare arr[i] with max
  jle max_next            # if arr[i] <= max, skip the update
  mov %rdx, %rax          # max = arr[i]
max_next:
  add $1, %rcx            # i++
max_test:
  cmp %rsi, %rcx          # compare i with length
  jl max_loop             # if i < length, go back
  pop %rbp                # restore main's %rbp
  ret                     # return max in %rax

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
  mov $5, %rsi            # second argument: length
  call maxArray           # %rax = maxArray(arr, 5)
  leave
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09maxArray%0A%20%20.type%09maxArray%2C%20%40function%0A%0AmaxArray%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20maxArray's%20frame%0A%20%20mov%20(%25rdi)%2C%20%25rax%20%20%20%20%20%20%20%20%23%20max%20%3D%20arr%5B0%5D%0A%20%20mov%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%201%0A%20%20jmp%20max_test%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Amax_loop%3A%0A%20%20mov%20(%25rdi%2C%25rcx%2C8)%2C%20%25rdx%20%23%20%25rdx%20%3D%20arr%5Bi%5D%0A%20%20cmp%20%25rax%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%23%20compare%20arr%5Bi%5D%20with%20max%0A%20%20jle%20max_next%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20arr%5Bi%5D%20%3C%3D%20max%2C%20skip%20the%20update%0A%20%20mov%20%25rdx%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20max%20%3D%20arr%5Bi%5D%0Amax_next%3A%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Amax_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20length%0A%20%20jl%20max_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20length%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20max%20in%20%25rax%0A%20%20.size%09maxArray%2C%20.-maxArray%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%242%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%202%0A%20%20movq%20%247%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%207%0A%20%20movq%20%241%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%201%0A%20%20movq%20%248%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%208%0A%20%20movq%20%242%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%202%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20length%0A%20%20call%20maxArray%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20maxArray(arr%2C%205)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 8.

- **This is an `if` inside a loop.** `jle` skips the update, the opposite of the C condition `arr[i] > max`.
- **`arr[i]` goes into `%rdx` first,** because we use it twice: once to compare and once to copy.

</details>

<details class="code-example" markdown="1">
<summary>Show code: scale</summary>

```text
scale:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start scale's frame
  mov $0, %rcx            # i = 0
  jmp scale_test          # go to the test
scale_loop:
  mov (%rdi,%rcx,8), %rax # %rax = arr[i]
  imul %rdx, %rax         # %rax = arr[i] * k
  mov %rax, (%rdi,%rcx,8) # arr[i] = %rax
  add $1, %rcx            # i++
scale_test:
  cmp %rsi, %rcx          # compare i with length
  jl scale_loop           # if i < length, go back
  pop %rbp                # restore main's %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09scale%0A%20%20.type%09scale%2C%20%40function%0A%0Ascale%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20scale's%20frame%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20jmp%20scale_test%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Ascale_loop%3A%0A%20%20mov%20(%25rdi%2C%25rcx%2C8)%2C%20%25rax%20%23%20%25rax%20%3D%20arr%5Bi%5D%0A%20%20imul%20%25rdx%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20arr%5Bi%5D%20*%20k%0A%20%20mov%20%25rax%2C%20(%25rdi%2C%25rcx%2C8)%20%23%20arr%5Bi%5D%20%3D%20%25rax%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Ascale_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20length%0A%20%20jl%20scale_loop%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20length%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09scale%2C%20.-scale%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2448%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20arr%0A%20%20movq%20%242%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20arr%5B0%5D%20%3D%202%0A%20%20movq%20%247%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20arr%5B1%5D%20%3D%207%0A%20%20movq%20%241%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20arr%5B2%5D%20%3D%201%0A%20%20movq%20%248%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20arr%5B3%5D%20%3D%208%0A%20%20movq%20%242%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20arr%5B4%5D%20%3D%202%0A%20%20lea%20-40(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20arr%0A%20%20mov%20%245%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20length%0A%20%20mov%20%2410%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%20%23%20third%20argument%3A%20k%0A%20%20call%20scale%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20scale(arr%2C%205%2C%2010)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). The Stack Content table ends with 0x14, 0x46, 0xa, 0x50, 0x14, which is 20, 70, 10, 80, 20.

- **Read, multiply, write back.** `imul` has to put its result in a register, and `mov` can't go memory to memory, so the element goes through `%rax`.
- **The store uses the same operand as the load.** `(%rdi,%rcx,8)` is `arr[i]` on either side.
- **`main` is the same as before,** plus `mov $10, %rdx` for `k`, and `call scale`.

</details>

<details class="code-example" markdown="1">
<summary>Answer: int instead of long</summary>

- **The scale becomes 4:** `(%rdi,%rcx,4)`. A constant index changes too: `arr[5]` is `20(%rdi)`, not `40(%rdi)`.
- **The compiler would also use the 32-bit register pieces,** like `%edx`, for the elements. That's why we use `long`.

</details>

Next time: structs in assembly.
