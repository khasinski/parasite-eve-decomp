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
