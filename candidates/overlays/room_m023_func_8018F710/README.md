# room_m023 func_8018F710 (0x728, 0x504 bytes): joint glow callback, parked

Yaml line: `[0x728, asm, func_8018F710]` in configs/USA/overlays/room_m023.yaml.

Candidate: `RoomEffect_JointGlow.c` (types and externs are already in
include/pe1/room_m023_effects.h on this branch: `RoomM023JointGlow`,
`D_8018EFFC`, `D_8018F000`, `D_800E11EA`, the sound helpers). Copy it to
src/overlays/room_m023/ and score with
`sc.sh <wt> src/overlays/room_m023/RoomEffect_JointGlow.c jg room_m023 728 504`.

State: same size, 18 real diffs, all inside the parameter block that
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
