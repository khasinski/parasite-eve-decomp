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
