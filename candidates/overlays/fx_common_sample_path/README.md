# fx_common func_8018F55C (0x56C, 0x3D0) path sampler: parked 2026-10-03

Candidate `FxCommon_SamplePath.c` (types `FxCommonPath`, `D_8019BFCC` already
in src/overlays/fx_common/fx_common.h). Called by FxCommon_InitMotionDeltas,
FxCommon_UpdateEffects, func_80195E4C and func_80193478 (keep the symbol).
Yaml split when flipping: `[0x56C, c, FxCommon_SamplePath]` then
`[0x93C, asm, fx_common_93C]` (the asm blob continues with func_8018F92C).

State: 14 real diffs WITH one volatile read, which is not acceptable as is.

1. The path header count is read as `lhu; sll 16; sra 16` into s4. Every
   non-volatile form tried gives a bare `lh` (u16/s16 field, s16/int local,
   `(s16)` casts, unsigned bitfield, `((s32 *)p)[1] >> 16`): combine folds the
   zero-extend plus shifts. Only `((volatile GteShortVector *)points)->pad`
   reproduces it, and retail does not re-read the field, so the volatile is a
   hack under the project rules.
2. The remaining 14 words are one register swap in the bank block: retail
   keeps the sign-extended yaw in v0 and the (next - yaw) product input in v1
   (then `mflo v1`), mine has them swapped and takes the product in t3.
   Statement order of the angle stores, `bank` forms and declaration order
   (40 random scalar orders) do not change it.

Matched along the way (keep):
- the offset lookup must be the same `base + index * sizeof(s32)` cursor form
  as FxCommon_GetOffsetEntryValue (func_8019959C) to get `addu a1,a2,a1`;
- treating the header as the first `GteShortVector` (`points++` after reading
  `pad`) gives retail's single t1 register for header and points;
- `segment += count - 2` (not `span`) for the negative modulo fixup gives
  `addiu v0,a1,-2; addu a1,v0,s4` without cse folding it into `span`;
- the 24.8 interpolation is three `<<= 8` per vector, then three `+=`, then
  three `>>= 8` (flow keeps all three stores per field only in that order).
