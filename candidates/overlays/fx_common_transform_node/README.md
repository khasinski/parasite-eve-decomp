# fx_common func_80190E04 (0x1E14, 0x310) transform node draw: parked 2026-10-03

Candidate `FxCommon_UpdateTransformNode.c` (node fields `kind`/`margin`,
`D_8018F014` and the renderer prototypes are already in fx_common.h).
Called 30 times by func_80192800. Uses only existing gte.h macros
(`gte_CompMatrix` = PSY-Q gte_MulMatrix0 + gte_ldlv0/gte_rt/gte_stlvl).
Yaml split when flipping: `[0x1E14, c, FxCommon_UpdateTransformNode]` then
`[0x2124, asm, fx_common_2124]` (func_80191114 follows).

State: 21 real diffs (register allocation only), with an explicit
`mirrorMatrix = &mirror;` pointer in the then-block.

The blocker is the address of the stack copy of the mirror matrix
(`mirror = D_8018F014`, sp+0x20). Retail computes `addiu s3, sp, 0x20` at the
start of the visible branch (before func_800794C4). Stock cc1 keeps the
block-move destination address (reg = frame+16) from the struct copy and cse
reuses it for the GTE operand, so it is computed before the bounds call and
held in s3 across it (501 diffs, one instruction shorter). Initializer form,
aggregate initializer, copy after the position fill, `||` instead of `|`
all keep the reuse. The explicit pointer variable moves the copy into the
branch but cse still links it (s2 -> s3 move) and shifts the s-register
assignment (mirrored s5 / pass s6 vs retail s2 / s5).

func_80191114 (0x2124, 0x46C) has the same shape (mirror copy at sp+0x20,
`addiu s3, sp, 0x20` in the mode branch delay slot), so it hits the same cse
behaviour; a permuter run (25k iterations) on func_80190E04 found only no-op rewrites.

2026-10-03 (agent5, second pass): wrapping `position` and `mirror` in one
local struct (`work.position` at sp+0x10, `work.mirror` at sp+0x20) changes
nothing (still 21). Note: fx_common_motion.h now declares
`void func_80190E04(void *node, FxCommonBuffer *context, u8 pass, u8 force,
u8 mirrored)` for the matched caller FxCommon_DrawScene, so the definition
must take `void *node` and assign it to a typed local when this is flipped.
