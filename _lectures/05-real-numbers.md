---
title: "05 Real Numbers in Binary"
published: false
---

[*Reading: Dive into Systems §4.8*](https://diveintosystems.org/book/C4-Binary/reals.html)

Unlike integers, which are countably infinite, the real numbers are *uncountably* infinite. Between any two of them sit infinitely many more, so no finite encoding can name them all.

Real number encodings therefore store only **approximations** of values, truncated to a predetermined number of bits.

We present two methods: **fixed point** and **floating point**.

## Fixed-Point Representation

Like a *decimal point*, we use a **binary point** to mark where the fractional portion of the number begins. In fixed point, this binary point never moves.

The digits after the binary point represent powers of two raised to a *negative* value.

![An eight-bit fixed-point number, 0b000101.10, with each digit labeled by its place value: the six bits before the binary point carry the places 2^5 down to 2^0, and the two bits after it carry 2^-1 and 2^-2]({{ "/figures/ch4/Fixed.png" | relative_url }})

Applying the standard unsigned formula shows:

```text
(0 × 2⁵) + (0 × 2⁴) + (0 × 2³) + (1 × 2²) + (0 × 2¹) + (1 × 2⁰) + (1 × 2⁻¹) + (0 × 2⁻²)

= 0 + 0 + 0 + 4 + 0 + 1 + 0.5 + 0  =  5.5
```

With two bits after the binary point, the fractional portion holds one of four sequences: `00` (.00), `01` (.25), `10` (.50), or `11` (.75). Thus two fractional bits allow a fixed-point number to represent fractional values that are precise to 0.25 (2<sup>−2</sup>). Adding a third bit increases the precision to 0.125 (2<sup>−3</sup>), and so on, with N bits after the binary point enabling 2<sup>−N</sup> precision.

### Rounding

That fixed precision is also the limitation. This encoding cannot represent 0.75 / 2 = 0.375, which would need a third bit after the binary point (`0b000000.011`). Truncating that rightmost 1 makes the result fit, but turns the answer into 0.25. The rounding is egregious here because we have so few bits, but every fixed-width encoding hits this wall somewhere.

Worse, rounding errors *compound* across intermediate calculations, so the result can depend on the order the operations are performed in:

```text
(0.75 / 2) * 3  =  0.75
(0.75 * 3) / 2  =  1.00
```

Notice that in many of these calculations a large number of the high-order bits are unused, but we still lose precision.

## Floating-Point Representation

At the cost of some additional complexity, modern systems use a **floating-point representation**, in which the binary point is free to move. The standard is IEEE 754.

![The 32-bit IEEE 754 floating-point format, partitioned into a 1-bit sign field at d31, an 8-bit exponent field spanning d30 through d23, and a 23-bit significand field spanning d22 through d0]({{ "/figures/ch4/IEEE754.png" | relative_url }})

The format splits the 32 bits into three regions.

**The significand** (sometimes called the mantissa) is the low-order 23 bits, d<sub>22</sub> through d<sub>0</sub>. It is the foundation of the value, and it behaves like the fixed-point fraction from the previous section — except that its value implicitly follows a leading `1.`

So if the significand holds `0b110000…0000`, the first bit contributes 0.5 (1 × 2<sup>−1</sup>) and the second contributes 0.25 (1 × 2<sup>−2</sup>). The rest are zeros. The significand is therefore 1.(0.5 + 0.25), or **1.75**.

**The exponent** is the next eight bits, d<sub>30</sub> through d<sub>23</sub>. It scales the significand to give a wide range: the significand gets multiplied by 2<sup>exponent − 127</sup>. The 127 is a **bias**, and it is what lets a float reach both very large and very small values.

**The sign bit** is the high-order bit, d<sub>31</sub> — positive (0) or negative (1).

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — decoding an IEEE 754 value</p>

Decode `0b11000001101101000000000000000000`.

Split the sequence into its three fields:

```text
  1   10000011   01101000000000000000000
  ^   ^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^
 sign  exponent         significand
```

**Significand.** The set bits sit in the 2<sup>−2</sup>, 2<sup>−3</sup>, and 2<sup>−5</sup> places:

```text
2⁻² + 2⁻³ + 2⁻⁵  =  0.25 + 0.125 + 0.03125  =  0.40625
```

Remember the implicit leading 1, so the significand contributes **1.40625**.

**Exponent.** `0b10000011` is 131 as an unsigned integer. Subtract the bias:

```text
2^(131 - 127)  =  2⁴  =  16
```

**Sign.** The high-order bit is 1, so the value is negative.

Putting it together:

```text
1.40625 × 16 × -1  =  -22.5
```

</div>

Floating point is clearly more complex than fixed point, and it buys a much wider range of representable values. But a fixed number of bits still can't represent every value, so rounding problems affect floating point just as they affect fixed point.

## Converting Fractions Between Bases

In Lecture 3 we converted *integers* between bases by **repeated division**: divide by the base, and the remainder is the next digit. Fractions extend the same idea, with two changes. We **multiply** by the base instead of dividing, and the digits come out **left to right** — most significant first — so no reversal is needed at the end.

Lab 2 has you write this two ways.

**Program 1** takes the fraction as a `double`. Each pass through the loop produces one digit:

```c
u = u * b;   // shift one digit past the binary point
r = u;       // truncating conversion to int extracts that digit
u = u - r;   // drop the digit, keep the remaining fraction
```

**Program 2** takes the fraction as a numerator and denominator, both `int`, and never touches floating point at all:

```c
u = u * b;   // scale the numerator
r = u / q;   // integer division extracts the digit
u = u % q;   // the modulo keeps the leftover fraction
```

In both cases each `r` is one digit of the answer, and you stop when you have as many digits as you were asked for.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — 0.625 to base 2 (the double method)</p>

Convert 0.625 to base 2, 4 digits. Build the table downward; each row is one pass through the loop.

| u | u × b | r = (int) | u − r | digit |
|---:|---:|:--:|---:|:--:|
| 0.625 | 1.25 | 1 | 0.25 | d<sub>−1</sub> |
| 0.25 | 0.5 | 0 | 0.5 | d<sub>−2</sub> |
| 0.5 | 1.0 | 1 | 0.0 | d<sub>−3</sub> |
| 0.0 | 0.0 | 0 | 0.0 | d<sub>−4</sub> |

Read the digit column **top to bottom** — the opposite of last week:

```text
0.625 = 0b0.1010
```

Check it with the place values: (1 × 0.5) + (0 × 0.25) + (1 × 0.125) + (0 × 0.0625) = 0.625 ✓

Once u reaches 0 the fraction has terminated, and every remaining digit is 0.

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — 11/16 to base 16 (the integer method)</p>

Convert 11/16 to base 16, 1 digit. Here u = 11, q = 16, b = 16.

| u | u × b | r = u / q | u % q | digit |
|---:|---:|:--:|---:|:--:|
| 11 | 176 | 11 | 0 | `B` |

```text
11/16 = 0x0.B
```

Two things to notice. The remainder 11 is above 9, so it has to be translated into the letter `B` — the same ASCII arithmetic you did in Lab 1, just applied to a different digit position. And this method never used a `double`, so nothing was ever approximated.

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 4 — 3/4 to base 2</p>

Convert 3/4 to base 2, 3 digits. u = 3, q = 4, b = 2.

| u | u × b | r = u / q | u % q | digit |
|---:|---:|:--:|---:|:--:|
| 3 | 6 | 1 | 2 | d<sub>−1</sub> |
| 2 | 4 | 1 | 0 | d<sub>−2</sub> |
| 0 | 0 | 0 | 0 | d<sub>−3</sub> |

```text
3/4 = 0b0.110
```

Check: 0.5 + 0.25 = 0.75 ✓

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 5 — 1/3 to base 2, and why we ask for a digit count</p>

Convert 1/3 to base 2. u = 1, q = 3, b = 2.

| u | u × b | r = u / q | u % q | digit |
|---:|---:|:--:|---:|:--:|
| 1 | 2 | 0 | 2 | d<sub>−1</sub> |
| 2 | 4 | 1 | 1 | d<sub>−2</sub> |
| 1 | 2 | 0 | 2 | d<sub>−3</sub> |
| 2 | 4 | 1 | 1 | d<sub>−4</sub> |

The value of u is back to 1, which is where we started — the table has entered a cycle and will repeat `01` forever:

```text
1/3 = 0b0.01010101…
```

This is the same phenomenon as 1/3 = 0.333… in decimal. A fraction terminates in base b only when its denominator's prime factors all divide b. Three does not divide 2, so 1/3 has no finite binary expansion.

This is exactly why the lab has you pass in a **number of digits**: the loop has no natural stopping point, so you have to supply one. And it is the same reason 0.1 and 0.2 are not stored exactly in a `float` or `double` — ten has a factor of 5, which does not divide 2.

</div>

One caution for the `double` version in Lab 2. When you enter 0.3, what actually gets stored is 0.29999999999999998890…, because of exactly what we saw in Example 5. The conversion still prints `300`, because the rounding in the multiplication happens to land back on 3.0 — but the value you typed and the value in memory were never the same number. The integer numerator/denominator version in Program 2 has no such gap, which is a large part of why the lab asks you to write it both ways.

## Looking Ahead

Lab 2 is the second half of the base conversion work: the same algorithm, applied to fractions, in both of the forms above. Everything you need for it is in this lecture.

That also closes out Chapter 4. Next time we leave data representation behind and start looking at how a C program is actually laid out in memory — program memory, scope, and pointers.
