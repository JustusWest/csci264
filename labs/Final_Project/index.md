---
title: "Final Project"
order: 99
due: "Tuesday, December 1, 11:59 PM"
handout_first: true
---


## Input Files

| Problem | Program | Function | Input file |
|---|---|---|---|
| 1. Bubble Sort | `bubble.c` | `bubble_sort` | [`bubble.txt`]({{ "/labs/Final_Project/bubble.txt" | relative_url }}) |
| 2. Longest Collatz Chain | `collatz.c` | `chain_length` | [`collatz.txt`]({{ "/labs/Final_Project/collatz.txt" | relative_url }}) |
| 3. Maximum Subarray Sum | `maxsub.c` | `max_subarray` | [`maxsub.txt`]({{ "/labs/Final_Project/maxsub.txt" | relative_url }}) |
| 4. Counting Inversions | `inversions.c` | `count_inversions` | [`inversions.txt`]({{ "/labs/Final_Project/inversions.txt" | relative_url }}) |
| 5. Pythagorean Triples | `pythag.c` | `count_triples` | [`pythag.txt`]({{ "/labs/Final_Project/pythag.txt" | relative_url }}) |



## Get the Input File

Download your input file from the table above, or in the terminal. Run only the line for your problem:

```bash
wget {{ "/labs/Final_Project/bubble.txt" | absolute_url }}
wget {{ "/labs/Final_Project/collatz.txt" | absolute_url }}
wget {{ "/labs/Final_Project/maxsub.txt" | absolute_url }}
wget {{ "/labs/Final_Project/inversions.txt" | absolute_url }}
wget {{ "/labs/Final_Project/pythag.txt" | absolute_url }}
```

## Terminal Commands

Read the handout first: it has the full instructions and the problems. This section has the commands so you can copy and paste them into the terminal. The commands use the bubble sort problem as an example. Replace `bubble` with your program name and `bubble_sort` with your function name.


## Step 1: Write and Test the C

Write your program in `bubble.c`, then build it, run it, and save the output:

```bash
gcc bubble.c -o bubble
./bubble < bubble.txt > before.txt
```

## Step 2: Compile to Assembly

```bash
gcc -O0 -S -fno-asynchronous-unwind-tables bubble.c
cp bubble.s bubble_original.s
```

**Run the first command only once.** It overwrites `bubble.s`, so running it again after you start editing throws away your work.

Build from the assembly and check that nothing changed. `diff` prints nothing if the two files are the same:

```bash
gcc bubble.s -o bubble
./bubble < bubble.txt > after.txt
diff before.txt after.txt
```

## Step 3: Measure the Original

Count the instructions with Callgrind. The first number on each line of output is the count: for your function, then for the whole program.

```bash
valgrind --tool=callgrind --callgrind-out-file=before.out ./bubble < bubble.txt > /dev/null
callgrind_annotate --threshold=100 before.out | grep bubble_sort
callgrind_annotate before.out | grep "PROGRAM TOTALS"
```

Count the memory reads and cache misses with Cachegrind:

```bash
valgrind --tool=cachegrind --cache-sim=yes --cachegrind-out-file=before.cg ./bubble < bubble.txt > /dev/null
cg_annotate --show=Ir,Dr,D1mr,DLmr before.cg | grep bubble_sort
```

The `grep` prints two lines with the same numbers. In order, they are:

- `Ir`: instructions executed
- `Dr`: data reads from memory
- `D1mr`: reads that missed in the L1 cache
- `DLmr`: reads that missed in the last-level cache

Record all of these for your presentation.

## Step 4: Optimize

Every time you change `bubble.s`, rebuild and check the output:

```bash
gcc bubble.s -o bubble
./bubble < bubble.txt > after.txt
diff before.txt after.txt
```

If `diff` prints anything, the change broke something: fix it or undo it before you go on. Then measure again:

```bash
valgrind --tool=callgrind --callgrind-out-file=after.out ./bubble < bubble.txt > /dev/null
callgrind_annotate --threshold=100 after.out | grep bubble_sort
```

```bash
valgrind --tool=cachegrind --cache-sim=yes --cachegrind-out-file=after.cg ./bubble < bubble.txt > /dev/null
cg_annotate --show=Ir,Dr,D1mr,DLmr after.cg | grep bubble_sort
```

To start over from `gcc`'s version:

```bash
cp bubble_original.s bubble.s
```

## Step 5 (Optional): Compare with gcc

It may print warnings about `scanf`. You can ignore those.

```bash
gcc -O1 -fno-inline bubble.c -o bubble_O1
valgrind --tool=callgrind --callgrind-out-file=O1.out ./bubble_O1 < bubble.txt > /dev/null
callgrind_annotate --threshold=100 O1.out | grep bubble_sort
```
