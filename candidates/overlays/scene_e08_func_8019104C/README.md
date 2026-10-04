# scene_e08 func_8019104C (0x2064, 0x2FC bytes): ring sprites draw

Parked at 10 real diffs (sc.sh), all register naming.

Files: `Scene_DrawRingSprites_8019104C.c` (instance for src/overlays/scene_e08/)
and `scene_ring_sprites.h` (goes to include/pe1/).

Logic, frame layout, stores and calls all match. Retail keeps the state
pointer in two pseudos: the incoming `$a2` copy in s1 is used by the
`ticks < 12` flash-sprite block, and a second copy `s2 = s1` (made in the
delay slot of `jal func_800C2EAC`) is used by everything after that block;
s1 is then reused for `&matrix`. Stock cse always merges the copy
(`ring = state` in every combination of uses in the condition / body / tail
was tried; cse1 or cse2 canonicalises one away and flow deletes the copy),
so the build uses one register (s2) throughout: prologue save order of s1/s2
and the base register of the ten state loads differ.

Not tried: inline helper functions, permuter.
Also note `(ticks % 12 & 0xFE) - 0x5C` (not `+ 0xA4`) gives retail's addiu.

## Retry (agent 5, 2026-10-04, near-miss pass 3)

A `static inline` helper for the tail (glow sprite and rings) taking the
state pointer as a parameter does not keep the copy: integrate's parameter
pseudo is merged like a plain copy and the helper's locals get their own
frame slots (size 0x304, frame 200 bytes). Still at 10.

## Retry (agent 4, 2026-10-04): still 10

- A `static inline` helper for the `ticks < 12` block (taking the state
  pointer, with `matrix`/`scale` passed by pointer to keep the frame): the
  inline parameter is merged into the incoming pointer as well (one register,
  s1, throughout; size 0x2F0). With the helper's own locals the frame grows
  to 0x98 (62 diffs).
- Why no plain copy can survive (cse.c `make_regs_eqv`): for
  `ring = state`, the new pseudo becomes the canonical register only if it
  lives past the end of the current cse path and longer than the old one.
  With skip_blocks the path covers the `ticks < 12` block and the tail, so
  `ring` is not canonical and its uses are rewritten to `state`. If the path
  ended at the block (label with two uses), `ring` would be canonical and the
  flash block's `state` uses would be rewritten to `ring` instead. Either way
  one pseudo is left, while retail keeps the incoming copy for the flash block
  and the second register for the tail. The copy is retail's
  `addu s2,s1,zero` in the `jal func_800C2EAC` delay slot, i.e. before the
  `ticks` test, so it cannot be a copy made after the block either.
