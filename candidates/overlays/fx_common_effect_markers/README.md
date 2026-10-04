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
