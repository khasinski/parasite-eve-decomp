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

## Rescore (agent 4, 2026-10-04): lev 50

With main's current headers and `field_glow_layers.h.patch` the draft is
lev 50 (retail 593 words, mine 590). The three missing words are not a
separate problem: they are the same allocation swap. Retail spills the
`rotation + 20` pointer (sp+0x44, the gte_ldlv0 operand), and each of the
three reloads in layers 2..4 is a `lw a3,0xC0(sp)` immediately followed by
its `lhu`, so it needs a load-delay `nop`. This build spills `matrix + 4`
(sp+0x14, a gte_stclmv operand) instead, whose reload has independent work
before its first use, so no nop is needed. Fixing which column pointer is
spilled fixes the size and the swap together.

## Local-alloc priority model (agent 14, 2026-10-04): still lev 50

Exact qty spans from the -dl dump (one block, sets right before each GTE
asm, which are full sched barriers so births/deaths cannot move): 105
(rot+2) 113, 106 (mat+2) 112, 108 (rot+4) 111, 109 (mat+4) 110, 111
(rot+20) 109, 112 (mat+20) 108, 114 (&D_800C21F4) 107, 116 (&D_800F34C8)
106; all have 5 refs, so local-alloc order is 116 > 114 > 112 > 111 > 109
and they take s3..s7 in that order. Global then gives 108 fp, 106/105
take s4/s3 from 114/116, &spinB (77) takes s7 from 109.

Retail's registers (105 s3, 106 s4, 112 s5, 108 s6, 109 fp, 111 spilled,
77 s7) fit exactly one local order: 116 > 114 > 112 > 108 > 111, with 109
left to global (first global allocno, fp) and 77 evicting 111 from s7.
With the spans above that needs one extra reference on each of 108, 112,
114 and 116 (6 refs: 12/span), or one fewer on 105, 106, 109 and 111.
cse folds every macro operand to the same frame-address pseudo, so pointer
variables for &rotation / &matrix and splitting gte_CompMatrix into its
parts in layer 4 all leave the refs and lev unchanged (tested, lev 50).
No natural source form adding those four references was found.
