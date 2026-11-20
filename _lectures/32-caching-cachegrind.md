---
title: "32 Caching and Cachegrind"
published: true
---

[*Reading: Dive into Systems §11.1–11.3*](https://diveintosystems.org/book/C11-MemHierarchy/mem_hierarchy.html) and [*§11.5*](https://diveintosystems.org/book/C11-MemHierarchy/cachegrind.html). [*§11.4*](https://diveintosystems.org/book/C11-MemHierarchy/caching.html) has more detail.

## The Memory Hierarchy

| Device | Capacity | Approx. latency |
|---|---|---|
| Register | 8 bytes each | < 1 ns |
| CPU cache | KB to MB | 5 ns |
| Main memory (RAM) | 4–64 GB | 100 ns |
| Flash disk (SSD) | 0.5–2 TB | 0.1–1 ms |
| Hard disk | 0.5–10 TB | 5–10 ms |

- **Faster means smaller and more expensive.** No device is both fast and big, so a computer uses a little of each.
- **At 3 GHz, 100 ns is 300 cycles.** One trip to main memory costs as much as hundreds of `add`s.
- **The cache sits in between.** It's on the CPU chip and holds copies of memory the program used recently. The hardware manages it: no instruction names the cache.
- **Real CPUs have several levels of cache.** A small, fast L1 (tens of KB) next to each core, a bigger L2, and a big L3 shared by all the cores.

To see the caches on a lab machine, run `lscpu | grep cache`.

## Locality

The idea behind maintaining a cache is to copy chunks of main memory that are likely to be accessed soon. That likelihood is estimated two ways:

- **Temporal locality:** a value used now will probably be used again soon. A loop counter is used every trip.
- **Spatial locality:** after using one address, a program will probably use the ones next to it soon. Walking through an array in order does this.

The book compares this to a student with a desk, a bookshelf, and the library. The books she's using now stay on the desk. When she has to walk to the library, she brings back several books at once, not just the one she needs.

<div class="board-example" markdown="1">
<p class="board-title">Example 1 — finding locality</p>

```c
long sumArray(long *arr, long length) {
    long i, total = 0;
    for (i = 0; i < length; i++) {
        total += arr[i];
    }
    return total;
}
```

<div class="board-setup" markdown="1">

1. Which variables show temporal locality?
2. Which accesses show spatial locality?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

1. `i`, `total`, `length`, and `arr` (the base address) are all used every trip.
2. `arr[i]`. Each element is next to the one before it.

- **Each element is used only once,** so the elements have spatial locality but not temporal locality.
- **Keeping `i` and `total` in registers is temporal locality managed by hand.** That's what you did in the last lab.

</details>

</div>

## How a Cache Works

- **The cache moves whole blocks.** Memory is divided into **blocks** (also called lines), usually 64 bytes. A block is the smallest thing the cache copies.
- **A hit** means the block is already in the cache. **A miss** means it has to come from the next level down, and the whole block comes with it.
- **One miss on `arr[0]` brings in `arr[0]` through `arr[7]`.** Eight `long`s fit in 64 bytes, so the next seven reads hit. That's spatial locality paying off.
- **When the cache is full, something has to go.** Usually it's the **least recently used** (LRU) block. That's temporal locality: what you used recently stays.
- **Each block can only go in a few places.** In an **8-way set associative** cache, a block can go in any of 8 slots in its set. You'll see "8-way associative" in Cachegrind's output. How the hardware finds a block (tag, index, and offset bits) is in §11.4.
- **Writes go to the cached copy.** The block is written back to memory when it's evicted.

## Row Order vs. Column Order

Recall from earlier in the course that a 2D array is stored row by row. `mat[i][j]` and `mat[i][j+1]` are next to each other in memory. `mat[i][j]` and `mat[i+1][j]` are a row apart.

Consider this C code that accesses matrix values:

```c
#define N 1000

long mat[N][N];

long sum_v1(long n) {
    long total = 0;
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            total += mat[i][j];
        }
    }
    return total;
}

long sum_v2(long n) {
    long total = 0;
    for (long i = 0; i < n; i++) {
        for (long j = 0; j < n; j++) {
            total += mat[j][i];
        }
    }
    return total;
}
```

`main` fills `mat` with random numbers, then prints `sum_v1(N)` and `sum_v2(N)`.

<div class="board-example" markdown="1">
<p class="board-title">Example 2 — predict the misses</p>

<div class="board-setup" markdown="1">

The only difference is `[i][j]` vs. `[j][i]`. Both functions read every element once and run the same instructions.

1. How big is `mat`? Does it fit in a 48 KB L1 cache?
2. About how many misses does `sum_v1` have?
3. About how many does `sum_v2` have?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

1. 1,000,000 × 8 = 8,000,000 bytes. Nowhere close.
2. It walks the array in order: 8,000,000 / 64 = 125,000.
3. About 1,000,000: every read misses.

- **`sum_v2` walks down a column.** Each read is 8,000 bytes past the last one, in a different block.
- **One column touches 1,000 blocks.** That's 64,000 bytes, more than the L1 holds. By the time `sum_v2` comes back for `mat[0][1]`, the block holding it has been evicted.

</details>

</div>

## Cachegrind

Cachegrind is part of Valgrind, like Callgrind. It runs your program on a simulated cache and counts every hit and miss.

```bash
gcc cachex.c -o cachex
valgrind --tool=cachegrind --cache-sim=yes --cachegrind-out-file=cachex.cg ./cachex
```

- **`--cache-sim=yes` is required.** Without it, the version of Cachegrind on the lab machines only counts instructions, like Callgrind.
- **It's slow.** Simulating every memory access makes the program run slower than normal.

Cachegrind prints a summary of the whole program when it finishes:

```console
==1234== I refs:        105,279,376
==1234== I1  misses:          1,351
==1234== LLi misses:          1,324
==1234== I1  miss rate:        0.00%
==1234== LLi miss rate:        0.00%
==1234==
==1234== D refs:         49,086,109  (38,070,800 rd   + 11,015,309 wr)
==1234== D1  misses:      1,251,823  ( 1,126,411 rd   +    125,412 wr)
==1234== LLd misses:        126,535  (     1,173 rd   +    125,362 wr)
==1234== D1  miss rate:         2.6% (       3.0%     +        1.1%  )
==1234== LLd miss rate:         0.3% (       0.0%     +        1.1%  )
```

- **`I refs` is the instruction count,** the same number Callgrind gives.
- **`D refs` is data: memory reads (`rd`) and writes (`wr`).**
- **`D1` is the L1 data cache. `LL` is the last-level cache,** the one just above main memory. Cachegrind only simulates these two levels.
- **The `I1` and `LLi` lines are misses on the instructions themselves.** Our programs are small, so these stay near zero.

This summary covers the whole program, including `main`, `scanf`, and `rand`. To see one function, use `cg_annotate`, like `callgrind_annotate`:

```bash
cg_annotate --show=Ir,Dr,D1mr,DLmr cachex.cg | grep sum_v
```

```console
  12,009,013 (11.4%)         7,005,005 (18.4%)          125,001 (11.1%)          0                   sum_v1
  12,009,013 (11.4%)         7,005,005 (18.4%)        1,000,001 (88.8%)          0                   sum_v2
> 12,009,013 (11.4%, 80.7%) 7,005,005 (18.4%, 73.6%)   125,001 (11.1%, 11.1%)   0  (0.0%,  0.2%)  sum_v1:???
> 12,009,013 (11.4%, 92.1%) 7,005,005 (18.4%, 92.0%) 1,000,001 (88.8%, 99.9%)   0  (0.0%,  0.2%)  sum_v2:???
```


<div class="board-example" markdown="1">
<p class="board-title">Example 4 — what changed?</p>

<div class="board-setup" markdown="1">

```c
void min_max(long *arr, long size, long *min, long *max) {
    *min = 0;
    *max = 0;
    for (long i = 1; i < size; i++) {
        if (arr[i] < arr[*min]) {
            *min = i;
        }
        if (arr[i] > arr[*max]) {
            *max = i;
        }
    }
}
```

In the last lab, you rewrote the assembly for `min_max` to keep its variables in registers instead of on the stack. The input file `big.txt` has 100,000 `long`s.

1. Should `Data refs` go down?
2. Should `D1 misses` go down? About how many misses do you expect?

</div>

<details class="board-answer" markdown="1">
<summary>Answers</summary>

`gcc`'s version:

```console
   3,000,059  (1.9%)         1,700,035  (4.7%)        12,500 (47.2%)         0                 min_max
```

A hand-optimized version:

```console
     800,027  (0.5%)           299,999  (0.9%)        12,500 (47.2%)         0                 min_max
```

1. Yes: 1.7 million to 300,000. `gcc`'s version reads `i`, `arr`, `min`, and `max` from the stack every trip, about 17 reads per trip. The hand-optimized one reads only `arr[i]`, `arr[*min]`, and `arr[*max]`: 3 per trip.
2. No. 12,500 both times.

- **The stack reads we removed were all hits.** The stack frame is small and used every trip, so it never leaves the cache.
- **The array's misses come from its size and the order we read it in.** Neither changed.
- **A hit still costs a few cycles.** A register costs nothing extra, so cutting `Data refs` still made `min_max` a bit faster.

</details>

</div>
