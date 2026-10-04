# room_m350 func_80192E4C (0x3E64, 0x7C0 bytes): sweeping beam trap, parked

Yaml line: `[0x3E64, asm, func_80192E4C]` in configs/USA/overlays/room_m350.yaml.
Unique function (no masked byte copy in any overlay bin).

Candidate: `RoomEffect_SweepTrap_80192E4C.c` + `room_m350_sweep_trap.h` (move
the header to include/pe1/). Score with
`sc.sh <wt> src/overlays/room_m350/RoomEffect_SweepTrap_80192E4C.c st room_m350 3e64 7c0`.
First draft only (about 20 minutes): 2004 vs 1984 bytes. Clean C, no casts,
pins or volatile; GTE through the stock gte.h macros (ldrotmatrix,
ldtransmatrix, ldv0, rtv0tr, stsv, ldsxy0/1/2, nclip, stmac0).

Structure (all decoded, see the draft):
- mode 1: `if (D_800E27EC < 0x10) return 0; return 1;`; retail tests mode 1
  first with `bne` and falls into mode 2 via a second compare (draft emits a
  different switch tree, try if/else on mode).
- mode 2: column sweep of sprites (step vector rotated through the GTE),
  flare sprite, Math_IntSqrt distance, two func_800D0E88 fan blades,
  func_800D0728 glow, then the player quad test (four rotated corners from
  D_8019A444, nclip against the player's packed xz) that tags the player.

Known differences in the draft:
- `angle = 8; if (early) angle = phase;` must keep angle in s0 (retail uses
  `li s0,8` with `move s0,s6` in the branch).
- retail keeps `la s1, D_800966EC` for the second table read
  (`addu v0,v0,s1; lh 2(v0)`); the draft uses the absolute form and lhu.
- the parameter block stores (palette 3, parameter06 0) must sit after the
  floor load; the tpage table read happens before them in retail.
- the fan blade loop: retail moves intensity to s4 and puts the constant 1
  in s2.
- step vector initial `sw zero` of x,y is reproduced by a union word store.
