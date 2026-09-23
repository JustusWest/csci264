from style import *

def adder(d, x):
    ic = elm.Ic(pins=[elm.IcPin(name='', side='top', slot='1/2'),
                      elm.IcPin(name='', side='top', slot='2/2'),
                      elm.IcPin(name='', side='right'),
                      elm.IcPin(name='', side='left'),
                      elm.IcPin(name='', side='bottom')],
                w=3.2, h=2.6).at((x, 0)).fill(BOXFILL).color(BOXEDGE)
    d += ic
    ctr = tuple(ic.absanchors['center'])
    d += elm.Label().at((ctr[0], ctr[1] + 0.32)).label('1-bit', fontsize=25, color='black')
    d += elm.Label().at((ctr[0], ctr[1] - 0.32)).label('adder', fontsize=25, color='black')
    return ic

SUBY = 7.6
TOPY = 5.8

with schemdraw.Drawing(show=False) as d:
    xs = [0, 7.2, 14.4, 21.6]
    boxes = [adder(d, x) for x in xs]

    for i, b in enumerate(boxes):
        n = 3 - i
        A = tuple(b.absanchors['inT1'])   # left top pin  -> A
        B = tuple(b.absanchors['inT2'])   # right top pin -> B (through XOR)
        S = tuple(b.absanchors['inB1'])
        # A straight up
        d += elm.Line().at(A).to((A[0], TOPY)).color(BOXEDGE)
        lbl(d, (A[0] - 0.45, TOPY + 0.35), f'A$_{n}$', loc='center', size=26)
        # XOR above B
        g = gate(d, 'Xor', (B[0], 4.4), '', direction='down', color=BOXEDGE)
        d += elm.Line().at(anch(g, 'out')).to((B[0], B[1])).color(BOXEDGE)
        bi = anch(g, 'in1')
        d += elm.Line().at(bi).to((bi[0], TOPY)).color(BOXEDGE)
        lbl(d, (bi[0] + 0.3, TOPY + 0.35), f'B$_{n}$', loc='center', size=26)
        si = anch(g, 'in2')
        d += elm.Line().at((si[0], SUBY)).to(si).color(BOXEDGE)
        d += elm.Dot(radius=0.12).at((si[0], SUBY)).color(BOXEDGE)
        # sum out
        d += elm.Arrow(arrowwidth=0.25, arrowlength=0.4).at(S).to((S[0], S[1] - 1.6)).color(BOXEDGE)
        lbl(d, (S[0], S[1] - 2.1), f'Sum$_{n}$', loc='center', size=26)

    # carry chain, right to left
    for i in range(3):
        src = tuple(boxes[i+1].absanchors['inL1'])
        dst = tuple(boxes[i].absanchors['inR1'])
        d += elm.Arrow(arrowwidth=0.25, arrowlength=0.4).at(src).to(dst).color(BOXEDGE)
        lbl(d, ((src[0] + dst[0]) / 2, src[1] + 0.5), 'C$_{out}$', loc='center', size=22)

    # SUB rail
    left = tuple(boxes[0].absanchors['inL1'])
    right_pin = tuple(boxes[3].absanchors['inR1'])
    railR = right_pin[0] + 3.4
    d += elm.Line().at((left[0] - 1.0, SUBY)).to((railR, SUBY)).color(BOXEDGE)
    lbl(d, (left[0] - 1.2, SUBY), 'SUB', loc='left', size=26)
    d += elm.Line().at((railR, SUBY)).to((railR, right_pin[1])).color(BOXEDGE)
    d += elm.Arrow(arrowwidth=0.25, arrowlength=0.4).at((railR, right_pin[1])).to(right_pin).color(BOXEDGE)
    d += elm.Dot(radius=0.12).at((railR, SUBY)).color(BOXEDGE)
    lbl(d, (railR - 1.0, right_pin[1] + 0.5), 'C$_{in}$', loc='center', size=22)
    # final carry out
    d += elm.Arrow(arrowwidth=0.25, arrowlength=0.4).at(left).to((left[0] - 2.6, left[1])).color(BOXEDGE)
    lbl(d, (left[0] - 1.3, left[1] + 0.5), 'C$_{out}$', loc='center', size=22)
    d.save('add-sub.png', dpi=200)
