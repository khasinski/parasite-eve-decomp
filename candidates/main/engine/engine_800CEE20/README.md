# func_800CEE20 — field shape quads, matched (2026-10-05)

Promoted to `src/main/engine/FieldEng_ShapeQuads.c` and enabled in the main
manifest. Shared shape-table declarations are in `include/pe1/field_shape_quads.h`.
The function rotates/scales and projects each shape quad, copies a POLY_FT4
packet template, and links visible packets into the ordering table.

Stock native GCC 2.7.2 (`-O2 -G0 -funsigned-char -mips1 -mcpu=3000`) and
stock MASPSX (2.56 mode) give decomp.me-compatible Levenshtein score **0**
and **1420/1420 identical linked bytes**, verified on darwine.
Target main offset `0xBF620`, address `0x800CEE20`, size `0x58C`.
Target SHA-256: `10042188443b5ef035e80fabd31614735e05f6d8c045e0b88095fab24692bd18`.

The old four-instruction near-match depended on legacy matrix-load and
SZ3-store macros containing CPU instructions. Those macros are gone from this
function. All matrix loads, the SZ3 right shift, and its memory store are C.
GTE transfers/commands and seven hazard nops use individual existing macros.
The final source was rebuilt independently as `promoted`, with score 0.
The integrated `make -j32 verify` passed: the complete `main.exe` matches
retail. Source-policy, source-mapping and debt checks also pass. The expanded
ASM templates were audited: only individual COP2 instructions and nops remain.

Matching debt: **15 register pins, 11 empty barriers, seven explicit nops**.
The pins cover matrix transfers, the view/depth pointers, the green channel,
selected vertex/packet pointers, and the template-copy endpoint. Empty operands
preserve register allocation, transfer ordering and pointer lifetimes. The
copy-end operand `&template.x3` steers the preheader order; it is matching debt,
not an additional game operation. The explicit initial count check and inner
loop keep depth-pointer initialization after the empty-shape exit. Several
unnecessary pointer pins were removed during matching. No CPU instruction ASM,
modified compiler, modified assembler, or EABI is used.

Research: `scratch/engine_800CEE20` locally and
`/home/hasik/fx-search-archives/engine_800CEE20` on darwine. Key progression:
`clean` (2756), `refine` (631), `endclobber8` (440), `finalshape1` (235),
`preloop3_2` (210), `guardshape0` (20), `endpin0` (0).
