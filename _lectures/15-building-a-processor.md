---
title: "15 Building a Processor"
published: true
---

[*Reading: Dive into Systems §5.5*](https://diveintosystems.org/book/C5-Arch/cpu.html)

The **central processing unit (CPU)** implements the processing unit and the control unit of the von Neumann architecture. A CPU consists of:

- the **arithmetic logic unit (ALU)**, which performs arithmetic and logic operations
- a set of **general-purpose registers** for storing program data
- some **control circuits** and **special-purpose registers** that implement instruction execution
- a **clock** that drives the CPU to execute program instructions

## The CPU Visual Simulator

Today we will watch each of these pieces work in the [CPU Visual Simulator](https://cpuvisualsimulator.github.io/){:target="_blank"}. Each demo below has a link that loads its program into the simulator.

Before you start, open **Settings** and turn on **Display micro step text**. A box then describes each step as the CPU performs it.

The controls run the program at three speeds:

- **Program** runs until the program halts
- **Instruction** runs one whole instruction
- **Micro step** runs one step of an instruction

The ▶ button animates the step; the ▶\| button does it instantly. The ↻ button resets the CPU.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 1 — a tour of the CPU</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EICM2CiAgICAgICAgICAgICAgIEFERCAjOAogICAgICAgICAgICAgICBTVE8gWAogICAgICAgICAgICAgICBITFQKWDogICAgICAgICAgICAgMA%3D%3D){:target="_blank"}

Match each part of the CPU to the simulator:

| CPU part | In the simulator |
|---|---|
| ALU | **ALU** |
| general-purpose registers | **ACC**, the accumulator (there is only one) |
| control circuits | **Control Unit / Decoder** and the **MUX** |
| special-purpose registers | **PC** and **IR** |
| condition codes | **SW**, the status word: flags **Z** (zero) and **N** (negative) |
| clock | you: each click of a step button is one tick |

RAM sits outside the CPU, connected by the data, address, and control buses. Addresses go up by 2 because each instruction is 16 bits (2 bytes).

</div>

## The ALU

The ALU implements arithmetic and logic on signed and unsigned integers. A separate **floating-point unit** performs arithmetic on floating-point values.

The ALU takes two inputs:

- the integer **operands**
- an **opcode** that specifies which operation to perform (e.g. addition)

It produces two outputs:

- the **result** of the operation
- **condition codes** that encode information about the result, such as whether it is negative, zero, or produced a carry out

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 2 — the ALU computes 6 + 8</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EICM2CiAgICAgICAgICAgICAgIEFERCAjOAogICAgICAgICAgICAgICBTVE8gWAogICAgICAgICAgICAgICBITFQKWDogICAgICAgICAgICAgMA%3D%3D){:target="_blank"}

```c
x = 6 + 8;
```

```text
       LOD #6     # ACC = 6
       ADD #8     # ACC = ACC + 8
       STO X      # store ACC in X
       HLT
X:     0
```

Step through one instruction at a time. On `ADD #8` the ALU shows its two operands and the operation, `6 + 8`, and the result 14 goes into ACC. The status word shows Z = 0 and N = 0: the result is not zero and not negative. `STO X` then writes 14 into X at address 8.

Notice that `LOD` also goes through the ALU, with the operation `=`: it just passes the operand through.

</div>

### Condition Codes

Each condition code is a single bit: 1 means the condition holds, 0 means it does not. The ALU sets them as part of every arithmetic operation.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 3 — setting the flags</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EICM2CiAgICAgICAgICAgICAgIEFERCAjMgogICAgICAgICAgICAgICBTVUIgIzgKICAgICAgICAgICAgICAgU1VCICMxCiAgICAgICAgICAgICAgIEFERCAjNQogICAgICAgICAgICAgICBITFQ%3D){:target="_blank"}

Before each step, predict Z and N.

| instruction | ACC | Z | N |
|---|:-:|:-:|:-:|
| `LOD #6` | 6 | 1 | 0 |
| `ADD #2` | 8 | 0 | 0 |
| `SUB #8` | 0 | 1 | 0 |
| `SUB #1` | -1 | 0 | 1 |
| `ADD #5` | 4 | 0 | 0 |

Z starts at 1 because ACC starts at 0. `LOD` does not update the flags, so Z is still 1 after the first line: only arithmetic and logic operations set them.

</div>

Condition codes are how the CPU makes decisions. Consider:

```c
if ((x + 8) != 0) {
    x++;
}
```

The ALU computes `x + 8` and sets the zero flag. The next instruction is a **conditional jump**, which checks that bit. If it is 1, the result was zero, so the CPU skips the body of the `if` by writing the address of the first instruction after the body into the program counter. If it is 0, the CPU runs the body.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 4 — a condition code decides a jump</p>

Open with [x = 3](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EIFgKICAgICAgICAgICAgICAgQUREICM4CiAgICAgICAgICAgICAgIEpaIEVORElGCiAgICAgICAgICAgICAgIExPRCBYCiAgICAgICAgICAgICAgIEFERCAjMQogICAgICAgICAgICAgICBTVE8gWApFTkRJRjogICAgICAgICBITFQKWDogICAgICAgICAgICAgMw%3D%3D){:target="_blank"} or [x = -8](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EIFgKICAgICAgICAgICAgICAgQUREICM4CiAgICAgICAgICAgICAgIEpaIEVORElGCiAgICAgICAgICAgICAgIExPRCBYCiAgICAgICAgICAgICAgIEFERCAjMQogICAgICAgICAgICAgICBTVE8gWApFTkRJRjogICAgICAgICBITFQKWDogICAgICAgICAgICAgLTg%3D){:target="_blank"}.

```text
       LOD X
       ADD #8       # x + 8, sets Z
       JZ ENDIF     # if Z is 1, jump to ENDIF
       LOD X
       ADD #1
       STO X        # x++
ENDIF: HLT
X:     3
```

Watch the PC. Normally it goes up by 2 after each instruction.

- **x = 3:** `x + 8` is 11, so Z = 0. `JZ` does not jump, the PC goes from 4 to 6, and the body runs. X ends as 4.
- **x = -8:** `x + 8` is 0, so Z = 1. `JZ` writes 12, the address of `ENDIF`, into the PC. The body is skipped and X stays -8.

We will see this pattern again when we get to control flow in assembly.

</div>

### Inside the ALU

The ALU combines a separate circuit for each of its operations: an adder, an equality circuit, and so on.

Rather than turning on only the circuit for the selected operation, the ALU sends its inputs to *all* of its circuits. Every circuit computes its result, and a multiplexer uses the opcode as its select input to pick which result comes out.

![An ALU that performs four operations on two 32-bit operands. Inputs A and B enter on the left and branch to four circuits: a 32-bit OR gate, a 32-bit adder, a 32-bit AND gate, and a 32-bit equality circuit. The four outputs, labeled A or B, A + B, A and B, and A == B, feed a 4-way multiplexer. The opcode enters the top of the multiplexer as its select input. The multiplexer output is the ALU result, which also feeds an "== 0" circuit that produces the condition codes. To the right, the same ALU is drawn as a single abstract block with inputs A, B, and opcode, and outputs result and condition codes.]({{ "/figures/ch5/alu.png" | relative_url }})

This is the same idea as the MUX from last lecture, just with 4 inputs instead of 2. The condition code comes from checking the MUX output: here, a circuit that tests whether the result is 0.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 5 — same operands, different opcodes</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EICMxMgogICAgICAgICAgICAgICBBREQgIzEwCiAgICAgICAgICAgICAgIExPRCAjMTIKICAgICAgICAgICAgICAgQU5EICMxMAogICAgICAgICAgICAgICBMT0QgIzEyCiAgICAgICAgICAgICAgIFNVQiAjMTAKICAgICAgICAgICAgICAgSExU){:target="_blank"}

```text
       LOD #12
       ADD #10
       LOD #12
       AND #10
       LOD #12
       SUB #10
       HLT
```

Each pair loads 12 and combines it with 10. Micro-step through one of them: at "The Control Unit sets the Arithmetic Logic Unit operation" the symbol in the ALU changes to `+`, `&`, or `-`. That is the opcode choosing which result comes out.

The results are 22, 2, and 8. For the AND, write it in binary: `1100 & 1010` is `1000`.

</div>

### The Opcode

The opcode comes from the bits of the instruction the CPU is executing. An ADD instruction might be encoded in four parts:

```text
| OPCODE BITS | OPERAND A SOURCE | OPERAND B SOURCE | RESULT DESTINATION |
```

Depending on the architecture, the operand bits may encode a CPU register, a memory address, or a literal value.

An ALU that performs N operations needs log<sub>2</sub>(N) opcode bits. The four-operation ALU above needs 2 bits; one with 16 operations needs 4.

The simulator uses a simpler 16-bit format. ACC is always one operand and usually the destination, so the instruction only names the other operand:

```text
| IMMEDIATE FLAG (1 bit) | OPCODE (7 bits) | OPERAND (8 bits) |
```

It has 16 instructions, so 4 opcode bits would be enough; it reserves 7.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 6 — literal value or memory address?</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EICM2CiAgICAgICAgICAgICAgIExPRCA2CiAgICAgICAgICAgICAgIEhMVAogICAgICAgICAgICAgICA0Mg%3D%3D){:target="_blank"}

```text
       LOD #6     # immediate: the operand is the value 6
       LOD 6      # direct: the operand is the address 6
       HLT
       42         # stored at address 6
```

Run it: the first instruction puts 6 in ACC, the second puts 42.

Now turn on **Binary**:

```text
LOD #6    10000111 00000110
LOD 6     00000111 00000110
```

The two instructions differ by one bit, the immediate flag. Micro-step through them: at "The Control Unit sets the Multiplexer" the flag decides whether the MUX passes the operand from the IR (immediate) or reads it from RAM (direct).

</div>

## The Register File

The simulator has no register file: its only general-purpose register is ACC. Real CPUs have several, so for this section we use the textbook's figure.

At the top of the memory hierarchy, the CPU's **general-purpose registers** store temporary values. There are very few of them: IA32 provides 8, and ARM provides 13.

Instructions often get their operands from registers and store their results in registers. An ADD instruction might say "add the value in Register 1 to the value in Register 2, and store the result in Register 3."

The CPU organizes its registers into a **register file**: a set of register circuits (storage) and control circuits that manage reads and writes. Each register is a row of 1-bit storage circuits like the latch from last lecture, one per bit.

![A register file with four 32-bit registers, numbered 0 through 3, stacked in the middle. On the right, the outputs of all four registers feed two multiplexers. Select input Sr0 picks which register the top MUX sends to Data out 0, and select input Sr1 picks which register the bottom MUX sends to Data out 1. On the left, Data in is wired to every register. The WE (write enable) input passes through a DMUX, whose select input Sw chooses which single register receives the write enable signal.]({{ "/figures/ch5/regfile.png" | relative_url }})

The register file has one input and two outputs, so it can read two operands at once and write one result.

- **Reading.** Each output has its own MUX. The select inputs Sr<sub>0</sub> and Sr<sub>1</sub> choose which register each output reads.
- **Writing.** Data in goes to every register, but only one register actually stores it. The **write enable (WE)** bit goes through a **demultiplexer (DMUX)**, the reverse of a MUX: it takes one input and sends it to one of N outputs, chosen by Sw, and sends 0 to the rest. If WE is 1, only the register chosen by Sw gets a 1 and stores Data in. If WE is 0, nothing is written.

### Special-Purpose Registers

Recall from the von Neumann architecture that the CPU also has registers that keep track of the program:

- the **program counter (PC)** stores the memory address of the next instruction
- the **instruction register (IR)** stores the bits of the instruction currently being executed

The bits in the IR are what feed the ALU's opcode and the register file's select inputs. We have already seen both in the simulator: the PC moving up by 2 after each instruction, and `JZ` overwriting it.

## The CPU

Now we connect the pieces. Instruction operands often come from registers, so the register file's outputs are sent to the ALU's inputs. Instruction results are often stored in registers, so the ALU's result is sent back to the register file's input.

![The main parts of the CPU. The register file sits on the left, with inputs WE, Sw, Sr0, and Sr1. Its two outputs, Data out 0 and Data out 1, each pass through a MUX into ALU inputs A and B; each MUX can also choose inputs from other sources such as memory, the PC, or the IR. The opcode enters the top of the ALU, and condition codes leave the bottom. The ALU result loops back along the bottom of the diagram through a MUX into the register file's Data in, and can also go to other destinations such as memory or the PC. The PC and IR are shown in the top right as special-purpose registers. Main memory sits outside the CPU, connected by a bus.]({{ "/figures/ch5/cpu.png" | relative_url }})

The extra MUXes let the CPU move data between the ALU, the register file, and other components such as main memory. For example, an operand might come from memory instead of a register. The simulator's MUX from Demo 6 is one of these.

The ALU, the registers, and the buses that connect them make up the CPU's **data path**. The circuits that drive instruction execution, telling the data path what to do and when, make up the **control path**.

<div class="sim-demo" markdown="1">
<p class="sim-title">Demo 7 — one instruction through the data path</p>

[Open in the simulator](https://cpuvisualsimulator.github.io/?program=ICAgICAgICAgICAgICAgTE9EIFgKICAgICAgICAgICAgICAgQUREIFkKICAgICAgICAgICAgICAgU1RPIFoKICAgICAgICAgICAgICAgSExUClg6ICAgICAgICAgICAgIDYKWTogICAgICAgICAgICAgOApaOiAgICAgICAgICAgICAw){:target="_blank"}

```c
z = x + y;
```

```text
       LOD X
       ADD Y
       STO Z
       HLT
X:     6
Y:     8
Z:     0
```

Run the first instruction, then micro-step through `ADD Y`. The step text shows:

1. The PC is placed on the address bus, and a fetch is signalled on the control bus.
2. The instruction is loaded into the IR.
3. The opcode is decoded. The control unit sets the MUX (direct operand) and the ALU operation (`+`).
4. ACC is loaded into the ALU.
5. The operand's address is placed on the address bus, and the operand is loaded from RAM into the ALU.
6. The operation is executed: 6 + 8 = 14 goes into ACC, and the status word is updated.
7. The PC is incremented.

Steps 1–2 fetch the instruction, step 3 decodes it, and steps 4–6 execute it. Every click is driven by you here; in a real CPU, the clock drives each step.

</div>

## Looking Ahead

Next time we look at these stages in detail (fetch, decode, execute, write back), and how the clock drives them.
