---
title: "12 The von Neumann Architecture"
published: True
---

<!-- Next semester, add one more example - ran through too quickly, only took ~30 minutes -->

[*Reading: Dive into Systems §5.1*](https://diveintosystems.org/book/C5-Arch/hist.html) and [*§5.2*](https://diveintosystems.org/book/C5-Arch/von.html)

## The von Neumann Architecture

The architecture has **five major components**:

1. the **processing unit**
2. the **control unit**
3. the **memory unit**
4. the **input units**
5. the **output units**

![A block diagram of the von Neumann architecture. A dashed box labeled "The CPU" encloses two blocks, 1. Processing Unit (containing the ALU and registers) and 2. Control Unit (containing the PC and IR). Outside the CPU are three more blocks: 3. Memory Unit, 4. Input Units, and 5. Output Units. Three horizontal lines labeled address bus, control bus, and data bus run beneath the blocks, with vertical connections tying every unit to all three buses.]({{ "/figures/ch5/vonNArch.png" | relative_url }})

The units are connected by **buses**, which they use to send control and data information to one another. A bus is a communication channel that transfers binary values between communication endpoints (the senders and receivers of the values). For example, a data bus that connects the memory unit and the CPU could be implemented as 32 parallel wires that together transfer a 4-byte value, 1 bit transferred on each wire.

There are three buses in the figure, and their names describe what travels on each one. The **address bus** carries *which* memory location is being talked about, the **control bus** carries *what* is being done to it (a read or a write), and the **data bus** carries the value itself.

### The CPU

In hardware, the processing unit and the control unit are packaged together as one unit: the **CPU**. The other three units sit outside of it, and everything the CPU knows about the outside world arrives over the buses.

**The processing unit** is where the work of the program actually happens. It contains two parts:

- the **arithmetic/logic unit (ALU)**, which performs mathematical operations such as addition and subtraction, and logical operations such as or, and, and xor
- **registers**, a small, fast unit of storage used to hold program data and the instructions that are being executed by the ALU

**The control unit** drives the execution of program instructions by loading them from memory and feeding instruction operands and operations through the processing unit. It has storage of its own, dedicated to keeping track of where the program is:

- the **program counter (PC)** keeps the memory address of the next instruction to execute
- the **instruction register (IR)** stores the instruction, loaded from memory, that is currently being executed

The processing unit does the computing; the control unit decides what gets computed and when. Neither is useful without the other.

### The Memory Unit

Internal memory is a key innovation of the von Neumann architecture. It provides program data storage that is close to the processing unit, significantly reducing the amount of time to perform calculations. The memory unit stores both program data and program instructions — storing program instructions is a key part of the stored-program model of the von Neumann architecture.

We discuss the memory hierarchy in more detail later in the semester, but for now think of this basic tradeoff: memory that is closer to the CPU is faster to access, but must be smaller.

### Input and Output Units

The **input unit** consists of the set of devices that enable a user or program to get data from the outside world into the computer. The most common forms of input devices today are the keyboard and mouse.

The **output unit** consists of the set of devices that relay results of computation from the computer back to the outside world, or that store results outside internal memory. For example, the monitor is a common output device.

Some devices do both. Solid-state and hard drives are an example of devices that act as both input and output devices.

## Executing a Program

The five units that make up the von Neumann architecture work together to implement a **fetch-decode-execute-store** cycle of actions that together execute program instructions:

1. The control unit **fetches** the next instruction from memory.
2. The control unit **decodes** the instruction stored in the Instruction Register.
3. The processing unit **executes** the instruction.
4. The control unit **stores** the result to memory.

The cycle then repeats with whatever instruction the PC now names. A running program is nothing more than this loop, repeated billions of times a second.

<div class="board-example" markdown="1">
<p class="board-title">Example — one trip around the cycle</p>

Suppose the next instruction is an addition, it is stored at memory address 1234, and its two operands are already sitting in registers holding the values 3 and 4.

**1. Fetch.** The Program Counter holds 1234, so the control unit puts 1234 on the address bus and READ on the control bus. Memory responds by sending the bits stored at 1234 back over the data bus, and the control unit stores those bits in the Instruction Register. The Program Counter is then updated to hold the address of the next instruction, 1238.

**2. Decode.** The bits now in the IR encode both which operation to perform and which registers hold its operands. The control unit decodes them and wires up the processing unit accordingly: these two registers feed the ALU, and the ALU performs this operation.

![Two panels of the fetch-decode cycle. In the first, the CPU sends the address 1234 on the address bus and READ on the control bus to the memory unit, which returns the instruction bits 101011... on the data bus into the IR; the PC holds 1234. In the second, the PC has advanced to 1238 and arrows run from the IR back into the registers and the ALU, showing the instruction bits selecting which registers supply the operands and which operation the ALU performs.]({{ "/figures/ch5/vonFD.png" | relative_url }})

**3. Execute.** The operands 3 and 4 travel from the registers into the ALU, which performs the operation the instruction named (+) and produces the result 7.

**4. Store.** The result has to go somewhere. Here the control unit writes it to memory: it puts the destination address 5678 on the address bus, WRITE on the control bus, and the result 7 — in binary, 00000111 — on the data bus. Memory writes that value at address 5678.

![Two panels of the execute-store cycle. In the first, the ALU takes the operands 3 and 4 from the registers and produces the result 7. In the second, the CPU sends the address 5678 on the address bus, WRITE on the control bus, and the value 00000111 on the data bus, and the memory unit stores that value at address 5678.]({{ "/figures/ch5/vonES.png" | relative_url }})

</div>

Notice that memory is touched twice in a single instruction: once to fetch the instruction itself, and once to store the result. Both trips go over the same buses, and both are far slower than anything happening inside the CPU. Optimizing computer performance is largely a game of reducing the time data spends in transit.