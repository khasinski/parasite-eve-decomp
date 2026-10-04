# room_m086 func_8018F004 (0x1C, 0x730 bytes): falling seeker, parked

Yaml line: `[0x1C, asm, func_8018F004]` in configs/USA/overlays/room_m086.yaml.

Candidate: `RoomEffect_M086FallingSeeker.c` plus its narrow header
`room_m086_seeker.h` (copy it to include/pe1/). Score with
`sc.sh <wt> src/overlays/room_m086/RoomEffect_M086FallingSeeker.c fs room_m086 1c 730`.

State: same size, 7 real diffs, all in the mode 2 / state 2 flare draw
(offsets 0x660..0x680 into the function). Retail loads D_800E27EC (for
`rotation.z = frame << 7`) right after the parameter02 store and before the
`li 0x10` for parameter00/extent_x/extent_y, so 0x10 lands in v1. Here the
single-set load launches late (after the 0x10 stores, 0x10 in v0). A
multi-set temporary (`bounce = D_800E27EC`) hoists it to the very top
(19 diffs); every placement of the rotation statements among the stores
gives 7; reading the counter as a one-field record does not help (two
different symbols never conflict).

Everything else matches, including:
- `seeker->timer * 256` for the model rotation (keeps lh; `<< 8` gives lhu).
- The ignored `func_80077DC4(timer << 5)` in state 2 and the constant 1
  page argument (retail reuses s0 = 1 from the mode compare).
- `index = &D_800E11EA` local pointer in state 0, `D_800E11E4[2]` for the
  state 1 tile index (keeps the load below the base-register store).

Cleanup before landing: replace the two `(GteRotation *)&rotation` casts with
a GteShortVector/GteRotation union local.
