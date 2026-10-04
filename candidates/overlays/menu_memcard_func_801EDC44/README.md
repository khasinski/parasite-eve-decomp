# menu_memcard func_801EDC44 (0x244C, 0x950 bytes): ring burst controller

Parked at 34 real diffs (sc.sh, 2026-10-04 retry), stock GCC 2.7.2, no pins,
no barriers, no volatile, no hand-written rounding (see the last section). All remaining diffs are register allocation in the last
straight-line block of mode 2 state 1 plus the schedule that follows from it.

Files:
- `Memcard_RingBurstController.c`: best build (49). Instance for
  `src/overlays/menu_memcard/`, yaml `[0x244C, c, Memcard_RingBurstController]`.
- `Memcard_RingBurstController_natural.c`: the plain first draft (87): one
  `fade` variable, `fade = func_80077CF4(angle) / 32;`.
- `headers.diff`: adds `MemcardRingBurst` (s16 state and timer at 0x10/0x12)
  to `src/overlays/menu_memcard/menu_memcard_glint.h` and retypes the existing
  `Memcard_RingBurstController` prototype to it.

Unique bytes: a relocation-masked window search over every overlay and the
main executable finds no twin.

What matched along the way (504 -> 87 -> 49):
- `s16 timer`: `if (++burst->timer < 24)` gives the lhu/addiu/sh/sll/sra/slti
  chain; with the u16 RoomDampedSpark timer GCC folds the compare.
- State 1 of mode 1 as `if (++burst->timer >= 48) return 1; break;` keeps the
  two branches; `if (... < 48) return 0; return 1;` becomes a store-flag.
- `return func_800CE560(..., Memcard_DriftGlowParticle)` in mode 0 (no v0 reset).
- Child velocity `func_80071A54() % 140 - 70` (0xEA0EA0EB, sra 7) and
  `child->state = (i & 3) == 0`.
- A separate variable for the state 1 fade read before the 1000 ring
  (`ring2Fade`, 87 -> 57): with one `fade` the frame-parity fade, the matrix
  pointer and the constant 1 take the wrong callee-saved registers.
- Retail's `move s0,v0; bgez s0; addiu s0,s0,31; ... sra s0,s0,5` keeps the
  rounding temporary in fade's own register. `fade = rcos(angle) / 32` makes
  a fresh expand_divmod pseudo (copy_to_mode_reg) that global-alloc gives v1
  (it is allocated before fade, so the non-copy preference never applies).
  Writing the rounding out (`fade = rcos(angle); if (fade < 0) fade += 31;
  fade >>= 5;`) reproduces retail exactly (57 -> 49). This is plain C but
  spells out what `/ 32` means; `fade = rcos(angle); fade /= 32;` gets 55.

Remaining (block after the 0xC00 scale division in state 1):

    retail: fade s0, &band s1, &tilt s2, &offset s3, lift s4, scale s5
    build:  lift s0, &band s1, &tilt s2, &offset s3, fade s5, scale s4

`lift` (`rcos(angle) / 6 + 80`), &band, &tilt and &offset are block-local
(reg_n_deaths 1, one basic block), so local-alloc assigns them before global
alloc sees fade. Local-alloc gives lift s0 first; retail's locals start at s1,
i.e. s0 was already taken in that block when local-alloc ran, which only a
local pseudo with higher priority (or a suggested register) can do.
Tried without success: `fade * 2 / 3` in place / as a new local `dim` /
inline in both calls (dim takes s1, lift still s0), lift before or after the
2/3 update, s16/u16 lift, `offset.y = position.y - lift`, a block-local copy
`alpha = ring2Fade` after the 0xC00 scale (64), and two permuter runs on
darwine (scratch/a4lrb, a4lrb2; best hit only `ring2Fade = scale = ...`,
33 diffs, not acceptable C).

## Retry (agent 4, 2026-10-04): 49 -> 34, plain `/ 32`

- Bug fix: the lift is `func_80077CF4(angle) / 12 + 80`, not `/ 6`. Retail's
  `mfhi; sra v1,t3,1` after the 0x2AAAAAAB multiply is /12 (sra 0 would be /6).
  The earlier register analysis of the last block was done on the wrong
  expression.
- One `fade` variable for all of state 1 again (the `ring2Fade` split is no
  longer needed), plus a block-local `dim = fade * 2 / 3` for the two
  1000 bands. local-alloc then gives dim s0, as in retail (retail computes
  `sll s0,s0,1` / `subu s0,t3,s0` in fade's register).
- With these the explicit rounding is no longer needed: every fade is the
  natural `fade = func_80077CF4(angle) / 32;` (34 either way).

Remaining 34 words, one cause: retail's `lift` (s4) is allocated after the
block locals &band s1, &tilt s2, &offset s3 (and angle s3 / &ring s4 swap in
the earlier blocks follows from it). In this build lift is block-local with
3 refs over 24 insns, local priority 0.125, above &offset (3 over 30, 0.1),
so it takes s1. Making lift global reproduces retail's order for the last
block and for angle/&ring: sharing it with the loop index `i` gives 22 diffs,
but then i and lift are one pseudo (s5) while retail has i in s2, so that is
not the original either. Tried: s16/u16 lift, `offset.y = position.y - lift`,
every order of lift/dim/offset copy (brute force), `fade = fade * 2 / 3` in
place (122), lift split into two statements (size change).
- Reusing `angle` for the lift (`angle = func_80077CF4(angle) / 12 + 80;`)
  makes it global: the last block then matches register for register
  (&band s1, &tilt s2, &offset s3), 26 diffs, all global-alloc order
  (angle+lift s5, scale s4, &ring s3; retail angle s3, &ring s4, scale s5,
  lift s4). Retail keeps angle and lift in different registers, so this is
  not the original either; it only confirms that lift is a global pseudo in
  retail. Look for a function-scope variable that is otherwise unused in
  mode 2 (or set in another block) rather than a block-local lift.

## Lift carried from the mode 1 loop (agent 4, 2026-10-04): 2 diffs

`Memcard_RingBurstController_lift_global.c` (found by the permuter on
darwine, scratch a4lrb3): the spark loop in mode 1 writes its last velocity
through the same variable, `lift = func_80071A54() % 140 - 70;
child->vz = lift;`. That makes lift a global pseudo, and global-alloc then
reproduces retail's whole register map (angle s3, &ring s4, scale s5, lift
s4, block locals &band s1 / &tilt s2 / &offset s3, dim s0). The only
remaining words are in the loop: retail computes vz in v0
(`addiu v0,v0,-70; sh v0,12(s0)`), this build in lift's register s4.

So retail's lift is referenced outside the state 1 block at flow time, but
not in an insn that survives to register allocation (or not in the loop).
Next idea: a set of the shared variable that combine folds away after flow
has marked it global (combine does not recompute reg_basic_block), e.g. a
copy that merges into a store of a register or of zero. Plain `lift = ...;
child->vz = lift;` variants: `child->vz = lift = ...` (2), all three
velocities through lift (6), remainder in lift and `- 70` at the store (10).

## Round 2 on the global lift (agent 4, 2026-10-04): still 2

Variants on the 34-diff base (`Memcard_RingBurstController.c`):

| form | diffs |
|---|---|
| `lift = 0;` at function start or before the mode 2 state switch | 34 (dead, deleted by flow) |
| `child->timer = lift = 0;` / `lift = 0; child->timer = lift;` | 34 (cse folds the constant) |
| `lift = 1; burst->state = lift;` | 18 |
| `for (i = 0, lift = 50; i < lift; i++)` | size change |
| `lift = (i & 3) == 0; child->state = lift;` | 4 (state computed in s4) |
| `lift = ++burst->timer;` in mode 1 state 0 or 1 | 17 |
| statement-order brute force over the loop body with the vz form (120 orders) | best 2 |

Every reference that survives to register allocation puts that value in
lift's register s4, and retail has no other s4 use outside &ring and the
lift. Retail's lift is therefore marked global by a reference that flow
sees but that is gone before local-alloc (not deleted by flow itself and not
folded by cse), or it is global for another reason (more than one death).

## Retry (agent 9, 2026-10-04): still lev 2

`Memcard_RingBurstController_lift_global.c` is lev 2 with lev.py (the vz
register in the mode 1 loop); `Memcard_RingBurstController.c` is lev 34.
- local-alloc only takes a pseudo with reg_basic_block >= 0 and
  reg_n_deaths == 1; between flow and local-alloc only combine and sched1
  run, so a reference that makes lift global and then disappears has to be
  an insn combine merges away. Tried the load + argument forms in mode 0
  (`lift = burst->x/y/z;` passed to func_8006DDCC): combine does not fold
  the copy into a2/a3 or the stack argument (lev 16 to 18, size 597).
- Copy, add-constant and compare forms (`lift = a - b; if (lift == 0)`)
  would fold, but this function has no such site with retail's code shape.
- In the 34 build, local-alloc gives dim s0, then lift s1, &band s2, &tilt
  s3, &offset s4 (lreg: lift 3 refs over 24 insns, &band 4 over 59, &tilt
  4 over 60, &offset 3 over 30); retail needs lift after &offset, which a
  block-local lift cannot get with 3 refs unless its span grows past
  &offset's.
