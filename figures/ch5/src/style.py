import matplotlib; matplotlib.use('Agg')
import matplotlib.pyplot as plt
plt.rcParams['font.family'] = 'Carlito'
plt.rcParams['mathtext.fontset'] = 'custom'
plt.rcParams['mathtext.rm'] = 'Carlito'
plt.rcParams['mathtext.it'] = 'Carlito:italic'
plt.rcParams['mathtext.default'] = 'regular'
import schemdraw
from schemdraw import logic, elements as elm

GATEFILL = '#CDEBFB'
BOXFILL  = '#BDD7EE'
BOXEDGE  = '#2E75B6'
WIRE     = '#4472C4'

schemdraw.config(fontsize=15, lw=2.5)

def gate(d, kind, at, label, direction='right', color='black'):
    g = getattr(logic, kind)().fill(GATEFILL).color(color)
    g = g.at(at)
    if direction == 'down':
        g = g.down()
    d += g
    xy = tuple(g.absanchors['xy']); out = tuple(g.absanchors['out'])
    cx, cy = (xy[0] + out[0]) / 2, (xy[1] + out[1]) / 2
    if label:
        off = {'And': -0.15, 'Nand': -0.15, 'Or': 0.05, 'Nor': 0.05, 'Xor': 0.05, 'Xnor': 0.05}.get(kind, 0.0)
        if direction == 'down':
            d += elm.Label().at((cx, cy - off)).label(label, fontsize=12)
        else:
            d += elm.Label().at((cx + off, cy)).label(label, fontsize=12)
    return g

def wire(d, *pts):
    for p, q in zip(pts, pts[1:]):
        d += elm.Line().at(p).to(q)

def dot(d, p):
    d += elm.Dot(radius=0.11).at(p)

def lbl(d, p, text, loc='left', size=15):
    d += elm.Label().at(p).label(text, loc=loc, fontsize=size)

def anch(g, name):
    return tuple(g.absanchors[name])
