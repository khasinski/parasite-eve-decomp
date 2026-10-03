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

2026-10-03 (agent5, third pass): the three renderers it calls are now matched
as FxCommon_DrawModel (func_801995BC, normal), FxCommon_DrawModelTinted
(func_8019A318, mirrored copy: gouraud colours / 4, textured colour from
D_801EA264) and FxCommon_DrawModelFlat (func_8019B1D0, fixed depth
D_801EA5E4). Their prototypes moved to fx_common.h (forward-declared
`FxCommonPolyModel` union), so the candidate now includes only fx_common.h
and keeps the typed `FxCommonTransformNode *node` parameter, like
FxCommon_ApplyNodeTransform does for func_80190D3C (taking `void *node`
and assigning a typed local costs 25 extra diffs: the node copy to s1 moves
below the mirror block copy). Still 21 real diffs.

Retail evidence for the blocker: in func_80191114 the mirror copy, the
position fill and `addiu s3, sp, 0x20` (delay slot of `bne mode, 2`) are all
in one basic block before any call, yet the copy is addressed off sp and s3
is computed separately. Stock mips.c expand_block_move always copies the
destination address into a pseudo (copy_addr_to_reg), and cse makes any
later `&mirror` reuse that pseudo, so the separate s3 is unreachable unless
the copy's destination pseudo is not in cse's table at that point.

func_80191114 (0x2124, 0x46C, not drafted): LOD node. Mode 2 copies the
unaligned seed (lwl/lwr) and node->matrix to the stack, runs
gte_CompMatrix(&copy, &mirror, &node->matrix) and restores the copy's
translation; other modes run func_800794C4 directly. Then the usual
D_8019BFF0 transform calls, func_80190254(&position, margin) | force, and a
kind chosen by D_8019BFF0->t[2] against thresholds at node+0x38/0x3C (kinds
at +2/+4/+6), drawn with FxCommon_DrawModel for mode 0, Tinted for 1 and 2.
