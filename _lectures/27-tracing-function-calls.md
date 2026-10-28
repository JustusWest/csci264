---
title: "27 Tracing Function Calls"
published: true
---

[*No new reading. Review Dive into Systems §7.5*](https://diveintosystems.org/book/C7-x86_64/functions.html)

To check your answers, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).

Last time we traced the `assign`/`adder` program from start to finish. Now it's your turn: each problem below is a short program compiled by `gcc`. Your job is to trace what happens to the registers and the stack.

The listings are the compiler's code without optimization, cleaned up a little: they use the 64-bit registers and function names instead of call addresses. As before, code addresses drop the leading `0x401`, and the stack addresses are made up.

Work in pairs, on paper.

## Tracing a Program

1. **Write down the starting values** of `%rip`, `%rsp`, and `%rbp`.
2. **Follow `%rip`, not the listing.** After a `call`, the next row is the first line of the function being called.
3. **Fill in only what changes.**
4. **Draw the stack next to the table.** Never erase a value. Cross it out only when something overwrites it.
5. **Check at every `ret`.** `%rsp` and `%rbp` should be back to what they were before the `call`.

| Instruction | What changes |
|---|---|
| `push %rbp` | `%rsp` goes down 8, then `%rbp` is copied to the new top |
| `pop %rbp` | the top is copied to `%rbp`, then `%rsp` goes up 8 |
| `sub $n,%rsp` | `%rsp` goes down `n`. Nothing is written |
| `mov` to memory | only that memory. `%rsp` doesn't move |
| `call F` | `%rsp` goes down 8, the return address is copied there, then `%rip` = F |
| `ret` | the top is copied to `%rip`, then `%rsp` goes up 8 |
| `leave` | `%rsp` = `%rbp`, then `pop %rbp` |

Recall that the first argument goes in `%rdi`, the second in `%rsi`, the third in `%rdx`, and the return value comes back in `%rax`.

## Problem 1

<div class="board-example" markdown="1">
<p class="board-title">Problem 1 — calling a function twice</p>

```c
long add10(long n) {
    return n + 10;
}

int main(void) {
    long a = add10(5);
    long b = add10(a);
    printf("%ld %ld\n", a, b);
    return 0;
}
```

```text
add10:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  mov    %rdi,-0x8(%rbp)
  40112e:  mov    -0x8(%rbp),%rax
  401132:  add    $0xa,%rax
  401136:  pop    %rbp
  401137:  ret

main:
  401138:  push   %rbp
  401139:  mov    %rsp,%rbp
  40113c:  sub    $0x10,%rsp
  401140:  mov    $0x5,%rdi
  401145:  call   add10
  40114a:  mov    %rax,-0x10(%rbp)
  40114e:  mov    -0x10(%rbp),%rax
  401152:  mov    %rax,%rdi
  401155:  call   add10
  40115a:  mov    %rax,-0x8(%rbp)
  40115e:  mov    -0x8(%rbp),%rdx
  401162:  mov    -0x10(%rbp),%rax
  401166:  mov    %rax,%rsi
  401169:  mov    $0x402004,%rdi
  40116e:  mov    $0x0,%rax
  401173:  call   printf
  401178:  mov    $0x0,%rax
  40117d:  leave
  40117e:  ret
```

`main` has already set up its frame. Fill in the table, then answer the questions.

<div class="board-setup" markdown="1">

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x140 | 0xd30 | 0xd40 | |
| 0x140 | `mov $0x5,%rdi` | | | | |
| 0x145 | `call add10` | | | | |
| 0x126 | `push %rbp` | | | | |
| 0x127 | `mov %rsp,%rbp` | | | | |
| 0x12a | `mov %rdi,-0x8(%rbp)` | | | | |
| 0x12e | `mov -0x8(%rbp),%rax` | | | | |
| 0x132 | `add $0xa,%rax` | | | | |
| 0x136 | `pop %rbp` | | | | |
| 0x137 | `ret` | | | | |
| 0x14a | `mov %rax,-0x10(%rbp)` | | | | |
| 0x14e | `mov -0x10(%rbp),%rax` | | | | |
| 0x152 | `mov %rax,%rdi` | | | | |
| 0x155 | `call add10` | | | | |
| 0x126 | `push %rbp` | | | | |
| 0x127 | `mov %rsp,%rbp` | | | | |
| 0x12a | `mov %rdi,-0x8(%rbp)` | | | | |
| 0x12e | `mov -0x8(%rbp),%rax` | | | | |
| 0x132 | `add $0xa,%rax` | | | | |
| 0x136 | `pop %rbp` | | | | |
| 0x137 | `ret` | | | | |
| 0x15a | `mov %rax,-0x8(%rbp)` | | | | |

1. What return address does each `call` push?
2. Where is `n` each time `add10` runs?
3. What does the program print?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| Address | Instruction | `%rip` | `%rsp` | `%rbp` | Other changes |
|---|---|---|---|---|---|
|  | (start) | 0x140 | 0xd30 | 0xd40 | |
| 0x140 | `mov $0x5,%rdi` | 0x145 | | | `%rdi` = 5 |
| 0x145 | `call add10` | **0x126** | 0xd28 | | M[0xd28] = 0x14a |
| 0x126 | `push %rbp` | 0x127 | 0xd20 | | M[0xd20] = 0xd40 |
| 0x127 | `mov %rsp,%rbp` | 0x12a | | 0xd20 | |
| 0x12a | `mov %rdi,-0x8(%rbp)` | 0x12e | | | M[0xd18] = 5 (`n`) |
| 0x12e | `mov -0x8(%rbp),%rax` | 0x132 | | | `%rax` = 5 |
| 0x132 | `add $0xa,%rax` | 0x136 | | | `%rax` = 0xf |
| 0x136 | `pop %rbp` | 0x137 | 0xd28 | 0xd40 | |
| 0x137 | `ret` | **0x14a** | 0xd30 | | |
| 0x14a | `mov %rax,-0x10(%rbp)` | 0x14e | | | M[0xd30] = 0xf (`a`) |
| 0x14e | `mov -0x10(%rbp),%rax` | 0x152 | | | `%rax` = 0xf |
| 0x152 | `mov %rax,%rdi` | 0x155 | | | `%rdi` = 0xf |
| 0x155 | `call add10` | **0x126** | 0xd28 | | M[0xd28] = 0x15a |
| 0x126 | `push %rbp` | 0x127 | 0xd20 | | M[0xd20] = 0xd40 |
| 0x127 | `mov %rsp,%rbp` | 0x12a | | 0xd20 | |
| 0x12a | `mov %rdi,-0x8(%rbp)` | 0x12e | | | M[0xd18] = 0xf (`n`) |
| 0x12e | `mov -0x8(%rbp),%rax` | 0x132 | | | `%rax` = 0xf |
| 0x132 | `add $0xa,%rax` | 0x136 | | | `%rax` = 0x19 |
| 0x136 | `pop %rbp` | 0x137 | 0xd28 | 0xd40 | |
| 0x137 | `ret` | **0x15a** | 0xd30 | | |
| 0x15a | `mov %rax,-0x8(%rbp)` | 0x15e | | | M[0xd38] = 0x19 (`b`) |

1. The first `call` pushes 0x14a and the second pushes 0x15a. Each is the address of the instruction right after that `call`.
2. At 0xd18 both times. It holds 5 the first time and 15 the second.
3. `15 25`.

- **Both calls get the same frame.** `%rbp` is 0xd20 both times, so the second call's `n` overwrites the first call's.
- **Only the return address is different.** It is how the same `ret` goes back to two different places.
- **`a` is at 0xd30, right where `%rsp` points.** Each `call` pushes at 0xd28, just below it, so `a` is safe. That's why `main` moved `%rsp` with `sub` before calling anything.
- **`printf` gets three arguments:** the format string in `%rdi`, `a` in `%rsi`, and `b` in `%rdx`. The compiler loaded `%rdx` first. The order doesn't matter, only which register each one ends up in.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFkZDEwKGxvbmcgbikge1xuICAgIHJldHVybiBuICsgMTA7XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIGEgPSBhZGQxMCg1KTtcbiAgICBsb25nIGIgPSBhZGQxMChhKTtcbiAgICBwcmludGYoXCIlbGQgJWxkXFxuXCIsIGEsIGIpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D) or [run it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.section%20.rodata%0A.LC0%3A%0A%20%20.string%20%22%25ld%20%25ld%5Cn%22%0A%0A%20%20.text%0A%20%20.globl%09add10%0A%20%20.type%09add10%2C%20%40function%0A%0Aadd10%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%25rdi%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20addq%20%2410%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09add10%2C%20.-add10%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20subq%20%2416%2C%20%25rsp%0A%20%20movq%20%245%2C%20%25rdi%0A%20%20call%20add10%0A%20%20movq%20%25rax%2C%20-16(%25rbp)%0A%20%20movq%20-16(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rdi%0A%20%20call%20add10%0A%20%20movq%20%25rax%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rdx%0A%20%20movq%20-16(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rsi%0A%20%20movq%20%24.LC0%2C%20%25rdi%0A%20%20movq%20%240%2C%20%25rax%0A%20%20call%20printf%0A%20%20movq%20%240%2C%20%25rax%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). The stack addresses are different in the visualizer, but `%rsp` and `%rbp` move by the same amounts. `call printf` runs as one step, and the visualizer doesn't show what it prints, so check `a` and `b` on the stack instead.

</details>

</div>

## Problem 2

<div class="board-example" markdown="1">
<p class="board-title">Problem 2 — a call inside a call</p>

```c
long add(long x, long y) {
    return x + y;
}

long sum3(long a, long b, long c) {
    long t = add(a, b);
    return add(t, c);
}

int main(void) {
    long r = sum3(1, 2, 3);
    printf("%ld\n", r);
    return 0;
}
```

```text
add:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  mov    %rdi,-0x8(%rbp)
  40112e:  mov    %rsi,-0x10(%rbp)
  401132:  mov    -0x8(%rbp),%rdx
  401136:  mov    -0x10(%rbp),%rax
  40113a:  add    %rdx,%rax
  40113d:  pop    %rbp
  40113e:  ret

sum3:
  40113f:  push   %rbp
  401140:  mov    %rsp,%rbp
  401143:  sub    $0x28,%rsp
  401147:  mov    %rdi,-0x18(%rbp)
  40114b:  mov    %rsi,-0x20(%rbp)
  40114f:  mov    %rdx,-0x28(%rbp)
  401153:  mov    -0x20(%rbp),%rdx
  401157:  mov    -0x18(%rbp),%rax
  40115b:  mov    %rdx,%rsi
  40115e:  mov    %rax,%rdi
  401161:  call   add
  401166:  mov    %rax,-0x8(%rbp)
  40116a:  mov    -0x28(%rbp),%rdx
  40116e:  mov    -0x8(%rbp),%rax
  401172:  mov    %rdx,%rsi
  401175:  mov    %rax,%rdi
  401178:  call   add
  40117d:  leave
  40117e:  ret

main:
  40117f:  push   %rbp
  401180:  mov    %rsp,%rbp
  401183:  sub    $0x10,%rsp
  401187:  mov    $0x3,%rdx
  40118c:  mov    $0x2,%rsi
  401191:  mov    $0x1,%rdi
  401196:  call   sum3
  40119b:  mov    %rax,-0x8(%rbp)
  ...
```

Just before `call sum3`, `%rsp` is 0xd30 and `%rbp` is 0xd40.

<div class="board-setup" markdown="1">

1. Draw the stack the **first** time `add` runs, right after `mov %rsi,-0x10(%rbp)`. Label every value. What are `%rsp` and `%rbp`?
2. `sum3` has `sub $0x28,%rsp` and ends with `leave`. `add` has neither. Why?
3. Start at `add`'s `%rbp` and follow the saved `%rbp` values. Where do you end up?
4. What changes on the stack for the second call to `add`?
5. `sum3` ends with `return add(t, c);`. Which instruction puts `sum3`'s return value in `%rax`?

For question 1:

| Address | Value | |
|---|---|---|
| 0xcd8 | | |
| 0xce0 | | |
| 0xce8 | | |
| 0xcf0 | | |
| 0xcf8 | | |
| 0xd00 | | |
| 0xd08 | | |
| 0xd10 | | |
| 0xd18 | | |
| 0xd20 | | |
| 0xd28 | | |
| 0xd30 | | |
| 0xd38 | | |
| 0xd40 | | |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

1. `%rsp` and `%rbp` are both 0xce8. The stack is in the table below.
2. `add` calls nothing, so nothing will be pushed on top of its variables. It can leave `%rsp` where it is. `sum3` calls `add`, and `call` pushes at `%rsp`. If `%rsp` were still 0xd20, the return address would land on `t` at 0xd18. So `sum3` moves `%rsp` below its variables first, and `leave` puts it back.
3. 0xce8 holds 0xd20, `sum3`'s `%rbp`. 0xd20 holds 0xd40, `main`'s `%rbp`. 0xd40 holds the `%rbp` of whatever called `main`. The saved `%rbp` values link every frame on the stack, newest to oldest.
4. Everything is at the same addresses, because `sum3`'s `%rsp` hasn't moved. The return address at 0xcf0 is now 0x17d, `x` is 3 (`t`), and `y` is 3 (`c`). `t` = 3 is at 0xd18.
5. `add`'s `add %rdx,%rax`, the second time `add` runs. `sum3` doesn't touch `%rax` after the second call, and `leave` and `ret` don't change it. So `%rax` = 6 goes straight back to `main`, and the program prints `6`.

| Address | Value | |
|---|---|---|
| 0xcd8 | 2 (`y`) | |
| 0xce0 | 1 (`x`) | |
| 0xce8 | 0xd20 (`sum3`'s `%rbp`) | ← `%rsp`, `%rbp` |
| 0xcf0 | 0x166 (return address into `sum3`) | |
| 0xcf8 | 3 (`c`) | ← `sum3`'s `%rsp` |
| 0xd00 | 2 (`b`) | |
| 0xd08 | 1 (`a`) | |
| 0xd10 | (unused) | |
| 0xd18 | (`t`, not set yet) | |
| 0xd20 | 0xd40 (`main`'s `%rbp`) | ← `sum3`'s `%rbp` |
| 0xd28 | 0x19b (return address into `main`) | |
| 0xd30 | | ← `main`'s `%rsp` |
| 0xd38 | (`r`, not set yet) | |
| 0xd40 | (saved by `main`) | ← `main`'s `%rbp` |

- **Three frames, one on top of the next.** Each has the same layout: the return address at `8(%rbp)` and the caller's `%rbp` at `0(%rbp)`.
- **A function that calls another has `sub` and `leave`.** A function that calls nothing just has `pop %rbp`.
- **The compiler doesn't always pack a frame tightly.** Nothing uses 0xd10.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIGFkZChsb25nIHgsIGxvbmcgeSkge1xuICAgIHJldHVybiB4ICsgeTtcbn1cblxubG9uZyBzdW0zKGxvbmcgYSwgbG9uZyBiLCBsb25nIGMpIHtcbiAgICBsb25nIHQgPSBhZGQoYSwgYik7XG4gICAgcmV0dXJuIGFkZCh0LCBjKTtcbn1cblxuaW50IG1haW4odm9pZCkge1xuICAgIGxvbmcgciA9IHN1bTMoMSwgMiwgMyk7XG4gICAgcHJpbnRmKFwiJWxkXFxuXCIsIHIpO1xuICAgIHJldHVybiAwO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6IiIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D) or [run it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.section%20.rodata%0A.LC0%3A%0A%20%20.string%20%22%25ld%5Cn%22%0A%0A%20%20.text%0A%20%20.globl%09add%0A%20%20.type%09add%2C%20%40function%0A%0Aadd%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%25rdi%2C%20-8(%25rbp)%0A%20%20movq%20%25rsi%2C%20-16(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rdx%0A%20%20movq%20-16(%25rbp)%2C%20%25rax%0A%20%20addq%20%25rdx%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09add%2C%20.-add%0A%0A%20%20.globl%09sum3%0A%20%20.type%09sum3%2C%20%40function%0A%0Asum3%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20subq%20%2440%2C%20%25rsp%0A%20%20movq%20%25rdi%2C%20-24(%25rbp)%0A%20%20movq%20%25rsi%2C%20-32(%25rbp)%0A%20%20movq%20%25rdx%2C%20-40(%25rbp)%0A%20%20movq%20-32(%25rbp)%2C%20%25rdx%0A%20%20movq%20-24(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rdx%2C%20%25rsi%0A%20%20movq%20%25rax%2C%20%25rdi%0A%20%20call%20add%0A%20%20movq%20%25rax%2C%20-8(%25rbp)%0A%20%20movq%20-40(%25rbp)%2C%20%25rdx%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rdx%2C%20%25rsi%0A%20%20movq%20%25rax%2C%20%25rdi%0A%20%20call%20add%0A%20%20leave%0A%20%20ret%0A%20%20.size%09sum3%2C%20.-sum3%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20subq%20%2416%2C%20%25rsp%0A%20%20movq%20%243%2C%20%25rdx%0A%20%20movq%20%242%2C%20%25rsi%0A%20%20movq%20%241%2C%20%25rdi%0A%20%20call%20sum3%0A%20%20movq%20%25rax%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rsi%0A%20%20movq%20%24.LC0%2C%20%25rdi%0A%20%20movq%20%240%2C%20%25rax%0A%20%20call%20printf%0A%20%20movq%20%240%2C%20%25rax%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A).

</details>

</div>

## Problem 3

<div class="board-example" markdown="1">
<p class="board-title">Problem 3 — what does it print?</p>

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
<p class="board-title">Problem 4 — a call in a loop</p>

```c
long square(long n) {
    return n * n;
}
```

```text
square:
  401126:  push   %rbp
  401127:  mov    %rsp,%rbp
  40112a:  mov    %rdi,-0x8(%rbp)
  40112e:  mov    -0x8(%rbp),%rax
  401132:  imul   %rax,%rax
  401136:  pop    %rbp
  401137:  ret

main:
  401138:  push   %rbp
  401139:  mov    %rsp,%rbp
  40113c:  sub    $0x10,%rsp
  401140:  movq   $0x0,-0x10(%rbp)
  401148:  movq   $0x1,-0x8(%rbp)
  401150:  jmp    L2
L1:
  401152:  mov    -0x8(%rbp),%rax
  401156:  mov    %rax,%rdi
  401159:  call   square
  40115e:  add    %rax,-0x10(%rbp)
  401162:  addq   $0x1,-0x8(%rbp)
L2:
  401167:  cmpq   $0x3,-0x8(%rbp)
  40116c:  jle    L1
  40116e:  mov    -0x10(%rbp),%rax
  401172:  mov    %rax,%rsi
  401175:  mov    $0x402004,%rdi
  40117a:  mov    $0x0,%rax
  40117f:  call   printf
  401184:  mov    $0x0,%rax
  401189:  leave
  40118a:  ret
```

After `main` sets up its frame, `%rsp` is 0xd30 and `%rbp` is 0xd40.

<div class="board-setup" markdown="1">

1. Write the C for `main`. The format string is `"%ld\n"`.
2. How many times does `call square` run? What return address does it push each time?
3. What are `%rsp` and `%rbp` inside `square` each time? Where is `n`?
4. What is the value at `-0x10(%rbp)` after each trip through the loop? What does the program print?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

```c
int main(void) {
    long total = 0;
    for (long i = 1; i <= 3; i++) {
        total = total + square(i);
    }
    printf("%ld\n", total);
    return 0;
}
```

1. `total` is at `-0x10(%rbp)` and `i` at `-0x8(%rbp)`. The `jmp` down to the test and the `jle` back up are a `for` loop.
2. Three times, and it pushes 0x15e every time. It is the same `call` instruction each time.
3. `%rsp` and `%rbp` are 0xd20 every time, and `n` is at 0xd18. It holds 1, then 2, then 3.
4. 1, then 5, then 14. The program prints `14`.

- **Every call to `square` gets the same frame.** The loop runs inside `main`'s frame, so `%rsp` is the same at every `call`.
- **`add %rax,-0x10(%rbp)` uses the return value straight from `%rax`.** `total = total + square(i)` takes one instruction after the `call`.

[Open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6IiNpbmNsdWRlIDxzdGRpby5oPlxuXG5sb25nIHNxdWFyZShsb25nIG4pIHtcbiAgICByZXR1cm4gbiAqIG47XG59XG5cbmludCBtYWluKHZvaWQpIHtcbiAgICBsb25nIHRvdGFsID0gMDtcbiAgICBmb3IgKGxvbmcgaSA9IDE7IGkgPD0gMzsgaSsrKSB7XG4gICAgICAgIHRvdGFsID0gdG90YWwgKyBzcXVhcmUoaSk7XG4gICAgfVxuICAgIHByaW50ZihcIiVsZFxcblwiLCB0b3RhbCk7XG4gICAgcmV0dXJuIDA7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19) or [run it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.section%20.rodata%0A.LC0%3A%0A%20%20.string%20%22%25ld%5Cn%22%0A%0A%20%20.text%0A%20%20.globl%09square%0A%20%20.type%09square%2C%20%40function%0A%0Asquare%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20movq%20%25rdi%2C%20-8(%25rbp)%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20imulq%20%25rax%2C%20%25rax%0A%20%20popq%20%25rbp%0A%20%20ret%0A%20%20.size%09square%2C%20.-square%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20pushq%20%25rbp%0A%20%20movq%20%25rsp%2C%20%25rbp%0A%20%20subq%20%2416%2C%20%25rsp%0A%20%20movq%20%240%2C%20-16(%25rbp)%0A%20%20movq%20%241%2C%20-8(%25rbp)%0A%20%20jmp%20.L4%0A.L5%3A%0A%20%20movq%20-8(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rdi%0A%20%20call%20square%0A%20%20addq%20%25rax%2C%20-16(%25rbp)%0A%20%20addq%20%241%2C%20-8(%25rbp)%0A.L4%3A%0A%20%20cmpq%20%243%2C%20-8(%25rbp)%0A%20%20jle%20.L5%0A%20%20movq%20-16(%25rbp)%2C%20%25rax%0A%20%20movq%20%25rax%2C%20%25rsi%0A%20%20movq%20%24.LC0%2C%20%25rdi%0A%20%20movq%20%240%2C%20%25rax%0A%20%20call%20printf%0A%20%20movq%20%240%2C%20%25rax%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A).

</details>

</div>


For more practice, try the [Exercises from the book for §7.5](https://diveintosystems.org/exercises/section-7_5.html).

Next time we look at recursion: a function that calls itself, with a new frame for every call.
