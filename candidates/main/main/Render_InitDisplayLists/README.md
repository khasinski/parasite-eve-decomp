# Render_InitDisplayLists (main 0x5A308, 0x5E0 bytes): 1500 vs 1504 bytes

The asm file holds one function: splat's `PeImage_Mount_Dispatch`
(0x80069E30) is case 4 of the state switch (it is the jump-table target
for state 4), not a function. Matching it needs the yaml line
`[0x5A308, c, main/Render_InitDisplayLists]`, the rodata line
`[0x1B88, .rodata, main/Render_InitDisplayLists]` (the 10-word jump table
is the whole 0x1B88 subsegment) and dropping `PeImage_Mount_Dispatch` from
sym.main.txt.

Use: copy `boot_disc_check.h` to include/pe1/ and the .c to src/main/main/,
then score with offset 5A308 size 5E0. Two goto restart loops (CD reads of
the notice TIM and the fog layer), the same shape as Memcard_PlayVideo.

What already matches: the read/poll retry blocks (la of g_GameState+0x100
with 0x8C/0x94 offsets comes from cse related values on two g_GameState
fields), the shared `status` variable (gives retail's `move v1,v0` after
every status call), the case 1/3/4 compare trees, the cross-jumped
`Tbl_ResetAll(); Render_SetupColorTable(mode == 1 ? 1 : 2, ...); state = 2`
tail that case 1 enters past the Tbl_ResetAll call (`goto ready`), the
post-loop code.

Remaining (cc1 -dL): the display loop has 203 real insns. loop.c moves the
D_800B0E38 base (life 4, savings 2: 29*2*4 = 232 >= 203), the constant 1
and the constant 2 (23*3*3 = 207 >= 203). Retail moves only the constant 1
and keeps `lui at; addu; lw %lo(D_800B0E38)` and `li v0,2` in the body, so
its loop counted at least ~235 insns, or the movables come in another
order. The extra hoisted registers also renumber the s-registers
(mode s5 vs s4, state s1 vs s2, wait s2 vs s1, read -1 s4 vs s5). The
LOOP_HOIST.md levers (sign-extension casts, literals, record pointers) were
not tried yet.
