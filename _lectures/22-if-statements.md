---
title: "22 if Statements in Assembly"
published: true
---

[*Reading: Dive into Systems §7.4.2*](https://diveintosystems.org/book/C7-x86_64/if_statements.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we saw the condition code flags, `cmp` and `test`, and the jump instructions. Today we put them together to read an `if`/`else` in assembly and translate it back to C. Then we look at `cmov`, which handles some `if`s without jumping at all.

## getSmallest in Assembly

Here is the book's `getSmallest`:

```c
int getSmallest(int x, int y) {
    int smallest;
    if (x > y) {
        smallest = y;
    }
    else {
        smallest = x;
    }
    return smallest;
}
```

Let's write it ourselves in [ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64). In Arithmetic Mode the variables start on the stack, so we'll put `x` at `-8(%rbp)` and `y` at `-16(%rbp)`, and keep `smallest` in `%rax`.

<details class="code-example" markdown="1">
<summary>Show code: getSmallest</summary>

Set the `%rbp - 8` stack slot (`x`) to 8 and the `%rbp - 16` slot (`y`) to 3.

```text
mov -8(%rbp), %rax      # copy x to %rax
cmp -16(%rbp), %rax     # compare x with y
jle assign_x            # if x <= y, jump to assign_x
mov -16(%rbp), %rax     # smallest = y
jmp done                # skip over assign_x
assign_x:
mov -8(%rbp), %rax      # smallest = x
done:
```

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-8%28%25rbp%29%2C%20%25rax%0Acmp%20-16%28%25rbp%29%2C%20%25rax%0Ajle%20assign_x%0Amov%20-16%28%25rbp%29%2C%20%25rax%0Ajmp%20done%0Aassign_x%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax%0Adone%3A/0/0/3/8/0/0/0)

</details>

- **`cmp -16(%rbp), %rax` compares `x` with `y`.** `jle` jumps to `assign_x` if `x <= y`.
- **The C says `x > y`, but the jump is `jle`.** The jump skips the code for `x > y`, so it has to happen when `x > y` is false.
- **`jmp done` skips over `assign_x`.** Without it, both assignments would run.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — both paths</p>

The visualizer numbers our lines starting at 20, so `mov -8(%rbp), %rax` is line 20 and `done:` is line 27. For each call, which lines run, and what is in `%rax` at the end?

<div class="board-setup" markdown="1">

| Call | Lines that run | `%rax` |
|---|---|---|
| `getSmallest(8, 3)` | ? | ? |
| `getSmallest(3, 8)` | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Call | Lines that run | `%rax` |
|---|---|---|
| `getSmallest(8, 3)` | 20, 21, 22 (falls through), 23, 24 (jumps) | 3 |
| `getSmallest(3, 8)` | 20, 21, 22 (jumps), 26 | 3 |

Each call runs exactly one of the two assignments, line 23 or line 26. That is an `if`/`else`.

</details>

</div>

Check both in the visualizer and watch which lines the arrows skip.

The compiler's version ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)) has the same `cmp`, `jle`, and `jmp`. It also stores `smallest` on the stack, and it shows addresses where we wrote labels.

## Conditional Moves

Jumps are a problem for a pipeline. Recall from earlier in the course that the CPU has to guess which way a jump goes and keep fetching, and a wrong guess means flushing the pipeline. One fix was **eager execution**: work out both answers and keep the right one. x86-64 does this with a **conditional move**.

| Instruction | In C |
|---|---|
| `cmov{suffix} S, D` | `if (condition) D = S` |

The suffixes are the same as the jumps: `cmove`, `cmovne`, `cmovs`, `cmovns`, `cmovg`, `cmovl`, `cmova`, `cmovb`, and so on.

Let's rewrite `getSmallest` in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64), this time with no jumps. Put `y` in `%rax` first, then replace it with `x` if `x <= y`.

<details class="code-example" markdown="1">
<summary>Show code: getSmallest with cmov</summary>

Set the `%rbp - 8` stack slot (`x`) to 8 and the `%rbp - 16` slot (`y`) to 3.

```text
mov -8(%rbp), %rdx      # %rdx = x
mov -16(%rbp), %rax     # %rax = y
cmp %rax, %rdx          # compare x with y
cmovle %rdx, %rax       # if x <= y, %rax = x
```

`%rax` ends at 3. Try `x` = 3 and `y` = 8 too.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-8%28%25rbp%29%2C%20%25rdx%0Amov%20-16%28%25rbp%29%2C%20%25rax%0Acmp%20%25rax%2C%20%25rdx%0Acmovle%20%25rdx%2C%20%25rax/0/0/3/8/0/0/0)

</details>

- **No jumps and no labels.** Every line runs every time. The only thing that depends on `x` and `y` is whether `cmovle` copies.
- **Both answers are ready before the test.** `x` is in `%rdx` and `y` is in `%rax`. `cmovle` just picks one.

### The Ternary Operator

C has an operator that does the same thing. The **ternary operator** `condition ? a : b` is `a` if the condition is true and `b` if it is false. It is an `if`/`else` that picks a value:

```c
result = condition ? a : b;
```

It is equivalent to:

```c
if (condition) {
    result = a;
}
else {
    result = b;
}
```

Here is `getSmallest` written with one ([open both versions in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuXG5pbnQgZ2V0U21hbGxlc3RfY21vdihpbnQgeCwgaW50IHkpIHtcbiAgICByZXR1cm4geCA%2BIHkgPyB5IDogeDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

```c
int getSmallest_cmov(int x, int y) {
    return x > y ? y : x;
}
```

```text
getSmallest_cmov:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  mov    %edi,-0x4(%rbp)
   7:  mov    %esi,-0x8(%rbp)
   a:  mov    -0x4(%rbp),%edx     # %edx = x
   d:  mov    -0x8(%rbp),%eax     # %eax = y
  10:  cmp    %eax,%edx           # compare x with y
  12:  cmovle %edx,%eax           # if x <= y, %eax = x
  15:  pop    %rbp
  16:  ret
```

- **It is the code we just wrote.** The instructions at `a` through `12` are our four lines, with `%edx` and `%eax` in place of `%rdx` and `%rax`. The rest is the stack frame and copying the parameters to the stack.
- **The compiler uses `cmov` here even without optimization.** A ternary is just choosing between two values, so it is a natural fit.

**The compiler decides, not you.** With `-O1` ([open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuXG5pbnQgZ2V0U21hbGxlc3RfY21vdihpbnQgeCwgaW50IHkpIHtcbiAgICByZXR1cm4geCA%2BIHkgPyB5IDogeDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)), the original `getSmallest` with its `if`/`else` compiles to the exact same code as the ternary version:

```text
getSmallest:
   0:  cmp    %esi,%edi           # compare x with y
   2:  mov    %esi,%eax           # %eax = y
   4:  cmovle %edi,%eax           # if x <= y, %eax = x
   7:  ret
```

The `mov` between the `cmp` and the `cmovle` is fine, because `mov` doesn't change the flags.

### When the Compiler Won't Use cmov

`cmov` runs both sides of the `if`. That is only safe when running the side you didn't want can't hurt. Here are two versions of a function that adds 1 to `*x`, unless `x` is `NULL` ([open them in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRkZWYuaD5cblxuaW50IGluY3JlbWVudFgoaW50ICp4KSB7XG4gICAgaWYgKHggIT0gTlVMTCkge1xuICAgICAgICByZXR1cm4gKCp4KSsrO1xuICAgIH1cbiAgICBlbHNlIHtcbiAgICAgICAgcmV0dXJuIDE7XG4gICAgfVxufVxuXG5pbnQgaW5jcmVtZW50WDIoaW50ICp4KSB7XG4gICAgcmV0dXJuIHggPyAoKngpKysgOiAxO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

```c
int incrementX(int *x) {
    if (x != NULL) {
        return (*x)++;
    }
    else {
        return 1;
    }
}

int incrementX2(int *x) {
    return x ? (*x)++ : 1;
}
```

Even with a ternary and `-O1`, both compile to the same code, with a jump:

```text
incrementX:
   0:  mov    $0x1,%eax           # result = 1
   5:  test   %rdi,%rdi           # is x NULL?
   8:  je     11 <incrementX+0x11>   # if so, skip to the end
   a:  mov    (%rdi),%eax         # result = *x
   c:  lea    0x1(%rax),%edx      # *x + 1
   f:  mov    %edx,(%rdi)         # store it back in *x
  11:  ret
```

With `cmov`, `*x` would be read even when `x` is `NULL`, and the program would crash with a segmentation fault. So the compiler plays it safe and jumps.

## Try It

Here is a mystery function, compiled without optimization. The C is in the answers.

```text
mystery:
    cmp $0, -8(%rbp)
    jns  done
    neg -8(%rbp)
    done:
    mov  -8(%rbp), %rax
```

1. Annotate it and try to re-write it as C. What does `mystery` compute?
2. Predict: with `-O1` (optimization on), will the compiler use a jump or a `cmov`?
3. Try running the Assembly in the [ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/cmpq%20%240%2C%20-8%28%25rbp%29%0Ajns%20done%0Anegq%20-8%28%25rbp%29%0Adone%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax/0/0/0/-5/0/0/0), with `x` at `-8(%rbp)`. It starts with `x` = -5. Try a positive `x` too.


<details class="code-example" markdown="1">
<summary>Show answers</summary>

1. `cmp $0, -8(%rbp)` computes `x - 0`, so the flags describe `x`. `jns` jumps over the `neg` if `x` is not negative, so `x = -x` only runs when `x < 0`.

    ```c
    int mystery(int x) {
        if (x < 0) {
            x = -x;
        }
        return x;
    }
    ```

    It is the absolute value of `x` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5KGludCB4KSB7XG4gICAgaWYgKHggPCAwKSB7XG4gICAgICAgIHggPSAteDtcbiAgICB9XG4gICAgcmV0dXJuIHg7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)). (Except for the most negative `int`, which has no positive version. `neg` overflows and gives it back unchanged.)

2. A `cmov`. Both sides are just values in registers, so running both is safe ([open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBteXN0ZXJ5KGludCB4KSB7XG4gICAgaWYgKHggPCAwKSB7XG4gICAgICAgIHggPSAteDtcbiAgICB9XG4gICAgcmV0dXJuIHg7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiLU8xIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)):

    ```text
    mystery:
       0:  mov    %edi,%eax           # %eax = x
       2:  neg    %eax                # %eax = -x
       4:  cmovs  %edi,%eax           # if -x is negative, %eax = x
       7:  ret
    ```

    There is no `cmp`. `neg` is arithmetic, so it already set the flags.

3. With `x` = -5, `jns` falls through, `neg` runs, and `%rax` ends at 5. With a positive `x`, `jns` jumps over the `neg`. Here is [the `-O1` version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rcx%2C%20%25rax%0Aneg%20%25rax%0Acmovs%20%25rcx%2C%20%25rax/0/0/0/0/0/-5/0), with `x` in `%rcx`.

</details>

Next time we look at loops, which are jumps backward.
