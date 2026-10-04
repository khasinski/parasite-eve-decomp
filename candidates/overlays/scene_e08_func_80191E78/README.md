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
