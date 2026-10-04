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
- mode 1: retail divides the radius in place (`lh s0; bgez s0; addiu
  s0,s0,15; sra s0,s0,4`) and puts the compare result in s0; ours adjusts in
  a copy (expand_divmod refuses target == op0). Tried `i /= 16`, a second
  variable, a block local; none.
- mode 2 ring loop: retail has ring size in s1, 0x1000 in s2, &D_8019AE1C in
  s3; ours rotates those three.
- the first parameter block store (`sh 0x20, 0(s0)`) is one slot later.
- A darwine permuter run (scratch/a5ring2, 13k iterations) found nothing
  usable.
