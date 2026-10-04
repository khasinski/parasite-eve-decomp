# room_m273 func_8019665C (0x7674, 0x37C bytes): queued drop callback

Candidate: `RoomEffect_QueuedDropCallback.c` (uses `src/overlays/room_m273/room_m273_boss.h`).

Status: mode 1 (fall, landing queue, player contact) and the whole mode 2
flash branch match. Remaining difference: in the two-ring loop of mode 2,
stock loop.c hoists the constant `1` (last argument of func_800D0E88) into a
saved register (`li s4,1` before the loop), which adds one callee-saved
register (frame save/restore, +2 words, s-register renumbering). Retail keeps
`addiu v0,zero,1` inside the loop body while hoisting the two colour
addresses.

Diagnosis (cc1 -dL): the loop has 19 real insns; all three invariants are
moved with savings 1 (lifetimes 1, 2, 1). `-ffixed-t8 -ffixed-t9` does NOT
change this, so it is not the known two-register threshold difference.
Retail would need the third move to fail, i.e. a much larger loop insn count
or a non-movable pseudo for the `1`.

Tried without success: sharing a function-scope variable for the `1` with the
mode 1 flag stores; wrapping the flash branch inside the ring loop (moves the
timer test into the loop); declaration order.

Useful findings kept in the candidate: the sine table is
`RoomM273BossTrig D_800966EC[]` with signed 16-bit bitfields (gives the bare
`lh` + `sra 5`), and a `trig` pointer to the table in the common block gives
retail's early `la D_800966EC`.
