# scene_e20 func_8018F028 — candidate, not matched

`RoomEffect_FlareParticle_8018F028.c` has linked score **30**, six register
instruction differences and the correct 1832-byte size with stock native GCC
2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and stock MASPSX 2.56 mode.
It is not byte-identical. The build therefore continues to use the original ASM.

The earlier score-zero promotion used `gte_ldrotmatrix` and
`gte_ldtransmatrix`, whose bodies include CPU `lw` instructions. That result
violates the source constraints and is withdrawn. This candidate performs all
matrix loads in C and uses individual `gte_ctc2_0` through `gte_ctc2_7` macros.
The particle and floor-height types remain shared with the spawning controller
through `include/pe1/scene_e20_flare.h`.

## Remaining differences

| Address | Retail | Candidate |
|---|---|---|
| 8018F1C0 / 8018F1C4 | division high result in t1 | t0 |
| 8018F5BC / 8018F5C0 | glow product in t0 | t1 |
| 8018F6F8 / 8018F6FC | fade division high result in t0 | t1 |

Seven pins and four empty barriers currently reproduce both matrix transfer
blocks. GCC's reload pass then chooses t1 as its scratch register instead of t0.
Without the pointer constraint, arithmetic matches but the matrix pointers use
v0 instead of t0. Explicit reciprocal arithmetic and pins on the final products
added spills or changed scheduling and were not retained.

All 1023 nonempty subsets of the original ten pins were checked on darwine.
The two `matrixSlot` pins and the `streakKind` pin can be removed together:
the resulting linked function is byte-identical to the score-30 candidate
(not to retail). The remaining pins are the six matrix-word pins and
`specialKind` in v1.

Further bounded experiments covered pins on the actual arithmetic operands
and results, empty allocation guards, and reserving unused registers. The
best guard variant scored 20, but only moved the mismatch: the glow and fade
results used t2 instead of t0. It was not retained. Reserving more registers
introduced spills or changed other register assignments. No score-zero
candidate resulted from these trials; extra pins alone are not yet a fix.

A bounded run on darwine completed 32,443 permutations on 2026-10-05, including
5,447 rejected compilations, with no improvement over score 30. No worker from
that run remains active. Compiler and assembler sources were not modified.
Research artifacts: `scratch/scene_e20_8018F028` locally and
`/home/hasik/fx-search-archives/scene_e20_8018F028` on darwine.
