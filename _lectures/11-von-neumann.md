---
title: "11 The Origins of Computing and the von Neumann Architecture"
published: True
---

[*Reading: Dive into Systems §5.1*](https://diveintosystems.org/book/C5-Arch/hist.html) and [*§5.2*](https://diveintosystems.org/book/C5-Arch/von.html)

## Pre-History

From the 1700s to early 1900s, the word computer was originally used to refer to "one who computes", as in a person who performed mathematical calculations by hand. This role was most often performed by women, who were widely considered to be better at doing mathematical calculations than men.

The first general purpose computer was developed by British mathematicians Charles Babbage and Ada Lovelace. Their Analytical Engine was a mechanical calculator based on an earlier invention of Babbage. Ada Lovelace was the first person to develop a computer program and publish an algorithm that could be executed by the machine. However, this distinction of 'first computer' is largely retrospective - most of the designers of what became the direct forerunners to the modern computer were unaware of the work of Babbage and Lovelace when they developed their own machines.

## Early Theory

It is perhaps more accurate to think about modern computer architecture rising out of a primordial soup of ideas and innovations that arose in the 1930s and 1940s. For example, in 1937, Claude Shannon, a student at MIT, wrote what would go on to be perhaps the most influential masters thesis of all time. Drawing upon the work of George Boole (the mathematician who developed Boolean algebra), Shannon showed that Boolean logic could be applied to circuits and could be used to develop electrical switches. This would lead to the development of the binary computing system, and much of future digital circuit design. While men would design many early electronic computers, women (who were not allowed to be engineers) became programming pioneers, leading the design and development of many early software innovations, such as programming languages, compilers, algorithms, and operating systems.

In 1937, British mathematician Alan Turing proposed the "Logical Computing Machine", a theoretical computer. Alonzo Church, Turing's dissertation advisor at Princeton University, was the first to refer to the logical computing machine as the Turing machine, and its universal form as the universal Turing machine. Turing used this machine to prove that there exists no solution to the decision problem, posed by the mathematicians David Hilbert and Wilhelm Ackermann in 1928, by showing that the halting problem was undecidable for a Turing machine.

Turing later returned to England and served his country as part of the code breaking unit in Bletchley Park during World War II. He was instrumental in the design and construction of the Bombe, an electromechanical device that helped break the cipher produced by the Enigma machine, which was commonly used by Nazi Germany to protect sensitive communication during World War II.

After the war, Turing designed the automatic computing engine (ACE). The ACE was a stored-program computer, meaning that both the program instructions and its data are loaded into the computer memory and run by the general-purpose computer. His paper, published in 1946, is perhaps the most detailed description of such a computer.

In 1952, the British government convicted Alan Turing of "gross indecency" for having a sexual relationship with a man. As a result of this conviction, Turing's security clearance in Britain was revoked, he was denied entry to the United States and he was forced to undergo a course of hormone injections known then as "chemical castration". On June 8, 1954, Alan Turing was found dead in his home; the cause of death was determined to be cyanide poisoning.

## Early Electronic Computers

Much of the early development of computers was done during the second world war. In 1943, British engineer Tommy Flowers developed the Colossus, a code breaking machine which is arguably the first programmable, digital, and fully electronic computer. The Women's Royal Naval Service (WRNS, known as the "Wrens") served as operators of Colossus. In spite of the General Report of the Tunny noting that several of the Wrens showed ability in cryptographic work, none of them were given the position of cryptographer, and instead were delegated more menial Colossus operation tasks.

Harvard professor Howard Aiken (who was also a Naval Commander in the U.S. Navy Reserves) designed the Mark I, an electromechanical, general-purpose programmable computer. Meanwhile, American engineers John Mauchly and Presper Eckert of the University of Pennsylvania designed and built the Electronic Numerical Integrator and Computer (ENIAC) in 1945. ENIAC is arguably the forerunner of modern computers. It was digital (though it used decimal rather than binary), fully electronic, programmable, and general purpose.

As men were drafted into the armed forces during World War II, women were hired to help in the war effort as human computers. With the arrival of the first electronic computers, women became the first programmers, as programming was considered secretarial work. It should come as no surprise that many of the early innovations in programming, such as the first compiler, the notion of modularizing programs, debugging, and assembly language, are credited to women inventors. Grace Hopper, for example, developed the first compiler (A-0) and the first high-level programming language (FLOW-MATIC), the latter which heavily influenced the design of COBOL, the first machine-independent programming language. Hopper was also a programmer for the Mark I and wrote the book that described its operation.

The ENIAC programmers were six women: Jean Jennings Bartik, Betty Snyder Holberton, Kay McNulty Mauchly, Frances Bilas Spence, Marlyn Wescoff Meltzer, and Ruth Lichterman Teitelbaum. Unlike the Wrens, the ENIAC women were given a great deal of autonomy in their task; given just the wiring diagrams of ENIAC, they were told to figure out how it worked and how to program it. In addition to their innovation in solving how to program (and debug) one of the world's first electronic general-purpose computers, the ENIAC programmers also developed the idea of algorithmic flow charts, and developed important programming concepts such as subroutines and nesting. Like Grace Hopper, Jean Jennings Bartik and Betty Snyder Holberton would go on to have long careers in computing, and are some of the early computing pioneers. Unfortunately, the full extent of women's contributions in early computing is not known. Unable to advance, many women left the field after World War II. To learn more about early women programmers, we encourage readers to check out Recoding Gender by Janet Abbate, Top Secret Rosies, a PBS documentary directed by LeAnn Erickson, and "The Computers" by Kathy Kleiman.

## John von Neumann and his Architecture

John von Neumann is considered by some to be the greatest mathematician in history. He made foundational contributions to mathematics, physics, economics, statistics and computing. Born in 1903 in Budapest, von Neumann was considered a child prodigy rumored to speak Hungarian, French, English, German and Italian as well as being skilled in differential and integral calculus by age 8. He completed a PhD in mathematics from the University of Budapest in 1926, and in 1933 accepted a tenured professorship at the Institute for Advanced Study in Princeton, New Jersey where he would remain for the rest of his career.

![A person standing in the snow beside John von Neumann's grave marker in Princeton Cemetery. The granite stone is engraved JOHN von NEUMANN 1903 - 1957, with Margaret von Neumann's name below it, and small stones left by visitors line the top of the marker.]({{ "/figures/ch5/von-neumann.png" | relative_url }})

John von Neumann's contributions weave in and out of several of the previous stories in computing. A Hungarian mathematician, he was a professor at both the Institute of Advanced Study and Princeton University, and he served as an early mentor to Alan Turing. Later, von Neumann became a research scientist on the Manhattan Project, which led him to Howard Aiken and the Mark I; he would later serve as a consultant on the ENIAC project, and correspond regularly with Eckert and Mauchly. His famous paper describing EDVAC came from his work on the Electronic Discrete Variable Automatic Computer (EDVAC), proposed to the U.S. Army by Eckert and Mauchly, and built at the University of Pennsylvania. EDVAC included several architectural design innovations that form the foundation of almost all modern computers: it was general purpose, used the binary numeric system, had internal memory, and was fully electric. In large part because von Neumann was the sole author of the paper, the architectural design the paper describes is primarily credited to von Neumann and has become known as the von Neumann architecture. It should be noted that Turing described in great detail the design of a similar machine in 1946. However, since von Neumann's paper was published before Turing's, von Neumann received the chief credit for these innovations.

In 1945, John von Neumann published a paper, "First draft of a report on the EDVAC", which describes an architecture on which modern computers are based. EDVAC was the successor of ENIAC. It differed from ENIAC in that it was a binary computer instead of decimal, and it was a stored-program computer. Today, this description of EDVAC's architectural design is known as the von Neumann architecture.

The von Neumann architecture describes a general-purpose computer, one that is designed to run any program. It also uses a stored-program model, meaning that program instructions and data are both loaded onto the computer to run. In the von Neumann model there is no distinction between instructions and data; both are loaded into the computer's internal memory, and program instructions are fetched from memory and executed by the computer's functional units that execute program instructions on program data.

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
2. The control unit **decodes** the instruction stored in the IR.
3. The processing unit **executes** the instruction.
4. The control unit **stores** the result to memory.

The cycle then repeats with whatever instruction the PC now names. A running program is nothing more than this loop, repeated billions of times a second.

<div class="board-example" markdown="1">
<p class="board-title">Example — one trip around the cycle</p>

Suppose the next instruction is an addition, it is stored at memory address 1234, and its two operands are already sitting in registers holding the values 3 and 4.

**1. Fetch.** The PC holds 1234, so the control unit puts 1234 on the address bus and READ on the control bus. Memory responds by sending the bits stored at 1234 back over the data bus, and the control unit stores those bits in the IR. The PC is then updated to hold the address of the next instruction, 1238.

**2. Decode.** The bits now in the IR encode both which operation to perform and which registers hold its operands. The control unit decodes them and wires up the processing unit accordingly: these two registers feed the ALU, and the ALU performs this operation.

![Two panels of the fetch-decode cycle. In the first, the CPU sends the address 1234 on the address bus and READ on the control bus to the memory unit, which returns the instruction bits 101011... on the data bus into the IR; the PC holds 1234. In the second, the PC has advanced to 1238 and arrows run from the IR back into the registers and the ALU, showing the instruction bits selecting which registers supply the operands and which operation the ALU performs.]({{ "/figures/ch5/vonFD.png" | relative_url }})

**3. Execute.** The operands 3 and 4 travel from the registers into the ALU, which performs the operation the instruction named (+) and produces the result 7.

**4. Store.** The result has to go somewhere. Here the control unit writes it to memory: it puts the destination address 5678 on the address bus, WRITE on the control bus, and the result 7 — in binary, 00000111 — on the data bus. Memory writes that value at address 5678.

![Two panels of the execute-store cycle. In the first, the ALU takes the operands 3 and 4 from the registers and produces the result 7. In the second, the CPU sends the address 5678 on the address bus, WRITE on the control bus, and the value 00000111 on the data bus, and the memory unit stores that value at address 5678.]({{ "/figures/ch5/vonES.png" | relative_url }})

</div>

Notice that memory is touched twice in a single instruction: once to fetch the instruction itself, and once to store the result. Both trips go over the same buses, and both are far slower than anything happening inside the CPU. That cost is the reason registers exist, and it is a thread we pick up again when we get to the memory hierarchy.
