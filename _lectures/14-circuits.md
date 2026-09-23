---
title: "14 Arithmetic, Control, and Storage Circuits"
published: true
---

[*Reading: Dive into Systems §5.4*](https://diveintosystems.org/book/C5-Arch/circuits.html)

Last time we looked at the 1-bit equality circuit. Here we look at arthimetic, control and storage circuits.

## The 1-bit Adder

We start with the smallest possible addition: two bits in, and a two bit results, one for the sum and one carry out.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — adding two bits</p>

Two inputs, and two outputs: the SUM bit and the CARRY OUT bit.

| A | B | SUM | CARRY OUT |
|:-:|:-:|:-:|:-:|
| 0 | 0 | 0 | 0 |
| 0 | 1 | 1 | 0 |
| 1 | 0 | 1 | 0 |
| 1 | 1 | 0 | 1 |

The last row is `1 + 1 = 2`, which is `10` in binary: the sum bit is 0 and the carry is 1.

Now write each output as an expression. CARRY OUT is 1 only in the last row:

```text
CARRY OUT:  A AND B
```

SUM is 1 in the two middle rows, so it's a disjunction of two conjunctions:

```text
SUM:  (NOT(A) AND B) OR (A AND NOT(B))       # 1 when exactly one input is 1
```

"Exactly one of the two" is XOR, so we can simplify `SUM` with `XOR` if available, or leave it as is if using a minimal gate set.

</div>

![]({{ "/figures/ch5/1bitadder.png" | relative_url }})

This circuit is called a **half adder**, because it is incomplete for multi-bit addition.

## Carry In

When we add multi-bit numbers we work from the low-order bit up, and a carry out of one column becomes a carry *into* the next. So the adder we actually need has three inputs: A, B, and CARRY IN.

We build this by combining two half adders: add A and B, then add CARRY IN to that result. Either addition can produce a carry, so OR the two carries together.

![The 1-bit full adder drawn from gates. Inputs A, B, and Cin enter on the left. A and B feed an XOR gate, whose output feeds a second XOR gate together with Cin; that second XOR's output is SUM. A and B also feed an AND gate. The first XOR's output and Cin feed a second AND gate. The outputs of the two AND gates feed an OR gate, whose output is Cout.]({{ "/figures/ch5/fa-gates.png" | relative_url }})

This is the **full adder**. Three inputs, two outputs:

```text
SUM:   (A XOR B) XOR Cin
Cout:  (A AND B) OR ((A XOR B) AND Cin)
```
Now we can use this as a circuit building block:

![]({{ "/figures/ch5/1bitaddcin.png" | relative_url }})


## The Ripple-Carry Adder

Chain four of the boxes, carry out to carry in, and you can add two 4-bit numbers.

![A 4-bit ripple-carry adder. Four boxes labeled FA3, FA2, FA1, and FA0 sit in a row, with bit 3 on the left. Each box takes inputs A and B from above and produces S below. The Cout of each box connects to the Cin of the box to its left. The Cin of the rightmost box is 0, and the Cout of the leftmost box is the circuit's carry out.]({{ "/figures/ch5/rippleadder.png" | relative_url }})

The carry into bit 0 is 0, because there is nothing below bit 0 to carry from.

Notice that using the output of one circuit as input to another introduces a dependency. Bit 1 cannot compute its sum until bit 0 has produced its carry, and bit 2 waits on bit 1. The result **ripples** from the low-order bit to the high-order bit, and a 64-bit version of this circuit waits through 64 of those steps.

## Control Circuits: the Multiplexer

Arithmetic circuits compute values. **Control circuits** choose which values move where.

A **multiplexer**, or MUX, picks one of several inputs and passes it along. The CPU uses one every time it decides which register supplies an operand.

The simplest is a two-way 1-bit MUX: inputs A and B, a select input S, and one output. When S is 1 it outputs A; when S is 0 it outputs B.

![A two-way 1-bit multiplexer built from gates. Input B and the negation of S feed the upper AND gate. Input A and S feed the lower AND gate. The outputs of both AND gates feed an OR gate, which produces the multiplexer's output.]({{ "/figures/ch5/mux2.png" | relative_url }})

<div class="board-example" markdown="1">
<p class="board-title">Example 3 — how the select bit works</p>

Set S to 1, with A = 1 and B = 0.

- Upper AND gate: its inputs are B and `NOT(S)`, so `0 AND 0` — it outputs 0.
- Lower AND gate: its inputs are A and S, so `1 AND 1` — it outputs the value of A.
- OR gate: `0 OR A`, which is A.

The trick is that whichever AND gate has a 0 select input is forced to output 0 no matter what its data input is, so OR-ing the two outputs together passes through the one that was chosen. Set S to 0 and the same argument runs the other way.

</div>

## Storage Circuits

To store a bit, a circuit uses a **feedback loop**: its output is fed back as one of its own inputs, so its next value depends on its current value.

An **RS latch** stores one bit, using two NAND gates wired into each other.

![An RS latch. Input S enters the upper NAND gate and input R enters the lower NAND gate. The output of the upper gate is Q, which also feeds back as an input to the lower gate. The output of the lower gate is NOT(Q), which feeds back as an input to the upper gate.]({{ "/figures/ch5/rslatch.png" | relative_url }})

When R and S are both 1, the loop holds whatever Q already is — that is the latch storing a value. To write, drop exactly one of them to 0: R to 0 stores a 0, S to 0 stores a 1. Then set it back to 1 and the new value stays.

This kind of circuit-based memory is called **Static RAM (SRAM)**, and it is used for registers and on-chip caches. Memory units use **Dynamic RAM (DRAM)**, which stores bits in capacitors that leak and must be refreshed.


## Subtraction

We do not need a subtraction circuit. Recall from Lecture 4 on signed intgers, we can do a subtraction by adding the negation, and negating in two's complement is done by flipping the bits and adding one:

```text
A - B  ==  A + NOT(B) + 1
```

So we can make our adder do subtraction if we can (a) invert every bit of B and (b) add one:

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — XOR as a controlled inverter</p>

Look at what XOR does when one input is held fixed:

| X | X XOR 0 | X XOR 1 |
|:-:|:-:|:-:|
| 0 | 0 | 1 |
| 1 | 1 | 0 |

`X XOR 0` is X, and `X XOR 1` is NOT X. So one XOR gate on each B bit, with a control line as the other input, inverts B on demand.

That handles the flip. For the add one: the carry into bit 0 is sitting there unused, wired to 0. Feed the control line into it instead.

</div>

Call the control line SUB. When SUB is 0 the B bits pass through unchanged and the carry in is 0, so the circuit computes `A + B`. When SUB is 1, every `B` bit is inverted and a `1` enters at the bottom, so the circuit computes `A + NOT(B) + 1`, which is `A - B`.

![A 4-bit adder/subtractor. The same four full adder boxes are chained by their carries, but each B input now passes through an XOR gate before entering its box. A horizontal SUB line runs across the top and feeds the second input of all four XOR gates, and also runs down the right side into the Cin of the rightmost box.]({{ "/figures/ch5/add-sub.png" | relative_url }})

This is the circuit we will build in the next lab.
