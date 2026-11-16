---
title: "Lab 9 — Hand-Optimizing Assembly"
order: 9
due: "Tuesday, November 24"
source_note: "This is the program you will optimize. You don't need to change the C."
---

The handout below has the full instructions. This page collects its commands so you can copy and paste them into the terminal. Run all of them from your Lab 9 directory, on the lab machines.

## Get the Files

Download `stats.c` and `big.txt` from the bottom of this page into your Lab 9 directory, or download them in the terminal:

```bash
wget {{ "/labs/Lab09/stats.c" | absolute_url }}
wget {{ "/labs/Lab09/big.txt" | absolute_url }}
```

## Part 1: Build and Save the Output

```bash
gcc stats.c -o stats
echo "5 3 1 4 1 5" | ./stats
```

It should print `3 5 4 1 1`. Then save the output for `big.txt`:

```bash
./stats < big.txt > before.txt
```

## Part 2: The Unoptimized Assembly

Compile to assembly:

```bash
gcc -O0 -S -fno-asynchronous-unwind-tables stats.c
```

**Run that command only once.** It overwrites `stats.s`, so running it again after you start editing throws away your work. Save a copy of the original now:

```bash
cp stats.s stats_original.s
```

Build from `stats.s` and check that nothing changed. `diff` prints nothing if the two files are the same:

```bash
gcc stats.s -o stats
./stats < big.txt > after.txt
diff before.txt after.txt
```

## Part 3: Measure

Run the program under Valgrind, then print the counts:

```bash
valgrind --tool=callgrind --callgrind-out-file=before.out ./stats < big.txt > /dev/null
callgrind_annotate before.out | head -30
```

Valgrind prints a few lines of its own that start with something like `==12345==`. That's normal. The line to look for in the second command's output is `PROGRAM TOTALS`.

To pick out `min_max`:

```bash
callgrind_annotate before.out | grep min_max
```

The output is one line. The first number on it is the count for `min_max`.

## Part 4: Optimize min_max

Every time you change `stats.s`, run all five of these. If `diff` prints anything, your `min_max` gives a different answer: fix that before you trust the count.

```bash
gcc stats.s -o stats
./stats < big.txt > after.txt
diff before.txt after.txt
valgrind --tool=callgrind --callgrind-out-file=after.out ./stats < big.txt > /dev/null
callgrind_annotate after.out | grep min_max
```

To start over from `gcc`'s version:

```bash
cp stats_original.s stats.s
```

## Part 5, Question 4 (Optional)

Build a second program at `-O1` and measure its `min_max`. It may print warnings about `scanf`. You can ignore those.

```bash
gcc -O1 stats.c -o stats_O1
valgrind --tool=callgrind --callgrind-out-file=O1.out ./stats_O1 < big.txt > /dev/null
callgrind_annotate O1.out | grep min_max
```

## If Something Goes Wrong

- **`./stats` just sits there.** It's waiting for you to type the input. You forgot `< big.txt`. Press Ctrl-C and run it again.
- **`grep min_max` prints nothing.** Check that `min_max:` and the lines starting with `.type` and `.size` near it are still in `stats.s`. Valgrind uses them to find the function.
- **`valgrind: command not found`.** You're not on a lab machine.
- **`gcc stats.s` gives an error with a line number.** That line of `stats.s` has a typo. Each error message tells you the line.
