# room_m273 func_80198E94 (0x9EAC, 0x55C bytes): ground ring callback, parked

Yaml line: `[0x9EAC, asm, func_80198E94]` in configs/USA/overlays/room_m273.yaml
(pool callback created by RoomEffect_PairedEmitter).

Candidate: `RoomEffect_GroundRingCallback.c` (uses
`src/overlays/room_m273/room_m273_boss.h`, which already carries the needed
declarations: `D_8019AF04.model`, `D_8019AE1C`, the model draw helpers).
Copy it to src/overlays/room_m273/ (fix the include) and score with
`sc.sh <wt> src/overlays/room_m273/RoomEffect_GroundRingCallback.c gr room_m273 9EAC 55C`.

State: same size (0x55C), 25 real diffs, all register allocation or
scheduling; control flow, calls, arguments and the frame (one unused 8-byte
local) match. No pins, barriers, casts or volatile.

Found on the way:
- The flash clut `GetClut(0x10, D_800E120A)` is the palette read-back form
  (`kind = D_800F3368.palette; palette = D_800E1204[kind]; if (kind == 4 &&
  D_800F3428) palette += 4;`), which also gives the `la s0, D_800F3368` base
  used by the later `lh 2(s0)` parameter02 read.
- The model clut must be a conditional expression inside the call
  (`(kind == 4 && D_800F3428) ? palette + 6 : palette + 2`); the if/else form
  costs 400 diffs of schedule.
- `D_8019AFA0` is `D_8019AF74.cooldown`, `D_8019AF6C` is `D_8019AF04.model`
  (in-struct accesses give retail's la base registers).

Remaining:
- mode 1: FIXED in rooms7 (see below).
- mode 2 ring loop: retail has ring size in s1, 0x1000 in s2, &D_8019AE1C in
  s3; ours rotates those three.
- the first parameter block store (`sh 0x20, 0(s0)`) is one slot later.
- A darwine permuter run (scratch/a5ring2, 13k iterations) found nothing
  usable.

Retry (agent 5, rooms7): 20 diffs, mode 1 now byte-identical. The radius is
a block-local `int radius = D_800966EC[...].sine / 16 + 0x80;` in one
statement. expand_divmod always copies the dividend into a fresh pseudo
(`t1`) for the bgez adjust; cse then keeps whichever register of the
equivalence class lives longest as the canonical one. With the
function-scope `i` (also used by the mode 2 loops) the dividend stayed
canonical, the adjust became `t1 = i + 15` and t1 conflicted with i (a2 vs
s0). With a short-lived temporary dividend, t1 is canonical, the compare and
the adjust use t1 in place, the copy dies and global alloc puts everything
in s0 (the compare result included). Splitting the statement
(`radius = sine; radius = radius / 16 + 0x80;`) breaks the dx/dz schedule.

Left (align2, s-registers normalised): `size = value >> 2` is scheduled into
the D_800E11FA load slot instead of the bne delay slot (moving the statement
anywhere in the block changes nothing), the first parameter block store one
slot later, plus the ring loop s1/s2/s3 rotation.

Retry (agent 5, near-miss pass, 2026-10-04): 4 real diffs. The include is
now `"room_m273_boss.h"`; copy the file to src/overlays/room_m273/ and score
with `sc.sh <wt> src/overlays/room_m273/RoomEffect_GroundRingCallback.c gr
room_m273 9EAC 55C`.
What fixed the register rotation (all plain C):
- The model phase index is the function-scope `i`
  (`i = D_800E27EC - 5; if (i < 8) value = ...[(i << 7) & 0xF80].sine;`)
  instead of reusing `value`, and the ring loop size reuses `value`
  (`value = sine / 16; ... value += 0x100;`) instead of `size`. `size` now
  only carries the model scale and the flash size (retail s3), `value`
  carries the model wave and the ring size (s1), `i` the index, the page
  and the ring counter (s0). With `size` shared by all three, the scale
  shift was scheduled early (multi-set) and the ring constants rotated.
- The duplicated extent stores are kept (retail stores extent_x/extent_y
  twice); the tpage statement is a plain
  `D_800F3368.tpage = D_800E2850[D_800E11EA];` after them.
Left (4 words): retail stores parameter00 through `la s0, D_800F3368`
before the D_800E11EA load; sched1 here has the same order, but sched2
hoists the load above the store (the base register's known value is
D_800F3368, so the two refs never conflict). Retail must have had a
dependence there (a base pseudo without a single known value). A block
around the tpage read, a block-local index and an index read before the
stores do not change it.
