# CSCI 264 — Lecture Plan, Weeks 1–7 (draft 9)

**Text:** *Dive into Systems* v1.2 (Matthews, Newhall, Webb)
**Meetings:** Lecture MWF; Lab Tue (Sec A 12:30, Sec B 3:30)

**Changes from draft 8.** Two changes, both inside Week 5–6.

1. **§4.6.1–4.6.4 (bitwise AND/OR/XOR/NOT) is cut**, not relocated again. It was folded into Fri 9/25, and with §5.4 now starting in that period there is no room for it. Shifting still moves to §7.3 as before.
2. **§5.4 is split across two periods.** The circuits framing and the 1-bit circuit design method move up into the second half of Fri 9/25, and Mon 9/28 carries the rest of §5.4. §5.3 alone did not fill a period once the bitwise material came out; §5.4 was never going to fit in one.

**No dates move and nothing else is cut.** All lab dates are unchanged, Midterm 1 stays Tue Oct 6, Ch 5 still finishes on Fri 10/2, and the assignment count stays at eight. Fri 10/2 is still overloaded on purpose — see the note under Week 6.

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

**§4.6 (Bitwise Operators) is cut as a lecture topic.** Draft 8 folded AND/OR/XOR/NOT into the Fri 9/25 gates lecture, but that period now also opens §5.4. The truth tables are still taught — as gates, in §5.3 — so what is actually lost is the C operators (`&`, `|`, `^`, `~`) and the distinction between them and `&&`, `||`, `!`. Nothing graded exercised it, and nothing before Week 8 depends on it.

- **Shifting → §7.3**, where `sall` and `sarl` show up as the compiler's strength reduction for `*2` and `/2`. Unchanged.
- **The debt is now in Ch 7.** `xorl %eax, %eax` (the zeroing idiom, §7.2 onward) and `testl %eax, %eax` (§7.4, the most common comparison in compiled code) both assume the C operators. Budget five minutes for `&`, `^`, and `~` when §7.2 introduces the idiom — students will have seen the gates five weeks earlier and only need the syntax.
- **It is off Midterm 1.** DiS 4.19–4.26 remain a good optional problem set, but nothing on the exam should require the C bitwise operators.

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
| **Mon 9/21** | §5.1 | **Origins of modern computing — lecture and discussion.** ~25 min narrative, ~20 min discussion. No §5.2 content |
| **Tue 9/22** | — | **Lab 4 — strings and structs in C. A3 due; A4 out** |
| **Wed 9/23** | §5.2 | The von Neumann architecture: the five units, the buses, and the fetch-decode-execute-store cycle |
| **Fri 9/25** | §5.3, §5.4 *(opening)* | Logic gates: transistors, AND/OR/NOT, truth tables, minimal complete sets. **Then §5.4 begins:** what a circuit is, the three-step method for designing a 1-bit circuit, and the 1-bit equality circuit worked start to finish |

**Mon 9/21 is a history-and-discussion day.** §5.1 is lectured as a narrative for roughly 25 minutes and the rest of the period is student discussion. Three threads run through the narrative and are the discussion's pickup points: the role of women in early computing and the ways their contributions were under-attributed; the extent to which early computing was funded and driven by the military; and von Neumann himself, whose *First Draft of a Report on the EDVAC* is where all three threads meet. §5.2 is not touched — the lecture ends on the stored-program *idea*, and Wednesday opens the box.

This is the one Monday in the architecture block where a discussion day costs nothing downstream. The usual rhythm is concept Monday → lab Tuesday, but Lab 4 is a strings-and-structs C lab whose starter code was built in the Fri 9/18 lecture. Tuesday does not depend on Monday this week.

**Lab 4 is a C lab and A4 is a C assignment.** Draft 6 put the circuit simulator here with only §5.1–5.2 behind it — von Neumann's five components and no gates. Worse, A4 was *build an adder/ALU*, and the ALU material (§5.4) does not arrive until Week 6, after the assignment would have gone out. Moving the circuit work to Lab 5 fixes both halves of that.

**Fri 9/25 ends inside §5.4.** §5.3 is thin — three gates, two truth tables, and the completeness argument — and with the bitwise material cut it does not fill a period. Rather than pad it, the lecture crosses into §5.4 and spends the second half on the 1-bit equality circuit: truth table, expression, gates, verification, abstraction. That is the method every later circuit uses, and doing it once slowly is worth more than seeing three circuits quickly on Monday. It also leaves Monday free to spend real time on the adder, which is what Lab 5 actually builds.

Making A4 a strings-and-structs assignment also buys back some of what Ch 2's compression cost. Ch 2 is on Midterm 1 and is the foundation for §7.7–7.10, and until now the only graded C work has been A3. A4 goes out Tue 9/22 with the strings lecture (Wed 9/16) and the structs lecture (Fri 9/18) behind it, and is due Tue 9/29 — graded and returned before the Oct 6 exam.

## Week 6 · Sep 28–Oct 2 — finishing architecture

| Day | Sections | Content |
|---|---|---|
| **Mon 9/28** | §5.4 *(rest)* | The 1-bit adder, carry-in, and the ripple-carry adder — Wed 9/2's §4.4 in gates; then the ALU, control circuits, and storage circuits |
| **Tue 9/29** | — | **Lab 5 — circuit simulator, in-lab only. A4 due** |
| **Wed 9/30** | §5.5 | Building a processor: putting it all together |
| **Fri 10/2** | §5.6–5.9 | The processor's execution of program instructions; pipelining; hazards; CPUs today |

**Fri 10/2 carries four sections and will not fit in one period.** That is accepted rather than solved: it is the last lecture before the Midterm 1 review, it is the cheapest place in the course to lose material, and the decision about what to drop is better made in the room than here. For when that moment arrives, in rough order of what is worth protecting: §5.6 (execution on the real datapath) is the section the rest of the course actually uses and should be lectured in full; §5.7 (pipelining) is the idea students will meet again everywhere and deserves a real, if quick, treatment; §5.8 (hazards) can be a sketch of the problem with one example rather than a taxonomy; §5.9 (CPUs today) is the cheapest thing to make a read-on-your-own slide and costs nothing on the exam.

**Lab 5 now has two lectures behind it, not three.** §5.3 (Fri 9/25) and §5.4 (Fri 9/25 and Mon 9/28) both precede it, and those are the two sections the lab actually exercises — half adder, full adder, a short ripple-carry chain, and a trace. The split helps here: Monday is now entirely adders and the circuits built from them, the day before students build one. What it loses is §5.5, which now falls on Wed 9/30, the day *after*. Nothing in the adder work depends on having seen the whole processor first, so the lab still runs as written; what no longer works is the datapath-tracing step draft 7 folded into the trace. Drop that step and let §5.6 on Fri 10/2 do the datapath tracing in lecture, or move it into the Midterm 1 review on Mon 10/5.

Lab 5 is still scoped to finish inside the period and still carries no take-home. This is the last lab before Midterm 1, and the point of keeping it in-lab is that no student leaves the week of Sep 29 with unfinished coursework hanging over the exam run-up; whatever time they have at home goes to studying.

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

- **Still eight graded assignments, but Ch 5 no longer has one.** A4 changes topic from gates/ALU to strings and structs, so the count is unchanged — what disappears is the only graded take-home on architecture. Ch 5's graded contact is now the in-lab exercise and Midterm 1. If that feels thin, the cheapest fix is a short problem set on §4.6 (DiS 4.19–4.26) — though with §4.6 no longer lectured, that would now be assigned as self-study rather than review.
- **Midterm 1 scope.** Still four chapters (1, 2, 4, 5), and Ch 5 still finishes only two class days before it. Ch 2 is better reinforced than in draft 6 now that A4 is a C assignment. Consider weighting toward Ch 1/2/4.
- **§4.6 is no longer lectured.** Keep it off Midterm 1, and carry the five-minute debt into §7.2 (see the Week 2 note). DiS 4.19–4.26 still make a good optional problem set, but they now come after the gates lecture rather than with it.
- **Version control is out of the course.** Draft 6 kept looking for a home for git, first in the multi-file refactor lab and then in Week 5. Nothing in the course uses it, so it is dropped rather than parked. Revisit only if a later semester adds multi-file or team work.
