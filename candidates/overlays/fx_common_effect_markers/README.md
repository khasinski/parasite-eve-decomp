# fx_common func_80193B5C (0x4B6C, 0x5AC) effect marker draw: parked 2026-10-03

Candidate `FxCommon_DrawEffectMarkers.c` (copy into src/overlays/fx_common to
score; types `FxCommonLinePacket`, `FxCommonMarkerQuad`,
`FxCommonMarkerCursor`, `g_FxCommonMotionWhole` and the GPU prototypes are
already in fx_common_setup.h, the `s16 func_80193B5C(s16)` prototype in
fx_common.h). Yaml flip: `[0x4B6C, c, FxCommon_DrawEffectMarkers]`.
No casts, pins, barriers or volatile.

State: about 500 real diffs; the control flow, packet layout and call
sequence match, the register allocation does not.

Matched along the way (keep):
- `value` is an ordinary s16 parameter: retail spills it to sp+0x50 for
  register pressure, it is not volatile;
- the clut call must be two calls in an if/else
  (`if (D_8019CC52 == i) clut = GetClut(0x3C0, 0x90); else ... 0x8F`): jump2
  cross-jumps the tails into retail's `bne; j; addiu a1` shape, a ternary or a
  palette variable gives `li a1,0x8F; bne; li a1,0x90`;
- a per-quad `u` (0 then 0x80) used for u0/u2, u1 = u + 0x7F and the second
  quad's colours keeps 0x80 from being hoisted out of the loop (a constant
  set once in the loop is a movable; a variable set twice is not);
- the 16.16 motion vectors are read as `int fraction:16; int whole:16`
  bitfields to get retail's `lh` (a plain s16 view gives `lhu`);
- the packet cursor is a union of pointers (`bytes`/`line`/`quad`), so
  `packet.line++` / `packet.quad++` replace pointer casts.

Remaining:
1. Retail computes `level << 16` once before the loop and derives
   `half = srl 17` (kept in s2), `dim = srl 19` (spilled with sb to 0x68)
   and the per-iteration `!= 0` test (spilled word at 0x60). Every C form
   tried (u16/s16/int level, half/dim before or inside the loop, explicit
   `(level << 16) >> 17`) gives `andi 0xFFFF` or keeps level itself in a
   stack slot, which changes the whole s-register assignment.
2. Retail keeps i*0x34 / i*13 / i*13+12 in s6 / s7 / fp and spills i and
   i*16; mine keeps the second quad's x+0x7F, y+12, y+13 alive across the
   calls (cse) and spills the induction variables instead. Retail's first
   quad uses the raw `screen1` word (s0, then s0+0x7F) and the second quad
   recomputes from copies (s4/s3), so the two quads use different
   expressions for x and y.
3. Prologue: retail reloads `buffer->allocation` and re-tests the frame for
   the mode packet address; mine reuses the first selection.

2026-10-04 (agent5): not retried in depth. Note on blocker 1: a small cc1
test shows `u16 level`, `int level` with `(u16)level` and `s16 level` all
give one `andi 0xFFFF` shared by both shifts and the zero test, while retail
shares `sll 16` and derives `srl 17` / `srl 19` and the test from it. The
`sh/lhu 0x50(sp)` traffic on `value` is a spilled HImode parameter pseudo
(parameters keep HImode, locals are promoted to SImode), so `level` probably
needs to stay HImode as well, for example as a second s16 parameter-like
value, rather than a promoted local.

## Rescore (agent 4, 2026-10-04)

lev 224 (retail 363 words, mine 359 words) with lev.py; the "about 500"
above was the old positional count.

## Rework (agent 17, 2026-10-04): lev 96

lev 96 (retail 363 words, mine 362 words), down from lev 224. Plain C, no
pins, barriers or volatile. Score: copy the .c into src/overlays/fx_common,
then `lev.py <obj> fx_common 4B6C 5AC`. The candidate needs
`FxCommonMarkerProjection` (now in fx_common_setup.h).

What fixed what:
- Blocker 1 solved. `level` is an `s16` local (locals are not promoted in
  this GCC, so it stays an HImode pseudo) and the two shifts are written
  inline at every store (`line->r = level >> 1;`, `level >> 3` for the
  dim colour), with no half/dim variables. The sign extension's `sll 16`
  is shared, loop.c hoists the shifts (non-user temporaries may be moved
  out of conditional code, user variables may not), the QImode stores let
  combine turn `sra` into `srl 17/19`, and the `level == 0` test keeps its
  own `sra` so combine folds it to a test of the `sll` value. Result:
  retail's preheader exactly, dim spilled with `sb`, stack slots i, test,
  dim, i*16 in retail order.
- Separate `line` and `quad` pointers with `line++; buffer->data = (u8
  *)line; quad = (FxCommonMarkerQuad *)buffer->data;` give retail's
  in-place `addiu s0,s0,16; move s1,s0`. The cursor union made one
  register for both and the plain `quad = line` copy let combine fold the
  increment.
- One link union per packet (`lineLink`, `leftLink`, `rightLink`): a
  single shared `address` lives across the loop, so cse makes it the
  canonical register and the packet pointers lose their in-place forms.
- `s16 x, y` copies of the screen word reproduce retail's
  `move s4,s0; srl v1,s0,16; move s3,v1; addiu s0,s0,127`.
- `u8 u`: the second label's 0x80 colours and u0/u2 share one register
  (an int `u` gave a separate QImode 0x80 for u2).
- `FxCommonMarkerProjection { s32 xy[3]; s32 depth; }`: the in-struct
  screen reads keep `lw 56/sw 8/lw 60/sw 12` in order, and the 16-byte
  block puts depth at sp+0x44 and flag at sp+0x48 as in retail.
- Prologue: reading `D_8019C9C0` directly (no `buffer` local shared with
  the loop) gives retail's la/lw order and register set.

Remaining (lev 96):
1. Global allocation order. `y` (14 refs / 73 insns = 0.575) beats the
   hoisted `level >> 1` (31 / 226 = 0.549), so half gets s3 and y s2
   (retail: half s2, y s3, about 20 words). The loop mask 0xFFFFFF
   (13 / 223 = 0.175) loses to the three induction registers (0.176 to
   0.178), so it gets fp instead of s5 and the givs shift by one (about
   20 words). A test-only extra use of `level >> 1` (a dead store) gives
   lev 72, so the order alone is worth about 24.
2. First label: `li 127` and the u1/u3 stores are scheduled before the
   colours; retail has them after the y2 store (also 3 insns of y's life).
3. Line packet: the `D_8019CC52` load is hoisted between the screen
   loads and stores (sched1 fills the load delay, then it gets v1).
   Reading it as an in-struct field (test-only alias of a 0x8019CC50
   record) gives lev 94 with equal size, but that needs a symbol alias.
4. Prologue: level in a3 and the mask in t0 (retail t0/a3), and the
   first tag's v0/v1 roles; link blocks keep `buffer` in a2 and the
   masked address in a1 (retail a1/a2).

## Allocation and alias fixes (agent 17, 2026-10-04): lev 24

lev 24 (retail 363 words, mine 363 words), down from lev 96. Plain C, no
pins, barriers, volatile, casts beyond the existing packet casts, or
aliases. Same scoring recipe as above.

What fixed what:
- Loop mask vs induction registers (about 20 words). Write the packet tag
  link as a bitfield copy, `line->tag.bits.address =
  buffer->allocation[10].bits.address;`, and the OT update as
  `buffer->allocation[10].bits.address = lineLink.word;`. The code is the
  same and/and/or, but the expansion uses the 0xFFFFFF pseudo more often
  before cse/combine fold the double masks, so flow counts 19 references
  instead of 13 and the hoisted mask is allocated before the three
  induction registers (s5, then s6/s7/fp as in retail).
- One buffer local per link block (`buffer`, `leftBuffer`,
  `rightBuffer`): a single `buffer` variable is one global pseudo across
  all three links; separate locals let local-alloc give retail's a1/a2
  (a0/a1 in the left link) roles.
- `half` vs `y` (about 20 words). The tie is decided by y's live length
  after sched1 (14 refs / 73 insns beats half 31 / 226). Setting the
  second label's `y0` after `v1` (field order otherwise:
  x0 x1 y1 x2 y2 x3 y3 u0 v0 u1 v1 y0 u2 v2 u3 v3) keeps y alive longer
  in sched1, so half takes s2 and y s3, and sched2 still emits retail's
  store order. Other natural orders (setXY4 then setUV4, per vertex,
  UV first) give lev 36..50.
- Line packet D_8019CC52 load and the missing word (about 6 words). In
  this GCC, sched true_dependence lets a fixed-address scalar read pass
  an in-struct SImode/HImode store through a pointer, but not a QImode
  one and not a store that is not in-struct. Retail keeps the load
  below the xy stores of the line (non in-struct, as the PSY-Q
  `*(long *)&p->x0 = sxy` idiom would be) while in the first label it
  passes the HImode xy stores and waits only for the QImode u1/u3 stores
  (a scalar read). `vertex = &line->xy0; *vertex++ = screen.xy[0];
  *vertex = screen.xy[1];` is a store through a plain `u32 *` without
  an address PLUS in the tree, so MEM_IN_STRUCT_P stays clear and the
  lh stays after the stores (expand_expr INDIRECT_REF marks a MEM
  in-struct only for PLUS addresses, aggregate types or &aggregate).
  The in-struct struct read of D_8019CC52 suggested earlier would fix
  the line but break the first label's placement, and there is no
  struct base: every function reads D_8019CC50 and D_8019CC52 by their
  own symbols.

Remaining (lev 24):
1. Prologue: level in a3 and the mask in t0 (retail t0/a3). Global
   alloc: level 4 refs / 30 insns beats the prologue mask 3 / 28; the
   mask needs one more counted use (none found; bitfield forms of the
   mode/OT link fold before flow). The mode tag line also has the v0/v1
   roles swapped: with `(mode->tag & 0xFF000000) | (... & 0xFFFFFF)`
   the order matches and only the registers differ (local-alloc tie
   between the allocation pointer and the tag load, lev 25), with the
   current operand order lev 24.
2. First label: `li v0,127` and the u1/u3 stores sit before the colours
   instead of after the y2 store. In sched1 the y+12 pseudo and the 127
   pseudo share v0, and their order comes from the QImode stores'
   priority 6 vs the HImode xy stores' 7 (the xy copies hang off the
   stack load's latency). Statement order alone does not move them
   (hill-climbed).

## MATCHED (agent 17, 2026-10-04): lev 0

Now src/overlays/fx_common/FxCommon_DrawEffectMarkers.c, yaml
`[0x4B6C, c, FxCommon_DrawEffectMarkers]`, `make overlay-check
OVERLAY=fx_common` OK. Plain C, no pins, barriers or volatile. The parked
candidate .c was removed (src holds the matched version).

What closed the last 24:
- Prologue (about 15 words). `FxCommonDrawModePacket.tag` is now an
  `FxCommonPacketTag`, and both prologue links are bitfield copies:
  `mode->tag.bits.address = D_8019C9C0->allocation[10].bits.address;` and
  `ot->bits.address = address.word;`. The extract mask and the insert mask
  are two separate uses of the 0xFFFFFF pseudo until combine folds them, so
  flow counts 4 references (priority 2*4/28 beats level's 2*4/30): the mask
  takes a3, level t0, and the mode tag's v0/v1 roles follow. (A redundant
  `bits.address & 0xFFFFFF` on the packed form gives the same allocation
  but keeps the v0/v1 swap.) FxCommon_DrawIntensityQuad reads the tag as
  `.tag.packed` (same code).
- First label (about 6 words) and the second label's y0 (2 words). The
  projection outputs are plain `s32 screen0, screen1, screen2, depth`
  locals, as for RotTransPers3, instead of the `FxCommonMarkerProjection`
  struct (removed, with the unused FxCommonMarkerCursor). The label's
  `xy = screen1` read is then not in-struct, so sched true_dependence lets
  it pass the in-struct SImode line link stores: its priority drops from 6
  to 1, the x/y copies and xy stores lose the load-latency priority 7 and
  sit with the colour and uv stores at priority 6, where source order
  decides. With setXY4 then setUV4 order (x0..y3, then u0..v3) in both
  labels, y+12 is computed and stored before `li 127`, both take v0, and
  sched2's anti-dependence keeps `li v0,127` and the u1/u3 stores after the
  y2 store as in retail. The half/y allocation no longer needs the moved y0.
