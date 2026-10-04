# Render_InitDisplayLists (main 0x5A308, 0x5E0 bytes): lev 2

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

Update (agent 6, third pass, scored with lev.py): lev 2. The only
remaining edit is the order of `li s1,180` / `li s2,9` around the `j` in
case 4's "wrong disc" path (retail sets wait first, state in the delay
slot).
- Case 5 sits before case 4 in the source (jump table: [4] = 0x80069E30,
  [5] = 0x80069E20, so case 5's code comes first).
- Case 1's DsSync switch has `case 1: break;` (DslDataReady): it turns the
  compare tree's `slti 2` into retail's `slti 3`.
- `wait = 180;` comes before `colors[0] = -1;` (prologue order).
- The near-tie: written `wait = 180; state = 9;` (retail's order) global
  alloc gives state s1 / wait s2 (lev 26): state 33 refs / 222 = 0.7432,
  wait 41 / 278 = 0.7374. Written `state = 9; wait = 180;` the lengths
  shift by one each (state 223, wait 277: 0.7399 vs 0.7401), the
  registers are right and only that pair is swapped (lev 2).
- Alternative with cases 6 and 7 shared and 8, 9 separate: priorities
  are right with retail's order (state 31 refs, floor_log2 4) but loop.c
  sees 232 real insns and still hoists the ordering table base
  (29*2*4 = 232 >= 232); with the `else if` disc-flag test (duplicated
  `done = 1`) it is lev 9. One more counted insn would make that variant
  match. Tried for +1 without effect: ternary forms, break-style case
  bodies, `!= 0` tests, MoveImage forms, pointer-add OT lookup, inline
  helper (fewer insns), if/else colour calls (changes size).
