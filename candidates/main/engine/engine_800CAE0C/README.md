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

## Second pass (2026-10-04): what retail's allocation implies

Reading global.c/local-alloc.c against the -dl/-dg dumps narrows the target:

- The whole function is one basic block, so every column pointer is a
  local-alloc candidate. Mine: local-alloc gives the five free callee-saved
  regs (s3..s7) to 116 (&D_800F34C8), 114 (&D_800C21F4), 112 (sp+0x24),
  111 (sp+0x44) and 109 (sp+0x14): with equal refs (5) the priority is the
  qty span, and later-born pseudos have shorter spans because layer 1 holds
  more insns than layer 4. Global-alloc then takes 108 -> fp (free), 106/105
  evict 114/116 (refs/live_length 5/216), and &spinB (2/35) evicts s7 = 109.
- Retail's result is only reachable if local-alloc took 105, 106, 108, 111,
  112 and NOT 109/114/116: then 109 is the first global allocno and gets fp,
  114/116 are rematerialised (lui/addiu per layer, as retail shows), &spinB
  evicts 111 (5/110 < 2/35) into s7, and &spinC/&spinD stay on the stack.
  So in retail 109 and both symbol pseudos ranked below 105 (span ~113),
  i.e. they had fewer refs (4 refs = priority 8 instead of 10) or longer
  spans than the structure of this source gives them.
- Tried without effect or worse: static inline per-layer helper (frees the
  per-layer scale slots, frame shrinks to 192), one shared scale variable,
  pointer variables for &matrix/&rotation, rotation.t stores before the
  matrix copy, reversed rotation.t store order, early `sprite`/`scaleSource`
  pointer variables (they move into saved registers).
