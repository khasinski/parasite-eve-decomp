# engine_800CAE0C (func_800CAE0C, 0x944) - parked, 1 register-allocation swap

Four-layer sibling of FieldEng_GlowLayers.inc (func_800C83A0 family): four
fixed spins (D_800C21D4/DC/E4/EC), one scale (D_800C21F4), sprite block
D_800F34C8 whose offset/depth halves are written as the scalars D_800F34D0 /
D_800F34D2 (a struct-field store would keep the glow->depth load below it;
retail hoists the load, which only the fixed-scalar vs varying-struct alias
rule allows). Apply field_glow_layers.h.patch for the extra declarations.

Plain C, no pins/barriers. Every instruction matches except the column
pointer registers inside the four gte_CompMatrix expansions:

    retail: sp+0x34 in s6, sp+0x14 in fp, sp+0x44 spilled (lw a3,0xC0(sp))
    mine:   sp+0x34 in fp, sp+0x44 in s6, sp+0x14 spilled (lw a3,0xC0(sp))

Diagnosis (cc1 -dl/-dg): all six column pointers are local-alloc qtys with 5
refs each; their lengths shrink by one per birth (sp+0x32 > 0x12 > 0x34 > 0x14
> 0x44 > 0x24) because layer 1 also holds the address sets. Local-alloc gives
21/22/23 to 0x24, 0x44, 0x14; global-alloc then evicts s7 (23) for &spinB.
Retail's order must have been 0x24 > 0x34 > 0x44 (0x14 left to global/fp),
i.e. a shorter life or more refs for sp+0x34. Statement reorders, a spin
array and rot.t store orders did not change it. The decomp-permuter cannot
help: pycparser drops the GTE asm statements, so its candidates are invalid.
