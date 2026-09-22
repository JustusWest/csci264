---
title: "13 Logic Gates and Circuits"
published: true
---

[*Reading: Dive into Systems §5.3*](https://diveintosystems.org/book/C5-Arch/gates.html) and [*§5.4.1*](https://diveintosystems.org/book/C5-Arch/arithlogiccircs.html)

Last time, we said that the ALU performs arithmetic and logical operations. Today we look at what it is built from: **logic gates**.

Gates are the building blocks of every digital circuit. Circuit design works by abstraction: combine a few gates into a simple circuit, then treat that circuit as a building block for a bigger one, and so on, all the way up to a full processor.

## Transistors

Logic gates are made from **transistors** etched into silicon. A transistor is a switch: its output is either high voltage or low voltage. We abstract this into binary, with high voltage for 1 and low voltage for 0. A gate is a set of transistors arranged to produce the right output for its inputs.

## Basic Gates: AND, OR, NOT

A logic gate takes one or two input bits and produces one output bit. A **truth table** lists the output for every combination of inputs.

| A | B | A AND B | A OR B | NOT A |
|:-:|:-:|:-:|:-:|:-:|
| 0 | 0 | 0 | 0 | 1 |
| 0 | 1 | 0 | 1 | 1 |
| 1 | 0 | 0 | 1 | 0 |
| 1 | 1 | 1 | 1 | 0 |

- **AND** outputs 1 only if *both* inputs are 1.
- **OR** outputs 1 if *any* input is 1.
- **NOT** takes one input and flips it.

AND, OR, and NOT form a **complete set**: any circuit can be built from them alone.

![The three basic logic gate symbols. The AND gate is a D shape with a flat back, inputs A and B on the left and output A AND B on the right. The OR gate has a curved back and a pointed front, inputs A and B, output A OR B. The NOT gate is a triangle with a small circle at its tip, one input A and output NOT A.]({{ "/figures/ch5/gates.png" | relative_url }})

## Minimal Sets of Gates

{AND, OR, NOT} is complete, but it is not the smallest complete set. {AND, NOT} is also complete, because OR can be built from it:

```text
A OR B  ==  NOT( NOT A  AND  NOT B )
```

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — check it with a truth table</p>

| A | B | NOT A | NOT B | NOT A AND NOT B | NOT( ... ) | A OR B |
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| 0 | 0 | 1 | 1 | 1 | 0 | 0 |
| 0 | 1 | 1 | 0 | 0 | 1 | 1 |
| 1 | 0 | 0 | 1 | 0 | 1 | 1 |
| 1 | 1 | 0 | 0 | 0 | 1 | 1 |

The last two columns match on every row, so the two expressions are the same function.

</div>

**NAND alone** is also a complete set. Modern chips are built with CMOS technology, which uses NAND as its basic building block. 

## Circuits

A **circuit** is built by wiring gates together. Once it works we use it as a building block in a larger circuit. Everything in a processor is built this way, in layers.

Circuits come in three kinds:

- **arithmetic/logic** circuits, which do the work of the ALU
- **control** circuits, which decide which values move where
- **storage** circuits, which hold bits

We will look at an example of building a logic circuit.

## Designing a 1-Bit Circuit

Circuit design usually starts with a 1-bit version of the circuit, which then becomes the building block for an M-bit version. It is generally done in three steps:

1. **Write the truth table.** Decide the inputs and outputs, and fill in a row for every combination of input bits.
2. **Write an expression** for when the output is 1, using AND, OR, and NOT on the inputs.
3. **Translate the expression into gates**, where each gate's inputs come from a circuit input or from the output of an earlier gate.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — a 1-bit equality circuit</p>

The circuit takes two bits, A and B, and outputs 1 when they are the same.

**Step 1: the truth table.**

| A | B | A == B |
|:-:|:-:|:-:|
| 0 | 0 | 1 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

**Step 2: the expression.** Take the rows whose output is 1, one at a time, and write an expression that is 1 for that row and nothing else.

Row 1 has A = 0 and B = 0. `NOT(A)` is 1 when A is 0, and `NOT(B)` is 1 when B is 0, so AND them:

```text
NOT(A) AND NOT(B)      # 1 when A and B are both 0
```

Row 4 has A = 1 and B = 1, which is simpler:

```text
A AND B                # 1 when A and B are both 1
```

The output is 1 if *either* row happens, so OR the two together:

```text
(NOT(A) AND NOT(B)) OR (A AND B)
```

**Step 3: the gates.** Work from the inside out. The NOTs come first, since they act directly on the inputs. Each AND is next, taking the inputs or the NOT outputs. The OR is last, taking the two AND outputs.

![The 1-bit equality circuit. Inputs A and B each branch two ways. On the upper path, each passes through a NOT gate and both NOT outputs feed an AND gate. On the lower path, A and B feed a second AND gate directly. The two AND outputs feed an OR gate, whose output is labeled A == B.]({{ "/figures/ch5/1biteq.png" | relative_url }})

</div>

To check a circuit, try every combination of inputs and compare against the truth table. Try with `A=0` and `B=0`:

![The same 1-bit equality circuit with values labeled on every wire. A and B both enter as 0. The NOT gates output 1 and 1 into the top AND gate, which outputs 1. The bottom AND gate receives 0 and 0 and outputs 0. The OR gate receives 1 and 0 and outputs 1, the correct value of A == B.]({{ "/figures/ch5/1biteqex.png" | relative_url }})

Once we've designed a circuit, we create a new placeholder for it to be reused:

![The 1-bit equality circuit drawn as an abstraction: a plain box labeled "1-bit equality" with inputs A and B entering on the left and a single output labeled A == B leaving on the right.]({{ "/figures/ch5/1biteqcircuit.png" | relative_url }})


## Looking Ahead

Next time we use the same three steps to build an adder, then stack adders into the circuit that does addition for the ALU.
