---
title: "30 Structs in Assembly"
published: true
---

[*Reading: Dive into Systems §7.9*](https://diveintosystems.org/book/C7-x86_64/structs.html)

To follow along with the examples, [set up Compiler Explorer]({{ "/godbolt/" | relative_url }}). To run assembly, use the [ASM Visualizer]({{ "/asm-visualizer/" | relative_url }}).


Earlier in the course we saw that a struct is one block of memory, with its fields laid out in the order they were declared. Last time we saw that an array is also one block of memory, and that `arr[i]` is at `arr + i * sizeof(T)`.

A struct's fields can be different sizes, so there's no single scale to multiply by. Instead, the compiler works out where each field is ahead of time. 

The key idea for today:
 
> In assembly, there are no field names. A field is just a fixed offset from the start of the struct.
 

## Where Is s->field?

We'll use this struct today. As before, every field is a `long` so the assembly uses the 64-bit registers:

```c
struct rect {
    long x;
    long y;
    long width;
    long height;
};
```

Recall the general memory operand:

```text
D(Rb,Ri,S)   →   M[D + Rb + Ri*S]
```

Arrays needed the index and scale. A struct field only needs the displacement. If `%rdi` holds a `struct rect *r`, then:

```text
16(%rdi)   →   M[%rdi + 16]   →   r->width
```

This is the same `D(%reg)` form we've used for stack variables like `-8(%rbp)` all along.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — struct operations</p>

<div class="board-setup" markdown="1">

`%rdi` holds `struct rect *r`, and `x` goes in `%rax`. Write one instruction for each line:

| C | Type of `x` | Assembly |
|---|---|---|
| `x = r;` | `struct rect *` | ? |
| `x = r->x;` | `long` | ? |
| `x = r->height;` | `long` | ? |
| `x = &r->width;` | `long *` | ? |
| `r->y = x;` | — | ? |

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

| C | Type of `x` | Assembly |
|---|---|---|
| `x = r;` | `struct rect *` | `mov %rdi, %rax` |
| `x = r->x;` | `long` | `mov (%rdi), %rax` |
| `x = r->height;` | `long` | `mov 24(%rdi), %rax` |
| `x = &r->width;` | `long *` | `lea 16(%rdi), %rax` |
| `r->y = x;` | — | `mov %rax, 8(%rdi)` |

- **The offset is a constant.** No index register, no scale. The compiler knows every offset before the program runs.
- **Look at the type, like last time.** A `long` field is read with `mov`. The address of a field comes from `lea`.
- **`r->x` has no displacement.** It's at offset 0.

</details>

</div>

## A Struct on the Stack

The ASM Visualizer doesn't show the heap, so our struct lives on the stack, as a local in `main`:

```c
struct rect r = {1, 2, 3, 4};
```

`main` makes room with `sub`, then stores each field. The first field goes at the lowest address, the same as an array:

```text
  sub $32, %rsp           # room for r
  movq $1, -32(%rbp)      # r.x = 1
  movq $2, -24(%rbp)      # r.y = 2
  movq $3, -16(%rbp)      # r.width = 3
  movq $4, -8(%rbp)       # r.height = 4
```

In `main`, `r.width` is just `-16(%rbp)`. `main` knows where `r` is, so the struct's offset and the field's offset add into one constant: -32 + 16 = -16.

To pass `&r` to a function, use `lea`, the same way we passed an array:

```text
  lea -32(%rbp), %rdi     # first argument: &r
```

## area

```c
long area(struct rect *r) {
    return r->width * r->height;
}
```

`r` arrives in `%rdi`. Let's write it in a [blank Function Mode](https://asm.diveintosystems.org/function/x86_64), starting with `main` and an `area` that only returns `r->width`:

<details class="code-example" markdown="1">
<summary>Show code: step 1, r->width</summary>

```text
  .text
  .globl	area
  .type	area, @function

area:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start area's frame
  mov 16(%rdi), %rax      # for now, return r->width
  pop %rbp                # restore main's %rbp
  ret
  .size	area, .-area

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $32, %rsp           # room for r
  movq $1, -32(%rbp)      # r.x = 1
  movq $2, -24(%rbp)      # r.y = 2
  movq $3, -16(%rbp)      # r.width = 3
  movq $4, -8(%rbp)       # r.height = 4
  lea -32(%rbp), %rdi     # first argument: &r
  call area               # %rax = area(&r)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09area%0A%20%20.type%09area%2C%20%40function%0A%0Aarea%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20area's%20frame%0A%20%20mov%2016(%25rdi)%2C%20%25rax%20%20%20%20%20%20%23%20for%20now%2C%20return%20r-%3Ewidth%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09area%2C%20.-area%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2432%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20r%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20r.x%20%3D%201%0A%20%20movq%20%242%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20r.y%20%3D%202%0A%20%20movq%20%243%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20r.width%20%3D%203%0A%20%20movq%20%244%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20r.height%20%3D%204%0A%20%20lea%20-32(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26r%0A%20%20call%20area%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20area(%26r)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 3.

</details>

- **Check the Stack Content table.** `r.x` is at the lowest address, and `r.height` is right below `main`'s saved `%rbp`.
- **Watch `%rdi` when `area` starts.** It holds the address of `r.x`. `16(%rdi)` is two rows further down the table.
- **Try `mov 24(%rdi), %rax`.** That's `r->height`, so `%rax` ends at 4.

Now multiply. `imul` needs a register, so `height` goes into `%rdx`:

<details class="code-example" markdown="1">
<summary>Show code: step 2, width * height</summary>

```text
  .text
  .globl	area
  .type	area, @function

area:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start area's frame
  mov 16(%rdi), %rax      # %rax = r->width
  mov 24(%rdi), %rdx      # %rdx = r->height
  imul %rdx, %rax         # %rax = width * height
  pop %rbp                # restore main's %rbp
  ret                     # return the area in %rax
  .size	area, .-area

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $32, %rsp           # room for r
  movq $1, -32(%rbp)      # r.x = 1
  movq $2, -24(%rbp)      # r.y = 2
  movq $3, -16(%rbp)      # r.width = 3
  movq $4, -8(%rbp)       # r.height = 4
  lea -32(%rbp), %rdi     # first argument: &r
  call area               # %rax = area(&r)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09area%0A%20%20.type%09area%2C%20%40function%0A%0Aarea%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20area's%20frame%0A%20%20mov%2016(%25rdi)%2C%20%25rax%20%20%20%20%20%20%23%20%25rax%20%3D%20r-%3Ewidth%0A%20%20mov%2024(%25rdi)%2C%20%25rdx%20%20%20%20%20%20%23%20%25rdx%20%3D%20r-%3Eheight%0A%20%20imul%20%25rdx%2C%20%25rax%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20width%20*%20height%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20the%20area%20in%20%25rax%0A%20%20.size%09area%2C%20.-area%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2432%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20r%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20r.x%20%3D%201%0A%20%20movq%20%242%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20r.y%20%3D%202%0A%20%20movq%20%243%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20r.width%20%3D%203%0A%20%20movq%20%244%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20r.height%20%3D%204%0A%20%20lea%20-32(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26r%0A%20%20call%20area%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20area(%26r)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xc, which is 12.

</details>

- **`%rdi` never changes.** Every field is reached from the same base address.

## What gcc Makes

Let's write `area` in [Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6InN0cnVjdCByZWN0IHtcbiAgICBsb25nIHg7XG4gICAgbG9uZyB5O1xuICAgIGxvbmcgd2lkdGg7XG4gICAgbG9uZyBoZWlnaHQ7XG59O1xuXG5sb25nIGFyZWEoc3RydWN0IHJlY3QgKnIpIHtcbiAgICByZXR1cm4gci0+d2lkdGggKiByLT5oZWlnaHQ7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19) and see what `gcc` does with it, without optimization:

```text
area:
  push   %rbp
  mov    %rsp,%rbp
  mov    %rdi,-0x8(%rbp)       # r
  mov    -0x8(%rbp),%rax       # %rax = r
  mov    0x10(%rax),%rdx       # %rdx = r->width
  mov    -0x8(%rbp),%rax       # %rax = r
  mov    0x18(%rax),%rax       # %rax = r->height
  imul   %rdx,%rax             # %rax = width * height
  pop    %rbp
  ret
```

- **The offsets are in hex.** 0x10 is 16 and 0x18 is 24. Don't read 0x18 as 18.
- **Every `->` reloads `r`.** Without optimization, `gcc` goes back to the stack for each use, the same as last time.

Change the options to `-O1` ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6InN0cnVjdCByZWN0IHtcbiAgICBsb25nIHg7XG4gICAgbG9uZyB5O1xuICAgIGxvbmcgd2lkdGg7XG4gICAgbG9uZyBoZWlnaHQ7XG59O1xuXG5sb25nIGFyZWEoc3RydWN0IHJlY3QgKnIpIHtcbiAgICByZXR1cm4gci0+d2lkdGggKiByLT5oZWlnaHQ7XG59XG4iLCJjb21waWxlcnMiOlt7ImlkIjoiY2cxNjIiLCJvcHRpb25zIjoiLU8xIiwiZmlsdGVycyI6eyJiaW5hcnlPYmplY3QiOnRydWUsImludGVsIjpmYWxzZSwibGFiZWxzIjp0cnVlLCJsaWJyYXJ5Q29kZSI6dHJ1ZSwiZGlyZWN0aXZlcyI6dHJ1ZSwiY29tbWVudE9ubHkiOnRydWUsImRlbWFuZ2xlIjp0cnVlfX1dfV19)) and it's three instructions:

```text
area:
  mov    0x10(%rdi),%rax       # %rax = r->width
  imul   0x18(%rdi),%rax       # %rax = width * height
  ret
```

Try swapping the order of `width` and `height` in the struct definition. The C function doesn't change, but the offsets in the assembly do.

## Padding

So far every offset has been the total size of the fields before it. That isn't always true. Let's ask the compiler where the fields go in a struct with mixed sizes ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6InN0cnVjdCByZWMge1xuICAgIGNoYXIgYztcbiAgICBsb25nIHg7XG4gICAgY2hhciBkO1xufTtcblxubG9uZyBnZXRYKHN0cnVjdCByZWMgKnApIHtcbiAgICByZXR1cm4gcC0+eDtcbn1cblxuY2hhciAqYWRkckQoc3RydWN0IHJlYyAqcCkge1xuICAgIHJldHVybiAmcC0+ZDtcbn1cblxubG9uZyBzaXplKHZvaWQpIHtcbiAgICByZXR1cm4gc2l6ZW9mKHN0cnVjdCByZWMpO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

```c
struct rec {
    char c;
    long x;
    char d;
};

long getX(struct rec *p) {
    return p->x;
}

char *addrD(struct rec *p) {
    return &p->d;
}

long size(void) {
    return sizeof(struct rec);
}
```

With `-O1`:

```text
getX:
  mov    0x8(%rdi),%rax        # p->x is at offset 8, not 1
  ret
addrD:
  lea    0x10(%rdi),%rax       # &p->d is at offset 16, not 9
  ret
size:
  mov    $0x18,%rax            # sizeof(struct rec) is 24, not 10
  ret
```

x86-64 wants each value to start at an address that's a multiple of its size:

| Type | Size | Starts at a multiple of |
|---|---|---|
| `char` | 1 | 1 |
| `int`, `float` | 4 | 4 |
| `long`, pointers | 8 | 8 |

Values that line up this way can be read or written in one memory operation. To keep each field lined up, the compiler adds unused bytes between fields, called **padding**.

- **`x` can't start at 1.** It's a `long`, so it moves to 8, and bytes 1–7 are padding.
- **There's padding at the end too.** The size of a struct is rounded up to a multiple of its largest field, so every element in an array of structs lines up. `d` ends at 17, and the size rounds up to 24.

Now move `x` to the top of the struct ([open it in Compiler Explorer](https://godbolt.org/clientstate/eyJzZXNzaW9ucyI6W3siaWQiOjEsImxhbmd1YWdlIjoiYyIsInNvdXJjZSI6InN0cnVjdCByZWMge1xuICAgIGxvbmcgeDtcbiAgICBjaGFyIGM7XG4gICAgY2hhciBkO1xufTtcblxubG9uZyBnZXRYKHN0cnVjdCByZWMgKnApIHtcbiAgICByZXR1cm4gcC0+eDtcbn1cblxuY2hhciAqYWRkckQoc3RydWN0IHJlYyAqcCkge1xuICAgIHJldHVybiAmcC0+ZDtcbn1cblxubG9uZyBzaXplKHZvaWQpIHtcbiAgICByZXR1cm4gc2l6ZW9mKHN0cnVjdCByZWMpO1xufVxuIiwiY29tcGlsZXJzIjpbeyJpZCI6ImNnMTYyIiwib3B0aW9ucyI6Ii1PMSIsImZpbHRlcnMiOnsiYmluYXJ5T2JqZWN0Ijp0cnVlLCJpbnRlbCI6ZmFsc2UsImxhYmVscyI6dHJ1ZSwibGlicmFyeUNvZGUiOnRydWUsImRpcmVjdGl2ZXMiOnRydWUsImNvbW1lbnRPbmx5Ijp0cnVlLCJkZW1hbmdsZSI6dHJ1ZX19XX1dfQ%3D%3D)):

```text
getX:
  mov    (%rdi),%rax           # p->x is at offset 0
  ret
addrD:
  lea    0x9(%rdi),%rax        # &p->d is at offset 9
  ret
size:
  mov    $0x10,%rax            # sizeof(struct rec) is 16
  ret
```

- **Put the largest fields first.** Same data, 8 fewer bytes.
- **The compiler never reorders fields for you.** C keeps them in the order you wrote them.

## Arrays of Structs

In an array `struct rect rs[n]`, each element is 32 bytes, so `rs[i].height` is at:

```text
rs + i * 32 + 24
```

The `i * 32` finds the element, and the `+ 24` finds the field.

```c
long sumHeights(struct rect *rs, long n) {
    long total = 0;
    for (long i = 0; i < n; i++) {
        total += rs[i].height;
    }
    return total;
}
```

This time `main` has an array of two `struct rect`s, so it needs 64 bytes. We'll start with a `sumHeights` that only returns `rs[1].height`:

<details class="code-example" markdown="1">
<summary>Show code: step 1, rs[1].height</summary>

```text
  .text
  .globl	sumHeights
  .type	sumHeights, @function

sumHeights:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start sumHeights's frame
  mov 56(%rdi), %rax      # for now, return rs[1].height
  pop %rbp                # restore main's %rbp
  ret
  .size	sumHeights, .-sumHeights

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $64, %rsp           # room for rs
  movq $1, -64(%rbp)      # rs[0].x = 1
  movq $2, -56(%rbp)      # rs[0].y = 2
  movq $3, -48(%rbp)      # rs[0].width = 3
  movq $4, -40(%rbp)      # rs[0].height = 4
  movq $5, -32(%rbp)      # rs[1].x = 5
  movq $6, -24(%rbp)      # rs[1].y = 6
  movq $7, -16(%rbp)      # rs[1].width = 7
  movq $10, -8(%rbp)      # rs[1].height = 10
  lea -64(%rbp), %rdi     # first argument: rs
  mov $2, %rsi            # second argument: n
  call sumHeights         # %rax = sumHeights(rs, 2)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumHeights%0A%20%20.type%09sumHeights%2C%20%40function%0A%0AsumHeights%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sumHeights's%20frame%0A%20%20mov%2056(%25rdi)%2C%20%25rax%20%20%20%20%20%20%23%20for%20now%2C%20return%20rs%5B1%5D.height%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09sumHeights%2C%20.-sumHeights%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2464%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20rs%0A%20%20movq%20%241%2C%20-64(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.x%20%3D%201%0A%20%20movq%20%242%2C%20-56(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.y%20%3D%202%0A%20%20movq%20%243%2C%20-48(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.width%20%3D%203%0A%20%20movq%20%244%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.height%20%3D%204%0A%20%20movq%20%245%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.x%20%3D%205%0A%20%20movq%20%246%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.y%20%3D%206%0A%20%20movq%20%247%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.width%20%3D%207%0A%20%20movq%20%2410%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.height%20%3D%2010%0A%20%20lea%20-64(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20rs%0A%20%20mov%20%242%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20n%0A%20%20call%20sumHeights%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumHeights(rs%2C%202)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xa, which is 10.

</details>

- **A constant index needs no index register.** The compiler adds it all up ahead of time: 1 × 32 + 24 = 56.
- **Check the Stack Content table.** `rs[1]` starts 32 bytes after `rs[0]`.

Now the loop. `total` goes in `%rax` and `i` in `%rcx`. 32 isn't a legal scale (only 1, 2, 4, or 8), so we multiply `i` by 32 ourselves:

<details class="code-example" markdown="1">
<summary>Show code: step 2, the loop</summary>

```text
  .text
  .globl	sumHeights
  .type	sumHeights, @function

sumHeights:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start sumHeights's frame
  mov $0, %rax            # total = 0
  mov $0, %rcx            # i = 0
  jmp sum_test            # go to the test
sum_loop:
  mov %rcx, %rdx          # %rdx = i
  imul $32, %rdx          # %rdx = i * 32
  add 24(%rdi,%rdx), %rax # total += rs[i].height
  add $1, %rcx            # i++
sum_test:
  cmp %rsi, %rcx          # compare i with n
  jl sum_loop             # if i < n, go back
  pop %rbp                # restore main's %rbp
  ret                     # return total in %rax
  .size	sumHeights, .-sumHeights

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $64, %rsp           # room for rs
  movq $1, -64(%rbp)      # rs[0].x = 1
  movq $2, -56(%rbp)      # rs[0].y = 2
  movq $3, -48(%rbp)      # rs[0].width = 3
  movq $4, -40(%rbp)      # rs[0].height = 4
  movq $5, -32(%rbp)      # rs[1].x = 5
  movq $6, -24(%rbp)      # rs[1].y = 6
  movq $7, -16(%rbp)      # rs[1].width = 7
  movq $10, -8(%rbp)      # rs[1].height = 10
  lea -64(%rbp), %rdi     # first argument: rs
  mov $2, %rsi            # second argument: n
  call sumHeights         # %rax = sumHeights(rs, 2)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09sumHeights%0A%20%20.type%09sumHeights%2C%20%40function%0A%0AsumHeights%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20sumHeights's%20frame%0A%20%20mov%20%240%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%20%20%23%20total%20%3D%200%0A%20%20mov%20%240%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%20%3D%200%0A%20%20jmp%20sum_test%20%20%20%20%20%20%20%20%20%20%20%20%23%20go%20to%20the%20test%0Asum_loop%3A%0A%20%20mov%20%25rcx%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%23%20%25rdx%20%3D%20i%0A%20%20imul%20%2432%2C%20%25rdx%20%20%20%20%20%20%20%20%20%20%23%20%25rdx%20%3D%20i%20*%2032%0A%20%20add%2024(%25rdi%2C%25rdx)%2C%20%25rax%20%23%20total%20%2B%3D%20rs%5Bi%5D.height%0A%20%20add%20%241%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%20%20%23%20i%2B%2B%0Asum_test%3A%0A%20%20cmp%20%25rsi%2C%20%25rcx%20%20%20%20%20%20%20%20%20%20%23%20compare%20i%20with%20n%0A%20%20jl%20sum_loop%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20if%20i%20%3C%20n%2C%20go%20back%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20total%20in%20%25rax%0A%20%20.size%09sumHeights%2C%20.-sumHeights%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2464%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20rs%0A%20%20movq%20%241%2C%20-64(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.x%20%3D%201%0A%20%20movq%20%242%2C%20-56(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.y%20%3D%202%0A%20%20movq%20%243%2C%20-48(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.width%20%3D%203%0A%20%20movq%20%244%2C%20-40(%25rbp)%20%20%20%20%20%20%23%20rs%5B0%5D.height%20%3D%204%0A%20%20movq%20%245%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.x%20%3D%205%0A%20%20movq%20%246%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.y%20%3D%206%0A%20%20movq%20%247%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.width%20%3D%207%0A%20%20movq%20%2410%2C%20-8(%25rbp)%20%20%20%20%20%20%23%20rs%5B1%5D.height%20%3D%2010%0A%20%20lea%20-64(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20rs%0A%20%20mov%20%242%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20n%0A%20%20call%20sumHeights%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20sumHeights(rs%2C%202)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xe, which is 14.

</details>

- **`24(%rdi,%rdx)` uses every part of `D(Rb,Ri,S)`.** `Rb` is the array, `Ri` gets us to element `i`, and `D` gets us to the field. The scale is 1, so we leave it out.
- **With `%rcx` = 1, `%rdx` is 32.** The operand is `rs + 32 + 24`, which is `rs[1].height`.

## Try It

Use the same `main` as `area`.

```c
void grow(struct rect *r, long k) {
    r->width = r->width + k;
    r->height = r->height + k;
}

long perimeter(struct rect *r) {
    return 2 * (r->width + r->height);
}
```

1. Write `grow`. `k` arrives in `%rsi`. Call it with `k` = 10 and check the Stack Content table when it's done.
2. Write `perimeter`.
3. Fill in the offset of each field and the `sizeof` of this struct. Then reorder it to make it as small as you can. Check your answers in Compiler Explorer.

```c
struct item {
    char tag;
    long a;
    int b;
    long c;
};
```

4. A function gets a `struct item *` in `%rdi` and runs `mov 0x18(%rdi), %rax`. Which field is it reading?

<details class="code-example" markdown="1">
<summary>Show code: grow</summary>

```text
  .text
  .globl	grow
  .type	grow, @function

grow:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start grow's frame
  add %rsi, 16(%rdi)      # r->width += k
  add %rsi, 24(%rdi)      # r->height += k
  pop %rbp                # restore main's %rbp
  ret
  .size	grow, .-grow

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $32, %rsp           # room for r
  movq $1, -32(%rbp)      # r.x = 1
  movq $2, -24(%rbp)      # r.y = 2
  movq $3, -16(%rbp)      # r.width = 3
  movq $4, -8(%rbp)       # r.height = 4
  lea -32(%rbp), %rdi     # first argument: &r
  mov $10, %rsi           # second argument: k
  call grow               # grow(&r, 10)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09grow%0A%20%20.type%09grow%2C%20%40function%0A%0Agrow%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20grow's%20frame%0A%20%20add%20%25rsi%2C%2016(%25rdi)%20%20%20%20%20%20%23%20r-%3Ewidth%20%2B%3D%20k%0A%20%20add%20%25rsi%2C%2024(%25rdi)%20%20%20%20%20%20%23%20r-%3Eheight%20%2B%3D%20k%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%0A%20%20.size%09grow%2C%20.-grow%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2432%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20r%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20r.x%20%3D%201%0A%20%20movq%20%242%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20r.y%20%3D%202%0A%20%20movq%20%243%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20r.width%20%3D%203%0A%20%20movq%20%244%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20r.height%20%3D%204%0A%20%20lea%20-32(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26r%0A%20%20mov%20%2410%2C%20%25rsi%20%20%20%20%20%20%20%20%20%20%20%23%20second%20argument%3A%20k%0A%20%20call%20grow%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20grow(%26r%2C%2010)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). The Stack Content table ends with 1, 2, 0xd, 0xe.

- **`add` can write straight to memory.** `add %rsi, 16(%rdi)` reads `r->width`, adds `k`, and writes it back.
- **`x` and `y` don't change.** Only offsets 16 and 24 are touched.
- **`main` is the same as before,** plus `mov $10, %rsi` for `k`, and `call grow`.

</details>

<details class="code-example" markdown="1">
<summary>Show code: perimeter</summary>

```text
  .text
  .globl	perimeter
  .type	perimeter, @function

perimeter:
  push %rbp               # save main's %rbp
  mov %rsp, %rbp          # start perimeter's frame
  mov 16(%rdi), %rax      # %rax = r->width
  add 24(%rdi), %rax      # %rax = width + height
  add %rax, %rax          # %rax = 2 * (width + height)
  pop %rbp                # restore main's %rbp
  ret                     # return the perimeter in %rax
  .size	perimeter, .-perimeter

  .globl	main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  sub $32, %rsp           # room for r
  movq $1, -32(%rbp)      # r.x = 1
  movq $2, -24(%rbp)      # r.y = 2
  movq $3, -16(%rbp)      # r.width = 3
  movq $4, -8(%rbp)       # r.height = 4
  lea -32(%rbp), %rdi     # first argument: &r
  call perimeter          # %rax = perimeter(&r)
  leave
  ret
  .size	main, .-main
```

[Open it in the ASM Visualizer](https://asm.diveintosystems.org/function/x86_64/%20%20.text%0A%20%20.globl%09perimeter%0A%20%20.type%09perimeter%2C%20%40function%0A%0Aperimeter%3A%0A%20%20push%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20save%20main's%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%20%20%20%20%20%20%20%20%20%20%23%20start%20perimeter's%20frame%0A%20%20mov%2016(%25rdi)%2C%20%25rax%20%20%20%20%20%20%23%20%25rax%20%3D%20r-%3Ewidth%0A%20%20add%2024(%25rdi)%2C%20%25rax%20%20%20%20%20%20%23%20%25rax%20%3D%20width%20%2B%20height%0A%20%20add%20%25rax%2C%20%25rax%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%202%20*%20(width%20%2B%20height)%0A%20%20pop%20%25rbp%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20restore%20main's%20%25rbp%0A%20%20ret%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%23%20return%20the%20perimeter%20in%20%25rax%0A%20%20.size%09perimeter%2C%20.-perimeter%0A%0A%20%20.globl%09main%0A%20%20.type%09main%2C%20%40function%0A%0Amain%3A%0A%20%20push%20%25rbp%0A%20%20mov%20%25rsp%2C%20%25rbp%0A%20%20sub%20%2432%2C%20%25rsp%20%20%20%20%20%20%20%20%20%20%20%23%20room%20for%20r%0A%20%20movq%20%241%2C%20-32(%25rbp)%20%20%20%20%20%20%23%20r.x%20%3D%201%0A%20%20movq%20%242%2C%20-24(%25rbp)%20%20%20%20%20%20%23%20r.y%20%3D%202%0A%20%20movq%20%243%2C%20-16(%25rbp)%20%20%20%20%20%20%23%20r.width%20%3D%203%0A%20%20movq%20%244%2C%20-8(%25rbp)%20%20%20%20%20%20%20%23%20r.height%20%3D%204%0A%20%20lea%20-32(%25rbp)%2C%20%25rdi%20%20%20%20%20%23%20first%20argument%3A%20%26r%0A%20%20call%20perimeter%20%20%20%20%20%20%20%20%20%20%23%20%25rax%20%3D%20perimeter(%26r)%0A%20%20leave%0A%20%20ret%0A%20%20.size%09main%2C%20.-main%0A). `%rax` ends at 0xe, which is 14.

- **`add %rax, %rax` doubles it.** `shl $1, %rax` works too.

</details>

<details class="code-example" markdown="1">
<summary>Answer: struct item</summary>

| `tag` | `a` | `b` | `c` | `sizeof` |
|---|---|---|---|---|
| 0 | 8 | 16 | 24 | 32 |

Bytes 1–7 and 20–23 are padding. Largest first:

```c
struct item {
    long a;     // offset 0
    long c;     // offset 8
    int b;      // offset 16
    char tag;   // offset 20
};              // sizeof = 24
```

`0x18` is 24, so `mov 0x18(%rdi), %rax` reads `c`. Working backward from an offset to a field like this is most of what reading struct code in assembly comes down to.

</details>

Next time we practice with arrays and structs.