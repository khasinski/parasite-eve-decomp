# func_800D3BC8 (main 0xC43C8, 0x39C, yaml engine/engine_800CEE20_C43C8): field pulled sprite

A POLY_FT4 centred on the projected position, pulled toward the screen
centre (160, 120) by LoadAverageShort12 with weight `pull * 2` (4.12); a
nonzero pull also forces the ordering depth to 14. Half sizes are the
cell extents times the 19.13 scales.

Status: the whole body matches (stock GCC 2.7.2, no pins, no barriers);
the 24 remaining real diffs are all in the prologue (0x04..0x60). To build
it, copy `field_pulled_sprite.h` to `include/pe1/` and the .c to
`src/main/engine/`, split the yaml blob line `[0xC43C8, asm,
engine/engine_800CEE20_C43C8]` to the C unit.

Retail emits every register save first (s6/s7 moves interleaved, then ra,
s5..s0) and only then loads the five stack arguments (0x78, 0x68, 0x6C,
0x70, 0x74), with `move t2,a0` as the very first instruction. Stock
GCC 2.7.2 hoists each stack-argument load right after the matching save
(`sw s5; lw s5,0x78(sp)`), exactly as retail itself does in func_800CEE20,
and sinks the remaining saves below the D_8009CDD8 loads and the centre
stores.

Tried without effect: centre initialisation placement and type (4-byte
pair, initialiser), view slot through a pointer or directly, `clut` as
u16, `depth` as int, `pull * 2` inline (breaks the body), and a 10 minute
decomp-permuter run (no improvement over the base).
