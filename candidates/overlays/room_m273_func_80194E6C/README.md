# room_m273 func_80194E6C (0x5E84, 0x77C bytes): falling trail callback, parked

Yaml line: `[0x5E84, asm, func_80194E6C]` in configs/USA/overlays/room_m273.yaml
(pool callback created by func_801955E8 with 100-byte records, ten of them).

Candidate: `RoomEffect_FallingTrail.c`. The record type
`RoomM273FallingTrail`, the boss state fields it writes (`hit`, `hit_flag`,
`landing_x/y/z`, `landing_count`) and the player fields (`mode`,
`transforms`) are already in src/overlays/room_m273/room_m273_boss.h on this
branch. Copy the file to src/overlays/room_m273/, restore the include and
score with `sc.sh <wt> src/overlays/room_m273/RoomEffect_FallingTrail.c ft room_m273 5E84 77C`.

State: the body is 2 instructions longer than retail (475 vs 473), frame
0xA8 vs 0xA0 (one extra saved register, s8). Control flow, calls, GTE
sequence (RotMatrixYXZ, gte_ldrotmatrix/ldtransmatrix, gte_ldv0 through t1,
gte_rtv0tr_mac, gte_stsv) and all argument setup match.

Remaining diffs:
1. Draw loop (mode 2): retail sign-extends `intensity` for the eighth
   func_800CEE20 argument inside the inner two-sided loop, while
   `(s16)scale` is computed once per outer iteration. Stock loop.c hoists
   both (`-dL`: insns for scale savings 2 life 15, intensity savings 2
   life 2, both moved). Lowering the threshold with -ffixed-t0..t9 does not
   keep intensity in the loop (it needs threshold < 14), so this is not the
   known two-register threshold gap; retail's source must make the value
   loop-variant or not a movable. Tried: u16 intensity (single andi stays in
   the loop but zero-extends), int/s16 combinations, a per-draw
   `s16 alpha = intensity` copy (cse folds it). The hoist costs the extra
   saved register.
2. Spin update: FIXED in rooms7 (see below).
3. First parameter block: retail loads D_800E11EA first and stores in
   source order (00, 02, 0E, 10, 04, 06, 08); mine groups the three 0x20
   stores. Second block: retail stores parameter02 first.
4. Translation copy loop and the hit copy loop use a2 instead of a0 for the
   source pointer in one of them.

No pins, barriers, casts or volatile.

Retry (agent 5, rooms7): spin update fixed, body now 1 instruction longer
than retail (1920 vs 1916 bytes, still the extra s8 save). Writing the
increment as `trail->spin += trail->spin < 0 ? -2 : 2;` (or an int temporary
read once, `spin < 0 ? spin - 2 : spin + 2`) gives retail's `bgez v1` with
`addiu v0,v1,-2` / `addiu v0,v1,2` arms and the yaw store in the delay
slot; the original `trail->spin = trail->spin < 0 ? trail->spin - 2 :
trail->spin + 2` and the if/else forms copy the value first (`move v0,v1`).
Items 1, 3 and 4 are unchanged.

Retry (agent 5, near-miss pass, 2026-10-04): same size as retail, 19 real
diffs, all in the second parameter block (mode 2 after the flare). The
include is now `"room_m273_boss.h"`; copy the file to src/overlays/room_m273/
and score with `sc.sh <wt> src/overlays/room_m273/RoomEffect_FallingTrail.c
ft room_m273 5E84 77C`.
What fixed the rest (plain C, see candidates/LOOP_HOIST.md):
- Draw loop: index the trail arrays as `trail_x[index * 2 + side]` instead
  of a `base` computed per outer iteration. The in-loop index arithmetic
  is hoisted first (3 moves, threshold 29 -> 20) and raises the count to
  58, so D_800E1204 stays an absolute `lui at` access and the intensity
  extension stays in the inner loop (`not desirable`), while the scale
  extension is still hoisted. Retail's `move s5,v0` copy is the hoisted
  index giv.
- `step++, index = (index + 1) & 7` in the for increment gives retail's
  outer loop tail (andi in the bnez delay slot).
- `i = trail->head = (trail->head - 1) & 7;` gives the lhu head read and
  the `move a1,v0` copy.
- Hit copy loop: `source = &trail->position.x;` before the loop and
  `source[i]` inside (a giv, so its init follows the hit pointer init).
- First parameter block: block-local `int index = D_800E11EA;` as the first
  statement and the tpage store right after extent_y; this matches retail
  exactly (load first, `sll a0,a0,1` in place, value in v1).
Left: second parameter block. Retail loads 0x10 into v1 and 2 (palette)
into v0 early, keeps v1 busy until the extent stores, so the D_800E27EC
load sits after them and the tile index is shifted in place
(`sll a0,a0,1`, value back in a0). Here 0x10 and 1/2 share v0, the counter
load moves up and the shift goes to v0. Tried: multi-set tile (best, 19),
single-set tile in several positions, palette store order variants (14 to
26 but wrong store order), a function-scope kind (32), a multi-set
variable holding 2 (23). A darwine permuter run (scratch/a5ft, 50k
iterations from 530) found nothing.
