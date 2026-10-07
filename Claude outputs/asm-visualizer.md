---
layout: page
title: ASM Visualizer Setup
permalink: /asm-visualizer/
---

The **ASM Visualizer** ([asm.diveintosystems.org](https://asm.diveintosystems.org)) runs assembly code one instruction at a time and shows you the registers and the stack after each step. It was made by the authors of our textbook. It is free and needs no account.

Use it for **every lab where you write assembly**. [Compiler Explorer]({{ "/godbolt/" | relative_url }}) shows what the compiler makes from C; the ASM Visualizer runs assembly.

## Pick a Mode

The home page has three modes. Each has three buttons: **x86**, **x86_64**, and **arm64**. **Always pick x86_64.** (x86 is the older 32-bit version and arm64 is what my Mac uses.) The title at the top of the page should end in "with x86_64 Architecture".

- **Arithmetic Mode**: just type instructions. The code to set up a function and the stack is added for you and hidden. Use this unless a lab says otherwise.
- **Function Mode**: write your own functions, called from `main`. It starts you with an empty `main`.
- **Full Program Mode**: paste in a complete program, including `main`. Use this for code copied from Compiler Explorer (see below).

There are no other settings to change. The visualizer uses AT&T syntax, like the book.

## Arithmetic Mode

1. Type your code in the box, one instruction per line. Comments start with `#`.
2. Next to the box, you can set starting values for four stack slots (`%rbp - 32` to `%rbp - 8`) and three registers (`%rax`, `%rcx`, `%rdx`). They all start at 0. Type numbers in decimal, or in hex starting with `0x`.
3. Click **Run Code**.

Try it: [this example](https://asm.diveintosystems.org/arithmetic/x86_64/mov%20-8%28%25rbp%29%2C%20%25rcx%0Aadd%20%25rcx%2C%20%25rax%0Asub%20%2410%2C%20%25rax/0/0/0/4/15/0/0) starts with 4 at `%rbp - 8` and 15 in `%rax`:

```text
mov -8(%rbp), %rcx
add %rcx, %rax
sub $10, %rax
```

When it finishes, `%rax` should hold 9 (`0x9`).

## Reading the Results

The page shows three tables: **Instructions**, **Stack Content**, and **Register Contents**.

- **Step through** with **First**, **Prev**, **Next**, and **Last**, or type a step number in the box.
- In the instructions, the **orange arrow** is the next line to run and the **blue arrow** is the line that just ran.
- In the stack, the **blue arrow** marks `%rsp` (the stack pointer) and the **orange arrow** marks `%rbp` (the frame pointer). Each row is 8 bytes.
- **Values are in hex.** Hover over any register or stack value to see it in decimal, binary, and two's complement.
- **A value in red** just changed.
- Only the registers your code uses are shown, plus `%rsp`, `%rbp`, and **RFLAG** (the flags register). Flags that are set are listed by name, like `ZF`.
- In Arithmetic Mode, check **Show all instructions** to see the hidden setup code.
- If the program prints something, it appears under **Program Output**.
- **Return to Editing Code** takes you back to the code box.

## Saving Your Work

**The visualizer does not save your code.** Keep your code in a file on the lab machines and paste it in.

**Click here to share the link of the current code!** copies a link with your code (and starting values) built in. Use it to come back to your code later, or to send it to me with a question.

## From Compiler Explorer to the Visualizer

To run code from Compiler Explorer:

1. In Compiler Explorer, open **Output...** and turn **off** **Compile to binary object**. The visualizer needs the compiler's version of the code, which uses labels like `.L4` instead of addresses.
2. Copy all of the assembly into **Full Program Mode**.
3. Add this line at the very top. (Compiler Explorer hides it.)

    ```text
            .globl  main
    ```

4. The visualizer can't read from the keyboard. If the program uses `scanf`, replace the lines that call it with `mov` instructions that set the variables directly.

Remember to turn **Compile to binary object** back on afterwards.

## Troubleshooting

| You see | Fix |
|---|---|
| Errors on registers like `%rax`, or the page title doesn't say x86_64 | Pick **x86_64**, not x86 or arm64 |
| `undefined reference to 'main'` | In Full Program Mode, add `.globl main` at the top |
| Errors on lines like `jg 83 <main+0x83>` | You copied disassembly. Turn off **Compile to binary object** in Compiler Explorer and copy again |
| **Last** shows a step past the end, and nothing changes | Type the last step number in the step box instead |

The visualizer is still in beta. If something breaks, let me know.
