# func_800D0728 (main 0xC0F28, 0x760): field ring band

An annulus of `segments` POLY_G4 quads between radius `inner` (a1, shaded
by color0) and `outer` (a2, shaded by color1), scaled, YXZ-turned (view
rotation applied when rotation->flags is set) and placed at a projected
anchor. Sibling of FieldEng_StarFan (func_800D004C), which matched with the
same structure.

Status: 4 extra instructions (stock GCC 2.7.2, no pins, no barriers).
To build it, copy `field_ring_band.h` to `include/pe1/`, the .c to
`src/main/engine/`, apply `render_object.h.patch` (typed colour
parameters; pointer-only, caller objects unchanged) and split
`[0xC0F28, asm, engine/engine_800CEE20_C0F28]` into
`[0xC0F28, c, engine/FieldEng_RingBand]`.

Everything matches up to the ordering-table link. Remaining:

    retail: beq mode,0xFF / delay addu a0,zero,zero; else path links
            through s0, and both link tails cross-jump into one
            `and/or/sw 0(s0)` tail
    mine:   delay `move a0,s0`; else path links through a0, so the tails
            differ and are not cross-jumped (+1 move, +3 tail insns)

Diagnosis (cc1 -dj/-ds): expansion gives `236 = depth*4 + table;
ot(92) = 236`. cse swaps them (ot becomes the canonical register) and the
jump-followed else path (label used once, -fcse-follow-jumps) keeps the
copy 236, which local-alloc puts in a0. In the star fan the same macro
expansion stays on ot. Tried without effect: TILE_OT_ENTRY rewritten four
ways (two-step union, pointer form, &table[depth*4]), block-scope OT
temporaries, `break` instead of `return`, stsxy2 before/after the entry,
`else { if (packet) }`, swapped mode test (worse), increment order.
