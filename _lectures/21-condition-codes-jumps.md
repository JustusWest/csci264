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

### OF and CF - Overflow Rules

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

Suppose we have 4 bit registers. For each `cmp S, D`, compute `D - S` and the four flags:

<div class="board-setup" markdown="1">

| D | S | D - S | ZF | SF | OF | CF | D < S? |
|---|---|---|---|---|---|---|---|
| `0101` (5) | `0101` (5) | ? | ? | ? | ? | ? | ? |
| `0011` (3) | `0101` (5) | ? | ? | ? | ? | ? | ? |
| `1111` (-1) | `0001` (1) | ? | ? | ? | ? | ? | ? |
| `0100` (4) | `1011` (-5) | ? | ? | ? | ? | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| D | S | D - S | ZF | SF | OF | CF | D < S? |
|---|---|---|---|---|---|---|---|
| `0101` (5) | `0101` (5) | `0000` (0) | 1 | 0 | 0 | 0 | no |
| `0011` (3) | `0101` (5) | `1110` (-2) | 0 | 1 | 0 | 1 | yes |
| `1111` (-1) | `0001` (1) | `1110` (-2) | 0 | 1 | 0 | 0 | yes |
| `0100` (4) | `1011` (-5) | `1001` (-7) | 0 | 1 | 1 | 1 | no |

- **Row 3:** as unsigned numbers, D is `1111` = 15 and S is 1. 15 - 1 doesn't wrap, so CF is 0. Signed, D is less than S. Unsigned, D is greater.
- **Row 4:** 4 - (-5) is 9, which doesn't fit in 4 bits, so the result looks negative. SF says "less," but 4 is not less than -5. OF is 1 to indicate SF is wrong.

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

Jumps check the flags to determine what to do, so we use `cmp` before a jump to set the flags.

**Read `cmp S, D` followed by `jX L` as "jump to L if D X S."** `cmp %rcx,%rax` then `jg L` jumps if `%rax > %rcx`. It reads backwards, the same as `sub`.

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

## Try It

Assume `x` is in `%rcx` and `y` is in `%rdx`. Write a program that puts the larger of the two in `%rax`.

<details class="code-example" markdown="1">
<summary>Show answers</summary>

```text
mov %rcx, %rax
cmp %rdx, %rcx
jge done
mov %rdx, %rax
done:
```
[ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rcx%2C%20%25rax%0Acmp%20%25rdx%2C%20%25rcx%0Ajge%20done%0Amov%20%25rdx%2C%20%25rax%0Adone%3A/0/0/0/0/0/3/8)

</details>


For some practice, try the [Exercises from the book for §7.4](https://diveintosystems.org/exercises/section-7_4.html).

Next time we put these together to read `if` statements in assembly.
