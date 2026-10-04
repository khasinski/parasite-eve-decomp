# Entity_UpdateAndRender (main 0xB640, 0x7BC bytes): parked

Typed rewrite of the floor-tracking driver (BattleEntity, CollisionFace,
CollisionPlane, FloorEdgePoint; no byte offsets, no casts to integers).
To build it, apply `headers.patch` (field_collision.h types, battle.h
`stepHeight`, PSY-Q `gte_ldsxy3` in gte.h, Entity_SlideOnRamp and
Entity_FindFloor adapted to the new declarations; `make check` stayed OK
with the patch applied) and copy `entity_floor.h` to include/pe1/.

## Why it is parked: needs the unapproved -G8 cc1 / -G4 assembler split

Retail mixes gp-relative and absolute accesses that no single -G value
reproduces with stock cc1 + maspsx:

- gp-relative: D_8009CE0C/CE10 (packed edge words, 4 bytes each),
  D_8009CE1C..CE2C (u16), D_8009CE18, D_8009D1FC, D_8009D1D8, D_8009CE08.
- absolute lui/lo: D_8009D254 (player slot, read) and D_8009D2E8 (field
  flags, read-modify-write `lw; and; sw` through `lui v0` / `lui at`).

With cc1 -G8, a symbol only gets the direct `lw $2,sym` / `sw $2,sym` macro
form when cc1 itself considers it small (size <= 8). Any larger
declaration (16-byte record, unknown-size array) makes explow.c force the
constant address into a register, and cse shares it between the load and
the store of `D_8009D2E8 &= ~8`, giving `la; lw 0(v0); sw 0(v0)` instead of
retail's lui v0/lw + lui at/sw (verified on a 4-line test file at G0/G2/G8:
every non-small declaration produces `la`). A <= 8 byte declaration is
direct in cc1 but then GNU as with -G8 makes it gp-relative. So retail is
"small for cc1, not small for the assembler": exactly the -G8/-G4 split
(Entity_FrameUpdate, commit 42c2161ba on decomp-agent5-mainold), with the
edge words declared as two 4-byte symbols so they stay gp-relative at -G4.
The only other way is the read/write alias pair used by Entity_SlideOnRamp
(`rampFlagsRead/rampFlagsWrite asm("D_8009D2E8")`), which is debt.

Current state at -G8/-G8 with 16-byte records: 211 diff lines (ds.py),
dominated by the `la` forms above and by control flow, because the draft
still uses gotos (leave/slide/found/blocked). Before un-parking, rewrite
the control flow without goto: duplicate the rollback block and the flag
clear where retail branches to them (jump2 cross-jumping merges identical
tails), and drive the ramp-slide entry with a flag.

Other findings:
- the nclip pair is PSY-Q `gte_ldsxy3(sxy0, sxy1, sxy2); gte_nclip();
  gte_stmac0(&a)` then `gte_ldsxy2(old); gte_nclip(); gte_stmac0(&b)`
  (retail loads the two edge words into t5/t6 before the three mtc2).
- `D_8009CE2C = radius; D_8009CE2C = radius * moveSpeed / 4096;` (two stores).
- the first bounding-box block compares `maxZ < z` (no radius) while the
  others use `z - radius`: keep it, it is in retail.
- the flat-mode branch falls through to the rollback when the height step
  exceeds `stepHeight << 16` (u16 at +0x10).
