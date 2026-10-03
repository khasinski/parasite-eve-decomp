# func_800D3BC8 (main 0xC43C8, 0x39C bytes, yaml `engine/engine_800CEE20_C43C8`)

Projected textured sprite: builds a POLY_FT4 (RenderTexturedQuad) in the
packet buffer, projects the position with RTPS, picks the cell UVs from the
D_800F3368 parameter block, eases the screen point toward (160, 120) with
LoadAverageShort12 by `blend` and links it at SZ/4 minus the block depth.

`FieldEng_ProjectedSprite.c` compiles to the same 231 instructions as
retail; the only remaining difference (26 words) is the sched2 placement of
the prologue saves in the first basic block. Retail keeps every save
(ra, s5..s0) ahead of the five stack argument loads; stock GCC 2.7.2 hoists
`sw s5; lw s5,0x78(sp)` and `sw s3; lw s3,0x68(sp)` to the top and spreads
the remaining saves after `li v0,120`. A 35k-iteration permuter run
(darwine, scratch a4psprite, base score 645) found nothing better, and
-fno-schedule-insns makes it worse.

Lessons kept in the C: `intensity * color->r` operand order gives retail's
`mult t1,v0`; `blend *= 2` before the LoadAverageShort12 call gives the
in-place `sll s5,s5,1`; `(u16)D_800F3368.depth` gives the `lhu`.
