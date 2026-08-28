# CSCI 264 — Lecture Plan, Weeks 1–7 (draft 6)

**Text:** *Dive into Systems* v1.2 (Matthews, Newhall, Webb)
**Meetings:** Lecture MWF; Lab Tue (Sec A 12:30, Sec B 3:30)

**Changes from draft 5.** Ch 4 compressed from five lectures to three: §4.3–4.5 consolidated onto Wed 9/2, all of §4.8 onto Fri 9/4. Number systems finish Fri 9/4. §4.6 relocated rather than cut (see note after Week 2). §4.7 stays cut. The two freed lectures (Wed 9/9, Fri 9/11) go to Ch 2, which grows from 3.5 lectures to 5.5 and now spans Weeks 3–4.

**Nothing shifts past Week 4.** Weeks 5–7, all lab dates, all assignment dates, and Midterm 1 on Tue Oct 6 are unchanged from draft 5.

---

## Week 1 · Aug 24–28

| Day | Sections | Content |
|---|---|---|
| **Mon 8/24** | — | Syllabus, course structure, demo-lab expectations, AI policy, introductions. No technical content |
| **Tue 8/25** | App. 2 §17.1–17.2, §17.4 | **Lab 0 — Using Unix and GNU C.** Instructor-led start to finish. No assignment |
| **Wed 8/26** | §1.1–1.2 | Compiling and running; types; variables; operators — **integer `/` and `%` emphasized**; `printf` and `scanf` |
| **Fri 8/28** | §1.3, §1.4, §1.5 | Conditionals and loops; functions (flag §1.4.1 *The Stack* as a preview of §7.5); arrays; `char` and ASCII arithmetic; strings and the string library (§1.5.4) |


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

## Week 4 · Sep 14–18 — Ch 2 continued

| Day | Sections | Content |
|---|---|---|
| **Mon 9/14** | §1.6, §2.5.2, §2.7.2, §2.9.4 | **C data in memory:** structs; two-dimensional arrays; pointers to structs; pointer arithmetic. Framed throughout as *what §7.7–7.9 will show you the machine doing* |
| **Tue 9/15** | — | **Lab 3 — Pointers and dynamic memory. A2 due; A3 out** |
| **Wed 9/16** | §2.6 | Strings and the string library. Flag `strcpy` and `gets` as dangerous and say why — §7.10 assumes it |
| **Fri 9/18** | §2.9.5–2.9.6, App. 2 §17.5 | Libraries; compiling and linking; multi-file programs; `make` |

**This fixes a real ordering bug in draft 5.** Lab 3 is a pointers *and dynamic memory* lab, but §2.4 (`malloc`/`free`) was lectured Wed 9/16 — the day after the lab. Students walked in having had one lecture that got as far as `&` and `*`. They now have three lectures of runway, including malloc, before Lab 3.

Restoring §2.9.5–2.9.6 recovers what draft 5 called "the real loss": multi-file compilation returns, `make` has motivation again, and linking gets explained before the `objdump` work in Week 10. It also gives the Lab 5 git plan a home again if you still want version control in the course.

## Week 5 · Sep 21–25 — architecture begins

| Day | Sections | Content |
|---|---|---|
| **Mon 9/21** | §5.1, §5.2 | Origins of modern computing; the von Neumann architecture |
| **Tue 9/22** | — | **Lab 4 — circuit simulator. A3 due; A4 out** |
| **Wed 9/23** | §5.3, §4.6.1–4.6.4 | Logic gates. **Bitwise AND/OR/XOR/NOT folded in here** — same truth tables, as the software face of the gates |
| **Fri 9/25** | §5.4 | Arithmetic, control, and storage circuits. The ripple-carry adder is Wed 9/2's §4.4 in gates |

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

---

## Open items carried forward

- **Midterm 1 scope.** Still four chapters (1, 2, 4, 5), and Ch 5 still finishes only two class days before it. Ch 4 is now more concentrated but not smaller. Consider weighting toward Ch 1/2/4.
- **§4.6 has no graded exercise.** DiS 4.19–4.26 make a good ungraded problem set. Worth assigning as optional practice before Wed 9/23 rather than after.
- **Ch 2 lecture split is provisional.** Wed 9/16 and Fri 9/18 are the two restored slots; if Ch 2 runs long earlier in the week, strings compress more easily than linking does.