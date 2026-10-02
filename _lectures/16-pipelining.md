---
title: "16 Pipelining and Modern CPUs"
published: true
---
[*Reading: Dive into Systems §5.7*](https://diveintosystems.org/book/C5-Arch/pipelining.html), [*§5.8*](https://diveintosystems.org/book/C5-Arch/pipelining_advanced.html), [*§5.9*](https://diveintosystems.org/book/C5-Arch/modern.html), and optionally [*§15.1*](https://diveintosystems.org/book/C15-Parallel/gpu.html) on GPUs and CUDA

Last time we saw the CPU run one instruction at a time: **Fetch**, **Decode**, **Execute**, **WriteBack**, one clock cycle per stage.

## Pipelining

Our four-stage CPU takes four cycles per instruction. Running N instructions takes 4N cycles.

![Three instructions executed one after another, drawn as three rows of four boxes labeled F, D, E, W. The first instruction's boxes are blue, the second's red, and the third's green, placed end to end in a single row. A bracket over the first F box is labeled "1 cycle", and an arrow under each group of four boxes is labeled "4 cycles".]({{ "/figures/ch5/4instrcycles.png" | relative_url }})

The **CPI** (cycles per instruction) here is 4.

Notice that each stage's circuitry is idle three cycles out of four. After the fetch circuitry fetches the first instruction, it sits doing nothing while that instruction is decoded, executed, and written back.

**Pipelining** fixes this by starting the next instruction before the current one finishes. Each cycle, every instruction moves forward one stage and a new instruction is fetched.

![Pipelined execution of five instructions, each a different color. Each row is a clock cycle. In the 1st cycle, one instruction is in F. In the 2nd cycle, a second instruction is in F and the first is in D. In the 3rd cycle, three instructions occupy F, D, and E. In the 4th and 5th cycles, all four stages F, D, E, and W are busy with four different instructions. A red circle around the W boxes of the 4th and 5th cycles marks that one instruction now finishes every cycle.]({{ "/figures/ch5/pipeline.png" | relative_url }})

Once the pipeline is full (cycle 4), one instruction finishes every cycle: a CPI of 1.

Pipelining does **not** make any single instruction faster. Each instruction still takes 4 cycles from fetch to writeback: its **latency** is unchanged. What improves is **throughput**, the number of instructions finished per unit of time.

Real pipelines have more stages. A common design adds a **Memory (M)** stage for instructions that load or store values in memory: Fetch–Decode–Execute–Memory–WriteBack. The Intel Core i7 has a 14-stage pipeline.

## Hazards

Pipelining works best when every instruction is independent of the one before it. When an instruction has to wait, the pipeline **stalls**: empty slots called **bubbles** move through it instead of useful work.

### Data Hazards

A **data hazard** happens when an instruction needs a result that an earlier instruction has not written back yet.

```text
MOV 4, Reg2            # Reg2 = 4
ADD Reg2, Reg2, Reg2   # Reg2 = Reg2 + Reg2
```

The ADD needs Reg2 in its Execute stage, but the MOV does not write Reg2 until its WriteBack stage.

![Three pipeline diagrams of the two instructions MOV 4, Reg2 and ADD Reg2, Reg2, Reg2. Left, labeled "Problem: ADD doesn't have the proper value of Reg2!": the ADD's E stage, shown in red, happens before the MOV's W. Middle, labeled "Solution (suboptimal): more bubbles!": green bubble cells delay the ADD's E until after the MOV's W. Right, labeled "Operand forwarding: read and use result from previous operation": the ADD has an R stage, read from the previous operation, in place of a bubble, and its E follows immediately.]({{ "/figures/ch5/dataHazard2.png" | relative_url }})

Adding bubbles works, but wastes cycles. Instead, CPUs use **operand forwarding**: the MOV's result is passed straight to the ADD as soon as it is computed, without waiting for it to reach the register file.

### Control Hazards

A **control hazard** happens at a conditional jump. Whether the CPU jumps is not known until the jump instruction executes. But by then, the pipeline has already fetched the next few instructions.

![Pipeline diagrams for a code snippet that loads two values, compares them with CMP, and then uses JLE to jump to L1 (a SUB) or fall through to an ADD followed by JMP L2. Left, labeled "If branch is not taken...": the ADD and JMP instructions after the JLE flow through the pipeline normally. Right, labeled "If branch is taken, we have 'junk' in the pipeline that needs to be flushed!": the ADD and JMP rows are shown in red because they were fetched but must be thrown away.]({{ "/figures/ch5/controlHazardprb.png" | relative_url }})

If the jump is taken, the instructions already in the pipeline are the wrong ones. They must be thrown out or **flushed**

Options for handling control hazards:

- **Stall:** stop fetching at every branch until the outcome is known. Correct, but slow.
- **Branch prediction:** guess which way the branch will go, based on what it did before, and keep fetching from the guess. If the guess is wrong, flush. Modern predictors are right the vast majority of the time.
- **Eager execution:** compute both sides and keep the right result, using a *conditional move* instead of a jump. We will see this (`cmov`) in x86 assembly.

![Two solutions for the same branch. Left, labeled "Solution 1: Stall pipeline execution (slow!)": after the JLE, a long staircase of green bubble cells fills the pipeline until the next instruction is fetched. Right, labeled "Solution 2: Use a branch predictor. If the predictor does well, we only need to flush every now and then.": instructions after the JLE, shown in blue, are fetched immediately based on the prediction.]({{ "/figures/ch5/controlHazardsol.png" | relative_url }})

## Moore's Law

Every circuit is made of **transistors**. The number of transistors that fit on a chip is a rough measure of how much a chip can do.

**Moore's law** is the observation that the number of transistors on a chip doubles about every two years. Gordon Moore (co-founder of Intel) first observed a doubling every year in 1965, and revised it to every two years in 1975.

Doubling every two years for 50 years is a factor of 2<sup>25</sup>, about 33 million.

![A log-scale scatter plot titled "50 Years of Microprocessor Trend Data", covering 1970 to about 2021. Five series are plotted. Transistors (thousands), orange triangles, rise in a nearly straight line the whole time, from about 1 thousand to over 10 billion. Single-thread performance, blue circles, rises steeply until the mid-2000s and then flattens. Frequency in MHz, green squares, rises until about 2005 and then levels off around 3,000 to 4,000 MHz. Typical power in watts, red triangles, rises until about 2005 and then levels off around 100 watts. Number of logical cores, black diamonds, stays at 1 until about 2005 and then rises to dozens.]({{ "/figures/ch5/moores-law-50yrs.png" | relative_url }})

*Figure: Karl Rupp, [Microprocessor Trend Data](https://github.com/karlrupp/microprocessor-trend-data), CC BY 4.0. Data through 2010 collected by M. Horowitz, F. Labonte, O. Shacham, K. Olukotun, L. Hammond, and C. Batten.*

- **Transistors** (orange) keep climbing.
- Until the mid-2000s, **frequency** (green) climbed too. Architects used the extra transistors and the faster clock to make a single core execute one instruction stream faster.
- Around 2005, frequency and **power** (red) flatten. A faster clock means more heat, and chips hit the limit of what can be cooled. This is called the **power wall**.
- At the same time, the **number of cores** (black) starts to rise. The extra transistors now go into more cores instead of faster ones.

Transistor density improvements have slowed since the early 2010s, and Moore himself predicted the trend would end around the mid-2020s.

## Instruction-Level Parallelism and Multicore

Pipelining is one example of **instruction-level parallelism (ILP)**: the CPU runs several instructions of one program at the same time, without the programmer doing anything. A pipelined CPU finishes at most one instruction per cycle.

To do better than one, a CPU needs more than one pipeline. A **superscalar** processor has several execution pipelines, and finds instructions in the stream that do not depend on each other to run side by side, even out of order. It is limited by dependencies in the program: if every instruction needs the previous one's result, there is nothing to run in parallel.

With CPI below 1, it is easier to talk about the inverse: **IPC**, instructions per cycle.

Since the power wall, the main way to use more transistors is to put several complete CPUs, called **cores**, on one chip.

![A multicore computer. Inside a box labeled Processor Chip are cores labeled Core 0, Core 1, through Core N. Each core contains its own register file, ALU, and cache. The cores connect by a bus to a larger shared cache memory on the chip. Outside the chip, a memory bus connects the processor to main memory (RAM) and to an I/O controller, which connects through an I/O bus to input and output devices.]({{ "/figures/ch5/multicore.png" | relative_url }})

Each core is a full CPU with its own ALU, registers, and pipeline. The operating system schedules a different stream of instructions on each core. A multicore chip can run several programs at once, but **it only speeds up a single program if that program is written to run in parallel** (multithreaded). The hardware no longer does this for you.

Many cores also support **hardware multithreading** (Intel calls it hyper-threading): one core keeps two instruction streams loaded and switches between them, so one can use the core while the other waits.


## CPUs vs. GPUs

A **graphics processing unit (GPU)** is a processor designed for a different job than a CPU. Both are built from the same transistors; they spend them differently.

![Side-by-side comparison labeled CPU and GPU. The CPU side has four large green Core blocks, each with its own yellow Control block and purple L1 cache, then two L2 caches, a large L3 cache, and DRAM at the bottom. The GPU side is mostly a large grid of many small green cores; each row of cores shares a thin strip of yellow control and purple cache on the left. Below the grid are a single L2 cache and DRAM.]({{ "/figures/ch5/cpu-vs-gpu.png" | relative_url }})

*Figure: NVIDIA, [CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/introduction.html).*

A **CPU** has a few large, complex cores. Much of each core is control logic (branch prediction, out-of-order execution, forwarding) and cache. All of this is there to run *one instruction stream as fast as possible*, even code full of branches and dependencies.

A **GPU** has thousands of small, simple cores. Groups of cores share a single control unit, so they all execute **the same instruction at the same time, each on different data**. This is called **SIMT**, single instruction / multiple threads.

![A simplified GPU with 2,048 cores. On the left is GPU global memory. The main block contains 64 units labeled SM (streaming multiprocessor), arranged in four groups of 16, with an L2 cache in the middle and an interface to the host at the bottom. One SM is magnified on the right: it contains a grid of 32 cores labeled SP (scalar processor), plus a warp scheduler and execution control unit, a register file, an L1 cache, and shared memory.]({{ "/figures/ch5/gpugpu.png" | relative_url }})

GPUs were built for graphics: to draw a frame, the same computation runs on every pixel of the screen. For example, converting an image to grayscale on a CPU is a loop:

```c
for (int i = 0; i < num_pixels; i++) {
    gray[i] = 0.3 * red[i] + 0.59 * green[i] + 0.11 * blue[i];
}
```

On a GPU, each thread handles one pixel, and thousands of them run in lockstep.

### Graphics and Machine Learning

Most graphics work is **matrix multiplication**. A 3D model is made of thousands of corner points (vertices), each a vector of coordinates. To move, rotate, or scale the model, the GPU multiplies every vertex by the same small matrix.

A layer of a **neural network** does the same thing. The inputs (the pixels of an image, or the words of a sentence, encoded as numbers) are a vector, and the layer multiplies that vector by a matrix of learned **weights**. Training and running a model is mostly doing this, over and over, with very large matrices.

![Two side-by-side panels with the same structure. Left panel, titled "Graphics: moving a 3D model": a wireframe cube is moved by (tx, ty, tz) to a new position; each corner (vertex) is a vector (x, y, z, 1). Below, the equation shows the new vertex (x', y', z', 1) equals a 4-by-4 transform matrix, with ones on the diagonal and tx, ty, tz in the last column, times the old vertex (x, y, z, 1). Caption: repeat for every vertex of every model, millions of vertices per frame. Right panel, titled "Neural network: one layer": three input nodes x1, x2, x3, each connected by a weighted line to two output nodes y1 and y2. Below, the equation shows (y1, y2) equals a 2-by-3 weight matrix w11 through w23 times (x1, x2, x3). Caption: repeat for every input, in every layer; real layers have thousands of rows and columns. A banner across the bottom reads: Both are the same matrix applied to many vectors: a matrix multiplication. This is the job a GPU is built for.]({{ "/figures/ch5/matmul.png" | relative_url }})

Hardware built to draw video game frames turned out to be exactly the hardware machine learning needs. That is why GPUs are now used far beyond graphics.

### CUDA

**CUDA** is NVIDIA's platform for writing general-purpose programs that run on its GPUs, released in 2007. **CUDA programs are written in C** (and C++), with a few extensions, and compiled with NVIDIA's compiler, `nvcc`.

Here is the grayscale loop from above as a CUDA **kernel**, a function that runs on the GPU:

```c
__global__ void grayscale(float *red, float *green, float *blue, float *gray, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;   // which pixel is mine?
    if (i < n) {
        gray[i] = 0.3 * red[i] + 0.59 * green[i] + 0.11 * blue[i];
    }
}
```

Most people do machine learning in Python with libraries like PyTorch. But when PyTorch multiplies two matrices on a GPU, the code doing the work is CUDA, written in C and C++. Python is the steering wheel; C is the engine. If you want to work on the systems that make AI fast, you need C.
