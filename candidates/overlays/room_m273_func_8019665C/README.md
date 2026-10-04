# room_m273 func_8019665C (0x7674, 0x37C bytes): queued drop callback

Yaml line: `[0x7674, asm, func_8019665C]` in configs/USA/overlays/room_m273.yaml.

Candidate: `RoomEffect_QueuedDropCallback.c` (uses `src/overlays/room_m273/room_m273_boss.h`).

Status: 41 word diffs, all from one s0 <-> s1 swap. Every instruction
otherwise equals retail.

- Loop hoisting is solved: `(s16)shade` as the intensity argument adds a
  sign-extension movable that loop.c hoists before the trailing `1`. That
  drops the threshold below the loop's insn count, so `li 1` stays in the loop
  as in retail. Combine then removes the extension. See
  candidates/LOOP_HOIST.md.
- `GteShortVector unused;` gives retail's 0x60 frame.
- Remaining: retail allocates the loop index (shared s0 with the flash
  `size`) before `drop`. Global-alloc priorities (from `-dl`): drop 31 refs /
  126 insns = 0.98, i 7 / 21 = 0.67, i shared with size 9 / 35 = 0.77. Only
  loop-depth weighting lifts i, for example a banned `do { } while (0)`
  around mode 2 (permuter score 45). A mode-2 copy of `drop` adds a move.

Still to clean up before landing: the pointer casts on D_8019AB68/D_8019AD5C
and the implicit u8[] -> RenderColor * arguments (retype the externs in
room_m273_boss.h).
