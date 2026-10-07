### 5. Running It

Compiler Explorer shows us the assembly, but it doesn't let us watch it run. For that we use the **ASM Visualizer**, made by the authors of our textbook. [Here is how it works]({{ "/asm-visualizer/" | relative_url }}).

To move the unoptimized code over:

1. In Compiler Explorer, clear `-O1` and turn **off** Output... → **Compile to binary object**. The visualizer needs the compiler's version of the code, with labels like `.L4` instead of addresses.
2. Copy the assembly into the visualizer's **Full Program Mode** (x86_64).
3. Add `.globl main` as the first line. Compiler Explorer hides it.
4. The visualizer can't read from the keyboard, so replace the six lines that call `scanf` with two lines that set `u` and `b`:

```text
        movl    $26, -16(%rbp)      # u = 26
        movl    $16, -20(%rbp)      # b = 16
```

Or [open the finished program in the ASM Visualizer](https://asm.diveintosystems.org/fullprog/x86_64/%20%20%20%20%20%20%20%20.globl%20%20main%0A.LC0%3A%0A%20%20%20%20%20%20%20%20.string%20%22Enter%20a%20number%20and%20a%20base%3A%20%22%0A.LC1%3A%0A%20%20%20%20%20%20%20%20.string%20%22%25d%20%25d%22%0A.LC2%3A%0A%20%20%20%20%20%20%20%20.string%20%22%25s%22%0Amain%3A%0A%20%20%20%20%20%20%20%20pushq%20%20%20%25rbp%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rsp%2C%20%25rbp%0A%20%20%20%20%20%20%20%20addq%20%20%20%20%24-128%2C%20%25rsp%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC0%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2426%2C%20-16%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%2416%2C%20-20%28%25rbp%29%0A%20%20%20%20%20%20%20%20leaq%20%20%20%20-128%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20addq%20%20%20%20%2499%2C%20%25rax%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rax%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%240%2C%20%28%25rax%29%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%241%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%2410%2C%20%28%25rax%29%0A.L4%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-20%28%25rbp%29%2C%20%25ecx%0A%20%20%20%20%20%20%20%20cltd%0A%20%20%20%20%20%20%20%20idivl%20%20%20%25ecx%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25edx%2C%20-12%28%25rbp%29%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-20%28%25rbp%29%2C%20%25ecx%0A%20%20%20%20%20%20%20%20cltd%0A%20%20%20%20%20%20%20%20idivl%20%20%20%25ecx%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20-16%28%25rbp%29%0A%20%20%20%20%20%20%20%20subq%20%20%20%20%241%2C%20-8%28%25rbp%29%0A%20%20%20%20%20%20%20%20cmpl%20%20%20%20%249%2C%20-12%28%25rbp%29%0A%20%20%20%20%20%20%20%20jg%20%20%20%20%20%20.L2%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-12%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%2448%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25edx%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%25dl%2C%20%28%25rax%29%0A%20%20%20%20%20%20%20%20jmp%20%20%20%20%20.L3%0A.L2%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-12%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20addl%20%20%20%20%2455%2C%20%25eax%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%25eax%2C%20%25edx%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movb%20%20%20%20%25dl%2C%20%28%25rax%29%0A.L3%3A%0A%20%20%20%20%20%20%20%20movl%20%20%20%20-16%28%25rbp%29%2C%20%25eax%0A%20%20%20%20%20%20%20%20testl%20%20%20%25eax%2C%20%25eax%0A%20%20%20%20%20%20%20%20jne%20%20%20%20%20.L4%0A%20%20%20%20%20%20%20%20movq%20%20%20%20-8%28%25rbp%29%2C%20%25rax%0A%20%20%20%20%20%20%20%20movq%20%20%20%20%25rax%2C%20%25rsi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%24.LC2%2C%20%25edi%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20call%20%20%20%20printf%0A%20%20%20%20%20%20%20%20movl%20%20%20%20%240%2C%20%25eax%0A%20%20%20%20%20%20%20%20leave%0A%20%20%20%20%20%20%20%20ret) and click **Run Code**.

<details class="code-example" markdown="1">
<summary>Show code: the finished program</summary>

```text
        .globl  main
.LC0:
        .string "Enter a number and a base: "
.LC1:
        .string "%d %d"
.LC2:
        .string "%s"
main:
        pushq   %rbp
        movq    %rsp, %rbp
        addq    $-128, %rsp
        movl    $.LC0, %edi
        movl    $0, %eax
        call    printf
        movl    $26, -16(%rbp)
        movl    $16, -20(%rbp)
        leaq    -128(%rbp), %rax
        addq    $99, %rax
        movq    %rax, -8(%rbp)
        movq    -8(%rbp), %rax
        movb    $0, (%rax)
        subq    $1, -8(%rbp)
        movq    -8(%rbp), %rax
        movb    $10, (%rax)
.L4:
        movl    -16(%rbp), %eax
        movl    -20(%rbp), %ecx
        cltd
        idivl   %ecx
        movl    %edx, -12(%rbp)
        movl    -16(%rbp), %eax
        movl    -20(%rbp), %ecx
        cltd
        idivl   %ecx
        movl    %eax, -16(%rbp)
        subq    $1, -8(%rbp)
        cmpl    $9, -12(%rbp)
        jg      .L2
        movl    -12(%rbp), %eax
        addl    $48, %eax
        movl    %eax, %edx
        movq    -8(%rbp), %rax
        movb    %dl, (%rax)
        jmp     .L3
.L2:
        movl    -12(%rbp), %eax
        addl    $55, %eax
        movl    %eax, %edx
        movq    -8(%rbp), %rax
        movb    %dl, (%rax)
.L3:
        movl    -16(%rbp), %eax
        testl   %eax, %eax
        jne     .L4
        movq    -8(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC2, %edi
        movl    $0, %eax
        call    printf
        movl    $0, %eax
        leave
        ret
```

</details>

Step through it with **Next** and watch:

- **The loop runs twice**, once for each digit of 26 in base 16: `1A`.
- **The first time, `r` is 10**, so `jg` jumps to `.L2`, the `else`.
- **The second time, `r` is 1**, so the code falls through to the `if` part, and `jmp` skips over the `else`.
- **The program prints `1A`** under Program Output at the end.

