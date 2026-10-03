# scene_e22 func_scene_e22_0067E4 (func_801957CC, 0x374): swirl ring particle

Candidate: `RoomEffect_SwirlRingParticle.c` (clean C, no pins/barriers). It
needs `extern u8 D_80199434[];` added to include/pe1/scene_e22_ring_burst.h.
Logic is complete (both update states, ring draw via func_800D0E88, swinging
glow via func_800CFB7C + func_800CEE20); the instruction stream is 2 words
longer than retail.

Remaining differences (sc.sh, align2.py):
- `&spin` (sp+0x28, initialised from D_8018F1F4 at the top) is kept in a
  saved register (s5) across the whole function; retail recomputes
  `addiu a1, sp, 0x28` at the draw, so the frame address must not be cse'd
  from the struct copy.
- The particle state is loaded into s4 instead of v0 in the draw dispatch.
- Retail materialises `la s1, D_800F3368` after the particle pointer dies,
  stores `parameter00` through it and reads `(s16)parameter02` back through
  it after the clut call; a `RenderEffectParameters *params = &D_800F3368`
  local is folded back to absolute addressing by GCC, so that is not it.

## Agent 5 follow-up (2026-10-03, still 2 words long)

cc1 -ds dump: the struct initialiser's movstrsi expander copies the frame
address into pseudo 74 (`reg74 = $fp + 40`). cse pass 1 follows the jump
path (mode == 2 taken, state == 1 taken, the two rounding branches skipped:
6 path entries, under PATHLENGTH 10) and replaces the call argument
`a1 = $fp + 40` with reg 74, which then needs a saved register (s5) for the
whole function. The same path also gives cse `state == 1`, so the last `1`
argument becomes the state register (s4) instead of the mode constant.

Retail still reuses the mode constant register for both `1` arguments, so
the path is followed there too; only the frame-address equivalence is
missing. Tried with no change: plain `spin = D_8018F1F4;` assignment before
the switch, `(GteRotation *)&spin.x`, `&spin + 0`, moving the `spin.z`
store, `default: break;` in the draw switch, `break` instead of `return 0`
in draw state 0. RoomEffect_TwistBeamParticle (matched, same overlay) does
not hit this because its path to the call is longer (three-way state switch
plus the palette ternary), so this function probably differs in control
flow rather than in the copy.
