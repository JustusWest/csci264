---
title: "22 if Statements in Assembly"
published: false
---

[*Reading: Dive into Systems §7.4.2*](https://diveintosystems.org/book/C7-x86_64/if_statements.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}). In class we write the code live in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64).

Last time we saw the condition code flags, `cmp` and `test`, the jump instructions, and `goto`. Today we put them together to read an `if`/`else` in assembly and translate it back to C. Then we look at `cmov`, which handles some `if`s without jumping at all.

## getSmallest in Assembly

Here is the book's `getSmallest` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

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

```text
getSmallest:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  mov    %edi,-0x14(%rbp)
   7:  mov    %esi,-0x18(%rbp)
   a:  mov    -0x14(%rbp),%eax
   d:  cmp    -0x18(%rbp),%eax
  10:  jle    1a <getSmallest+0x1a>
  12:  mov    -0x18(%rbp),%eax
  15:  mov    %eax,-0x4(%rbp)
  18:  jmp    20 <getSmallest+0x20>
  1a:  mov    -0x14(%rbp),%eax
  1d:  mov    %eax,-0x4(%rbp)
  20:  mov    -0x4(%rbp),%eax
  23:  pop    %rbp
  24:  ret
```

- **`x` and `y` arrive in `%edi` and `%esi`.** The first two parameters of a function always come in these registers. We'll see why when we get to functions. The function copies them to the stack: `x` at `-0x14(%rbp)` and `y` at `-0x18(%rbp)`. `smallest` is at `-0x4(%rbp)`.
- **`cmp -0x18(%rbp),%eax` compares `x` with `y`.** `jle` jumps to `1a` if `x <= y`.
- **`jmp 20` skips over the code at `1a`.** Without it, both assignments would run.

The book's listing is "lightly edited." It leaves out the store to `smallest` at `-0x4(%rbp)` and keeps the result in `%eax` instead.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — both paths</p>

Trace `getSmallest` for each call. Starting at `10`, which addresses run, and what does it return?

<div class="board-setup" markdown="1">

| Call | Addresses that run | Returns |
|---|---|---|
| `getSmallest(3, 8)` | ? | ? |
| `getSmallest(8, 3)` | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Call | Addresses that run | Returns |
|---|---|---|
| `getSmallest(3, 8)` | `10` (jumps), `1a`, `1d`, `20`, `23`, `24` | 3 |
| `getSmallest(8, 3)` | `10` (falls through), `12`, `15`, `18` (jumps), `20`, `23`, `24` | 3 |

Each call runs exactly one of the two blocks, `12`–`18` or `1a`–`1d`. That is an `if`/`else`.

</details>

</div>

Let's run it in the ASM Visualizer. In Arithmetic Mode the variables start on the stack, so `x` is at `-8(%rbp)` and `y` is at `-16(%rbp)`, and the result is left in `%rax`.

<details class="code-example" markdown="1">
<summary>Show code: getSmallest</summary>

Set the `%rbp - 8` stack slot to 8 and the `%rbp - 16` slot to 3.

```text
mov -8(%rbp), %rax      # copy x to %rax
cmp -16(%rbp), %rax     # compare x with y
jle assign_x            # if x <= y, jump to assign_x
mov -16(%rbp), %rax     # smallest = y
jmp done
assign_x:
mov -8(%rbp), %rax      # smallest = x
done:
```

`%rax` ends at 3. Run it again with `x` = 3 and `y` = 8, and watch which lines the arrows skip.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-8%28%25rbp%29%2C%20%25rax%0Acmp%20-16%28%25rbp%29%2C%20%25rax%0Ajle%20assign_x%0Amov%20-16%28%25rbp%29%2C%20%25rax%0Ajmp%20done%0Aassign_x%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax%0Adone%3A/0/0/3/8/0/0/0)

</details>

## From Assembly to C

To turn assembly back into C, go in three steps:

1. **Annotate.** Write C-like pseudocode next to each line, as we have been doing.
2. **Write the `goto` form.** Every jump becomes one line of C. A `cmp` and a conditional jump become `if (...) goto ...;`, and a `jmp` becomes `goto ...;`. Every jump target gets a label.
3. **Rewrite it without `goto`.** The `goto`s that jump over blocks of code become an `if` or an `if`/`else`.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — getSmallest back to C</p>

Write the `goto` form of `getSmallest` from its assembly, one piece at a time. Then rewrite it as an `if`/`else`.

<div class="board-setup" markdown="1">

| Assembly | `goto` form |
|---|---|
| `d`–`10`: `cmp`, `jle 1a` | ? |
| `12`–`15` | ? |
| `18`: `jmp 20` | ? |
| `1a`–`1d` | ? |
| `20`–`24` | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Assembly | `goto` form |
|---|---|
| `d`–`10`: `cmp`, `jle 1a` | `if (x <= y) goto assign_x;` |
| `12`–`15` | `smallest = y;` |
| `18`: `jmp 20` | `goto done;` |
| `1a`–`1d` | `assign_x:`<br>`smallest = x;` |
| `20`–`24` | `done:`<br>`return smallest;` |

All together, then without `goto`:

```c
int getSmallest(int x, int y) {
    int smallest;
    if (x <= y) {           // cmp, jle
        goto assign_x;
    }
    smallest = y;           // 12, 15
    goto done;              // 18: jmp
assign_x:
    smallest = x;           // 1a, 1d
done:
    return smallest;        // 20
}
```

```c
int getSmallest(int x, int y) {
    int smallest;
    if (x <= y) {
        smallest = x;
    }
    else {
        smallest = y;
    }
    return smallest;
}
```

</details>

</div>

That isn't quite the C we started with. The original tested `x > y`; ours tests `x <= y` and has the two branches swapped. Both do the same thing, and both compile to the same assembly.

### The Compiler Flips the Test

Every `if`/`else` turns into the same `goto` form:

```c
if (condition) {
    then_statement;
}
else {
    else_statement;
}
```

```c
if (!condition) {
    goto else;
}
then_statement;
goto done;
else:
    else_statement;
done:
```

- **The jump happens when the condition is false.** It jumps over the `then` part. So the jump in the assembly is usually the opposite of the test in the C: `>` becomes `jle`, `==` becomes `jne`, `<` becomes `jge`.
- **When you translate back, you can keep it or flip it.** Keep the test and the `then` part is the code at the jump's target, as in Example 2. Flip it and the `then` part is the code that falls through. Either answer is correct.
- **An `if` with no `else` has only one jump**, over the body: `if (!condition) goto done; then_statement; done:`. This is the `JZ ENDIF` demo on the simple CPU from earlier in the course.

## Conditional Moves

Jumps are a problem for a pipeline. Recall from earlier in the course that the CPU has to guess which way a jump goes and keep fetching, and a wrong guess means flushing the pipeline. One fix was **eager execution**: work out both answers and keep the right one. x86-64 does this with a **conditional move**.

| Instruction | In C |
|---|---|
| `cmovX S, D` | `if (condition) D = S` |

The suffixes are the same as the jumps: `cmove`, `cmovne`, `cmovs`, `cmovns`, `cmovg`, `cmovl`, `cmova`, `cmovb`, and so on. We have already seen one: `div4` used `cmovs`, "move if negative."

C's ternary operator, `result = condition ? a : b;`, is an `if`/`else` that picks a value, which is exactly what `cmov` does. Here is `getSmallest` written with one ([open both versions in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuXG5pbnQgZ2V0U21hbGxlc3RfY21vdihpbnQgeCwgaW50IHkpIHtcbiAgICByZXR1cm4geCA%2BIHkgPyB5IDogeDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

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

- **No jumps.** `y` goes into `%eax` first. If `x <= y`, `cmovle` replaces it with `x`. Every instruction runs every time.
- **The compiler decides, not you.** With `-O1` ([open it with `-O1`](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBnZXRTbWFsbGVzdChpbnQgeCwgaW50IHkpIHtcbiAgICBpbnQgc21hbGxlc3Q7XG4gICAgaWYgKHggPiB5KSB7XG4gICAgICAgIHNtYWxsZXN0ID0geTtcbiAgICB9XG4gICAgZWxzZSB7XG4gICAgICAgIHNtYWxsZXN0ID0geDtcbiAgICB9XG4gICAgcmV0dXJuIHNtYWxsZXN0O1xufVxuXG5pbnQgZ2V0U21hbGxlc3RfY21vdihpbnQgeCwgaW50IHkpIHtcbiAgICByZXR1cm4geCA%2BIHkgPyB5IDogeDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiItTzEiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)), the original `getSmallest` with its `if`/`else` compiles to the exact same code as the ternary version:

```text
getSmallest:
   0:  cmp    %esi,%edi           # compare x with y
   2:  mov    %esi,%eax           # %eax = y
   4:  cmovle %edi,%eax           # if x <= y, %eax = x
   7:  ret
```

The `mov` between the `cmp` and the `cmovle` is fine, because `mov` doesn't change the flags.

<details class="code-example" markdown="1">
<summary>Show code: getSmallest with cmov</summary>

Set the `%rbp - 8` stack slot (`x`) to 8 and the `%rbp - 16` slot (`y`) to 3.

```text
mov -8(%rbp), %rdx      # %rdx = x
mov -16(%rbp), %rax     # %rax = y
cmp %rax, %rdx          # compare x with y
cmovle %rdx, %rax       # if x <= y, %rax = x
```

`%rax` ends at 3. Every line runs, whatever `x` and `y` are.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-8%28%25rbp%29%2C%20%25rdx%0Amov%20-16%28%25rbp%29%2C%20%25rax%0Acmp%20%25rax%2C%20%25rdx%0Acmovle%20%25rdx%2C%20%25rax/0/0/3/8/0/0/0)

</details>

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
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  mov    %edi,-0x4(%rbp)
   7:  cmpl   $0x0,-0x4(%rbp)
   b:  jns    10 <mystery+0x10>
   d:  negl   -0x4(%rbp)
  10:  mov    -0x4(%rbp),%eax
  13:  pop    %rbp
  14:  ret
```

1. Annotate it, write the `goto` form, then write it as plain C. What does `mystery` compute?
2. Predict: with `-O1`, will the compiler use a jump or a `cmov`?
3. Check your C by running this version in the [ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/cmpq%20%240%2C%20-8%28%25rbp%29%0Ajns%20done%0Anegq%20-8%28%25rbp%29%0Adone%3A%0Amov%20-8%28%25rbp%29%2C%20%25rax/0/0/0/-5/0/0/0), with `x` at `-8(%rbp)`. It starts with `x` = -5. Try a positive `x` too.

```text
cmpq $0, -8(%rbp)
jns  done
negq -8(%rbp)
done:
mov  -8(%rbp), %rax
```

<details class="code-example" markdown="1">
<summary>Show answers</summary>

1. `cmpl $0x0,-0x4(%rbp)` computes `x - 0`, so the flags describe `x`. `jns` jumps if `x` is not negative.

    ```c
    int mystery(int x) {
        if (x >= 0) {
            goto done;
        }
        x = -x;
    done:
        return x;
    }
    ```

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

3. With `x` = -5, `jns` falls through, `negq` runs, and `%rax` ends at 5. With a positive `x`, `jns` jumps over the `negq`. Here is [the `-O1` version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rcx%2C%20%25rax%0Aneg%20%25rax%0Acmovs%20%25rcx%2C%20%25rax/0/0/0/0/0/-5/0), with `x` in `%rcx`.

</details>

{% comment %}
INSTRUCTOR: before showing the getSmallest assembly, ask which jump they expect for x > y. Most will say jg. The jle is the setup for "the compiler flips the test."
INSTRUCTOR: draw an arrow from each jump to its target on the projected listing (10 -> 1a, 18 -> 20). Example 1 is then just following arrows.
INSTRUCTOR: for Example 2, have pairs write the goto form on paper first. If someone flips the test back to x > y, that is the other correct answer; put both on the board.
INSTRUCTOR: before showing the -O1 getSmallest, ask whether they think the compiler will keep the jumps. The ternary version already uses cmovle at -O0.
INSTRUCTOR: the listings in these notes come from gcc 13 at -O0 and -O1 with -fcf-protection=none (no endbr64). Compiler Explorer's x86-64 gcc 16.2 gave identical output for all of them.
INSTRUCTOR: all visualizer programs were run on the visualizer and give the results shown. The links were built from the Arithmetic Mode URL format; click through them before class.
INSTRUCTOR: the Try It is A6/A8-style reverse engineering. Give them five minutes in pairs before opening the answers.
{% endcomment %}

Next time we look at loops, which are jumps backward.
