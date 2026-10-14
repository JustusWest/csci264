---
title: "20 Arithmetic Instructions"
published: true
---

[*Reading: Dive into Systems §7.3*](https://diveintosystems.org/book/C7-x86_64/arithmetic.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}). In class we write the code live in a [blank ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64).

Last time we saw `mov`, `add`, `sub`, `push`, and `pop`, and how a function sets up its stack frame. Today we finish the arithmetic instructions: the rest of the math, shifts, bitwise instructions, and `lea`.

## More Arithmetic

| Instruction | In C |
|---|---|
| `inc D` | `D++` |
| `dec D` | `D--` |
| `neg D` | `D = -D` |
| `imul S, D` | `D *= S` |
| `idiv S` | `%rax = %rax / S`<br>`%rdx = %rax % S` |

- **`inc`, `dec`, and `neg` take one operand.** It is both the input and the output.
- **`neg` is two's complement negation**: flip the bits, then add 1.
- **`imul` keeps only as many bits as the destination holds.** If the product doesn't fit, it overflows and wraps around, the same as it does in C.

`idiv` is the odd one out. It has one operand, the divisor, and it always uses `%rax` and `%rdx`:

- **The quotient goes in `%rax` and the remainder in `%rdx`.** C's `/` and `%` compile to the same instruction. They just keep different registers.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — idiv</p>

```text
idiv %rcx
```

<div class="board-setup" markdown="1">

| Start | After |
|---|---|
| `%rax` = 17, `%rcx` = 5 | `%rax` = ?, `%rdx` = ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Start | After |
|---|---|
| `%rax` = 17, `%rcx` = 5 | `%rax` = 3, `%rdx` = 2 |

</details>

</div>

## Shifts

Bit shifting instructions move all the bits in a register right or left a specified number of times.

A right shift has to decide what to fill in on the left:

- A **logical** shift fills with 0s. This is right for unsigned values.
- An **arithmetic** shift fills with copies of the sign bit, so a negative number stays negative. 

The compiler picks based on the type. Unsigned values always get a logical shift. For signed values the C standard leaves it up to the compiler, and `gcc` uses an arithmetic shift.

| Instruction | In C | Fills with |
|---|---|---|
| `sal v, D` | `D <<= v` | 0s |
| `shl v, D` | `D <<= v` | 0s |
| `sar v, D` | `D >>= v` (signed) | copies of the sign bit |
| `shr v, D` | `D >>= v` (unsigned) | 0s |

- **Assembly has no types, but the type still shows up.** If you see `sar`, the C variable was signed. If you see `shr`, it was unsigned.
- **`sal` and `shl` are the same instruction.** A left shift fills with 0s either way, so they assemble to the same bytes. `gcc -S` writes `sal`, but `objdump` and Compiler Explorer print `shl`.
- **The shift amount `v` is a constant or `%cl`**, the lowest byte of `%rcx`. No other register works. For `x >> n`, the compiler first copies `n` into `%ecx`.


Let's try it in the ASM Visualizer, starting with `%rax` = -16.

<details class="code-example" markdown="1">
<summary>Show code: sar vs. shr</summary>

Set `%rax` to -16.

```text
mov %rax, %rcx      # copy -16 into %rcx
sar $2, %rax        # arithmetic shift
shr $2, %rcx        # logical shift
```

After `sar`, `%rax` is -4 (`0xfffffffffffffffc`). After `shr`, `%rcx` is `0x3ffffffffffffffc`, a huge positive number.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rax%2C%20%25rcx%0Asar%20%242%2C%20%25rax%0Ashr%20%242%2C%20%25rcx/0/0/0/0/-16/0/0)

</details>

### Shifting Instead of Multiplying

Multiplying and dividing take much longer than shifting. When one side is a constant power of 2, the compiler shifts instead. `x * 8` becomes `sal $3`. It also shifts and adds: `x * 5` is `x * 4 + x`.

Dividing is trickier. Shifting right rounds *down*, toward negative infinity. C's `/` rounds toward *zero*. For positive numbers these are the same. For negative numbers they are not:

```text
-7 >> 2   is  -2     (-1.75 rounded down)
-7 / 4    is  -1     (-1.75 rounded toward zero)
```

The book's example, turning `77 / 4` into `77 >> 2`, only works because 77 is positive. See the Try It below for what the compiler does about this.

Don't replace `*` and `/` with shifts in your own C code. The compiler already does it when it is safe, and `x * 8` is easier to read than `x << 3`.

## Bitwise Instructions

The bitwise instructions perform the standard logical operators on each bit individually:


| Instruction | In C |
|---|---|
| `and S, D` | `D &= S` |
| `or S, D` | <code>D &#124;= S</code> |
| `xor S, D` | `D ^= S` |
| `not D` | `D = ~D` |

- **`not` is not `neg`.** `not` flips the bits. `neg` flips the bits *and adds 1*. So `~x` is always `-x - 1`, which is why `~6` is -7.
- **`xor %eax,%eax` sets `%eax` to 0.** Anything XOR itself is 0. It does the same thing as `mov $0x0,%eax` but takes 2 bytes instead of 5, so you will see it in optimized code.
- **The compiler uses `and` for `%` by a power of 2.** For an unsigned `x`, `x % 4` is `x & 3`: the remainder is just the bits that a shift by 2 would throw away. Like division, a signed `x` needs extra steps to handle negative numbers.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — 6 and 3</p>

In 4 bits, 6 is `0110` and 3 is `0011`:

<div class="board-setup" markdown="1">

| Expression | Bits | Value |
|---|---|---|
| `6 and 3` | ? | ? |
| `6 or 3`  | ? | ? |
| `6 xor 3` | ? | ? |
| `not 6` | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Expression | Bits | Value |
|---|---|---|
| `6 and 3` | `0010` | 2 |
| `6 or 3` | `0111` | 7 |
| `6 xor 3` | `0101` | 5 |
| `not 6` | `1001` | -7 |

</details>

</div>

### Masks

The most common use of the bitwise operators is to pick out or change certain bits. The constant used to do this is called a **mask**.

- `x & 1` is 1 if `x` is odd and 0 if it is even.
- `x & 0xF` keeps the lowest 4 bits and clears the rest.
- `x | 0x20` sets bit 5. In ASCII, that turns `'A'` into `'a'`.
- `x xor x` is always 0.




## The lea Instruction

**`lea`** stands for **load effective address**. It takes a memory operand, does the address arithmetic, and stops there. It puts the address itself in the destination instead of looking it up in memory.

- `mov 8(%rax),%rbx` computes `%rax + 8`, *goes to memory at that address*, and puts what it finds in `%rbx`.
- `lea 8(%rax),%rbx` computes `%rax + 8` and puts that number in `%rbx`.

Recall that a memory operand is like a pointer dereference: `-0x4(%rbp)` is `*(%rbp - 4)`. `lea` is the same thing without the `*`.

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — lea</p>

Using the values from the book, `%rax` is 5, `%rdx` is 4, and `%rcx` is 0x808. Each row starts from these same values:

<div class="board-setup" markdown="1">

| Instruction | Computes | Result in `%rax` |
|---|---|---|
| `lea 8(%rax),%rax` | ? | ? |
| `lea (%rax,%rdx),%rax` | ? | ? |
| `lea (,%rax,4),%rax` | ? | ? |
| `lea -0x8(%rcx),%rax` | ? | ? |
| `lea -0x4(%rcx,%rdx,2),%rax` | ? | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Instruction | Computes | Result in `%rax` |
|---|---|---|
| `lea 8(%rax),%rax` | 8 + `%rax` | 13 |
| `lea (%rax,%rdx),%rax` | `%rax` + `%rdx` | 9 |
| `lea (,%rax,4),%rax` | `%rax` × 4 | 20 |
| `lea -0x8(%rcx),%rax` | `%rcx` - 8 | 0x800 |
| `lea -0x4(%rcx,%rdx,2),%rax` | `%rcx` + `%rdx` × 2 - 4 | 0x80c |

None of them read memory, even though 0x808 is an address.

</details>

</div>

### lea is &

Here is a function that takes the address of a local variable ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCBwb2ludCh2b2lkKSB7XG4gICAgaW50IHggPSA1O1xuICAgIGludCAqcCA9ICZ4O1xuICAgICpwID0gNztcbiAgICByZXR1cm4geDtcbn1cbiIsImNvbXBpbGVycyI6W3siaWQiOiJjZzE2MiIsIm9wdGlvbnMiOiIiLCJmaWx0ZXJzIjp7ImJpbmFyeU9iamVjdCI6dHJ1ZSwiaW50ZWwiOmZhbHNlLCJsYWJlbHMiOnRydWUsImxpYnJhcnlDb2RlIjp0cnVlLCJkaXJlY3RpdmVzIjp0cnVlLCJjb21tZW50T25seSI6dHJ1ZSwiZGVtYW5nbGUiOnRydWV9fV19XX0%3D)):

```c
int point(void) {
    int x = 5;
    int *p = &x;
    *p = 7;
    return x;
}
```

```text
point:
   0:  push   %rbp
   1:  mov    %rsp,%rbp
   4:  movl   $0x5,-0xc(%rbp)     # int x = 5;
   b:  lea    -0xc(%rbp),%rax     # int *p = &x;
   f:  mov    %rax,-0x8(%rbp)
  13:  mov    -0x8(%rbp),%rax     # *p = 7;
  17:  movl   $0x7,(%rax)
  1d:  mov    -0xc(%rbp),%eax     # return x;
  20:  pop    %rbp
  21:  ret
```

- **`&x` is `lea -0xc(%rbp),%rax`**: the address of `x`. Reading `x` is `mov -0xc(%rbp),%eax`. Same operand, different instruction.
- **`p` is 8 bytes** because it is a pointer, so it uses `%rax`, not `%eax`.
- **`*p = 7` is `movl $0x7,(%rax)`**: write to the address in `%rax`.

Let's compare `lea` and `mov` in the ASM Visualizer, with 42 stored at `-8(%rbp)`.

<details class="code-example" markdown="1">
<summary>Show code: lea vs. mov</summary>

Set the `%rbp - 8` stack slot to 42.

```text
lea -8(%rbp), %rcx    # %rcx = the address %rbp - 8
mov -8(%rbp), %rdx    # %rdx = the value at %rbp - 8
mov (%rcx), %rax      # %rax = the value at the address in %rcx
```

The `lea` puts the *address* `%rbp - 8` in `%rcx`. Both `mov`s get 42, once directly and once through the pointer in `%rcx`.

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/lea%20-8%28%25rbp%29%2C%20%25rcx%0Amov%20-8%28%25rbp%29%2C%20%25rdx%0Amov%20%28%25rcx%29%2C%20%25rax/0/0/0/42/0/0/0)

</details>

## Try It

[Open these in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6ImludCB0aW1lczgoaW50IHgpIHtcbiAgICByZXR1cm4geCAqIDg7XG59XG5cbmludCB0aW1lczUoaW50IHgpIHtcbiAgICByZXR1cm4geCAqIDU7XG59XG5cbmludCBkaXY0KGludCB4KSB7XG4gICAgcmV0dXJuIHggLyA0O1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D), but before you look at the output, predict what instructions `gcc` will use for each `return` line:

```c
int times8(int x) {
    return x * 8;
}

int times5(int x) {
    return x * 5;
}

int div4(int x) {
    return x / 4;
}
```

Then:

1. `div4` uses two instructions we haven't seen yet: `test` and `cmovs`. Together they mean "if `x` is negative, use `x + 3` instead." Why add 3? Run the code below in the ASM Visualizer and compare `%rcx` and `%rax` at the end.
2. What would `div4` look like if `x` were `unsigned`?

<details class="code-example" markdown="1">
<summary>Show code: dividing -7 by 4</summary>

Set `%rax` to -7.

```text
mov %rax, %rcx      # copy x into %rcx
sar $2, %rcx        # %rcx = x >> 2
add $3, %rax        # %rax = x + 3
sar $2, %rax        # %rax = (x + 3) >> 2
```

[Open the finished version in the ASM Visualizer](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20%25rax%2C%20%25rcx%0Asar%20%242%2C%20%25rcx%0Aadd%20%243%2C%20%25rax%0Asar%20%242%2C%20%25rax/0/0/0/0/-7/0/0)

</details>

<details class="code-example" markdown="1">
<summary>Show answers</summary>

```text
times8:
   7:  mov    -0x4(%rbp),%eax
   a:  shl    $0x3,%eax           # x * 8

times5:
  16:  mov    -0x4(%rbp),%edx
  19:  mov    %edx,%eax
  1b:  shl    $0x2,%eax           # x * 4
  1e:  add    %edx,%eax           # x * 4 + x

div4:
  29:  mov    -0x4(%rbp),%eax
  2c:  lea    0x3(%rax),%edx      # %edx = x + 3
  2f:  test   %eax,%eax           # if x is negative,
  31:  cmovs  %edx,%eax           #   %eax = x + 3
  34:  sar    $0x2,%eax           # divide by 4
```

None of them use `imul` or `idiv`. Compiler Explorer prints `shl`, but it is the same instruction as `sal`.

1. In the visualizer, `%rcx` ends at -2 and `%rax` ends at -1. `sar` alone rounds -1.75 down to -2, but C needs -1. Adding 3 (one less than the divisor) first pushes a negative number just far enough that rounding down gives the same answer as rounding toward zero. For a positive `x`, the extra lines do nothing.
2. With `unsigned x`, there are no negative numbers to worry about, so the whole thing is one `shr $0x2,%eax`.

</details>

{% comment %}
INSTRUCTOR: have them write predictions down in pairs before opening the link. Most will predict sal for x * 8, many will predict imul for x * 5, and almost nobody will predict the extra lines in div4.
INSTRUCTOR: the listings in these notes come from gcc 13 at -O0 on x86-64. Compiler Explorer's gcc may be newer; open each Compiler Explorer link before class and check that the output matches.
INSTRUCTOR: for each "Show code" block, open the blank visualizer link at the top, type the code live, and set the starting values. The "finished version" links were built by hand from the Arithmetic Mode URL format and not opened; click through them before class.
INSTRUCTOR: for each board example, project the set-up table, give them a few minutes, then work it on the board. The Answers button swaps the blank table for the filled one.
{% endcomment %}

For some practice, try the [Exercises from the book for §7.3](https://diveintosystems.org/exercises/section-7_3.html).

Next time we start on conditionals: condition codes, `cmp`, `test`, and jumps.
