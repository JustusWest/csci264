---
title: "03 Binary, Hexadecimal, and Base Conversion"
---

[*Reading: Dive into Systems §4.1–4.2*](https://diveintosystems.org/book/C4-Binary/bases.html)

## Bits, Bytes, and Words

At the base level, computers represent information using **binary** — "binary" meaning *two*, as in a thing that can be in one of two states. On the hardware, that state is the presence or absence of voltage. In theory, we think of these two states as **0** and **1**, and we call a single such value a **bit**.


Through increasing complexity we can represent any information as a combination of 0s and 1s.
The number of unique values we can represent doubles each time we add a bit. 

Formally: **N bits represent 2<sup>N</sup> unique values.**

We commonly group bits into longer sequences called **bytes** which is 8 bits. 2^8=256 so we get 256 unqiue values from one byte.
Modern CPUs typically define a **word** of either 32 or 64 bits.

| Bits | Distinct values |
|---:|---:|
| 1 | 2 |
| 2 | 4 |
| 3 | 8 |
| 4 | 16 |
| 8 | 256 |
| 16 | 65,536 |
| 32 | 4,294,967,296 |


Two groupings of bits get their own names:

- A **byte** is 8 bits. Since 2<sup>8</sup> = 256, one byte holds 256 unique values.
- A **word** is whatever size the CPU natively works with. Modern CPUs define a word as either 32 or 64 bits.


## Decimal


In everyday life we use the **Decimal** number system, which is _base 10_, thus each digit can hold 10 unique values (0-9)
To store a value greater than 9, we carry over to the next digit to the left.
The position of each digit indicates its factor, or how much it contributes to the final value.

Labeling the digits from right to left as d<sub>0</sub>, d<sub>1</sub>, d<sub>2</sub>, …, each successive digit contributes a factor of ten more than the one before it.

![Each digit of the base 10 number 8425 labeled with its place: the 5 in the ones place contributes 5 * 1, the 2 in the tens place contributes 2 * 10, the 4 in the hundreds place contributes 4 * 100, and the 8 in the thousands place contributes 8 * 1000]({{ "/figures/ch4/BaseTen.png" | relative_url }})

So we can express 8425 as a sum of place values:

```text
(8 × 10³) + (4 × 10²) + (2 × 10¹) + (5 × 10⁰)
= 8000 + 400 + 20 + 5
= 8425
```

And in general, any N-digit decimal number has the value:

```text
(d[N-1] × 10^(N-1)) + (d[N-2] × 10^(N-2)) + … + (d[1] × 10¹) + (d[0] × 10⁰)
```


### Notation

With multiple number systems, the value of any sequence of digits is ambiguous. Does `1000` mean one thousand, or does it mean eight? To keep it clear, we attach a prefix to every non-decimal number:

| Prefix | Base | Example | Decimal value |
|---|---|---|---|
| *(none)* | 10 | `1000` | 1000 |
| `0b` | 2 | `0b1000` | 8 |
| `0x` | 16 | `0x1000` | 4096 |


## Unsigned Binary Numbers

**Unsigned** means the value is zero or positive — it has no sign, so it can never be negative.

Binary uses a base of 2 instead of decimal's 10.

**1. Each bit holds one of 2 unique values (0 or 1).** To store a value greater than 1, we carry to the next bit to the left.

**2. Each successive bit contributes a factor of 2 more than the one before it.** The places are the ones place, the twos place, the fours place, the eights place, and so on.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — counting up in binary</p>


| Decimal | Binary |
|---:|---:|
| 0 | `0` |
| 1 | `1` |
| 2 | `10` |
| 3 | `11` |
| 4 | `100` |
| 5 | `101` |
| 6 | `110` |
| 7 | `111` |
| 8 | `1000` |


- Going from 1 to 2 carries, because a single bit is already at its maximum. Same reason 9 + 1 carries in decimal.
- A new column opens at 2, 4, and 8 — the powers of two. This is the same phenomenon as a new decimal column opening at 10, 100, and 1000.
- With 3 bits we counted 0 through 7. That's 8 = 2<sup>3</sup> unique values.

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — evaluating a binary number</p>

Given `0b1101`, write the place value under each bit, then sum the columns.


```text
   1     1     0     1
   8     4     2     1

(1 × 8) + (1 × 4) + (0 × 2) + (1 × 1)  =  13
```

</div>

So the general formula for the value of any N-bit unsigned binary number is same as the decimal formula, but we swap the _base_ (2 for 10)
```text
(d[N-1] × 2^(N-1)) + (d[N-2] × 2^(N-2)) + … + (d[1] × 2¹) + (d[0] × 2⁰)
```

## Hexadecimal

So we have two systems to represent numbers. Decimal is natural to humans (from experience).
Binary is natural for computers because we can represent it with voltage.

So why use a third?

**Convenience.** Binary is very hard to read, and it does not convert cleanly to decimal. To make something more legible with a clean conversion, we introduce _Hexadecimal_ which is base 16.

**1. Each digit holds one of 16 unique values.** We've run out of numerals at 9, so by convention we use letters for the values above 9: `A` = 10, `B` = 11, `C` = 12, `D` = 13, `E` = 14, `F` = 15. Carrying works the same way — `0xF + 0x1 = 0x10`.

**2. Each successive digit contributes a factor of 16 more than the one before it.** The places are 1, 16, 256, 4096, …

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — hex to decimal</p>

Evaluate `0x23C8`. Same formula as always, with 16 as the base.

```text
(2 × 16³) + (3 × 16²) + (C × 16¹) + (8 × 16⁰)
```

First, replace each letter digit with its numeric value (i.e. `C` = 12):

```text
= (2 × 16³) + (3 × 16²) + (12 × 16¹) + (8 × 16⁰)
```

Next, expand the powers of 16:

```text
= (2 × 4096) + (3 × 256) + (12 × 16) + (8 × 1)
```

Finally, multiply and sum:

```text
= 8192 + 768 + 192 + 8
= 9160
```


</div>


## Converting Between Binary and Hexadecimal

The main practical reason we bother with hex is that converting to and from binary is nearly free. Hex is base 16 and 16 = 2<sup>4</sup>, so **one hex digit carries exactly as much information as four bits.** That means the conversion is a straight digit-by-digit substitution.

| Binary | Hex | | Binary | Hex |
|---|---|---|---|---|
| `0000` | `0` | | `1000` | `8` |
| `0001` | `1` | | `1001` | `9` |
| `0010` | `2` | | `1010` | `A` |
| `0011` | `3` | | `1011` | `B` |
| `0100` | `4` | | `1100` | `C` |
| `0101` | `5` | | `1101` | `D` |
| `0110` | `6` | | `1110` | `E` |
| `0111` | `7` | | `1111` | `F` |

Don't memorize this table. It's just counting from 0 to 15 in two systems side by side — if you can count in binary you can rebuild it.

<div class="board-example" markdown="1">
<p class="board-title">Example 4 — hex to binary</p>

Convert `0xB491` to binary. Substitute the four-bit pattern for each hex digit, in place:

```text
   B      4      9      1
   ↓      ↓      ↓      ↓
  1011   0100   1001   0001
```

Then run the groups together:

```text
0xB491 = 0b1011010010010001
```

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 5 — binary to hex</p>

Convert `0b1111011001` to hexadecimal. This direction has one extra wrinkle: we have to group the bits into fours first, and **the grouping goes right to left**, because that's the end where the place values are anchored.

Start by slicing from the right:

```text
1111011001  ->  11 1101 1001
```

The leftmost chunk came up short, with only 2 bits. Pad it with leading zeros:

```text
11 1101 1001  ->  0011 1101 1001
                   ^^
                 padding
```

Now substitute each group:

```text
  0011   1101   1001
    ↓      ↓      ↓
    3      D      9

0b1111011001 = 0x3D9
```

</div>


## Base Conversion Algorithm: Repeated Division

To convert a decimal value `u` to base `b`:

1. Divide `u` by `b`. The **remainder** is the next digit of the answer.
2. Replace `u` with the **quotient**.
3. Repeat until `u` reaches 0.
4. The digits come out **right to left** — the first remainder is the *last* digit of the answer.

Note how integer division works in our favor in C:

```c
r = u % b;   // remainder: the next digit
u = u / b;   // quotient: integer division truncates, which is what makes this terminate
```


<div class="board-example" markdown="1">
<p class="board-title">Example 6 — decimal to binary</p>

Convert 422 to base 2. Build the table downward, and keep a separate column for remainders on the right.

| u | u / 2 | u % 2 | digit |
|---:|---:|---:|:--:|
| 422 | 211 | 0 | d<sub>0</sub> |
| 211 | 105 | 1 | d<sub>1</sub> |
| 105 | 52 | 1 | d<sub>2</sub> |
| 52 | 26 | 0 | d<sub>3</sub> |
| 26 | 13 | 0 | d<sub>4</sub> |
| 13 | 6 | 1 | d<sub>5</sub> |
| 6 | 3 | 0 | d<sub>6</sub> |
| 3 | 1 | 1 | d<sub>7</sub> |
| 1 | 0 | 1 | d<sub>8</sub> |

The quotient hit 0, so we stop. Now read the remainder column **bottom to top**:

```text
422 = 0b110100110
```


For base 2 specifically there's a shortcut worth mentioning: `u % 2` is just asking whether `u` is odd. Even means the next bit is 0, odd means it's 1. You can run the whole conversion by halving and checking parity, without doing any real division.

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 7 — decimal to hex</p>

Same algorithm, `b` = 16 instead of 2. The only new step is translating remainders above 9 into letters.

| u | u / 16 | u % 16 | digit |
|---:|---:|---:|:--:|
| 9742 | 608 | 14 | `E` |
| 608 | 38 | 0 | `0` |
| 38 | 2 | 6 | `6` |
| 2 | 0 | 2 | `2` |

Read bottom to top:

```text
9742 = 0x260E
```

- The remainder 14 had to become `E`. Any base above 10 needs this translation step, and in Lab 1 you'll do it with ASCII arithmetic rather than by hand.

</div>

<div class="board-example" markdown="1">
<p class="board-title">Example 8 — decimal to an arbitrary base</p>


| u | u / 12 | u % 12 | digit |
|---:|---:|---:|:--:|
| 1000 | 83 | 4 | `4` |
| 83 | 6 | 11 | `B` |
| 6 | 0 | 6 | `6` |

Read bottom to top:

```text
1000 = 6B4  (base 12)
```

Check it with the general formula: (6 × 144) + (11 × 12) + (4 × 1) = 864 + 132 + 4 = 1000. ✓

</div>


## Storage Limitations

There an infinetly many unsigned integers (and infinetly many more floating point numbers). In practice, hardware has to commit to a fixed amount of space, so we have to decide up front how much memory a value gets. C does this with its types: a `long` stores larger values than a `short`.

Fixing the number of bits at N has a direct consequence. N bits give 2<sup>N</sup> unique sequences; one of those is reserved for 0, which leaves 2<sup>N</sup> − 1 nonzero values. So the largest unsigned value N bits can hold is **2<sup>N</sup> − 1**.

| Bits | Unsigned range | Typical C type |
|---:|---|---|
| 8 | 0 to 255 | `unsigned char` |
| 16 | 0 to 65,535 | `unsigned short` |
| 32 | 0 to 4,294,967,295 | `unsigned int` |
| 64 | 0 to 18,446,744,073,709,551,615 | `unsigned long` |

![Two number lines: (a) an infinite unsigned number line running from 0 rightward to infinity, and (b) a finite unsigned number line drawn as a circle from 0 up to Max, where the point between Max and 0 is labeled overflow]({{ "/figures/ch4/UnsignedLine.png" | relative_url }})


Attempting to store a larger value to a variable than the variable’s size allows is known as integer overflow. This chapter defers the details of overflow to a later section. For now, think of it like a car’s odometer that "rolls over" back to zero if it attempts to increase beyond its maximum value. Similarly, subtracting one from zero yields the maximum value.


## Looking Ahead

Lab 1 has you write a C program that takes a non-negative decimal integer and a base from 2 to 36 and prints the number in that base. 
Everything you need to complete that lab is in this lecture.