---
title: "04 Signed Integers, Arithmetic, and Overflow"
---

[*Reading: Dive into Systems §4.3–4.5*](https://diveintosystems.org/book/C4-Binary/signed.html)

## Signed Binary Integers

Signed number encodings use one bit to distinguish between the negative numbers and the non-negative numbers. By convention that is the left-most bit, which is 1 for negative and 0 for non-negative. This leftmost bit is called the **high-order bit** or the **most significant bit**.

We'll look at two possible signed encodings: **signed magnitude** and **two's complement**. Only one of them (two's complement) is still used in practice, but comparing the two is the clearest way to see why.

## Signed Magnitude

Signed magnitude treats the high-order bit *exclusively* as a sign bit. It does not affect the absolute value of the number at all — only whether the number is positive (0) or negative (1).

That makes two operations very simple:

- **To convert**, apply the standard unsigned method to everything except the most significant bit, then check the most significant bit to set the sign.
- **To negate**, flip the most significant bit. That's it.

![A four-bit signed magnitude number line, showing each bit pattern from 0b0000 through 0b1111 and the value it encodes, with the two encodings of zero highlighted]({{ "/figures/ch4/SignedMagnitude.png" | relative_url }})

At first glance signed magnitude looks attractive precisely because it is so simple. Unfortunately it has two major drawbacks.

**1. There are two zeros.** `0b0000` is zero, and so is `0b1000` — positive zero and negative zero. Two bit patterns for the same value wastes an encoding, and it erroneously creates a negative zero (zero is non-negative).

**2. Arithmetic breaks at the boundary.** Adding 1 to `0b1111` (−7) gives `0b0000` (0). The answer should be −6.

Signed magnitude has never been widely used in Computer Systems; we present it for pedagogical purposes. 
**Assume we are using two's complement unless explicitly stated as signed magnitude**

## Two's Complement

Two's complement solves both of those problems in an elegant way. As in signed magnitude, the high-order bit tells you whether the value is negative. The difference is that here the high-order bit also contributes to the value.

Computing the decimal value of an N-bit two's complement number is the same as the unsigned method, except that the high-order bit's contribution is **negated**. Instead of the first bit contributing d<sub>N-1</sub> × 2<sup>N-1</sup> to the sum, it contributes −d<sub>N-1</sub> × 2<sup>N-1</sup>.

```text
-(d[N-1] × 2^(N-1)) + (d[N-2] × 2^(N-2)) + … + (d[1] × 2¹) + (d[0] × 2⁰)
```

Note the leading negative sign on the first term only — every other term is unchanged from the unsigned formula.

The bit contributes the largest absolute value in the sum. In two's complement it contributes a negative value, so the overall value must be negative. If the most significant bit is 0, it contributes nothing, and the result is exactly the unsigned value.

For example, `0b1011` as a four-bit two's complement number:

```text
-(1 × 8) + (0 × 4) + (1 × 2) + (1 × 1)
= -8 + 0 + 2 + 1
= -5
```

![A four-bit two's complement number line, showing each bit pattern from 0b0000 through 0b1111 and the value it encodes, running from -8 through 7]({{ "/figures/ch4/TwosComplement.png" | relative_url }})


This fixes the two zeros problem by encoding one additional negative number, but that doesn't cause any issues.

The overflow problem is handled elegantly because a sequence of all 1's always represents -1. So adding 1 to 0b1111 (-1) rolls over to 0b0000 (0) as intended.

### Negation

Negating a twos complement is slightly trickier. To negate an N-bit value we find its *complement with respect to 2<sup>N</sup>* — that is, to negate X find Y such that X + Y = 2<sup>N</sup>.

Thankfully there's a shortcut: **flip all the bits and add 1.**

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — negating an eight-bit value</p>

Negate the eight-bit value 13.

Start by writing 13 in eight-bit binary:

```text
13 = 0b00001101
```

Flip every bit:

```text
0b00001101  ->  0b11110010
```

Add 1:

```text
   11110010
 +        1
   --------
   11110011
```

So −13 is `0b11110011`. Check it against the formula:

```text
-(1 × 128) + (1 × 64) + (1 × 32) + (1 × 16) + (0 × 8) + (0 × 4) + (1 × 2) + (1 × 1)
= -128 + 64 + 32 + 16 + 2 + 1
= -13
```

The shortcut works in both directions — flip-and-add-one applied to `0b11110011` takes you back to `0b00001101`.

</div>

## Binary Integer Arithmetic

### Addition

Binary addition works exactly like the decimal addition you already know. Add column by column from right to left, and when a column's sum is too large for a single digit, carry into the next column. The only difference is that a binary column overflows at 2 rather than at 10.

Adding `0b0101` and `0b0011`:

```text
   111       <- carries
   0101      (5)
 + 0011      (3)
   ----
   1000      (8)
```


<div class="board-example" markdown="1">
<p class="board-title">Example 2 — addition with a carry out</p>

Add the four-bit values `0b1010` and `0b0111`.

```text
   111       <- carries
   1010
 + 0111
   ----
 1 0001
 ^
 carry out
```

The carry out of the leftmost column has nowhere to go — we only have four bits — so it is discarded, and the stored result is `0b0001`.

Whether that result is *wrong* depends entirely on how we agreed to interpret those bits:

| Interpretation | `0b1010` | `0b0111` | Expected | Stored `0b0001` |
|---|---:|---:|---:|---:|
| Unsigned | 10 | 7 | 17 | 1 |
| Two's complement | −6 | 7 | 1 | 1 |


</div>

### Subtraction

Subtraction can be rebuilt out of addition and negation:

```text
X - Y  =  X + (-Y)
```

This is convenient because the hardware already contains an adder and already knows how to negate. No separate subtraction circuit is required.

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — subtraction by negation</p>

Compute `0b0111 - 0b0011` using four-bit two's complement.

First negate the second operand. Flip the bits:

```text
0b0011  ->  0b1100
```

Add 1:

```text
0b1100  ->  0b1101      (this is -3)
```

Now add that to the first operand instead of subtracting:

```text
   111       <- carries
   0111      (7)
 + 1101      (-3)
   ----
 1 0100      (4)
 ^
 discarded
```

The stored result is `0b0100`, which is 4. And 7 − 3 = 4. ✓


</div>

We won't cover binary multiplication and division in this course. They build on the same shift-and-add machinery, and the book covers them in §4.4.3 and §4.4.4 if you're curious.

## Overflow

**Integer overflow** happens when an arithmetic operation produces a result that the available bits cannot represent. We saw the mechanism in Example 2: the answer was correct, then the part that didn't fit got thrown away.

The useful way to picture this is as a circle rather than a line. Fixed-width arithmetic is *modular* — like an odometer, or a 12-hour clock. Counting up past the largest representable value wraps around to the smallest one. Overflow is what we call it when an operation crosses that seam.

Where the seam sits depends on the interpretation, and this is why the two cases have different rules.

**Unsigned.** N bits represent the range [0, 2<sup>N</sup> − 1], so the discontinuity sits between the maximum value and 0. Adding two numbers should make the result larger; if it comes out *smaller*, the addition overflowed. Subtracting should make it smaller; if it comes out *larger*, the subtraction overflowed.

> **Shortcut:** the carry out must match the carry in. If they differ, the operation overflowed. (Recall that the carry in is 1 only for subtraction, as part of the negation.)

**Signed.** Two's complement rolls cleanly from −1 to 0, so that's not where the seam is. It sits at the far end of the number space, between the largest positive value and the smallest negative one.

> **Shortcut:** if the two operands have *different* signs, signed overflow is impossible — the result is moving toward zero, away from the seam. If the operands have the *same* sign, overflow occurred if the result's sign differs from theirs.

Rewrite any subtraction as an addition first, then apply the rule.

Working through the four-bit cases:

| Operation | As addition | Result | Overflow? |
|---|---|---|---|
| 5 − 4 | 5 + (−4) | 1 | No — operands differ in sign |
| 4 + 2 | 4 + 2 | 6 | No — positive + positive = positive |
| −5 − 1 | (−5) + (−1) | −6 | No — negative + negative = negative |
| 4 + 5 | 4 + 5 | −7 | **Yes** — positive + positive gave negative |
| −3 − 8 | (−3) + (−8) | 5 | **Yes** — negative + negative gave positive |

## Looking Ahead

Next time we deal with floating point numbers.
