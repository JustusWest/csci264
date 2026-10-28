---
title: "23 Loops in Assembly"
published: true
---

[*Reading: Dive into Systems §7.4.3*](https://diveintosystems.org/book/C7-x86_64/loops.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we translated `if`/`else` into compares and jumps. An `if` jumps forward to skip code. Today we look at loops, which jump backward to repeat code.

## sumUp in Assembly

Here is the book's `sumUp`, which adds up the numbers from 1 to `n`:

```c
int sumUp(int n) {
    int total = 0;
    int i = 1;
    while (i <= n) {
        total += i;
        i += 1;
    }
    return total;
}
```

A `while` loop tests the condition, runs the body, and goes back to the test. When the test fails, it leaves the loop.

Let's write it ourselves in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64). We'll put `n` at `-8(%rbp)`, and keep `total` in `%rax` and `i` in `%rcx`.

<details class="code-example" markdown="1">
<summary>Show code: sumUp</summary>

Set the `%rbp - 8` stack slot (`n`) to 4.

```text
mov $0, %rax            # total = 0
mov $1, %rcx            # i = 1
top:
cmp -8(%rbp), %rcx      # compare i with n
jg done                 # if i > n, leave the loop
add %rcx, %rax          # total += i
add $1, %rcx            # i += 1
jmp top                 # go back to the test
done:
```

`%rax` ends at 10, which is 1 + 2 + 3 + 4.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%240%2C%20%25rax%0Amov%20%241%2C%20%25rcx%0Atop%3A%0Acmp%20-8%28%25rbp%29%2C%20%25rcx%0Ajg%20done%0Aadd%20%25rcx%2C%20%25rax%0Aadd%20%241%2C%20%25rcx%0Ajmp%20top%0Adone%3A/0/0/0/4/0/0/0)

</details>

- **`jmp top` is what makes it a loop.** It jumps backward, so the test and the body run again.
- **The test is flipped, like last time.** The C keeps going while `i <= n`. The jump *leaves* the loop, so it has to happen when `i <= n` is false: `jg`.
- **The test is at the top.** If `n` is 0, `jg` jumps on the first test and the body never runs, just like the C.

## The Compiler's Version

Here is what `gcc` makes from `sumUp` without optimization ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBzdW1VcChpbnQgbikge1xuICAgIGludCB0b3RhbCA9IDA7XG4gICAgaW50IGkgPSAxO1xuICAgIHdoaWxlIChpIDw9IG4pIHtcbiAgICAgICAgdG90YWwgKz0gaTtcbiAgICAgICAgaSArPSAxO1xuICAgIH1cbiAgICByZXR1cm4gdG90YWw7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)):

```text
sumUp:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  mov    %edi,-0x14(%rbp)    # n
   7:  movl   $0x0,-0x8(%rbp)     # total = 0
   e:  movl   $0x1,-0x4(%rbp)     # i = 1
  15:  jmp    21 <sumUp+0x21>     # go to the test
  17:  mov    -0x4(%rbp),%eax     # %eax = i
  1a:  add    %eax,-0x8(%rbp)     # total += i
  1d:  addl   $0x1,-0x4(%rbp)     # i += 1
  21:  mov    -0x4(%rbp),%eax     # %eax = i
  24:  cmp    -0x14(%rbp),%eax    # compare i with n
  27:  jle    17 <sumUp+0x17>     # if i <= n, go back to the body
  29:  mov    -0x8(%rbp),%eax     # return total
  2c:  pop    %rbp
  2d:  ret
```

Draw an arrow for each jump. The `jmp` at `15` goes forward to `21`. The `jle` at `27` goes back to `17`.

- **The test is at the bottom.** The compiler jumps down to the test once, at the start. After that, every trip through the body ends with the test.
- **The jump is not flipped.** `jle` jumps back when `i <= n`, which is exactly the C condition. This jump keeps us *in* the loop instead of leaving it.
- **The backward arrow is the loop.** Everything it covers, `17` through `27`, is the body and the test.
- **`n`, `total`, and `i` all live on the stack.** Without optimization, every use reads memory and every change writes it back.

Why move the test to the bottom? Count the jumps.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — counting jumps</p>

For `n` = 3, how many jump instructions run in our version? In the compiler's?

<details class="board-answer" markdown="1">
<summary>Answers</summary>

- **Ours:** `jg` runs 4 times and `jmp` runs 3 times, so 7 jumps.
- **The compiler's:** `jmp` runs once and `jle` runs 4 times, so 5 jumps.

Each trip through our loop has two jumps. Each trip through the compiler's has one. Recall that jumps are a problem for the pipeline. In a loop that runs a million times, that adds up.

</details>

</div>

Here is the compiler's layout in the visualizer, with our registers:

<details class="code-example" markdown="1">
<summary>Show code: sumUp, test at the bottom</summary>

Set the `%rbp - 8` stack slot (`n`) to 4.

```text
mov $0, %rax            # total = 0
mov $1, %rcx            # i = 1
jmp check               # go to the test
body:
add %rcx, %rax          # total += i
add $1, %rcx            # i += 1
check:
cmp -8(%rbp), %rcx      # compare i with n
jle body                # if i <= n, go back to the body
```

`%rax` ends at 10. Try `n` = 0 too: `jmp` lands on the test, `jle` doesn't jump, and the body never runs.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%240%2C%20%25rax%0Amov%20%241%2C%20%25rcx%0Ajmp%20check%0Abody%3A%0Aadd%20%25rcx%2C%20%25rax%0Aadd%20%241%2C%20%25rcx%0Acheck%3A%0Acmp%20-8%28%25rbp%29%2C%20%25rcx%0Ajle%20body/0/0/0/4/0/0/0)

</details>

### Optimization

The compiler evaluates fewer jumps, which is fewer clock cycles and makes pipelining easier.


Here is the compiler's unoptimized version in Arithmetic Mode, with `n` at `-8(%rbp)`, `total` at `-16(%rbp)`, and `i` at `-24(%rbp)`. Like the compiler's code, every variable lives on the stack ([open it in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/movq%20%240%2C%20-16%28%25rbp%29%0Amovq%20%241%2C%20-24%28%25rbp%29%0Ajmp%20check%0Abody%3A%0Amov%20-24%28%25rbp%29%2C%20%25rax%0Aadd%20%25rax%2C%20-16%28%25rbp%29%0Aaddq%20%241%2C%20-24%28%25rbp%29%0Acheck%3A%0Amov%20-24%28%25rbp%29%2C%20%25rax%0Acmp%20-8%28%25rbp%29%2C%20%25rax%0Ajle%20body%0Amov%20-16%28%25rbp%29%2C%20%25rax/0/0/0/4/0/0/0)):

How else can we optimize this to save clock cycles?


```text
movq $0, -16(%rbp)      # total = 0
movq $1, -24(%rbp)      # i = 1
jmp check               # go to the test
body:
mov -24(%rbp), %rax     # %rax = i
add %rax, -16(%rbp)     # total += i
addq $1, -24(%rbp)      # i += 1
check:
mov -24(%rbp), %rax     # %rax = i
cmp -8(%rbp), %rax      # compare i with n
jle body                # if i <= n, go back to the body
mov -16(%rbp), %rax     # return total
```

<details class="code-example" markdown="1">
<summary>Show code: sumUp, test at the bottom</summary>

Here is what the compiler produces for `sumUp` with `-O1` (optimization on) ([open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBzdW1VcChpbnQgbikge1xuICAgIGludCB0b3RhbCA9IDA7XG4gICAgaW50IGkgPSAxO1xuICAgIHdoaWxlIChpIDw9IG4pIHtcbiAgICAgICAgdG90YWwgKz0gaTtcbiAgICAgICAgaSArPSAxO1xuICAgIH1cbiAgICByZXR1cm4gdG90YWw7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiLU8xIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)):

```text
sumUp:
   0:  test   %edi,%edi           # is n <= 0?
   2:  jle    1d <sumUp+0x1d>     # if so, total = 0 (at 1d)
   4:  add    $0x1,%edi           # %edi = n + 1
   7:  mov    $0x1,%eax           # i = 1
   c:  mov    $0x0,%edx           # total = 0
  11:  add    %eax,%edx           # total += i
  13:  add    $0x1,%eax           # i += 1
  16:  cmp    %edi,%eax           # compare i with n + 1
  18:  jne    11 <sumUp+0x11>     # if i != n + 1, go back
  1a:  mov    %edx,%eax           # return total
  1c:  ret
  1d:  mov    $0x0,%edx           # total = 0
  22:  jmp    1a <sumUp+0x1a>
```

- **Everything is in registers.** `n` is in `%edi`, `i` is in `%eax`, and `total` is in `%edx`. No stack.
- **The loop is a `do`-`while`.** The `test` at the top runs once, to decide whether the loop runs at all. After that, `11` through `18` is the body and the test, with one jump.
- **`i <= n` became `i != n + 1`.** `i` counts up by 1 starting at 1, so it reaches `n + 1` exactly when `i <= n` first fails.

</details>

## for Loops

Here is `sumUp` written with a `for` loop:

```c
int sumUp2(int n) {
    int total = 0;
    int i;
    for (i = 1; i <= n; i++) {
        total += i;
    }
    return total;
}
```

It compiles to exactly the same assembly as `sumUp` ([open both in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBzdW1VcChpbnQgbikge1xuICAgIGludCB0b3RhbCA9IDA7XG4gICAgaW50IGkgPSAxO1xuICAgIHdoaWxlIChpIDw9IG4pIHtcbiAgICAgICAgdG90YWwgKz0gaTtcbiAgICAgICAgaSArPSAxO1xuICAgIH1cbiAgICByZXR1cm4gdG90YWw7XG59XG5cbmludCBzdW1VcDIoaW50IG4pIHtcbiAgICBpbnQgdG90YWwgPSAwO1xuICAgIGludCBpO1xuICAgIGZvciAoaSA9IDE7IGkgPD0gbjsgaSsrKSB7XG4gICAgICAgIHRvdGFsICs9IGk7XG4gICAgfVxuICAgIHJldHVybiB0b3RhbDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)). That's because a `for` loop is a `while` loop with the parts moved around:

```c
for ( <initialization>; <boolean expression>; <step> ) {
    <body>
}
```

is the same as:

```c
<initialization>;
while ( <boolean expression> ) {
    <body>
    <step>
}
```

So the assembly can't tell you whether the programmer wrote a `for` or a `while`. When you translate back, pick whichever reads better. If a variable is set before the loop, tested in the condition, and changed by the same amount at the end of every trip through the body, write a `for`.

## Reading a Loop

To translate a loop back to C:

1. **Annotate every line.**
2. **Draw an arrow for every jump.**
3. **A backward arrow is a loop.** The `cmp` (or `test`) and the jump at the bottom of the arrow are the loop's condition. The jump goes back when the condition is true, so it is not flipped.
4. **Everything else the arrow covers is the body.**
5. **Look for a `jmp` down to the test before the loop.** If there is one, the test runs first, so it is a `while` or a `for`. If not, the body runs first, so it is a `do`-`while`. (At `-O1`, look for a check before the loop that skips it entirely. That is a `while` too.)

## Try It

`factorial` computes `n!`, the product 1 × 2 × ... × `n`. For example, `5!` is 120. `0!` and `1!` are both 1.

```c
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result = result * i;
    }
    return result;
}
```

Write it in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64), with `n` at `-8(%rbp)` and the result in `%rax`. Test it with `n` = 5, 1, and 0.

<details class="code-example" markdown="1">
<summary>Show answers</summary>

Keep `result` in `%rax` and `i` in `%rcx`. With the test at the top, like our first `sumUp`:

```text
mov $1, %rax            # result = 1
mov $2, %rcx            # i = 2
top:
cmp -8(%rbp), %rcx      # compare i with n
jg done                 # if i > n, leave the loop
imul %rcx, %rax         # result = result * i
add $1, %rcx            # i++
jmp top                 # go back to the test
done:
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%241%2C%20%25rax%0Amov%20%242%2C%20%25rcx%0Atop%3A%0Acmp%20-8%28%25rbp%29%2C%20%25rcx%0Ajg%20done%0Aimul%20%25rcx%2C%20%25rax%0Aadd%20%241%2C%20%25rcx%0Ajmp%20top%0Adone%3A/0/0/0/5/0/0/0)

Move the test to the bottom to get one jump per trip through the loop:

```text
mov $1, %rax            # result = 1
mov $2, %rcx            # i = 2
jmp check               # go to the test
body:
imul %rcx, %rax         # result = result * i
add $1, %rcx            # i++
check:
cmp -8(%rbp), %rcx      # compare i with n
jle body                # if i <= n, go back to the body
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%241%2C%20%25rax%0Amov%20%242%2C%20%25rcx%0Ajmp%20check%0Abody%3A%0Aimul%20%25rcx%2C%20%25rax%0Aadd%20%241%2C%20%25rcx%0Acheck%3A%0Acmp%20-8%28%25rbp%29%2C%20%25rcx%0Ajle%20body/0/0/0/5/0/0/0)

- **`n` = 5 gives 120 (`0x78`).** For `n` = 0 or 1, `i` starts at 2, which is already bigger than `n`, so the body never runs and `%rax` stays 1.
- **The bottom-test version is the compiler's layout** ([compare it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBmYWN0b3JpYWwoaW50IG4pIHtcbiAgICBpbnQgcmVzdWx0ID0gMTtcbiAgICBmb3IgKGludCBpID0gMjsgaSA8PSBuOyBpKyspIHtcbiAgICAgICAgcmVzdWx0ID0gcmVzdWx0ICogaTtcbiAgICB9XG4gICAgcmV0dXJuIHJlc3VsdDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)). It has the same `jmp` to the test and `jle` back, with `result` and `i` on the stack.
- **Factorials grow fast.** `13!` doesn't fit in a 32-bit `int`, so in C, `factorial(13)` overflows. The visualizer uses 64-bit registers, so it gets the right answer up to `20!`.

</details>



For some practice, try the [Exercises from the book for §7.4](https://diveintosystems.org/exercises/section-7_4.html).

Next time we practice reading compiled `if` statements and loops and turning them back into C.
