---
title: "27 Writing and Tracing Functions"
published: true
---

[*No new reading. Review Dive into Systems §7.5*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To check your answers, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

These are the two kinds of problems you'll see on Midterm 2: write the assembly for a C function, and answer questions about what a program does to the registers and the stack.

Work in pairs, write your functions in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64). Trace the register and stack values on paper, then check them with the visualizer.

## Writing a Function

1. **A label** with the function's name.
2. **Set up the frame:** `push %rbp`, then `mov %rsp, %rbp`.
3. **The body.** The arguments arrive in `%rdi`, `%rsi`, and `%rdx`, in that order. Leave the return value in `%rax`.
4. **Clean up and return:** `pop %rbp` and `ret`, or `leave` and `ret` if the function moved `%rsp`.

- **A call can change `%rax`, `%rcx`, `%rdx`, `%rsi`, `%rdi`, and `%r8` through `%r11`.** If you need a value after a call, save it in your own frame: make room with `sub`, store it at `-8(%rbp)`, and end with `leave`.

## Tracing a Program

| Instruction | What changes |
|---|---|
| `push %rbp` | `%rsp` goes down 8, then `%rbp` is copied to the new top |
| `pop %rbp` | the top is copied to `%rbp`, then `%rsp` goes up 8 |
| `sub $n,%rsp` | `%rsp` goes down `n`. Nothing is written |
| `mov` to memory | only that memory. `%rsp` doesn't move |
| `call F` | `%rsp` goes down 8, the return address is copied there, then `%rip` = F |
| `ret` | the top is copied to `%rip`, then `%rsp` goes up 8 |
| `leave` | `%rsp` = `%rbp`, then `pop %rbp` |

Whatever is inside a function, `call` and `push %rbp` each move `%rsp` down 8, and after `ret` everything is back where it was.

## Problem 1

<div class="board-example" markdown="1">
<p class="board-title">Problem 1 — perimeter</p>

<div class="board-setup" markdown="1">

```c
long perimeter(long w, long h) {
    return 2 * (w + h);
}
```

1. Write `perimeter` in assembly, and a `main` that calls `perimeter(6, 7)`.
2. Just before the `call`, `%rsp` is 0xd30 and `%rbp` is 0xd40. Fill in the table.
3. What is in `%rax` after `ret`?

| Point | `%rsp` | `%rbp` | Top of the stack |
|---|---|---|---|
| Right after `call` | | | |
| Right after `perimeter`'s `mov %rsp, %rbp` | | | |
| Right after `ret` | | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
perimeter:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start perimeter's frame
  mov %rdi, %rax          # result = w
  add %rsi, %rax          # result = w + h
  add %rax, %rax          # result = 2 * (w + h)
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax

main:
  push %rbp
  mov %rsp, %rbp
  mov $6, %rdi            # w = 6
  mov $7, %rsi            # h = 7
  call perimeter          # %rax = perimeter(6, 7)
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09perimeter%0A%20%20.type%09perimeter%2C%20%40function%0A%0Aperimeter%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20perimeter's%20frame%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20w%0A%20%20add%20%25rsi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20w%20%2B%20h%0A%20%20add%20%25rax%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%202%20*%20(w%20%2B%20h)%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09perimeter%2C%20.-perimeter%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%246%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20w%20%3D%206%0A%20%20mov%20%247%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20h%20%3D%207%0A%20%20call%20perimeter%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20perimeter(6%2C%207)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A%0A)

| Point | `%rsp` | `%rbp` | Top of the stack |
|---|---|---|---|
| Right after `call` | 0xd28 | 0xd40 | the return address: the address of the instruction after the `call` |
| Right after `perimeter`'s `mov %rsp, %rbp` | 0xd20 | 0xd20 | 0xd40, `main`'s `%rbp` |
| Right after `ret` | 0xd30 | 0xd40 | whatever `main` had there |

`%rax` is 26.

- **Adding a number to itself doubles it,** so `add %rax, %rax` is `2 * (w + h)`. `imul $2, %rax` or `shl $1, %rax` work too.
- **The table doesn't depend on the body.** Whatever `perimeter` does in between, `call` and `push %rbp` move `%rsp` the same way.

</details>

</div>

## Problem 2

<div class="board-example" markdown="1">
<p class="board-title">Problem 2 — abs_diff</p>

<div class="board-setup" markdown="1">

```c
long abs_diff(long x, long y) {
    if (x > y) {
        return x - y;
    }
    return y - x;
}
```

1. Write `abs_diff` in assembly, and a `main` that calls `abs_diff(3, 10)`.
2. For `abs_diff(3, 10)`, list the instructions in `abs_diff` that run, in order. What is `%rax` after each one that changes it?
3. Which instructions run instead for `abs_diff(10, 3)`?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
abs_diff:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start abs_diff's frame
  cmp %rsi, %rdi          # compare x with y
  jle abs_else            # if x <= y, go to the else
  mov %rdi, %rax          # result = x
  sub %rsi, %rax          # result = x - y
  jmp abs_done            # skip the else
abs_else:
  mov %rsi, %rax          # result = y
  sub %rdi, %rax          # result = y - x
abs_done:
  pop %rbp                # restore main's %rbp
  ret                     # return result in %rax

main:
  push %rbp
  mov %rsp, %rbp
  mov $3, %rdi            # x = 3
  mov $10, %rsi           # y = 10
  call abs_diff           # %rax = abs_diff(3, 10)
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09abs_diff%0A%20%20.type%09abs_diff%2C%20%40function%0A%0Aabs_diff%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20abs_diff's%20frame%0A%20%20cmp%20%25rsi%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%23%20compare%20x%20with%20y%0A%20%20jle%20abs_else%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20x%20%3C%3D%20y%2C%20go%20to%20the%20else%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20x%0A%20%20sub%20%25rsi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20x%20-%20y%0A%20%20jmp%20abs_done%20%20%20%20%20%20%20%20%20%20%20%20%23%20skip%20the%20else%0Aabs_else%3A%0A%20%20mov%20%25rsi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20y%0A%20%20sub%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20y%20-%20x%0Aabs_done%3A%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09abs_diff%2C%20.-abs_diff%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%243%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%203%0A%20%20mov%20%2410%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20y%20%3D%2010%0A%20%20call%20abs_diff%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20abs_diff(3%2C%2010)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A%0A)

For `abs_diff(3, 10)`: `push`, `mov`, `cmp`, `jle` (it jumps), `mov %rsi, %rax` (`%rax` = 10), `sub %rdi, %rax` (`%rax` = 7), `pop`, `ret`.

For `abs_diff(10, 3)`: `push`, `mov`, `cmp`, `jle` (it doesn't jump), `mov %rdi, %rax` (`%rax` = 10), `sub %rsi, %rax` (`%rax` = 7), `jmp`, `pop`, `ret`.

- **The `if` is a compare and a jump, as before.** `cmp %rsi, %rdi` computes `x - y`, so `jle` jumps to the `else` when `x <= y`: the C condition, flipped.
- **Two `ret`s is fine too.** Each branch can end with its own `pop %rbp` and `ret`.

</details>

</div>

## Problem 3

<div class="board-example" markdown="1">
<p class="board-title">Problem 3 — a function that calls a function</p>

<div class="board-setup" markdown="1">

```c
long sq(long n) {
    return n * n;
}

long dist2(long x, long y) {
    return sq(x) + sq(y);
}
```

1. Write `sq`, `dist2`, and a `main` that calls `dist2(3, 4)`. `dist2` calls another function, so think about which values it still needs after each call.
2. Just before `main`'s `call dist2`, `%rsp` is 0xd30 and `%rbp` is 0xd40. Using your code, draw the stack during the **first** call to `sq`, right after `sq`'s `mov %rsp, %rbp`. Label every slot, and mark where `%rsp` and `%rbp` point.
3. Why does `dist2` need `sub` and `leave` when `sq` doesn't?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
sq:
  push %rbp               # save the caller's %rbp
  mov %rsp, %rbp          # start sq's frame
  mov %rdi, %rax          # result = n
  imul %rdi, %rax         # result = n * n
  pop %rbp                # restore the caller's %rbp
  ret                     # return result in %rax

dist2:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start dist2's frame
  sub $16, %rsp           # room for y and sq(x)
  mov %rsi, -8(%rbp)      # save y: a call can change %rsi
  call sq                 # %rax = sq(x); x is already in %rdi
  mov %rax, -16(%rbp)     # save sq(x)
  mov -8(%rbp), %rdi      # argument: y
  call sq                 # %rax = sq(y)
  add -16(%rbp), %rax     # result = sq(x) + sq(y)
  leave                   # throw away dist2's frame
  ret                     # return result in %rax

main:
  push %rbp
  mov %rsp, %rbp
  mov $3, %rdi            # x = 3
  mov $4, %rsi            # y = 4
  call dist2              # %rax = dist2(3, 4)
  pop %rbp
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sq%0A%20%20.type%09sq%2C%20%40function%0A%0Asq%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sq's%20frame%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20n%0A%20%20imul%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20n%20*%20n%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20the%20caller's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09sq%2C%20.-sq%0A%0A%20%20.globl%09dist2%0A%20%20.type%09dist2%2C%20%40function%0A%0Adist2%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20dist2's%20frame%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20y%20and%20sq(x)%0A%20%20mov%20%25rsi%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20save%20y%3A%20a%20call%20can%20change%20%25rsi%0A%20%20call%20sq%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sq(x)%3B%20x%20is%20already%20in%20%25rdi%0A%20%20mov%20%25rax%2C%20-16(%25rbp)%20%20%20%20%20%23%20save%20sq(x)%0A%20%20mov%20-8(%25rbp)%2C%20%25rdi%20%20%20%20%20%20%23%20argument%3A%20y%0A%20%20call%20sq%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sq(y)%0A%20%20add%20-16(%25rbp)%2C%20%25rax%20%20%20%20%20%23%20result%20%3D%20sq(x)%20%2B%20sq(y)%0A%20%20leave%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20throw%20away%20dist2's%20frame%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09dist2%2C%20.-dist2%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20mov%20%243%2C%20%25rdi%20%20%20%20%20%20%20%20%20%20%20%20%23%20x%20%3D%203%0A%20%20mov%20%244%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20y%20%3D%204%0A%20%20call%20dist2%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20dist2(3%2C%204)%0A%20%20pop%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A%0A). `%rax` ends at 25.

| Address | Value | |
|---|---|---|
| 0xd00 | 0xd20 (`dist2`'s `%rbp`) | ← `%rsp`, `%rbp` |
| 0xd08 | return address into `dist2` | |
| 0xd10 | (`sq(x)`, not saved yet) | ← `dist2`'s `%rsp` |
| 0xd18 | 4 (`y`) | |
| 0xd20 | 0xd40 (`main`'s `%rbp`) | ← `dist2`'s `%rbp` |
| 0xd28 | return address into `main` | |
| 0xd30 | | ← `main`'s `%rsp` |
| 0xd38 | | |
| 0xd40 | (saved by `main`) | ← `main`'s `%rbp` |

`dist2` keeps two values on the stack across calls: `y` and `sq(x)`. Each `call` pushes at `%rsp`, so `dist2` has to move `%rsp` below those values first, and `leave` puts it back. `sq` calls nothing, so nothing will be pushed on top of its frame.

- **`y` has to be saved before the first call.** It arrives in `%rsi`, and `sq` is allowed to change `%rsi`. Our `sq` happens not to, but you can't count on that.
- **`x` doesn't need saving.** It's already in `%rdi`, where `sq` wants its argument, and `dist2` never needs it again.
- **Your slots may be in a different order.** That's fine, as long as each saved value is in `dist2`'s frame.
- **Three frames, same layout.** In each one, the return address is at `8(%rbp)` and the caller's `%rbp` is at `0(%rbp)`.

</details>

</div>

## Problem 4

This one is compiled by `gcc` without optimization. The listing is cleaned up a little: it uses the 64-bit registers and function names instead of call addresses.

<div class="board-example" markdown="1">
<p class="board-title">Problem 4 — what does it print?</p>

```c
long setup(long n) {
    long p = n + 1;
    long q = n * 2;
    return p + q;
}

long use(void) {
    long u = 5;
    long v;
    return u + v;
}

int main(void) {
    setup(10);
    printf("%ld\n", use());
    return 0;
}
```

```text
setup:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  mov    %rdi,-0x18(%rbp)
  40112e:  mov    -0x18(%rbp),%rax
  401132:  add    $0x1,%rax
  401136:  mov    %rax,-0x10(%rbp)
  40113a:  mov    -0x18(%rbp),%rax
  40113e:  add    %rax,%rax
  401141:  mov    %rax,-0x8(%rbp)
  401145:  mov    -0x10(%rbp),%rdx
  401149:  mov    -0x8(%rbp),%rax
  40114d:  add    %rdx,%rax
  401150:  pop    %rbp
  401151:  ret

use:
  401152:  push   %rbp
  401153:  mov    %rsp,%rbp
  401156:  movq   $0x5,-0x10(%rbp)
  40115e:  mov    -0x10(%rbp),%rdx
  401162:  mov    -0x8(%rbp),%rax
  401166:  add    %rdx,%rax
  401169:  pop    %rbp
  40116a:  ret

main:
  40116b:  push   %rbp
  40116c:  mov    %rsp,%rbp
  40116f:  mov    $0xa,%rdi
  401174:  call   setup
  401179:  call   use
  40117e:  mov    %rax,%rsi
  401181:  mov    $0x402004,%rdi
  401186:  mov    $0x0,%rax
  40118b:  call   printf
  401190:  mov    $0x0,%rax
  401195:  pop    %rbp
  401196:  ret
```

<div class="board-setup" markdown="1">

1. Annotate `setup`. Where are `n`, `p`, and `q`?
2. Annotate `use`. Where are `u` and `v`?
3. Do `setup` and `use` get the same `%rbp`? Why?
4. What does the program print?
5. `main` has no `sub $...,%rsp`. Why not?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
setup:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  mov    %rdi,-0x18(%rbp)     # n
  40112e:  mov    -0x18(%rbp),%rax     # %rax = n
  401132:  add    $0x1,%rax            # %rax = n + 1
  401136:  mov    %rax,-0x10(%rbp)     # p = n + 1
  40113a:  mov    -0x18(%rbp),%rax     # %rax = n
  40113e:  add    %rax,%rax            # %rax = n + n
  401141:  mov    %rax,-0x8(%rbp)      # q = n * 2
  401145:  mov    -0x10(%rbp),%rdx     # %rdx = p
  401149:  mov    -0x8(%rbp),%rax      # %rax = q
  40114d:  add    %rdx,%rax            # return p + q
  401150:  pop    %rbp
  401151:  ret

use:
  401152:  push   %rbp
  401153:  mov    %rsp,%rbp
  401156:  movq   $0x5,-0x10(%rbp)     # u = 5
  40115e:  mov    -0x10(%rbp),%rdx     # %rdx = u
  401162:  mov    -0x8(%rbp),%rax      # %rax = v (never set)
  401166:  add    %rdx,%rax            # return u + v
  401169:  pop    %rbp
  40116a:  ret
```

1. `n` is at `-0x18(%rbp)`, `p` at `-0x10(%rbp)`, and `q` at `-0x8(%rbp)`.
2. `u` is at `-0x10(%rbp)` and `v` at `-0x8(%rbp)`.
3. Yes. Both are called from `main` with the same `%rsp`, so both frames start at the same address.
4. `25`. `v` is in the slot where `setup` left `q` = 20, and `u` = 5.
5. `main` has no local variables, so it doesn't need any space on the stack.

- **Read the offsets, don't guess them.** The compiler didn't put the variables in the order they are declared. If you assumed `v` lines up with `p`, you'd get 16.
- **`add %rax,%rax` is `n * 2`.** Adding a number to itself doubles it.
- **This is the same bug as `adder`.** With `-O1`, it prints `5`.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIHNldHVwKGxvbmcgbikge1xuICAgIGxvbmcgcCA9IG4gKyAxO1xuICAgIGxvbmcgcSA9IG4gKiAyO1xuICAgIHJldHVybiBwICsgcTtcbn1cblxubG9uZyB1c2Uodm9pZCkge1xuICAgIGxvbmcgdSA9IDU7XG4gICAgbG9uZyB2O1xuICAgIHJldHVybiB1ICsgdjtcbn1cblxuaW50IG1haW4odm9pZCkge1xuICAgIHNldHVwKDEwKTtcbiAgICBwcmludGYoXCIlbGRcXG5cIiwgdXNlKCkpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D) or [run it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.section%20.rodata%0A.LC0%3A%0A%20%20.string%20%22%25ld%5Cn%22%0A%0A%20%20.text%0A%20%20.globl%09setup%0A%20%20.type%09setup%2C%20%40function%0A%0Asetup%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%25rdi%2C%20-24(%25rbp)%0A%20%20movq%20-24(%25rbp)%2C%20%25rax%0A%20%20addq%20%241%2C%20%25rax%0A%20%20movq%20%25rax%2C%20-16(%25rbp)%0A%20%20movq%20-24(%25rbp)%2C%20%25rax%0A%20%20addq%20%25rax%2C%20%25rax%0A%20%20movq%20%25rax%2C%20-8(%25rbp)%0A%20%20movq%20-16(%25rbp)%2C%20%25rdx%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20addq%20%25rdx%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09setup%2C%20.-setup%0A%0A%20%20.globl%09use%0A%20%20.type%09use%2C%20%40function%0A%0Ause%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%245%2C%20-16(%25rbp)%0A%20%20movq%20-16(%25rbp)%2C%20%25rdx%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20addq%20%25rdx%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09use%2C%20.-use%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%2410%2C%20%25rdi%0A%20%20call%20setup%0A%20%20call%20use%0A%20%20movq%20%25rax%2C%20%25rsi%0A%20%20movq%20%24.LC0%2C%20%25rdi%0A%20%20movq%20%240%2C%20%25rax%0A%20%20call%20printf%0A%20%20movq%20%240%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A).

</details>

</div>

## More Practice

<div class="board-example" markdown="1">
<p class="board-title">Problem 5 — a call in a loop</p>

<div class="board-setup" markdown="1">

```c
int main(void) {
    long total = 0;
    for (long i = 1; i <= 3; i++) {
        total = total + sq(i);
    }
    return total;
}
```

1. Using `sq` from Problem 3, write `main`.
2. Where did you keep `total` and `i`? Why not in registers?
3. How many times does `call sq` run? Does it push the same return address each time?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```text
main:
  push %rbp
  mov %rsp, %rbp
  sub $16, %rsp           # room for total and i
  movq $0, -8(%rbp)       # total = 0
  movq $1, -16(%rbp)      # i = 1
  jmp main_test           # go to the test
main_loop:
  mov -16(%rbp), %rdi     # argument: i
  call sq                 # %rax = sq(i)
  add %rax, -8(%rbp)      # total = total + sq(i)
  addq $1, -16(%rbp)      # i++
main_test:
  cmpq $3, -16(%rbp)      # compare i with 3
  jle main_loop           # if i <= 3, go back
  mov -8(%rbp), %rax      # return total;
  leave
  ret
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sq%0A%20%20.type%09sq%2C%20%40function%0A%0Asq%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20the%20caller's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sq's%20frame%0A%20%20mov%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20n%0A%20%20imul%20%25rdi%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20result%20%3D%20n%20*%20n%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20the%20caller's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20result%20in%20%25rax%0A%20%20.size%09sq%2C%20.-sq%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2416%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20total%20and%20i%0A%20%20movq%20%240%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20total%20%3D%200%0A%20%20movq%20%241%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20i%20%3D%201%0A%20%20jmp%20main_test%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Amain_loop%3A%0A%20%20mov%20-16(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20argument%3A%20i%0A%20%20call%20sq%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sq(i)%0A%20%20add%20%25rax%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20total%20%3D%20total%20%2B%20sq(i)%0A%20%20addq%20%241%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20i%2B%2B%0Amain_test%3A%0A%20%20cmpq%20%243%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20compare%20i%20with%203%0A%20%20jle%20main_loop%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%3D%203%2C%20go%20back%0A%20%20mov%20-8(%25rbp)%2C%20%25rax%20%20%20%20%20%20%23%20return%20total%3B%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A%0A). `%rax` ends at 14.

1. See above.
2. On the stack, in `main`'s frame. `main` needs both after every call, and a call can change any of the caller-saved registers.
3. Three times, and it pushes the same return address each time: it's the same `call` instruction.

</details>

</div>

For more practice, try the [Exercises from the book for §7.5](https://diveintosystems.org/exercises/section-7_5.html).

Next time we look at recursion: a function that calls itself, with a new frame for every call.
