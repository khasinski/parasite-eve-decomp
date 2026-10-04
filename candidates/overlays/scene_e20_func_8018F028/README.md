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
