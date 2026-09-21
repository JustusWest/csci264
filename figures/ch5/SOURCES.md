# Figures for Lecture 11 (Origins of Modern Computing)

Five images are referenced by `_lectures/11-origins-history.md` and are **not yet downloaded**.
Claude could not fetch them: this session's egress policy returns 403 for
`commons.wikimedia.org` and `upload.wikimedia.org`, from the cloud container and from the
Cowork VM alike. Download them in an ordinary browser instead.

For each: search [Wikimedia Commons](https://commons.wikimedia.org), open the file page,
**confirm the licence on that page**, download the full-resolution file, and save it into
this folder under the filename in the first column. Then uncomment the matching block in
the lecture file and run `check-figures.sh`.

The "likely file" column is a starting point from memory and is **unverified** — treat it as
a search hint, not a fact. The "should show" column is what the alt text in the lecture
already describes; if your download shows something else, update the alt text.

| Save as | Search Commons for | Likely file (unverified) | Should show | Expected licence |
|---|---|---|---|---|
| `ada-lovelace.jpg` | "Ada Lovelace portrait" | `Ada_Lovelace_portrait.jpg` | DOWNLOADED: Chalon watercolour, standing, lace shawl, folded fan | Public domain (1830s artwork) |
| `alan-turing.jpg` | "Alan Turing" | `Alan_Turing_Aged_16.jpg` | DOWNLOADED: sepia head-and-shoulders, tweed jacket, spotted tie | Check the file page — Turing photo licensing varies |
| `colossus-wrens.jpg` | "Colossus computer Bletchley Park 1943" | `Colossus.jpg` | DOWNLOADED: matches — two Wrens, plugboards left, tape transport right | UK Crown / public domain |
| `grace-hopper.jpg` | "Grace Hopper Navy portrait" | `Commodore_Grace_M._Hopper,_USN_(covered).jpg` | DOWNLOADED: Hopper at a UNIVAC console with three colleagues (not the formal portrait; alt text updated to match) | Public domain (US Navy work) |
| `eniac-programmers.jpg` | "ENIAC programmers Moore School" | `Eniac.jpg` or `Glen_Beck_and_Betty_Snyder_program_the_ENIAC...jpg` | DOWNLOADED: two women operating the machine, cable at the control panel, plug at the switch panel | Public domain (US Army work) |

Notes:

- The Wrens photograph matters more than a generic Colossus shot — the lecture's point is that
  the women in it were kept as operators, so an image with people in it does work that an
  image of an empty machine does not.
- Same for the ENIAC photograph: pick one where the women are visibly *operating* the machine,
  not posed beside it. The lecture explicitly discusses that distinction.
- Already on disk and in use, no action needed: `von-neumann.png` (your grave-marker
  photograph) and `First_Computer_Bug_1947.jpg` (Mark II logbook moth).

## grace-hopper.jpg — who is in it

Computer History Museum, catalogue no. 102741216, "Grace Hopper at Univac I console."
Description: *"Image shows operator's console of Univac I computer and four programmers.
From left to right, Donald Cropper, K. C. Krishnan, Grace Hopper, and Norman Rothberg."*
Dated **1957** by CHM (Wikimedia and ww2db say "circa 1960" — CHM is the better authority).
Credit line: Gift of Dag Spicer. Original archive: Hagley Digital Archives, Sperry Corporation
UNIVAC Division photographs, accession 1985261, item 1985261_086_004_018, "Grace Hopper and
others with UNIVAC system."

Note that CHM calls all four **programmers** — Cropper and Krishnan are colleagues, not onlookers.

Biographical research (2026-09-20), results:

- **Norman Rothberg** — the only one of the three with a traceable footprint. Harold "Bud"
  Lawson's first-hand memoir on the Engineering and Technology History Wiki
  (https://ethw.org/First-Hand:Experiences_and_Reflections_of_a_Computer_Pioneer) thanks
  "Norman Rothberg for teaching me the practical skills needs for operation and even repair
  during many late hours of the night and early mornings." Lawson was in Hopper's Automatic
  Programming Department in Philadelphia from June 1959 to 1961, working on the B-0 / FLOW-MATIC
  compiler and the UNIVAC II COBOL compiler. So Rothberg was a hands-on UNIVAC I and II
  operations-and-repair man in that department. Lawson also names Bill Finley, Tom Jones,
  Dick Miner and Dan Goldstein as senior people there.
- **Donald Cropper** — nothing found. No biography, obituary, publication or other photograph.
- **K. C. Krishnan** — nothing found. Same.

Where to look next, offline: the Hagley Library itself (the Sperry/UNIVAC collection is large
and staffed); ACM and AFIPS proceedings and Communications of the ACM author indexes, 1955-1965;
Remington Rand / Sperry staff directories; and Dag Spicer at CHM, who donated the print.
