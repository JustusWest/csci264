---
title: "21 Condition Codes and Jumps"
published: true
---

[*Reading: Dive into Systems §7.4.1*](https://diveintosystems.org/book/C7-x86_64/preliminaries.html)

To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}). In class we write the code live in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64).

Last time we finished the arithmetic instructions. Today we cover the basics of conditional logic in Assembly, compares and jumps.

## Condition Codes

Recall from CPU architecture that the ALU maintains some special bits called flags. x86-64 maintains four flags, each flag is one bit in the **FLAGS** register. The ASM Visualizer calls it RFLAG and lists the flags that are set by name.

| Flag | Set to 1 when the result... |
|---|---|
| ZF (zero flag)| is zero |
| SF (sign flag)| is negative (its sign bit is 1) |
| OF (overflow flag)| overflowed as a signed number |
| CF (carry-out flag)| overflowed as an unsigned number |

The arithmetic and bitwise instructions we have seen so far (`add`, `sub`, `neg`, `and`, ...) all set the flags on every operation. `mov`, `lea`, `push`, and `pop` don't.

### OF and CF Are the Overflow Rules

Earlier in the course we saw that overflow has different rules for signed an unsigned numbers:

- **CF is the unsigned rule.** It is 1 if the result wrapped around past 0. For a subtraction `D - S`, that happens when D is smaller than S as unsigned numbers.
- **OF is the signed rule.** Rewrite a subtraction as an addition, then check the signs: if the two operands have the same sign and the result's sign is different, OF is 1.

The CPU doesn't know whether the operands are meant to be signed or unsigned, so it sets both flags on over operation. The programmer (you) needs to determine which flag to read.

## cmp and test

| Instruction | Computes | Keeps |
|---|---|---|
| `cmp S, D` | `D - S` | only the flags |
| `test S, D` | `D & S` | only the flags |

- **`cmp` is a `sub` and `test` is an `and`, but the result is thrown away.** D doesn't change. Only the flags do.
- **`cmp S, D` is D minus S**, the same order as `sub`. `cmp %rcx,%rax` compares `%rax` with `%rcx`.
- **The usual operand rules apply.** At most one operand can be memory, and a constant can only be S: `cmp $5,%rax` works, `cmp %rax,$5` doesn't.
- **`test %rax,%rax` asks "is it zero? is it negative?"** `x & x` is just `x`, so ZF and SF describe `%rax` itself. It does the same job as `cmp $0,%rax` in fewer bytes, so you will see it a lot.
- **`test` with a mask checks bits.** `test $1,%rax` sets ZF when `%rax` is even.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — flags after cmp</p>

Pretend registers are 4 bits, like when we first did overflow. For each `cmp S, D`, compute `D - S` and the four flags:

<div class="board-setup" markdown="1">

| D | S | D - S | ZF | SF | OF | CF |
|---|---|---|---|---|---|---|
| 5 | 5 | ? | ? | ? | ? | ? |
| 3 | 5 | ? | ? | ? | ? | ? |
| -1 | 1 | ? | ? | ? | ? | ? |
| 4 | -5 | ? | ? | ? | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| D | S | D - S | ZF | SF | OF | CF |
|---|---|---|---|---|---|---|
| 5 | 5 | `0000` (0) | 1 | 0 | 0 | 0 |
| 3 | 5 | `1110` (-2) | 0 | 1 | 0 | 1 |
| -1 | 1 | `1110` (-2) | 0 | 1 | 0 | 0 |
| 4 | -5 | `1001` (-7) | 0 | 1 | 1 | 1 |

- **Row 3:** as unsigned numbers, D is `1111` = 15 and S is 1. 15 - 1 doesn't wrap, so CF is 0. Signed, D is less than S. Unsigned, D is greater. Same bits, different answers.
- **Row 4:** 4 - (-5) is 9, which doesn't fit in 4 bits, so the result looks negative. SF says "less," but 4 is not less than -5. OF is 1 to say SF is wrong.

</details>

</div>

So signed "less than" is not just SF. It is **SF and OF are different**. Unsigned "less than" is **CF is 1**. That is why there are two sets of jumps.

## Jumps

Normally `%rip`, the program counter, moves on to the next instruction. A **jump** writes a new address into `%rip` instead. Jumping forward skips code, like the body of an `if`. Jumping backward repeats code, like a loop.

| Instruction | Meaning |
|---|---|
| `jmp L` | Always jump to L |

L is a **label**: a name followed by a colon, marking the next instruction. Labels look different depending on where you see them:

- In `gcc -S` output, labels inside a function look like `.L2:`.
- In Compiler Explorer and `objdump`, the label is replaced by an address: `jle 1a <getSmallest+0x1a>` jumps to address `1a`, which is 0x1a bytes from the start of `getSmallest`.
- The book's listings come from GDB, which writes the same thing as `jle 0x4005b0 <getSmallest+26>`, with the offset in decimal.
- In the ASM Visualizer you write the labels yourself, using any name, like `done:`.

### Conditional Jumps

A **conditional jump** checks the flags. If its condition is true it jumps. If not, it does nothing and the next instruction runs. This is called **falling through**.

| Signed | Unsigned | Jump if |
|---|---|---|
| `je` (`jz`) | | equal, or zero |
| `jne` (`jnz`) | | not equal, or not zero |
| `js` | | negative |
| `jns` | | not negative |
| `jg` | `ja` | greater (>) |
| `jge` | `jae` | greater or equal (>=) |
| `jl` | `jb` | less (<) |
| `jle` | `jbe` | less or equal (<=) |

Don't memorize the table. Sound out the letters: **j** jump, **n** not, **e** equal, **g**/**l** greater/less (signed), **a**/**b** above/below (unsigned). So `jge` is "jump if greater or equal," and its other name `jnl` is "jump if not less."

- **Read `cmp S, D` followed by `jX L` as "jump to L if D X S."** `cmp %rcx,%rax` then `jg L` jumps if `%rax > %rcx`. It reads backwards, the same as `sub`.
- **Assembly has no types, but the jump shows the type.** Greater and less mean the C variables were signed. Above and below mean they were unsigned. This is the same as `sar` and `shr` from last time.
- **You don't need to know which flags each jump checks.** For the curious: `jl` jumps when SF and OF are different, and `jb` jumps when CF is 1.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — which jumps are taken?</p>

After `cmp %rcx,%rax`, is each jump taken?

<div class="board-setup" markdown="1">

| `%rax` | `%rcx` | `je` | `jne` | `jg` | `jl` | `ja` | `jb` |
|---|---|---|---|---|---|---|---|
| 5 | 5 | ? | ? | ? | ? | ? | ? |
| 7 | 2 | ? | ? | ? | ? | ? | ? |
| -1 | 1 | ? | ? | ? | ? | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| `%rax` | `%rcx` | `je` | `jne` | `jg` | `jl` | `ja` | `jb` |
|---|---|---|---|---|---|---|---|
| 5 | 5 | yes | no | no | no | no | no |
| 7 | 2 | no | yes | yes | no | yes | no |
| -1 | 1 | no | yes | no | yes | yes | no |

As an unsigned number, -1 is `0xffffffffffffffff`, the largest 64-bit number, so it is *above* 1.

</details>

</div>

Let's check the last row in the ASM Visualizer, with `%rax` = -1 and `%rcx` = 1.

<details class="code-example" markdown="1">
<summary>Show code: signed vs. unsigned jumps</summary>

Set `%rax` to -1 and `%rcx` to 1.

```text
cmp %rcx, %rax      # flags from %rax - %rcx
jl  less            # signed: jump if %rax < %rcx
add $1, %rdx        # skipped if jl jumps
less:
cmp %rcx, %rax
jb  below           # unsigned: jump if %rax < %rcx
add $2, %rdx        # skipped if jb jumps
below:
```

`jl` jumps, so the first `add` never runs. `jb` falls through, so `%rdx` ends at 2. After each `cmp`, RFLAG shows only `SF`: the result, -2, is negative, with no overflow and no wrap around.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/cmp%20%25rcx%2C%20%25rax%0Ajl%20less%0Aadd%20%241%2C%20%25rdx%0Aless%3A%0Acmp%20%25rcx%2C%20%25rax%0Ajb%20below%0Aadd%20%242%2C%20%25rdx%0Abelow%3A/0/0/0/0/-1/1/0)

</details>

## goto

C has its own jump: **`goto`**. `goto done;` jumps to the line labeled `done:`. Here is the book's `getSmallest` written both ways:

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

```c
int getSmallest(int x, int y) {
    int smallest;
    if (x <= y) {
        goto else_statement;
    }
    smallest = y;
    goto done;
else_statement:
    smallest = x;
done:
    return smallest;
}
```

- **Don't write `goto` in your own code.** It makes code hard to follow. Edsger Dijkstra's 1968 letter *Go To Statement Considered Harmful* is the famous argument against it.
- **It is useful for reading assembly.** `goto` is `jmp`, and `if (...) goto` is a `cmp` followed by a conditional jump. `gcc` rewrites `if`s and loops into this form before it writes the assembly.

Notice that the `goto` version tests `x <= y`, the opposite of the original. Next time we'll see why the compiler does that.

## Try It

`x` is in `%rcx` and `y` is in `%rdx`:

```text
mov %rcx, %rax
cmp %rdx, %rcx
jge done
mov %rdx, %rax
done:
```

1. Predict what is in `%rax` at the end for each test, then check in the [ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rcx%2C%20%25rax%0Acmp%20%25rdx%2C%20%25rcx%0Ajge%20done%0Amov%20%25rdx%2C%20%25rax%0Adone%3A/0/0/0/0/0/3/8), which starts with the first test:
    - `x` = 3, `y` = 8
    - `x` = -2, `y` = -7
    - `x` = -1, `y` = 1
2. In a few words, what does this code compute?
3. Change `jge` to `jae`. Which test gives a different answer, and why?

<details class="code-example" markdown="1">
<summary>Show answers</summary>

1. `%rax` ends at 8, then -2, then 1.
2. The larger of `x` and `y`. `%rax` starts as `x`. If `x >= y`, the jump skips the line that replaces it with `y`.
3. The last test. With `jae`, `x` = -1 is compared as an unsigned number, the largest one there is, so the jump is taken and `%rax` ends at -1. The other two tests have the same answer either way. Picking the wrong set of jumps is an easy mistake to make, and only some tests catch it.

[Open the jae version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rcx%2C%20%25rax%0Acmp%20%25rdx%2C%20%25rcx%0Ajae%20done%0Amov%20%25rdx%2C%20%25rax%0Adone%3A/0/0/0/0/0/-1/1)

</details>

{% comment %}
INSTRUCTOR: work Example 1 on the board in 4 bits. Have them do the D - S column first, then the flags. Row 3 is the signed/unsigned punchline; row 4 is why "SF means less" (what the book says) is only true when OF is 0.
INSTRUCTOR: for Example 2, ask for a show of hands on each cell of the last row before revealing. Most will say ja is not taken.
INSTRUCTOR: at step 0 the visualizer shows ZF set. That comes from the hidden setup code; ignore it. RFLAG only names ZF, SF, OF, and CF, even when the hex value has other bits set.
INSTRUCTOR: all three visualizer programs were run on the visualizer and give the results shown. The links were built from the Arithmetic Mode URL format; click through them before class.
INSTRUCTOR: the Try It is the pattern next week's lab needs: compare, then skip a line. Question 3 is the signed vs. unsigned trap that a negative side length will catch in that lab.
{% endcomment %}

For some practice, try the [Exercises from the book for §7.4](https://diveintosystems.org/exercises/section-7_4.html).

Next time we put these together to read `if` statements in assembly.
