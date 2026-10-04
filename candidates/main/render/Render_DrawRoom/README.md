# Render_DrawRoom (main 0x58814, 0x68C bytes): 7 real diffs

Actually the field actor ground-shadow pass: builds a ground-aligned basis
from the actor's selected bone matrix (RT * (0,0,0x1000), normalised, outer
product with +Y), composes it with the camera matrix (stock PSY-Q
gte_CompMatrix), then projects a square of the model header's shadow radius
with four RotTransPers calls into a POLY_FT4 and links it with addPrim.

Plain C: no pins, barriers, casts between pointers and integers, gotos or
byte-pointer arithmetic; only the sanctioned gte.h macros (gte_ldrotmatrix,
gte_ldtransmatrix, gte_ldv0, gte_rt, gte_stmac, gte_ldopv1_psyq, gte_ldopv2,
gte_op12_psyq, gte_CompMatrix). To score: copy render_shadow.h to
include/pe1/, apply headers.diff (names RenderObjectHeader+0x14
shadow_radius), copy the .c to src/main/render/, offset 58814 size 68C.

Lessons that got it from 500+ to 7:
- 16.16 positions read as `int fraction : 16; int integer : 16;` bitfields
  give retail's `lh` copies (a plain s16 field gives `lhu`).
- The bone matrix copy is eight separate word assignments through a
  GteMatrixStorage view (a struct assignment becomes movstrsi with four
  temporaries and groups the loads).
- `shade = shade * k / 128;` and a block-scoped `half = shade / 2` (a single
  `/ 128 / 2` folds into `/ 256`; reusing `shade` hoists the halving).
- UV stores written in setUV4 order (u0,v0,u1,v1,...) give retail's 0x40/0x3F
  constant registers.
- addPrim as two TILE_OT_ENTRY evaluations (retail reloads the ordering-table
  pointer after the tag store but keeps the draw slot).

Remaining (all in the eight-word matrix copy, insns 34..62): retail loads
word 0 into a0 right after word 1 (sched2 fills the first load delay) and
keeps word 7 in v1 until after the translation zeroing; mine reuses v0 for
both, so the stores of words 0/7 land in other slots. The statement order
in the candidate (W1..W6, t0=0, W0, z, W7, x, t2=0, y, t1=0) is the best of a
6000-variant random order search; no order of the 14 statements reproduced
retail. The decomp-permuter could not parse the GTE asm operands.
