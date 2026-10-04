# scene_e20 func_8018F028 (0x40, 0x728 bytes): flare particle callback

Status: one instruction off (size 0x72C vs 0x728). Everything else lines up
(registers, schedule, relocations) with the files here:

- `RoomEffect_FlareParticle_8018F028.c`: the instance (would go to
  `src/overlays/scene_e20/`, yaml `[0x40, c, RoomEffect_FlareParticle_8018F028]`).
- `scene_e20_flare.h`: its narrow header (would go to `include/pe1/`).

Remaining diff, mode 2 state 0, first palette read-back:

    retail                          mine
    lui  a0, %hi(D_800F336C)        lui  v1, ...
    lhu  a0, %lo(D_800F336C)(a0)    lhu  v1, ...
    li   v1, 4                      nop
    sll  v0, a0, 1                  sll  v0, v1, 1
    ...                             ...  (lookup)
    bne  a0, v1, ...                li   v0, 4
                                    bne  v1, v0, ...

Retail's `4` pseudo was not a sched1 "birthing" insn (or was allocated
before `kind`), so it sits above the `sll` in v1 and sched2 fills the load
delay slot with it. Tried: ternary and if/else palette forms, separate clut
temporary, block-local or function-scope `special = 4` (single or multi
set, in case 0 and/or case 1), shared `index` temporaries. A multi-set
`special` in both case-0 palette blocks fixes the size but swaps kind/4
registers and loses the s5 reuse for the second compare (10 diffs).

Lessons that did land here:
- `state = p->state; switch (state)` and passing `state` as the page
  argument of the case-1 glow reproduces retail keeping the state in s3.
- `rotation.z = p->timer * 32` keeps the `lh` that `<< 5` narrows to `lhu`.
- A block-local `int top = 0x40;` before the cos call reproduces the
  constant held in s4 across two calls for the streak's `v` argument.
- `angle = time << 6` before the stores (not inside the call) puts the
  angle in s4 and lines up the callee-saved registers.

## Retry (agent 5, 2026-10-04, near-miss pass 3)

Why retail's `li v1, 4` sits above the `sll`: sched1 gives a single-set
pseudo LAUNCH priority (`birthing_insn_p` needs `reg_n_sets == 1`), so the
compare constant is placed right before the `bne`. A pseudo with two sets
is scheduled by its critical path and lands in the load delay slot, as in
retail. `RoomEffect_FlareParticle_8018F028_multiset.c` does this with a
case-scope `int special` set to 4 in both case-0 palette blocks
(`special = 4; kind = D_800F3368.palette; ... if (kind == special && ...)`):
size is right (0x728) and the first block's order matches; 9 real diffs
remain:

- first compare: kind/4 registers swapped (retail kind a0, 4 in v1). Both
  pseudos are global; global-alloc priority is floor_log2(refs) * refs /
  live_length: kind 6 refs over 10 insns (1.2) beats special 4 over 8 (1.0),
  so kind takes v1 first. Moving the `special = 4` statement does not change
  the live length (sched1 rewrites it).
- second block: retail compares against s5 (the texture argument 4 held
  across the call), this build re-loads `li a0, 4`; the texture argument
  then lands in v1 instead of s5 (two more words).

Not tried yet: a form where the second palette block reuses the texture
argument's pseudo while the first keeps a multi-set local (would need the
first block's 4 to be multi-set inside one block, or kind's priority below
special's: fewer refs or a longer life for kind).

## Retry (agent 4 audit, 2026-10-04): 7 diffs

`RoomEffect_FlareParticle_8018F028_single_special.c`: one function-scope
`special = 4` set only before the first palette kind load, used by both
case-0 compares (`kind == special`), with the literal `4` as the
func_800CEE20 texture argument. Size is right and the second compare and
the call match; the 7 diffs are the two 4 pseudos trading places: retail
loads a fresh `li v1,4` for the first compare (in the lhu delay slot) and
keeps the texture argument 4 in s5 for the second compare, this build keeps
`special` in s5 from the first compare and loads the argument into v1.
- Setting `special = 4` right before the call instead (argument and second
  compare reuse it, s5 matches retail) leaves the first compare's literal as
  a single-set launch pseudo: `li v0,4` lands right before the `bne`, one
  word too many (size 0x72C).
- A second multi-set variable for the first compare, also set in the case-1
  palette blocks, is folded by cse there; the dead case-1 sets disappear and
  it is single-set again (same 0x72C).
- Adding the pre-call set to any of the earlier combinations: 22 diffs.

## Retry (agent 4, 2026-10-04): still 7

With every compare written as the literal `4`, the first compare's pseudo is
local but single-set, so sched1 launches it right before the `bne` and
local-alloc gives it v0 (kind v1). Retail has kind a0, the 4 in v1 and the
`sll` temporary in v0, which means the 4 was born right after the `lhu`
(it overlaps the `sll` temporary), i.e. not launched: it must be a multi-set
pseudo that stays inside the block. A block-local `int special = 4;` set
again right before the palette read is not enough: the first set is dead and
is removed, so the pseudo is single-set again (size 0x72C).

## Retry (agent 9, 2026-10-04): lev 4

`RoomEffect_FlareParticle_8018F028_shared_temp.c`: one function-scope `int
special` used as a short-lived scratch for everything retail keeps in v1 in
case 0: the tpage value, the three rotation copies (`special =
(u16)p->heading.x; rotation.x = special;` and so on) and the 4 of the first
palette compare (`special = 4; ... kind == special`). The second compare
keeps the literal 4 and reuses the s5 texture argument. Why it works
(cc1 -dl/-dg): with several sets the 4 is no longer a single-set launch
pseudo, so sched1 leaves `li v1,4` in the lhu delay slot; with 10 refs it
outranks `kind` (6 refs over 9 insns) in global alloc and takes v1, kind
takes a0. The `(u16)` casts give the lhu copies retail has (an int copy
gives lh; a u16/s16 `special` lets cse fold the compare back to a launched
literal).

Remaining 4 words: the second palette block. Retail keeps kind in v1 there
and in a0 in the first block, so the two kind reads are different pseudos.
`kind` shared by both blocks is one global pseudo (a0 in both). A separate
block-local `kind2` for the second block makes the first kind local too,
local-alloc then gives it v1 before `special` is allocated (lev 13). Using
`special` for the second kind is lev 17. Not tried: making the first kind
global by another plain-C reference that leaves no code.

## Retry (agent 8, 2026-10-04): still lev 4

- A block-local `kind2` for the second palette block makes the first kind
  local, and local-alloc gives it v1 before `special` (lev 13). The first
  kind has to stay global (multi-set). Sharing it with mode 1's `bounce`
  (`bounce = -(s16)fall`) keeps it global with retail's a0 and gives the
  second block v1 (lev 2), but bounce then lands in a0 instead of v0, and
  reusing the variable that way only steers codegen. Sharing it with
  `glow` is lev 4 (s1). Any other variable (state, time, fall, angle,
  case 1's kinds, function-scope kind) is lev 13 to 81.
- Swapping the roles of kind and palette in the first block is lev 12.

## Retry (agent 19, 2026-10-04): still lev 4 (lev 9 plain)

Tried the menu_memcard ring burst fix (a function-scope variable whose
extra copy in another basic block is folded into an argument register by
combine, so flow already counted it and the variable becomes global):
- First-block kind as a function-scope `level` with a folded copy in
  state 2 (`level = (p->timer << 4) / 20; func_800CF3AC(..., level)`), in
  the streak block, before func_800D2104 or as the width source: lev 9,
  kind still v1. Global is not enough; the literal 4 is still loaded right
  before the `bne` in v0.
- A function-scope `special = 4` for the first compare plus a folded copy
  elsewhere (same sites): lev 9, the 4 still lands before the `bne` (sched1
  places the constant set next to its consumer even with two sets).
- Both together (all six site pairings): lev 9.
- On the shared_temp file with a block-local `kind2` for the second block
  and the folded copy on the first kind (five sites): lev 13, kind global
  but allocated before `special`, so it takes v1.
- Why the folded second set does not stop the launch: try_combine
  decrements reg_n_sets when it merges a set away (reg_n_refs, deaths and
  reg_basic_block are not recomputed), and sched1 runs after combine, so
  `special` is single-set again by the time birthing_insn_p looks at it
  (`-dS` shows insn 388 at 7f000001). A non-launched 4 needs a second set
  that survives combine, i.e. real code in v1 (which is what the
  shared_temp file does with the tpage and rotation copies).
- Compare forms (`kind != 4 ||`, switch on kind, `(u16)kind == 4`,
  `kind - 4 == 0`, the assignment inside the subscript): lev 9 to 21.
