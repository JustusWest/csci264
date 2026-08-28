# CSCI 264 — Lecture Plan, Weeks 1–7

**Text:** *Dive into Systems* v1.2 (Matthews, Newhall, Webb)
**Meetings:** Lecture MWF; Lab Tue (Sec A 12:30, Sec B 3:30)

---

## Week 1 · Aug 24–28

| Day | Sections | Content |
|---|---|---|
| **Mon 8/24** | — | Syllabus, course structure, demo-lab expectations, AI policy, introductions. No technical content |
| **Tue 8/25** | App. 2 §17.1–17.2, §17.4 | **Lab 0 — Using Unix and GNU C.** Instructor-led start to finish. No assignment |
| **Wed 8/26** | §1.1–1.2 | Compiling and running; types; variables; operators — **integer `/` and `%` emphasized**; `printf` and `scanf` |
| **Fri 8/28** | §4.1, §4.2 | Positional notation in any base; unsigned binary; hexadecimal; converting between bases. Generalize the book's base-2 repeated division to arbitrary base |

## Week 2 · Aug 31–Sep 4

| Day | Sections | Content |
|---|---|---|
| **Mon 8/31** | §1.3, §1.4, §1.5 | Conditionals and loops; functions (flag §1.4.1 *The Stack* as a preview of §7.5); arrays; `char` and ASCII arithmetic; strings and the string library (§1.5.4) |
| **Tue 9/1** | — | **Lab 1 — Base conversion, integers. A1 out** |
| **Wed 9/2** | §4.3 | Signed magnitude; two's complement |
| **Fri 9/4** | §4.4, §4.8.1 | Binary integer arithmetic; fixed-point and **fractional positional notation** — supplies A2's algorithm, which is not in the book |

## Week 3 · Sep 7–11 *(no Mon — Labor Day)*

| Day | Sections | Content |
|---|---|---|
| **Tue 9/8** | — | **Lab 2 — Base conversion, fractions. A1 due; A2 out** |
| **Wed 9/9** | §4.5, §4.6 | Integer overflow and its consequences; bitwise operators; bit shifting |
| **Fri 9/11** | §4.8.2–4.8.3 | IEEE 754 floating point; rounding consequences (Patriot missile, Ariane 5) |

**§4.7 (Integer Byte Order) is cut.** Endianness gets one slide inside Wed's lecture and reappears where it's concrete: reading multi-byte values in `objdump` output.

## Week 4 · Sep 14–18 — all of Ch 2

| Day | Sections | Content |
|---|---|---|
| **Mon 9/14** | §2.1, §2.2 | Parts of program memory and scope; C's pointer variables; `&` and `*` |
| **Tue 9/15** | — | **Lab 3 — Pointers and dynamic memory. A2 due; A3 out** |
| **Wed 9/16** | §2.3, §2.4 | Pointers and functions; pass-by-reference; heap memory; `malloc` and `free` |
| **Fri 9/18** | §1.6, §2.5.2, §2.7.2, §2.9.4 | **C data in memory:** structs; two-dimensional arrays; pointers to structs; pointer arithmetic. Framed throughout as *what §7.7–7.9 will show you the machine doing* |

Friday is deliberately a memory-layout lecture rather than a C-features lecture. It exists to make Week 14 possible, not to make students fluent C programmers.

## Week 5 · Sep 21–25 — architecture begins

| Day | Sections | Content |
|---|---|---|
| **Mon 9/21** | §5.1, §5.2 | Origins of modern computing; the von Neumann architecture |
| **Tue 9/22** | — | **Lab 4 — circuit simulator. A3 due; A4 out** |
| **Wed 9/23** | §5.3 | Logic gates |
| **Fri 9/25** | §5.4 | Arithmetic, control, and storage circuits |

## Week 6 · Sep 28–Oct 2 — finishing architecture

| Day | Sections | Content |
|---|---|---|
| **Mon 9/28** | §5.5 | Building a processor: putting it all together |
| **Tue 9/29** | — | **Lab 5 — datapath tracing. A4 due** |
| **Wed 9/30** | §5.6 | The processor's execution of program instructions |
| **Fri 10/2** | §5.7–5.9 | Pipelining; hazards; CPUs today |

## Week 7 · Oct 5–9 — the pivot

| Day | Sections | Content |
|---|---|---|
| **Mon 10/5** | — | Review for Midterm 1 |
| **Tue 10/6** | — | **Lab 6 — MIDTERM 1.** Ch 1, 2, 4, 5 |
| **Wed 10/7** | Ch 6, §2.9.7 | **Under the C.** Why read assembly; compiling C to assembly; `gcc -S` revisited from Lab 0 |
| **Fri 10/9** | §7.1 | Diving into assembly: registers, operand forms, addressing modes |

Administering the exam in lab costs no lecture and buys 75 minutes instead of 50.