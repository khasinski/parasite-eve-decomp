# func_800D3BC8 — pulled sprite, matched (2026-10-05)

Promoted to `src/main/engine/FieldEng_PulledSprite.c`, with declarations in
`include/pe1/field_pulled_sprite.h`, and enabled in the main manifest.
It draws a textured sprite at a projected point, optionally pulling it toward
screen centre with LoadAverageShort12 and forcing its ordering depth to 14.

Stock native GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and
stock MASPSX (2.56 mode) produce decomp.me-compatible Levenshtein score **0**
and **924/924 identical linked bytes**, verified on darwine.
Target main offset `0xC43C8`, address `0x800D3BC8`, size `0x39C`.
Target SHA-256: `cfe7f40ac4679a04f95a435a55759c2bf8851a2954656315881b8a87db98e6b8`.
The final formatted source was independently rebuilt as `promoted`, score 0.
The integrated `make -j32 verify` passed: the complete `main.exe` matches
retail. Source-policy, source-mapping and debt checks passed. Expanded ASM
templates contain only individual GTE instructions and nops.

The former prologue mismatch is resolved: the five stack arguments have
volatile top-level qualifiers and are copied once, after an empty memory
barrier, in retail order (pull, clut, page, intensity, color). This lets GCC
save all registers before loading those arguments. The qualifiers affect
local parameter access, not the caller ABI or the pointed-to color data.
A second explicitly volatile barrier clobbers a2 before the color arithmetic,
producing the original intensity/channel allocation. A nonvolatile empty
clobber at that point was discarded and did not work.

Debt: **six pins, four empty barriers, five volatile parameter qualifiers,
and three explicit hazard nops**. Two other pins and two empty barriers were
removed together without changing any bytes. Matrix loads, the SZ3 right
shift, and its store are C. Every GTE instruction/nop uses an individual
existing macro; no legacy CPU-instruction helper is used. Compiler and
assembler were not modified.

Research: `scratch/engine_800D3BC8` locally and
`/home/hasik/fx-search-archives/engine_800D3BC8` on darwine. Key probes:
`clean` (1374), `entry0_0` (150), `pins6` (40), `redvolatile1` (0),
`minimal15` (0 with reduced debt), `promoted` (0).
