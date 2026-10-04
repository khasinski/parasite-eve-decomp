# scene_e08 func_80191E78 (0x2E90, 0x350 bytes): particle slots draw

Parked at 78 real diffs (sc.sh); the two loops match apart from register
choice.

Files: `Scene_DrawParticleSlots_80191E78.c` (instance), `scene_particle_slot_draw.h`
(include/pe1/), and `scene_particle_slots.h.patch` (offset[] becomes
GteShortVector so the slot offset can be struct-copied and passed to
func_800CEE20).

Remaining:
1. Prologue block: retail emits the nine D_800F3368 parameter stores after
   the three stack vector copies and the D_800E2850[D_800E11EA] load; stock
   sched1 hoists them above the copies (struct-field or scalar-global form,
   stores written before or after the copies in source: same result).
2. Second loop: retail hoists both `&D_800E1204` (s7) and the constant 4
   (s6) out of the loop and keeps a separate `li v1, 4` for the
   parameter02 store. Without help only the table base is hoisted; with
   `special = 4;` inside the loop both are hoisted but s6/s7 are swapped and
   the 0x40/4 constant sharing differs.
`offset.z <<= 1` gives retail's lhu/sll (the `*= 2` form gives lh).

## Retry (agent 5, 2026-10-04, near-miss pass 3)

Reading the page index as a one-field record (`SceneParticlePageIndex
D_800E11EA`, `.index`) does not change the score (78). With the record,
the parameter00, parameter02, extent_x, extent_y, direct tpage head order
and all 24 orders of the palette/parameter06/parameter0A/depth tail give
74..76; also permuting the position/offset/color copies against the
parameter block (both orders) bottoms out at 72. The parameter stores
still schedule above the stack copies (symbol vs stack slot never
conflict), so the prologue needs another idea.

## Rescore (agent 13, 2026-10-04): lev 61

`lev.py <obj> scene_e08 0x2E90 0x350 -v 70` gives lev 61 (212 words each).
The edits are the two issues above: the nine parameter stores scheduled
above the stack copies (about 40 edits) and the swapped s6/s7 loop
constants of the second loop. Raw-byte search found no copy of this
function in other overlays. Not retried beyond the rescore.

## Retry (agent 13, round 2, 2026-10-04): lev 22

- Writing `D_800F3368.tpage = D_800E2850[D_800E11EA];` FIRST, before the
  constant stores (still after the three stack copies), keeps the parameter
  block below the copies: lev 61 -> 23. The long tpage load chain gets the
  highest priority in the backward sched1 and drags the store block down
  with it; with tpage last, the constant stores fill the D_8019956C load
  delays at the top instead.
- Brute force over the 720 orders of the constant stores (0x40 trio kept
  together): best lev 22 with palette, parameter02, parameter06, depth,
  0x40 trio, parameter0A (the candidate). Chained `= 0x40` assignments and a
  shared `scale = 0x40` local do not move retail's early `li v1,64`.
- Pointer locals do not help here: a multi-set `GteShortVector *source` for
  the copies lets every parameter store rise to just after the first call
  (lev 65). Symbol and stack addresses never conflict, so the order is pure
  priority.
- Left: the early shared 0x40 register, and the second loop's s6/s7 swap.

## Round 3 (agent 13, 2026-10-04): lev 18

- Loop constants fixed: a `u16 *palettes = D_800E1204;` set just before the
  second parameter block (and `palettes[kind]` in the second loop) makes the
  table base live longer than the hoisted `special = 4`, so global alloc
  gives 4 s6 and the base s7 as in retail (lev 22 -> 18). Placing `special`
  before the loop instead, or `palette += special`, also fixes the swap but
  costs elsewhere.
- Remaining: retail's shared constants are loaded early and not right
  before their stores (`li v1,64` in block 1, `li v1,4` for parameter02 in
  block 2), which is what sched1 does for a pseudo that is not "birthing"
  (set more than once). A multi-set local (`size` or `kind` assigned 0x40 /
  4) loses the birthing priority but then floats to the very top of the
  block instead of retail's middle position: `kind = 4;` stored into
  parameter02 gives lev 16 (not kept, semantically odd), `size = 0x40` in
  both blocks lev 24, `size` in block 1 only lev 22.

## Round 4 (agent 16, 2026-10-04): lev 16

- `kind = 4; D_800F3368.parameter02 = kind;` in the second block (kind is
  multi-set, so the 4 is not a birthing insn) gives lev 16: the second
  block's `li v1,4` then sits early like retail's, but above the loop
  invariants instead of below them, and the block's 0x40 loses the branch
  delay slot. All 240 orders of the second block (with and without `kind`,
  chained or separate 0x40 stores) stay at 16 or worse.
- The page index is now a one-field record (`SceneParticlePageIndex`, in
  the header). It does not change the score yet: the stack copies are
  `movstrsi_internal` insns with frame-pointer plus constant addresses, so
  they never conflict with a symbol load either way.
- Why the first block's `li v1,64` cannot be reproduced by statement order:
  sched1 gives a single-set constant the launch priority right before its
  first store; a multi-set one keeps priority 1 and is picked after every
  priority-2 insn (stores, stack copies, tpage chain), so it lands at the
  top. Retail's position (between the index load and the `sll`) needs
  priority 2 with a LUID above the color copy, i.e. a constant that depends
  on something with latency. Reusing scale/kind/palette/special for the
  0x40 (88 variants, with and without a prior load into the same variable)
  gives 16 at best.

## Round 5 (agent 17, 2026-10-04): still lev 16

Diagnosis with the -dS/-dR dumps (no source change kept):
- Block 1. The tpage table load `D_800E2850[index]` has an address
  `(plus reg symbol)`, which memrefs_conflict_p cannot separate from the
  frame BLK copies, so it stays below all three stack copies. The index
  load itself is a plain symbol and floats. In sched1 the sll is launched,
  the index load (latency 2) is queued, and the color copy (priority 2,
  ready once the tpage load is scheduled) fills the load delay, so the
  index load lands above the copy. Retail's filler is `li v1,64`, which
  also explains its registers: with 64 born between the index load and the
  sll, local-alloc gives index v0, 64 v1 (2*4/len), 1000 v0 and the tpage
  a0, exactly retail.
- For li 64 to be that filler it must be ready at that point and win over
  the copy: priority >= 2 and not launched earlier. A single-set constant is
  launched right before its first store (too late). Reusing one local for
  the D_8019956C (or D_8019957C) load and then the 0x40 (`value = ...;
  offset.x = value; ... value = 0x40;`) gives the 0x40 set an anti
  dependence on the priority-2 stack store, so it is placed exactly as in
  retail, but the two-death pseudo goes to global alloc (a0): lev 22/23.
- Block 2. Reorg fills the first loop's delay slot with the first eligible
  insn of the fall-through (it skips the s-register moves and the 2-word
  `la`), so retail's sched2 order has li 64 before li 4. Retail's registers
  (64 v0, 4 v1 alive across the 0x40 stores, 5 v0) need sched1 order
  li 64 (top, not launched), moves, li 4, three 0x40 stores, li 5,
  parameter02/depth/0A stores. A launched 4 lands after the 0x40 stores
  and shares v0 with 64 (lev 18 form); `kind = 4` (multi-set) sits above
  the hoisted moves (lev 16). Moving `kind/special/scale/palette = 4`
  across all positions of the block: 16 at best.

## Round 6 (agent 19, 2026-10-04): lev 15

`Scene_DrawParticleSlots_80191E78_shared_loads.c`: the parameter block is
written in retail's store order (palette, parameter02, parameter06, the
0x40 trio, depth, parameter0A, then tpage last from a temporary read
before the stores), and two function-scope temporaries carry the stack
vector loads into the block: `palette = D_8019956C; offset.x = palette;`
... `palette = D_800E2850[D_800E11EA.index];` and `value = D_8019957C;
offset.z = value;` ... `value = 0x40;`. The 0x40 is then multi-set (no
launch) and anti-dependent on the offset.z store, so `li v1,64` lands
between the index load and the `sll` exactly as in retail, and the two
vector loads keep retail's order.
- Remaining (lev 15): the D_8019956C load and the tpage value sit in a1
  (palette is global; retail v0 and a0), the colour copy uses a1/v0
  (retail a2/v1), and the second block's `li v0,64` / `li v1,4` order.
- Same shape with other temporaries (kind, scale, special, a block-local
  tpage) for the two loads: lev 16 to 52. Moving the colour copy or the
  0x40 set relative to the tpage read: lev 15 to 46.
- The shared temporaries are steering-grade reuse; this is a direction for
  the register map, not a candidate to land as is.
