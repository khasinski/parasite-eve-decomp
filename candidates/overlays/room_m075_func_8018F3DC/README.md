# room_m075 func_8018F3DC (0x3F4, 0x338 bytes; same code in room_m080 and room_m082): motion particle init

Yaml line: `[0x3F4, asm, func_8018F3DC]` in configs/USA/overlays/room_m075.yaml
(also in room_m080.yaml and room_m082.yaml).

Candidate: `RoomLib_InitMotionParticles.inc` + `room_motion_init.h` (move the header to
include/pe1/, then add an instance `#define FUNC func_8018F3DC` +
`#include "../room_lib/RoomLib_InitMotionParticles.inc"`). Plain C: no casts, no pins,
no volatile.

Status: 38 word diffs, one cause. The prologue, the three frame copies, the
particle loop (registers included) and all glyph stores except one match.
`D_80194618[2].y = 32` is scheduled as the first glyph store, followed at once
by `li v0,-31`. Retail keeps it after `D_80194618[2].x = 0`.

The loop-hoisting blocker that parked this function is gone: see
candidates/LOOP_HOIST.md. With `particle = &table[i]` stock loop.c keeps -0x40,
`li 4` and `la D_80194370` inside the loop, as retail does.
