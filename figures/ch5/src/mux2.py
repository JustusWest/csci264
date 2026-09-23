from style import *

with schemdraw.Drawing(show=False) as d:
    n1 = gate(d, 'Not', (-2.5, -3.0), '')
    a1 = gate(d, 'And', (1.5, 1.0),   'AND')
    a2 = gate(d, 'And', (1.5, -1.5),  'AND')
    o1 = gate(d, 'Or',  (5.0, -0.25), 'OR')

    wire(d, (-6, 1.25), anch(a1,'in1')); lbl(d, (-6.1, 1.25), 'B')
    wire(d, (-6, -1.25), anch(a2,'in1')); lbl(d, (-6.1, -1.25), 'A')
    wire(d, (-6, -3.0), anch(n1,'in1')); lbl(d, (-6.1, -3.0), 'S')
    dot(d, (-4.0, -3.0))
    wire(d, (-4.0, -3.0), (-4.0, -4.2), (0.8, -4.2), (0.8, anch(a2,'in2')[1]), anch(a2,'in2'))
    wire(d, anch(n1,'out'), (0.4, -3.0), (0.4, anch(a1,'in2')[1]), anch(a1,'in2'))
    wire(d, anch(a1,'out'), (4.4, anch(a1,'out')[1]), (4.4, anch(o1,'in1')[1]), anch(o1,'in1'))
    wire(d, anch(a2,'out'), (4.4, anch(a2,'out')[1]), (4.4, anch(o1,'in2')[1]), anch(o1,'in2'))
    wire(d, anch(o1,'out'), (8.4, anch(o1,'out')[1]))
    lbl(d, (8.5, anch(o1,'out')[1]), 'out', loc='right')
    d.save('mux2.png', dpi=220)
