# func_800CEE20 (main 0xBF620, 0x58C): field shape quads

Every quad of shape `D_800F3368.parameter0A` (vertex lists `D_800E13BC[]`,
quad counts `D_800E1210[]`) is projected with a YXZ rotation scaled by the
texture cell size and drawn as a copy of one POLY_FT4 template; the loop
stops at the first quad whose MAC0 is zero.

Status: 6 real diffs (stock GCC 2.7.2, no pins, no barriers). To build it,
copy `field_shape_quads.h` to `include/pe1/` and the .c to `src/main/engine/`,
then flip `[0xBF620, asm, engine/engine_800CEE20]`.

Everything after the colour block matches, including the 40-byte template
block copy loop, the stszotz/rtps tail and the OT link. What remains:

- Register choice for the colour channels: retail keeps r in a2, g in v1,
  b in a0 (the null-colour path copies `intensity` v1 -> a0, a2); this build
  puts r in a1. Retail's width (D_800F3376) also lands in a2, u in a1 and
  v in a0.
- Reusing `width` for the GetTPage result (as the parked file does) moves
  width/u/v to the retail registers (30 -> 6 diffs) but r stays in a1, and
  two loop-tail increments (`addiu s0,0x28` / `addiu t0,0x20`) swap.
- Tried without success: every assignment chain for the null colour path,
  colour variable types (u8/s16/u16/u32), declaration orders, and sharing
  r/g/b with width/height/u/v (best 5 diffs, `r`->width, `b`->v with the
  GetTPage result in `u`, which reads as a hack).
- Matched findings worth reusing: the loop exits (`break`) when MAC0 is
  zero; the template uv pairs are chained stores
  (`template.u0 = template.u2 = u` etc.); `intensity * color->r / 128`
  keeps retail's mult operand order; reading the view matrix pointer into
  a local before `template.clut = clut` reproduces the early `lw t0`.

## Retry (2026-10-04)

With the GetTPage result written straight into `template.tpage` (no
variable sharing) the build is at 30 diffs. Swapping the colour branches
(53), `r = g = b = intensity` (33), assigning b/g/r in reverse (33),
declaring `int b, g, r` (30) and storing the channels straight into the
template (size change) did not move r from a1 to a2; the remaining
difference is global-alloc order, not sched1 launch priority.

## Retry (2026-10-04, agent 4)

Writing the loop increments as `i++, packet++, vertex += 4` fixes the
swapped loop-tail increments: 4 real diffs remain, all the register of
`r` (retail a2, this build a1) in the colour block and its two stores.
Swapping the null-colour assignments (`b = g = r = intensity` and
reversed statements) makes it worse (9).

## Retry (agent 5, 2026-10-04, near-miss pass 3)

Still 4 diffs (`r` in a1, retail a2). Analysis with `-dg`: r, g, b are
global pseudos 85/86/87; g takes v1, b a0, r is allocated last and takes
the first free register, a1. In global.c `find_reg`, r only skips a1 if it
has a hard-register preference for a2 (`hard_reg_preferences`, set by a
copy/operation between r and a hard register or an already allocated
pseudo, or tied through a dying source in the same insn) or if a
lower-priority conflicting allocno prefers a1 (`regs_someone_prefers`).
Neither exists here: the only a1/a2 copies are the incoming `rotation`
and `scale_x`, whose preferences are pruned because they cross calls.
Brute force over 180 combinations (declaration order int/u8, all six
orders of the null-colour and the product assignments) stays at 4.
Sharing r with `page` (19) or with `width` (19) is worse.

## Retry (agent 4, 2026-10-04): still 4

Brute force over the order of the three template colour stores combined
with all orders of the null-colour assignments (36 builds) stays at 4.
`-dg` shows r (85) conflicting only with g (v1), b (a0), the two local
multiply temporaries (a0, v0) and v0; nothing that conflicts with r holds a1
or prefers it. Sharing r with `width` (GetTPage result written straight into
the tpage) is 19.

## Retry (agent 9, 2026-10-04): still lev 4

Scored with lev.py: lev 4 (retail 355 words, equal size), the same `r`
register (a1, retail a2). Retail's register map in this region is r a2 /
g v1 / b a0 and later width a2 / height v1 / v a0 / u a1, which looks like
shared temporaries, but every sharing tried is worse: r=width, g=height,
b=v in all combinations and with the GetTPage result written straight into
the tpage (lev 14 to 33); b=v alone stays at lev 4. With -dg: r is pseudo
85 (3 refs over 14 insns), allocated last of r/g/b; its conflicts are g,
b, the multiply temporaries, t0 (view) and the parameters; page (a2) is
already dead, so nothing blocks a1. A conflicting lower-priority allocno
preferring a1 (regs_someone_prefers) or a preference of r for a2 would be
needed; u (a1) would do it if it were live across the colour stores, but
retail stores the colours before the parameter06 branch.
