# Entity_UpdateAndRender (main 0xB640, 0x7BC bytes): parked

Typed rewrite of the floor-tracking driver (BattleEntity, CollisionFace,
CollisionPlane, FloorEdgePoint; no byte offsets, no casts to integers).
To build it, apply `headers.patch` (field_collision.h types, battle.h
`stepHeight`, PSY-Q `gte_ldsxy3` in gte.h, Entity_SlideOnRamp and
Entity_FindFloor adapted to the new declarations; `make check` stayed OK
with the patch applied) and copy `entity_floor.h` to include/pe1/.

## State (2026-10-04, second pass): 182 diff lines with -G8 cc1 / -G4 maspsx

The split is approved now and the draft uses it. With it, the declarations
in `headers.patch`/`entity_floor.h` give the correct absolute/gp mix: the
two edge words are separate 4-byte symbols (D_8009CE0C, D_8009CE10) so
they stay gp-relative at -G4, and the player slot and flags word are
8-byte records (small for cc1, absolute for the assembler). Every
lui/gp access now matches. Remaining diffs (ds.py: 182) are allocation
and control flow:

1. Frame: mine reserves 40 bytes of locals, retail 32. The -dg dump shows
   reload spilling t4/t5 and HI/LO for the gte_ldsxy3 operands. Retail
   loads the two edge words into t5/t6, which means t4 was taken by a
   global pseudo (retail keeps (s16)maxX in t4 through the box tests).
2. Edge box tests: retail reuses sign-extended xs (a3), (s16)maxX (t4),
   (s16)minX (t2, first extended in the second test) and zs (a0, from the
   third test) along the fall-through path, but recomputes `xs - r`,
   `xs + r` and every compare. Mine CSEs a whole compare (`slt t3` kept
   and branched on later). Variants tried: locals per bound, globals used
   directly, int copies of x/maxX/minX, a shared `left = x - r` (permuter
   hint, -1 line).
3. Entry: retail loads x/z with lh and copies them to s4/s3; mine loads
   lhu into s4/s3 and extends separately.
4. The clear loops after the slide swap a0/v1 between counter and pointer.
   In retail, the entry test of the third loop compares the clip result
   register (known zero) against the count. A separate `next` variable is
   optimised away.

Already matched in this draft: the -G mix, the plane branch (a local
`planes` for the first index, then re-reads), the flat-mode branch
(`if (delta < stepHeight << 16) posY = ...; else rollback`), and the
unsigned loop counters.

Control flow: the draft still has gotos (leave/slide/found/blocked).
Retail jumps from the end of the edge test straight into the slide code
that the failed first clip also reaches, and that code contains loops, so
jump2 cross-jumping cannot merge two copies of it (it stops at loop
labels). Two goto-free versions were tried:
`Entity_UpdateAndRender_nogoto.c` (static inline edge test plus a
`found` flag, 372 lines) and a flag-only version (346). Both are worse,
because the inlined `return 0/1` and the flag tests are not threaded
away. If it lands with gotos, log them as debt.

Permuter setup (darwine): base.c has the GTE asm replaced by dummy calls
gte_ldsxy3_/gte_nclip_/gte_stmac0_/gte_ldsxy2_, and compile.sh rewrites
them back with perl before cc1 -G8 and maspsx -G4 (scratch/a5eur2). Two
hours of runs found only volatile-local frame tricks (rejected).

## Earlier analysis (first pass)

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
