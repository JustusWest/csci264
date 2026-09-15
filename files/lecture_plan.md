# CSCI 264 — Lecture Plan, Weeks 1–7 (draft 7)

**Text:** *Dive into Systems* v1.2 (Matthews, Newhall, Webb)
**Meetings:** Lecture MWF; Lab Tue (Sec A 12:30, Sec B 3:30)

**Changes from draft 6.** Three changes, all in Weeks 4–6. (1) Week 4 is corrected to what was actually delivered: Mon 9/14 was §2.5 (arrays), not the C-data-in-memory lecture draft 6 assigned it, so structs, pointers to structs, and pointer arithmetic move to Fri 9/18. (2) §2.9.5–2.9.6 and App. 2 §17.5 (libraries, linking, `make`) move out of Week 4 and into Wed 10/7, where the compilation pipeline is already the topic. (3) Lab 4 and Lab 5 swap content: Lab 4 becomes a strings-and-structs C lab carrying A4, and the circuit simulator moves to Lab 5, where three architecture lectures precede it instead of one.

**No lecture dates move.** Weeks 1–3 are unchanged, all lab dates are unchanged, and Midterm 1 stays Tue Oct 6. The assignment count stays at eight: A4 changes topic rather than disappearing.

---

## Week 1 · Aug 24–28

| Day | Sections | Content |
|---|---|---|
| **Mon 8/24** | — | Syllabus, course structure, demo-lab expectations, AI policy, introductions. No technical content |
| **Tue 8/25** | App. 2 §17.1–17.2, §17.4 | **Lab 0 — Using Unix and GNU C.** Instructor-led start to finish. No assignment |
| **Wed 8/26** | §1.1–1.2 | Compiling and running; types; variables; operators — **integer `/` and `%` emphasized**; `printf` and `scanf` |
| **Fri 8/28** | §1.3, §1.4, §1.5 | Conditionals and loops; functions (flag §1.4.1 *The Stack* as a preview of §7.5); arrays; `char` and ASCII arithmetic; strings and the string library (§1.5.4) |

**§1.6 (Structs) was not covered here.** It is picked up on Fri 9/18 alongside §2.7.

## Week 2 · Aug 31–Sep 4 — number systems, compressed

| Day | Sections | Content |
|---|---|---|
| **Mon 8/31** | §4.1, §4.2 | Positional notation in any base; unsigned binary; hexadecimal; converting between bases. Generalize the book's base-2 repeated division to arbitrary base |
| **Tue 9/1** | — | **Lab 1 — Base conversion, integers. A1 out** |
| **Wed 9/2** | §4.3, §4.4 *(abridged)*, §4.5 | Signed magnitude; two's complement. Negation by flip-and-add-one; binary addition; subtraction as addition of the negation. Unsigned and signed overflow; the carry-out/carry-in rule and the sign-bit rule; consequences (Pac-Man, Therac-25) |
| **Fri 9/4** | §4.8 | Fixed point and **fractional positional notation** — supplies A2's algorithm, which is not in the book. IEEE 754: sign bit, biased exponent, significand. Rounding consequences (Patriot missile, Ariane 5) |

**Wed 9/2 is the densest lecture of the semester.** Abridging §4.4 makes §4.5 harder, not easier — the overflow rules are stated in terms of the addition procedure. Keep negation, addition, and subtraction-as-addition intact; drop the multiplication and division detail. If the period runs short, §4.5.4 (the consequence stories) is the cheapest thing to push to a slide students read on their own.

**§4.6 (Bitwise Operators) is relocated, not cut.** Nothing graded exercises it, but three assembly idioms are unreadable without it. Split it:

- **AND / OR / XOR / NOT → Wed 9/23**, folded into §5.3 logic gates. Same truth tables; roughly ten minutes as the software face of the gates already on the board.
- **Shifting → §7.3**, where `sall` and `sarl` show up as the compiler's strength reduction for `*2` and `/2`.
- Also depends on this: `xorl %eax, %eax` (the zeroing idiom, §7.2 onward) and `testl %eax, %eax` (§7.4, the most common comparison in compiled code).

**§4.7 (Integer Byte Order) is cut.** Endianness gets one slide on Wed 9/2 and reappears where it's concrete: reading multi-byte values in `objdump` output.

**Ordering constraint:** §4.3.1 (signed magnitude) must precede §4.8.2 — IEEE 754 uses a signed-magnitude sign bit, not two's complement, and exercise 4.30 asks students this directly. Wed-then-Fri satisfies it. Two's complement is useful here only as the contrast that motivates the exponent bias.

## Week 3 · Sep 7–11 *(no Mon — Labor Day)* — Ch 2 begins

| Day | Sections | Content |
|---|---|---|
| **Tue 9/8** | — | **Lab 2 — Base conversion, fractions. A1 due; A2 out** |
| **Wed 9/9** | §2.1, §2.2 | Parts of program memory and scope; C's pointer variables; `&` and `*` |
| **Fri 9/11** | §2.3, §2.4 | Pointers and functions; pass-by-reference; heap memory; `malloc` and `free` |

Lab 2's prerequisite chain is §4.1 → §4.2 → §4.8.1, all of which now land by Fri 9/4. Under draft 5, §4.8.2 arrived Fri 9/11 — mid-A2. Students implementing repeated multiplication with `double` hit non-terminating expansions and unrepresentable values like 0.1 partway through the assignment and got the explanation a week later. Teaching §4.8.2 before the lab opens turns the most common A2 bug into a demonstration of Friday's lecture.

## Week 4 · Sep 14–18 — Ch 2 concludes

| Day | Sections | Content |
|---|---|---|
| **Mon 9/14** | §2.5 | Arrays in C: one-dimensional arrays, static and dynamic; arrays and functions; row-major order; static and dynamic 2D arrays (both allocation methods) |
| **Tue 9/15** | — | **Lab 3 — Pointers and dynamic memory. A2 due; A3 out** |
| **Wed 9/16** | §2.6 | Strings and the string library. Flag `strcpy` and `gets` as dangerous and say why — §7.10 assumes it |
| **Fri 9/18** | §1.6, §2.7.1–2.7.2, §2.9.4 | **C data in memory:** structs and struct field layout; pointers to structs, `->`, and `malloc(sizeof(struct ...))`; pointer arithmetic and why `ptr++` advances by the size of the pointed-to type. Framed throughout as *what §7.9 will show you the machine doing* |

**Draft 6 assigned the C-data-in-memory material to Mon 9/14, but that lecture became §2.5 in full.** Arrays deserved the period and got it. The consequence is that structs, pointers to structs, and pointer arithmetic were all still unclaimed going into Fri 9/18, along with §1.6, which Week 1 never reached.

**Friday is deliberately a memory-layout lecture rather than a C-features lecture.** It exists to make Week 14 possible, not to make students fluent C programmers. §7.9 (structs in assembly) and A8 (recovering C from a stripped binary) both assume struct field offsets; §7.7 assumes pointer arithmetic. These are the load-bearing items and they get a full period to themselves.

**§2.9.5–2.9.6 and App. 2 §17.5 move to Wed 10/7.** Draft 6 restored libraries, linking, and `make` into this Friday slot, which left Friday holding four unrelated topics. The toolchain material has a better home: Wed 10/7 is already the compilation-pipeline lecture (`gcc -S`, §2.9.7), and "what happens after the assembler" belongs there. Nothing before Week 7 depends on it — neither lab in Weeks 5–6 is a C lab, and the `objdump` work linking supports is Week 10.

## Week 5 · Sep 21–25 — architecture begins

| Day | Sections | Content |
|---|---|---|
| **Mon 9/21** | §5.1, §5.2 | Origins of modern computing; the von Neumann architecture |
| **Tue 9/22** | — | **Lab 4 — strings and structs in C. A3 due; A4 out** |
| **Wed 9/23** | §5.3, §4.6.1–4.6.4 | Logic gates. **Bitwise AND/OR/XOR/NOT folded in here** — same truth tables, as the software face of the gates |
| **Fri 9/25** | §5.4 | Arithmetic, control, and storage circuits. The ripple-carry adder is Wed 9/2's §4.4 in gates |

**Lab 4 is now a C lab and A4 is a C assignment.** Draft 6 put the circuit simulator here with only §5.1–5.2 behind it — von Neumann's five components and no gates. Worse, A4 was *build an adder/ALU*, and the ALU material (§5.4) does not arrive until Fri 9/25, three days after the assignment went out. Moving the circuit work to Lab 5 fixes both halves of that.

Making A4 a strings-and-structs assignment also buys back some of what Ch 2's compression cost. Ch 2 is on Midterm 1 and is the foundation for §7.7–7.10, and until now the only graded C work has been A3. A4 goes out Tue 9/22 with Wed's strings lecture and Fri's structs lecture behind it, and is due Tue 9/29 — graded and returned before the Oct 6 exam.

## Week 6 · Sep 28–Oct 2 — finishing architecture

| Day | Sections | Content |
|---|---|---|
| **Mon 9/28** | §5.5 | Building a processor: putting it all together |
| **Tue 9/29** | — | **Lab 5 — circuit simulator, in-lab only. A4 due** |
| **Wed 9/30** | §5.6 | The processor's execution of program instructions |
| **Fri 10/2** | §5.7–5.9 | Pipelining; hazards; CPUs today |

**Lab 5 is scoped to finish inside the period and carries no take-home assignment.** Build a half adder and a full adder from gates in the simulator, chain them into a small ripple-carry adder, and trace a few input patterns through it; submit the saved circuit and the trace before leaving. §5.3 (gates), §5.4 (adders and the ALU), and §5.5 (the processor) are all behind it, so nothing in the lab is new material — it is the hands-on pass over three lectures' worth of circuits.

This is the last lab before Midterm 1. The point of keeping it in-lab is that no student leaves the week of Sep 29 with unfinished coursework hanging over the exam run-up; whatever time they have at home goes to studying. The datapath tracing that draft 6 had scheduled here is folded into the same session as the trace step, and §5.6 on Wed 9/30 covers it again in lecture.

## Week 7 · Oct 5–9 — the pivot

| Day | Sections | Content |
|---|---|---|
| **Mon 10/5** | — | Review for Midterm 1 |
| **Tue 10/6** | — | **Lab 6 — MIDTERM 1.** Ch 1, 2, 4, 5 |
| **Wed 10/7** | Ch 6, §2.9.5–2.9.7, App. 2 §17.5 | **Under the C.** Why read assembly; compiling C to assembly; `gcc -S` revisited from Lab 0. **Then the rest of the pipeline:** `.o` files, what the linker does, headers vs. implementation, libraries, and `make` |
| **Fri 10/9** | §7.1 | Diving into assembly: registers, operand forms, addressing modes |

Administering the exam in lab costs no lecture and buys 75 minutes instead of 50.

Wed 10/7 is now a full toolchain lecture: source → assembly → object → executable, with `make` as the thing that automates it. Students have written single-file programs all semester, so multi-file compilation is introduced here as *what the tools were doing all along* rather than as a workflow they need to adopt. It also arrives three weeks before the `objdump` work in Week 10, which is where linking stops being abstract.

---

## Open items carried forward

- **Still eight graded assignments, but Ch 5 no longer has one.** A4 changes topic from gates/ALU to strings and structs, so the count is unchanged — what disappears is the only graded take-home on architecture. Ch 5's graded contact is now the in-lab exercise and Midterm 1. If that feels thin, the cheapest fix is a short problem set on §4.6 (DiS 4.19–4.26), which has no graded exercise anywhere in the course either.
- **Midterm 1 scope.** Still four chapters (1, 2, 4, 5), and Ch 5 still finishes only two class days before it. Ch 2 is better reinforced than in draft 6 now that A4 is a C assignment. Consider weighting toward Ch 1/2/4.
- **§4.6 has no graded exercise.** DiS 4.19–4.26 make a good ungraded problem set. Worth assigning as optional practice before Wed 9/23 rather than after.
- **Version control is out of the course.** Draft 6 kept looking for a home for git, first in the multi-file refactor lab and then in Week 5. Nothing in the course uses it, so it is dropped rather than parked. Revisit only if a later semester adds multi-file or team work.
