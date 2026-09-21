---
title: "11 The Origins of Modern Computing"
published: True
---

[*Reading: Dive into Systems §5.1*](https://diveintosystems.org/book/C5-Arch/hist.html)

{% comment %}
INSTRUCTOR - FIGURES, ACTION REQUIRED. Five figures are written into this file but commented out, because the image files are not on disk: ada-lovelace.jpg, alan-turing.jpg, colossus-wrens.jpg, grace-hopper.jpg, eniac-programmers.jpg. Each sits in a comment block at the point where it belongs, with its alt text already written.

I could not download them. This session's network policy blocks Wikimedia: commons.wikimedia.org and upload.wikimedia.org both return 403 at the egress proxy, from the cloud container and from the Cowork VM on your Mac alike, and Commons file pages are marked cache-only for fetching. That is an organization egress setting, not a transient failure, so there is nothing to retry.

The download is a few minutes in an ordinary browser on your Mac. figures/ch5/SOURCES.md says what to search for on Wikimedia Commons, what to save each file as, and which licence to confirm on the file page before you publish. This page is published: True, which is why the tags are commented rather than live - nothing breaks on the site while the files are missing.

After downloading: run figures/ch5/check-figures.sh to confirm every referenced file exists, then uncomment the five blocks. Check each image against the alt text I wrote; if what you downloaded shows something different, fix the alt text rather than leaving it wrong.
{% endcomment %}

{% comment %}
INSTRUCTOR — FORMAT

This is the history-and-discussion alternative to 11-von-neumann.md. It covers §5.1 only. §5.2 moves to Wed 9/23, which means this lecture does NOT put the five-unit block diagram on the board. It ends on the stored-program *idea* and hands off.

Period shape (50 min, realistically 45 usable after settling and the A3-due / A4-out announcements):

  0-2    Announcements. A3 due at Lab 4 tomorrow; A4 goes out.
  2-4    Cold open: "computer" as a job title
  4-6    Babbage and Lovelace, and why "first" is retrospective
  6-11   The primordial soup: Shannon and Turing
  11-17  The war builds the machines: Colossus, Mark I, ENIAC
  17-22  Who actually programmed them
  22-27  von Neumann and the First Draft
  27-28  Handoff to Wednesday
  28-30  Silent write (2 min, on the prompt below)
  30-48  Discussion
  48-50  Exit ticket / close

  PACE CHECK: if you are not starting ENIAC by minute 11, cut Babbage to one sentence and Shannon to one sentence. Those two are the compressible material; everything after minute 17 is load-bearing for the discussion.

THREE THREADS. Say them out loud at the top so students know what to listen for, and name them again as each one recurs. They are not three topics — they converge on one document.

  1. WOMEN IN EARLY COMPUTING. Not "women were also there," but the specific mechanism: the work they did was classified as clerical, and classification determined credit.
  2. THE MILITARY. Not "the military was involved," but that essentially every machine in this lecture was paid for by an army, and that this determined which problems counted as worth solving — and, in Colossus's case, which history got erased.
  3. VON NEUMANN. The whole narrative is built toward him, and specifically toward the First Draft.

  The convergence is the point: Grace Hopper sits at the intersection of (1) and (2) — a commissioned Navy officer AND an under-credited programmer. The ENIAC six sit at the intersection of (1) and (3). Von Neumann sits at (2) and (3). The First Draft sits at all three: Army-funded work, circulated by an Army liaison, with one name on the cover of a document describing a machine that other people built and other people would program.

DELIVERY. Board, not slides — this is a narrative, and the only thing worth writing up is a timeline across the top of the board that you extend as you go (1837 Babbage / 1937 Shannon + Turing / 1943 Colossus / 1944 Mark I / 1945 ENIAC + First Draft / 1946 ACE, ENIAC public). Students who are lost in the names can anchor on the dates. Project the von Neumann photograph when you get to him.

TWO SECTIONS. The 10:00 and 11:00 discussions will go differently and you should not calibrate the second on the first. If the 10:00 section produces something good, resist the urge to feed it to the 11:00 section as a starting point — it flattens their discussion into agreeing with strangers.

DECISIONS STILL OPEN (see the notes inline):
  - Whether Turing's conviction is a fourth thread or a stated fact you move past.
  - Whether any of this is examinable on Midterm 1. Students WILL ask. Suggested answer: the narrative is not, the stored-program idea and everything from Wednesday is.
  - Whether anything is collected. An exit ticket in the last two minutes costs nothing; a written reflection lands in the week A4 is out and due 9/29, which Lab 5 was deliberately kept clear to protect.
{% endcomment %}

## Pre-History

From the 1700s to early 1900s, the word computer was originally used to refer to "one who computes", as in a person who performed mathematical calculations by hand. This role was most often performed by women, who were widely considered to be better at doing mathematical calculations than men. These machines were built to replace a room full of people, and the people in that room did not disappear when the machine arrived.

The first general purpose computer was developed by British mathematicians Charles Babbage and Ada Lovelace. Their Analytical Engine was a mechanical calculator based on an earlier invention of Babbage. Ada Lovelace was the first person to develop a computer program and publish an algorithm that could be executed by the machine. However, this distinction of 'first computer' is largely retrospective; most of the designers of what became the direct forerunners to the modern computer were unaware of the work of Babbage and Lovelace when they developed their own machines.

![A watercolour portrait of Ada Lovelace standing in a lilac and white gown with a black lace shawl, holding a folded fan, her hair in ringlets with flowers pinned above one ear.]({{ "/figures/ch5/ada-lovelace.jpg" | relative_url }})

## Early Theory

It is more accurate to think of modern computer architecture as rising out of a primordial soup of ideas that arose in the 1930s and 1940s than as the invention of any one person.

In 1937, Claude Shannon, a student at MIT, wrote what may be the most influential master's thesis of all time. Drawing on the work of George Boole, Shannon showed that Boolean logic could be applied to circuits, and could be used to build electrical switches. This is the foundation of binary computing and of essentially all digital circuit design.

In the same year, the British mathematician Alan Turing proposed the "Logical Computing Machine," a theoretical computer. Alonzo Church, Turing's dissertation advisor at Princeton, was the first to call it the *Turing machine*, and its universal form the *universal Turing machine*. Turing used this machine to prove that there is no solution to the decision problem posed by Hilbert and Ackermann in 1928, by showing that the halting problem is undecidable.

![A sepia head-and-shoulders photograph of Alan Turing as a young man in a tweed jacket, white shirt and dark spotted tie, looking slightly to the right of the camera.]({{ "/figures/ch5/alan-turing.jpg" | relative_url }})

Shannon gave us a way to *build* a computer. Turing gave us a way to reason about what a computer can and cannot do. But neither of them actually built a machine.

## War Machines

Turing returned to England and served in the code-breaking unit at Bletchley Park. He was instrumental in the design of the Bombe, an electromechanical device that helped break the cipher produced by the Enigma machine used by Nazi Germany to protect sensitive communications.

In 1943, the British engineer Tommy Flowers built Colossus, a code-breaking machine that is arguably the first programmable, digital, fully electronic computer. The Women's Royal Naval Service, known as the "Wrens", operated it. The *General Report on Tunny* notes that several of the Wrens showed great ability in cryptographic work, but none of them were given the position of cryptographer; they were kept on the more menial operation tasks.

![Two Wrens operating Colossus at Bletchley Park in 1943. One stands at the machine’s tall bank of switches and plugboards; the other works at the paper tape transport to the right. The machine fills the room from floor to ceiling.]({{ "/figures/ch5/colossus-wrens.jpg" | relative_url }})

Meanwhile, in the United States, Harvard professor Howard Aiken (also a Commander in the U.S. Navy Reserve) designed the Mark I, an electromechanical, general-purpose programmable computer. At the same time, at the University of Pennsylvania, the engineers John Mauchly and J. Presper Eckert designed and built the Electronic Numerical Integrator and Computer (ENIAC) completed in 1945. ENIAC is arguably the forerunner of the modern computer: digital, fully electronic, programmable, and general purpose. Both Mark I and ENIAC used decimal rather than binary.

ENIAC was built under contract to U.S. Army Ordnance. Its purpose was to compute artillery firing tables: the printed booklets that tell a gunner how to aim. While ENIAC was being built, that job was being done by hand, in the same building. The Army's Ballistic Research Laboratory had moved its trajectory work to Penn in 1940 and recruited roughly two hundred women to do it, working with slide rules, desk calculators, and the Moore School's differential analyzer. Depending on the method used, a single trajectory took anywhere from twenty minutes to several days, and the computation had to be checked against Simpson's rule every four lines.

One of those women was Kay McNulty. Despite what the work actually required, her appointment was rated at a subprofessional grade, a clerical classification.

## The first computer programmers

During the war, many women were hired into the war effort as human computers. When the first electronic machines arrived, those same women became the first programmers because programming was considered clerical work. The machine was the engineering achievement; telling it what to do was clerical.

It should be no surprise, then, that many of the foundational innovations in programming are credited to women. Grace Hopper developed the first compiler, A-0, and the first high-level programming language, FLOW-MATIC, which heavily influenced the design of COBOL, the first machine-independent programming language. Hopper was also a programmer for the Mark I and wrote the book describing its operation. She was, at the same time, a commissioned officer in the U.S. Naval Reserve.

![Grace Hopper seated at a UNIVAC console with a printout open on her lap, three male colleagues gathered around her. The console’s dense panel of switches and indicator lights fills the left of the frame.]({{ "/figures/ch5/grace-hopper.jpg" | relative_url }})

There is an oft quoted, but apocryphal story, that Grace Hopper invented the term 'bug' in software after finding a dead moth inside the Harvard Mark II which was causing errors. The famous log, including the moth, is seen in the figure below. In reality, this is not the first recorded use of the term 'bug' in engineering, and while this is a real entry in the Mark II's log book, it is unlikely that Hopper is the one who wrote it.

![The log book of the Harvard Mark II, including a moth that was found inside the machine causing errors in reading the punchcards. A literal bug in the code!]({{ "/figures/ch5/First_Computer_Bug_1947.jpg" | relative_url }})

ENIAC was programmed by six women: Jean Jennings Bartik, Betty Snyder Holberton, Kay McNulty, Frances Bilas Spence, Marlyn Wescoff Meltzer, and Ruth Lichterman Teitelbaum. They were recruited out of the pool of human computers who had been calculating the firing tables by hand. The programmers were handed the wiring diagrams of ENIAC and told to work out how it worked and how to program it. In solving that, they also invented the algorithmic flow chart and developed concepts we still use such as subroutines and nesting.

![Two women operating the ENIAC at the Moore School. One stands at the left holding a cable at the machine’s main control panel; the other reaches into a panel of switches and sockets on the right, inserting a plug. Banks of cables and rows of dials run the length of the wall behind them.]({{ "/figures/ch5/eniac-programmers.jpg" | relative_url }})

The credit did not follow. At ENIAC's public unveiling on February 15, 1946, the machine ran a trajectory program Jean Jennings and Betty Snyder had written, but the congratulations went to Eckert and Mauchly. The *New York Times* account of the dedication described the labor the machine replaced as that of “100 trained men.” The women who had in fact done that labor appear in the publicity photographs, where the captions have them “setting switches,” “plugging cables,” and “standing at function tables”; and when one widely circulated photograph was reused as an Army recruitment advertisement, the women were removed from the image altogether.

The full extent of women's contributions to early computing is not known, and cannot now be recovered. Unable to advance, many left the field after the war.

While artillery trajectories were the stated purpose, the first use of ENIAC was a feasibility calculation for a thermonuclear explosion as part of the Manhattan Project.

## John von Neumann

John von Neumann is considered by some the greatest mathematician of the twentieth century, with foundational contributions to mathematics, physics, economics, statistics, and computing. Born in Budapest in 1903, he was a child prodigy — rumored to speak Hungarian, French, English, German, and Italian, and to handle differential and integral calculus, by the age of eight. He completed a PhD in mathematics at the University of Budapest in 1926, and in 1933 took a tenured professorship at the Institute for Advanced Study in Princeton, where he stayed for the rest of his career.

![Me, standing in the snow beside John von Neumann's grave marker in Princeton Cemetery. The granite stone is engraved JOHN von NEUMANN 1903 - 1957, with Margaret von Neumann's name below it, and small stones left by visitors line the top of the marker.]({{ "/figures/ch5/von-neumann.png" | relative_url }})

His contributions weave in and out of this early history of computing. At Princeton he was an early mentor to Turing. He became a research scientist on the Manhattan Project, which brought him to Aiken and the Mark I. He consulted on ENIAC and corresponded regularly with Eckert and Mauchly — and ENIAC's first major run was his hydrogen bomb calculation.

## The First Draft

In June 1945, a document titled *First Draft of a Report on the EDVAC* was circulated. EDVAC — the Electronic Discrete Variable Automatic Computer — was the successor to ENIAC, proposed to the U.S. Army by Eckert and Mauchly and built at Penn. It differed from ENIAC in two decisive ways: it was binary rather than decimal, and it was a **stored-program** computer.

In a stored-program computer, the program instructions and the data are *both* loaded into the machine's internal memory, and the machine makes no fundamental distinction between them. Instructions are just more bits in memory.

The architecture described in that report is what we now call the **von Neumann architecture**, and essentially every machine you have ever used is built on it.

The name *von Neumann architecture* comes from this report. The report was a draft, allegedly written by hand as von Neumann took the train back to Los Alamos; it was unfinished, with blank spaces where the references to other people's work were meant to go. It was circulated by Herman Goldstine, the Army's liaison to the project, to a couple of dozen recipients. It synthesized months of group discussion at the Moore School among Eckert, Mauchly, von Neumann, and others, but it went out with one name on the cover.

Eckert and Mauchly were furious, and not only about credit: circulating the design counted as publication, which their lawyers argued destroyed any patent claim on the ideas in it. Turing, for his part, described a similar machine in great detail in his 1946 ACE report — but von Neumann's came first, so von Neumann got the name.

## Afterword


In 1952, the British government convicted Alan Turing of "gross indecency" for having a sexual relationship with a man. As a result of this conviction, Turing's security clearance in Britain was revoked, he was denied entry to the United States and he was forced to undergo a course of hormone injections known then as "chemical castration". On June 8, 1954, Alan Turing was found dead in his home; the cause of death was determined to be cyanide poisoning.

The six ENIAC programmers were inducted into the Women in Technology International Hall of Fame in 1997, fifty-one years after the demonstration they ran. Ruth Lichterman Teitelbaum had died in 1986 and did not live to see it.


## Further reading

For the Moore School's women computers specifically, the scholarly source is Jennifer S. Light, ["When Computers Were Women,"](https://cs.brown.edu/courses/cs1951i/lightWhenComputersWereWomen.pdf) *Technology and Culture* 40:3 (July 1999), 455-483.

For more on the early women programmers generally: *Recoding Gender* by Janet Abbate; *Top Secret Rosies*, a PBS documentary directed by LeAnn Erickson; and *The Computers* by Kathy Kleiman.
