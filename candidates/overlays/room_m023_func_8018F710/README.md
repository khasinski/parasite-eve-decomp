# room_m023 func_8018F710 (0x728, 0x504 bytes): joint glow callback, parked

Yaml line: `[0x728, asm, func_8018F710]` in configs/USA/overlays/room_m023.yaml.

Candidate: `RoomEffect_JointGlow.c` (types and externs are already in
include/pe1/room_m023_effects.h on this branch: `RoomM023JointGlow`,
`D_8018EFFC`, `D_8018F000`, `D_800E11EA`, the sound helpers). Copy it to
src/overlays/room_m023/ and score with
`sc.sh <wt> src/overlays/room_m023/RoomEffect_JointGlow.c jg room_m023 728 504`.

State (rooms4): same size, 18 real diffs, all inside the parameter block that
follows the state switch (register choice and the placement of the
D_800E27EC load). Everything else, including the frame layout, the
expand-div division and both clut read-backs, matches.

What got it this far (worth reusing):
- The tpage array load `D_800E2850[index]` is in_struct (array) with a
  varying address, so sched keeps it below every earlier D_800F3368 field
  store and above every later one. Retail's order (00, 0E, 10, 02 stores,
  then the tpage load, then 04, 06, 0A, 0C) means the tpage statement sits
  right after parameter02 in the source; the tpage store itself sinks.
- The D_800E11EA index load only moves to the top of the block (as retail)
  when it is a multi-set variable (function-scope `kind`, also used for the
  palette read-back); a single-set temporary gets launch priority and is
  placed right before its use.

Remaining: retail keeps the index in a0, 0x40 in v1 and loads D_800E27EC
into v1 right after the three 0x40 stores. A single-set `D_800E27EC & 1`
launches too late; reading it into the multi-set `palette` loads it too
early (first in the block).

No pins, barriers, casts or volatile.

Retry (agent 5, rooms4): dropping the multi-set `palette = D_800E27EC` and
testing `if (D_800E27EC & 1)` directly fixes the store order (04, 06, 0A,
0C, then the tpage store last, as retail) but the load still launches after
the tpage lookup and kind stays in v1 (22 diffs). `palette = D_800E27EC & 1`
moves kind to a0 as retail but loads the counter first in the block (26).
Retail's allocation (kind a0, 0x40 v1, then D_800E27EC reloaded into v1 by
sched2 right after the last 0x40 store) is still not reproduced; 18 diffs
remains the best.

Retry (agent 5, rooms5): 10 diffs. Store order parameter00, extent_x,
parameter02, extent_y with the counter tested directly
(`if (D_800E27EC & 1)`) fixes the counter load and the 0x40 register. The
rest is register choice only: retail shifts the tile index in place
(`sll a0,a0,1`) and keeps the read-back palette kind in v1, here kind lands
in a0 in both clut blocks. Splitting `kind` (block-local kinds, a direct
`D_800E2850[D_800E11EA]`, a block-local index) all go to 30.

Retry (agent 5, rooms6): 4 diffs. Reading the tile index into the multi-set
`palette` (`palette = D_800E11EA; D_800F3368.tpage = D_800E2850[palette];`)
and keeping the function-scope `kind` only for the two clut read-backs puts
kind in v1 in both clut blocks, as retail. Left: the index lands in a1 and
is shifted into v0 (`sll v0,a1,1`), retail loads it into a0 and shifts in
place (`sll a0,a0,1`) before the tpage load reuses a0. Declaration order,
loading the tpage value back into the same variable (6), the conditional
inside the clut call (5) and a 33k-iteration permuter run (base 150, no
improvement) did not fix it.

Retry (agent 5, rooms7): 2 diffs. A function-scope `int tile` set twice
(`tile = D_800E11EA; tile = D_800E2850[tile]; D_800F3368.tpage = tile;`)
is a multi-set pseudo, so sched1 keeps the load at the top of the block and
global alloc gives it a0 for both values. Left: the shift result (a
separate single-set local pseudo) gets v0 (`sll v0,a0,1`), retail ties it to
the index (`sll a0,a0,1`).

Why (cc1 -dl/-dc/-df): local-alloc only ties the shift result to the index
when the index is a local quantity (one block, exactly one death). A
single-set block-local index does tie (`sll v1,v1,1`), but sched1 then treats
its load as a birthing insn (max priority, placed right before the shift)
and the block falls to 30 diffs. Making the index multi-set inside one life
does not survive: `index &= 0xFFFF` / `index = (u16)index` are folded by
combine, which decrements reg_n_sets again and leaves a USE insn at the
block label with a second REG_DEAD (global again, 2 diffs, same shape).
Retail therefore had a local single-death index that sched1 did not treat as
birthing; no plain-C form found for that. Permuter (darwine scratch/a5jg2,
packed attribute replaced by `u8 word[8]` for pycparser, base 130) found
nothing better in 36k iterations.
