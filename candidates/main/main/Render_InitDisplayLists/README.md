# Render_InitDisplayLists (main 0x5A308, 0x5E0 bytes): 67 diffs, size exact

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

Update (agent 6, second pass): size now exact, 67 word diffs, all one
s1<->s2 swap (retail wait s1 / state s2, mine state s1 / wait s2).
- The loop hoist is solved without tricks: case 1's DsSync == 2 path has
  its own `Render_SetupColorTable(mode == 1 ? 1 : 2, ...); state = 2;`
  (no shared label) and cases 6, 7 and 8 are three separate identical
  bodies. jump2 cross-jumping folds them back into retail's layout (6-8 on
  one table target, case 1 entering the shared tail), but loop.c sees 245
  real insns: the ordering table base (29*2*4 = 232) and the constant 2
  (26*3*3 = 234) are now `not desirable`, only the constant 1 moves.
- Remaining: global-alloc priority (cc1 -dl). state = reg 73, 33 refs over
  222 insns = 5*33/222 = 0.7432; wait = reg 76, 41 refs over 276 insns =
  0.7428. Retail allocates wait first. One more wait ref, one fewer state
  ref (or state refs <= 31, floor_log2 drops to 4) would flip it. Tried
  without effect: init order of the locals (as declarations or statements,
  wait before/after colors), state init before the loop (breaks layout).
  The permuter (darwine, merged 0x5E0 target from the .s with the
  PeImage_Mount_Dispatch glabel turned into a local label) found nothing
  in 20k iterations.
