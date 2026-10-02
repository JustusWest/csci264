# Graphics vs. neural networks: both are a matrix applied to many vectors.
# Writes ../matmul.png. Run from any directory.
import os
from style import *
from matplotlib.patches import Rectangle, Circle, FancyArrowPatch

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'matmul.png')
VEC = '#E2F0D9'; VECEDGE = '#548235'

fig, ax = plt.subplots(figsize=(16, 9.7))
ax.set_xlim(-0.4, 32.4); ax.set_ylim(-0.6, 18.8); ax.set_aspect('equal'); ax.axis('off')

def grid(x, y, rows, fill, edge, w=1.15, h=0.8, size=17):
    """Draw a matrix of labelled cells with its top-left corner at (x, y)."""
    for r, row in enumerate(rows):
        for c, t in enumerate(row):
            ax.add_patch(Rectangle((x + c*w, y - (r+1)*h), w, h, fc=fill, ec=edge, lw=1.8))
            ax.text(x + c*w + w/2, y - r*h - h/2, t, ha='center', va='center', fontsize=size)
    return x + len(rows[0])*w

def txt(x, y, s, size=20, **kw):
    ax.text(x, y, s, ha='center', va='center', fontsize=size, **kw)

def arrow(p, q, color='black'):
    ax.add_patch(FancyArrowPatch(p, q, arrowstyle='-|>', mutation_scale=22, lw=2, color=color))

# ---- panel frames and titles
for x0, title in [(0.3, 'Graphics: moving a 3D model'), (16.3, 'Neural network: one layer')]:
    ax.add_patch(Rectangle((x0, 3.0), 15.4, 15.1, fc='white', ec=BOXEDGE, lw=2))
    txt(x0 + 7.7, 17.4, title, size=25, weight='bold')

# ---- left picture: a cube before and after a translation
def cube(ox, oy, s=2.6, d=1.1, color=BOXEDGE):
    f = [(ox, oy), (ox+s, oy), (ox+s, oy+s), (ox, oy+s)]
    b = [(x+d, y+d) for x, y in f]
    for i in range(4):
        for P, Q in [(f[i], f[(i+1) % 4]), (b[i], b[(i+1) % 4]), (f[i], b[i])]:
            ax.plot([P[0], Q[0]], [P[1], Q[1]], color=color, lw=2.2)
    for P in f + b:
        ax.add_patch(Circle(P, 0.13, color='black', zorder=3))
cube(2.4, 11.6, color='#9DB9D9')
cube(10.0, 11.6)
arrow((6.7, 13.1), (9.5, 13.1))
txt(8.1, 14.35, 'move by', size=16)
txt(8.1, 13.75, '(t$_x$, t$_y$, t$_z$)', size=16)
txt(8.0, 10.7, 'each corner (vertex) is a vector (x, y, z, 1)', size=16)

# ---- left equation: new vertex = transform matrix x old vertex
y0 = 9.6
grid(1.0, y0, [["x'"], ["y'"], ["z'"], ['1']], VEC, VECEDGE)
txt(2.75, y0 - 1.6, '=', size=30)
xe = grid(3.4, y0, [['1', '0', '0', 't$_x$'], ['0', '1', '0', 't$_y$'], ['0', '0', '1', 't$_z$'], ['0', '0', '0', '1']], BOXFILL, BOXEDGE)
txt(xe + 0.5, y0 - 1.6, '×', size=30)
grid(xe + 1.0, y0, [['x'], ['y'], ['z'], ['1']], VEC, VECEDGE)
txt(13.2, y0 - 1.1, 'one 4×4', size=17); txt(13.2, y0 - 1.75, 'transform', size=17); txt(13.2, y0 - 2.4, 'matrix', size=17)
txt(8.0, 4.9, 'repeat for every vertex of every model:', size=17)
txt(8.0, 3.9, 'millions of vertices per frame', size=17, style='italic')

# ---- right picture: 3 inputs fully connected to 2 outputs
ins = [(20.3, 15.0), (20.3, 13.3), (20.3, 11.6)]
outs = [(27.3, 14.15), (27.3, 12.45)]
for i, P in enumerate(ins):
    for j, Q in enumerate(outs):
        ax.plot([P[0], Q[0]], [P[1], Q[1]], color=BOXEDGE, lw=1.8, zorder=1)
for i, P in enumerate(ins):
    ax.add_patch(Circle(P, 0.55, fc=VEC, ec=VECEDGE, lw=2, zorder=2)); txt(P[0], P[1], f'x$_{i+1}$', size=17)
for j, Q in enumerate(outs):
    ax.add_patch(Circle(Q, 0.55, fc=VEC, ec=VECEDGE, lw=2, zorder=2)); txt(Q[0], Q[1], f'y$_{j+1}$', size=17)
txt(24.0, 16.35, 'each line has a weight w', size=16)
txt(24.0, 10.7, 'the inputs (pixels, words, ...) are a vector', size=16)

# ---- right equation: outputs = weight matrix x inputs
grid(17.8, y0, [['y$_1$'], ['y$_2$']], VEC, VECEDGE)
txt(19.55, y0 - 0.8, '=', size=30)
xe = grid(20.2, y0, [['w$_{11}$', 'w$_{12}$', 'w$_{13}$'], ['w$_{21}$', 'w$_{22}$', 'w$_{23}$']], BOXFILL, BOXEDGE)
txt(xe + 0.5, y0 - 0.8, '×', size=30)
grid(xe + 1.0, y0, [['x$_1$'], ['x$_2$'], ['x$_3$']], VEC, VECEDGE)
txt(28.6, y0 - 0.4, 'one weight', size=17); txt(28.6, y0 - 1.05, 'matrix', size=17)
txt(24.0, 4.9, 'repeat for every input, in every layer:', size=17)
txt(24.0, 3.9, 'real layers have thousands of rows and columns', size=17, style='italic')

# ---- bottom banner
ax.add_patch(Rectangle((0.3, 0.3), 31.4, 2.2, fc=BOXFILL, ec=BOXEDGE, lw=2))
txt(16.0, 1.85, 'Both are the same matrix applied to many vectors: a matrix multiplication.', size=21)
txt(16.0, 0.95, 'This is the job a GPU is built for.', size=21)

fig.savefig(OUT, dpi=150, bbox_inches='tight', pad_inches=0.3, facecolor='white')
print('wrote', OUT)
