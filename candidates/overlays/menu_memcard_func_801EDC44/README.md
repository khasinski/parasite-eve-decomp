# menu_memcard func_801EDC44 (0x244C, 0x950 bytes): ring burst controller

Parked at 49 real diffs (sc.sh), stock GCC 2.7.2, no pins, no barriers,
no volatile. All remaining diffs are register allocation in the last
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
