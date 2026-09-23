from style import *

with schemdraw.Drawing(show=False) as d:
    n1 = gate(d, 'Nand', (0, 1.5),  'NAND')
    n2 = gate(d, 'Nand', (0, -1.5), 'NAND')
    wire(d, (-3.5, anch(n1,'in1')[1]), anch(n1,'in1')); lbl(d, (-3.6, anch(n1,'in1')[1]), 'S')
    wire(d, (-3.5, anch(n2,'in2')[1]), anch(n2,'in2')); lbl(d, (-3.6, anch(n2,'in2')[1]), 'R')
    q, qn = anch(n1,'out'), anch(n2,'out')
    wire(d, q, (5.0, q[1]));  lbl(d, (5.1, q[1]), 'Q', loc='right')
    wire(d, qn, (5.0, qn[1])); lbl(d, (5.1, qn[1]), 'NOT(Q)', loc='right')
    dot(d, (3.2, q[1]))
    wire(d, (3.2, q[1]), (3.2, 0.4), (-1.0, 0.4), (-1.0, anch(n2,'in1')[1]), anch(n2,'in1'))
    dot(d, (3.9, qn[1]))
    wire(d, (3.9, qn[1]), (3.9, -0.4), (-1.6, -0.4), (-1.6, anch(n1,'in2')[1]), anch(n1,'in2'))
    d.save('rs-latch.png', dpi=220)
