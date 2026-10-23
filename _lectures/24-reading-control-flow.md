---
title: "24 Reading Control Flow"
published: true
---

[*No new reading. Review Dive into Systems §7.4*](https://diveintosystems.org/book/C7-x86_64/conditional_control_loops.html)

To check your answers, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

So far we've gone from C to assembly: `if` statements jump forward, and loops jump backward. Today we go the other way. Each problem below is a function compiled by `gcc`. Your job is to recover the C. This is how you read code you didn't write, and it's the kind of long problem you'll see on exams.

The listings are the compiler's code, cleaned up a little: they use the 64-bit registers, 8-byte stack slots, and labels instead of addresses. Compiler Explorer's version uses `%eax` and friends and shows addresses, but the instructions are the same.

Work in pairs, on paper.

## Reading Compiled Code

1. **Name the parameters.** Without optimization, the compiler copies each parameter to the stack at the top of the function. The first parameter arrives in `%rdi` and the second in `%rsi`. Next time we see why.
2. **Annotate every line.** Use the variable names, not the stack offsets.
3. **Draw an arrow for every jump.**
4. **Match the arrows to C:**

| You see | In C |
|---|---|
| A jump forward over some code | `if`. The jump is the condition, flipped |
| A `jmp` at the end of that code, over more code | `else` |
| A jump backward | A loop. The jump at the bottom is the condition, not flipped |
| A `jmp` down to the test before the loop | `while` or `for` |

5. **Write the C, then clean it up.** Pick `for` or `while`, and turn hex constants into something readable.
6. **Test it.** Trace it with small values and say what the function does in one sentence.

## Problem 1

<div class="board-example" markdown="1">
<p class="board-title">Problem 1 — int mystery1(int x)</p>

Compiled without optimization:

<div class="board-setup" markdown="1">

```text
mystery1:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)
    cmp $0x0, -8(%rbp)
    jns L1
    movq $0x0, -16(%rbp)
    jmp L3
L1:
    cmp $0x64, -8(%rbp)
    jle L2
    movq $0x64, -16(%rbp)
    jmp L3
L2:
    mov -8(%rbp), %rax
    mov %rax, -16(%rbp)
L3:
    mov -16(%rbp), %rax
    pop %rbp
    ret
```

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
mystery1:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)          # x
    cmp $0x0, -8(%rbp)         # compare x with 0
    jns L1                      # if x is not negative, skip ahead
    movq $0x0, -16(%rbp)        # result = 0
    jmp L3                      # go to the end
L1:
    cmp $0x64, -8(%rbp)        # compare x with 100
    jle L2                      # if x <= 100, skip ahead
    movq $0x64, -16(%rbp)       # result = 100
    jmp L3                      # go to the end
L2:
    mov -8(%rbp), %rax          # %rax = x
    mov %rax, -16(%rbp)         # result = x
L3:
    mov -16(%rbp), %rax         # return result
    pop %rbp
    ret
```

```c
int mystery1(int x) {
    int result;
    if (x < 0) {
        result = 0;
    }
    else if (x > 100) {
        result = 100;
    }
    else {
        result = x;
    }
    return result;
}
```

`mystery1` keeps `x` between 0 and 100. This is called **clamping**.

- **`0x64` is 100.** Constants in a listing are in hex.
- **Both `jmp`s go to `L3`.** Each branch ends by jumping over the rest of the chain to the `return`. An `else if` chain is a row of forward jumps that all end up in the same place.

With `-O1` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5MShpbnQgeCkge1xuICAgIGludCByZXN1bHQ7XG4gICAgaWYgKHggPCAwKSB7XG4gICAgICAgIHJlc3VsdCA9IDA7XG4gICAgfVxuICAgIGVsc2UgaWYgKHggPiAxMDApIHtcbiAgICAgICAgcmVzdWx0ID0gMTAwO1xuICAgIH1cbiAgICBlbHNlIHtcbiAgICAgICAgcmVzdWx0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHJlc3VsdDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)), there are no jumps at all:

```text
mystery1:
    mov $0x64, %rax             # result = 100
    cmp %rax, %rdi              # compare x with 100
    cmovle %rdi, %rax           # if x <= 100, result = x
    test %rax, %rax             # is result negative?
    mov $0x0, %rdx              # %rdx = 0
    cmovs %rdx, %rax            # if so, result = 0
    ret
```

It checks 100 first and 0 second, the opposite of the C. The answer is the same, so the compiler is free to do that.

</details>

</div>

## Problem 2

<div class="board-example" markdown="1">
<p class="board-title">Problem 2 — int mystery2(char c)</p>

Compiled without optimization. `c` is a `char`, so its value is an ASCII code. An ASCII table will help.

<div class="board-setup" markdown="1">

```text
mystery2:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)
    cmp $0x2f, -8(%rbp)
    jle L1
    cmp $0x39, -8(%rbp)
    jg L1
    movq $0x1, -16(%rbp)
    jmp L2
L1:
    movq $0x0, -16(%rbp)
L2:
    mov -16(%rbp), %rax
    pop %rbp
    ret
```

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
mystery2:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)          # c
    cmp $0x2f, -8(%rbp)        # compare c with '/'
    jle L1                      # if c <= '/', go to the else
    cmp $0x39, -8(%rbp)        # compare c with '9'
    jg L1                       # if c > '9', go to the else
    movq $0x1, -16(%rbp)        # result = 1
    jmp L2                      # skip the else
L1:
    movq $0x0, -16(%rbp)        # result = 0
L2:
    mov -16(%rbp), %rax         # return result
    pop %rbp
    ret
```

```c
int mystery2(char c) {
    int result;
    if (c >= '0' && c <= '9') {
        result = 1;
    }
    else {
        result = 0;
    }
    return result;
}
```

`mystery2` returns 1 if `c` is a digit character, and 0 if not.

- **Two jumps to the same label is `&&`.** If either test fails, jump straight to the `else`. The second test only runs if the first one passed, which is C's short-circuit evaluation. (For `||` it flips: if the first test passes, jump straight into the body.)
- **`0x39` is `'9'` and `0x2f` is `'/'`**, the character just before `'0'`. The compiler turned `c >= '0'` into `c > '/'`. They mean the same thing for whole numbers. `c > 47 && c <= 57` is a correct answer too, but `'0'` and `'9'` say what the code is for.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5MihjaGFyIGMpIHtcbiAgICBpbnQgcmVzdWx0O1xuICAgIGlmIChjID49ICcwJyAmJiBjIDw9ICc5Jykge1xuICAgICAgICByZXN1bHQgPSAxO1xuICAgIH1cbiAgICBlbHNlIHtcbiAgICAgICAgcmVzdWx0ID0gMDtcbiAgICB9XG4gICAgcmV0dXJuIHJlc3VsdDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)

</details>

</div>

## Problem 3

<div class="board-example" markdown="1">
<p class="board-title">Problem 3 — int mystery3(int a, int b)</p>

Compiled without optimization. After you write the C, trace it with the values in the table: what are `a` and `b` after each trip through the loop?

<div class="board-setup" markdown="1">

```text
mystery3:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    jmp L3
L1:
    mov -8(%rbp), %rax
    cmp -16(%rbp), %rax
    jle L2
    mov -16(%rbp), %rax
    sub %rax, -8(%rbp)
    jmp L3
L2:
    mov -8(%rbp), %rax
    sub %rax, -16(%rbp)
L3:
    mov -8(%rbp), %rax
    cmp -16(%rbp), %rax
    jne L1
    mov -8(%rbp), %rax
    pop %rbp
    ret
```

| Call | `a`, `b` after each trip | Returns |
|---|---|---|
| `mystery3(12, 18)` | ? | ? |
| `mystery3(9, 4)` | ? | ? |
| `mystery3(5, 0)` | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
mystery3:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)          # a
    mov %rsi, -16(%rbp)         # b
    jmp L3                      # go to the test
L1:
    mov -8(%rbp), %rax          # %rax = a
    cmp -16(%rbp), %rax         # compare a with b
    jle L2                      # if a <= b, go to the else
    mov -16(%rbp), %rax         # %rax = b
    sub %rax, -8(%rbp)          # a = a - b
    jmp L3                      # skip the else
L2:
    mov -8(%rbp), %rax          # %rax = a
    sub %rax, -16(%rbp)         # b = b - a
L3:
    mov -8(%rbp), %rax          # %rax = a
    cmp -16(%rbp), %rax         # compare a with b
    jne L1                      # if a != b, go back
    mov -8(%rbp), %rax          # return a
    pop %rbp
    ret
```

```c
int mystery3(int a, int b) {
    while (a != b) {
        if (a > b) {
            a = a - b;
        }
        else {
            b = b - a;
        }
    }
    return a;
}
```

| Call | `a`, `b` after each trip | Returns |
|---|---|---|
| `mystery3(12, 18)` | (12, 6), (6, 6) | 6 |
| `mystery3(9, 4)` | (5, 4), (1, 4), (1, 3), (1, 2), (1, 1) | 1 |
| `mystery3(5, 0)` | (5, 0), (5, 0), ... | never returns |

`mystery3` computes the **greatest common divisor** of `a` and `b`. It is Euclid's algorithm, using subtraction instead of `%`.

- **The backward arrow, from `jne L1` up to `L1`, is the loop.** The `jmp L3` before it goes down to the test first, so it is a `while`.
- **There is an `if`/`else` inside the body.** Its `jmp L3` lands on the loop's test, because the end of the `if`/`else` is the end of the body.
- **`mystery3(5, 0)` is an infinite loop.** `a - 0` is still `a`, so nothing changes. Reading the code tells you what it does, bugs included.

Everything from `jmp L3` through `jne L1` runs as is in the ASM Visualizer, with `a` at `-8(%rbp)` and `b` at `-16(%rbp)`. [Open it in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/jmp%20L3%0AL1%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax%0Acmp%20-16%28%25rbp%29%2C%20%25rax%0Ajle%20L2%0Amov%20-16%28%25rbp%29%2C%20%25rax%0Asub%20%25rax%2C%20-8%28%25rbp%29%0Ajmp%20L3%0AL2%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax%0Asub%20%25rax%2C%20-16%28%25rbp%29%0AL3%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax%0Acmp%20-16%28%25rbp%29%2C%20%25rax%0Ajne%20L1/0/0/18/12/0/0/0), starting with `a` = 12 and `b` = 18. `%rax` ends at 6.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5MyhpbnQgYSwgaW50IGIpIHtcbiAgICB3aGlsZSAoYSAhPSBiKSB7XG4gICAgICAgIGlmIChhID4gYikge1xuICAgICAgICAgICAgYSA9IGEgLSBiO1xuICAgICAgICB9XG4gICAgICAgIGVsc2Uge1xuICAgICAgICAgICAgYiA9IGIgLSBhO1xuICAgICAgICB9XG4gICAgfVxuICAgIHJldHVybiBhO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)

</details>

</div>

## Problem 4

<div class="board-example" markdown="1">
<p class="board-title">Problem 4 — int mystery4(int x, int y)</p>

This one is compiled **with** `-O1`. Nothing goes on the stack: `x` stays in `%rdi`, `y` stays in `%rsi`, and the return value goes in `%rax`.

<div class="board-setup" markdown="1">

```text
mystery4:
    test %rsi, %rsi
    jle L2
    mov $0x1, %rax
L1:
    imul %rdi, %rax
    sub $0x1, %rsi
    jne L1
    ret
L2:
    mov $0x1, %rax
    ret
```

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
mystery4:
    test %rsi, %rsi             # is y <= 0?
    jle L2                      # if so, return 1
    mov $0x1, %rax              # result = 1
L1:
    imul %rdi, %rax             # result = result * x
    sub $0x1, %rsi              # y = y - 1
    jne L1                      # if y != 0, go back
    ret                         # return result
L2:
    mov $0x1, %rax              # result = 1
    ret                         # return result
```

```c
int mystery4(int x, int y) {
    int result = 1;
    while (y > 0) {
        result = result * x;
        y--;
    }
    return result;
}
```

`mystery4` computes `x` to the power `y`. `mystery4(3, 4)` is 81.

- **The backward arrow, from `jne L1` up to `L1`, is the loop.** There is no `jmp` down to a test, so the body runs first, like a `do`-`while`. But the `test` at the top skips the loop entirely when `y <= 0`, so the C is a `while`. This is the same shape as last time's `-O1` `sumUp`.
- **There is no `cmp` before `jne`.** `sub` is arithmetic, so it already set the flags. `jne` asks whether `y` just reached 0.
- **`y > 0` became `y != 0`.** `y` starts positive and goes down by 1, so it hits 0 exactly when `y > 0` first fails.
- **There are two `ret`s.** With optimization, a function can return from more than one place.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5NChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgcmVzdWx0ID0gMTtcbiAgICB3aGlsZSAoeSA%2BIDApIHtcbiAgICAgICAgcmVzdWx0ID0gcmVzdWx0ICogeDtcbiAgICAgICAgeS0tO1xuICAgIH1cbiAgICByZXR1cm4gcmVzdWx0O1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)

</details>

</div>

## More Practice

<div class="board-example" markdown="1">
<p class="board-title">Problem 5 — int mystery5(int n, int k)</p>

Compiled without optimization. Recall that `idiv` divides `%rax`, and puts the quotient in `%rax` and the remainder in `%rdx`.

<div class="board-setup" markdown="1">

```text
mystery5:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)
    mov %rsi, -16(%rbp)
    movq $0x0, -24(%rbp)
    movq $0x1, -32(%rbp)
    jmp L3
L1:
    mov -32(%rbp), %rax
    mov $0x0, %rdx
    idivq -16(%rbp)
    mov %rdx, %rax
    test %rax, %rax
    jne L2
    addq $0x1, -24(%rbp)
L2:
    addq $0x1, -32(%rbp)
L3:
    mov -32(%rbp), %rax
    cmp -8(%rbp), %rax
    jle L1
    mov -24(%rbp), %rax
    pop %rbp
    ret
```

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
mystery5:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, -8(%rbp)          # n
    mov %rsi, -16(%rbp)         # k
    movq $0x0, -24(%rbp)        # count = 0
    movq $0x1, -32(%rbp)        # i = 1
    jmp L3                      # go to the test
L1:
    mov -32(%rbp), %rax         # %rax = i
    mov $0x0, %rdx              # clear %rdx for the divide
    idivq -16(%rbp)             # %rdx = i % k
    mov %rdx, %rax              # %rax = i % k
    test %rax, %rax             # is i % k zero?
    jne L2                      # if not, skip count++
    addq $0x1, -24(%rbp)        # count++
L2:
    addq $0x1, -32(%rbp)        # i++
L3:
    mov -32(%rbp), %rax         # %rax = i
    cmp -8(%rbp), %rax          # compare i with n
    jle L1                      # if i <= n, go back
    mov -24(%rbp), %rax         # return count
    pop %rbp
    ret
```

```c
int mystery5(int n, int k) {
    int count = 0;
    for (int i = 1; i <= n; i++) {
        if (i % k == 0) {
            count++;
        }
    }
    return count;
}
```

`mystery5` counts the multiples of `k` from 1 to `n`. `mystery5(10, 3)` is 3.

- **It's a `for` loop.** `i` is set before the loop, tested in the condition, and goes up by 1 at the end of every trip.
- **The `if` has no `else`,** so there is no `jmp` over one. `jne L2` just skips `count++`.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5NShpbnQgbiwgaW50IGspIHtcbiAgICBpbnQgY291bnQgPSAwO1xuICAgIGZvciAoaW50IGkgPSAxOyBpIDw9IG47IGkrKykge1xuICAgICAgICBpZiAoaSAlIGsgPT0gMCkge1xuICAgICAgICAgICAgY291bnQrKztcbiAgICAgICAgfVxuICAgIH1cbiAgICByZXR1cm4gY291bnQ7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)

</details>

</div>

{% comment %}
INSTRUCTOR: practice day, no new material. Pairs on paper. For each problem, project the bare listing, give them about 8 minutes, then work it on the board: annotate, draw the arrows, write the C. The Answers button swaps the bare listing for the annotated one.
INSTRUCTOR: this is also the buffer day. If loops ran over last time, finish them first and drop Problems 4 and 5. Problem 5 is for students to do at home, or in class if the room runs fast.
INSTRUCTOR: the listings are hand-adapted from gcc 13 output (-O0 and -O1, x86-64 Linux, -fcf-protection=none): 32-bit registers widened to 64-bit, 4-byte stack slots moved to 8-byte slots at -8, -16, -24, -32, and addresses replaced with labels. Problem 2's char load (mov %al / cmpb) became a plain 8-byte load and cmp. Problem 5's cltd/idivl became mov $0, %rdx / idivq, which is right because i is never negative. Every adapted listing was assembled and run against the C over a range of inputs (including negatives for 1 and 4, all ASCII codes for 2), with no mismatches.
INSTRUCTOR: the Compiler Explorer links show the real compiler output, with %eax, 4-byte slots, and addresses. Open each before class and check it matches the instructions here.
INSTRUCTOR: the Problem 3 visualizer link was built from the Arithmetic Mode URL format and not opened. Click through it before class.
INSTRUCTOR: Problem 2 is the only new idea today: && as two jumps to the same label. Let them find it before saying it. If they ask about -O1 for mystery2, it uses sub, an unsigned compare, and setbe; skip it.
INSTRUCTOR: Problem 1 at -O1 checks 100 before 0. Ask why that's allowed before showing the answer bullet.
INSTRUCTOR: Problem 3's (5, 0) row never stops. Trace it on paper; don't run it in the visualizer.
{% endcomment %}

For more practice, try the [Exercises from the book for §7.4](https://diveintosystems.org/exercises/section-7_4.html).

Next time we look at functions: `call` and `ret`, and how parameters end up in `%rdi` and `%rsi`.
