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

Note (2026-10-04, agent 4): sched2 experiment. Wrapping the body in a loop
(banned do/while, test only) puts a LOOP_BEG barrier after the parameter
loads: every save then stays above the body (22 diffs), but each stack
argument load still sits right after the save of its register, because the
loads feed the barrier and outrank the saves. Retail's shape (all saves,
then the five loads in a row, then the body) needs the loads to have no
in-block consumers, i.e. a block boundary or barrier between the saves and
the loads; no plain-C source for that is known.

## sched2 trace (agent 4, 2026-10-04): still 24

`cc1 -dR` on the first block: every prologue save has priority 1 and becomes
ready at T-6, as soon as the branch, the `addu s0` delay-slot insn and the
packet pointer chain are scheduled. The `lw D_8009CDDC` / `lw D_8009CDD8`
loads (insns 37 and 48) then block for their load delay at T-8..T-13, and at
each of those cycles a save wins the tie on "greater potential hazard", so
ra/s4/s2/s1/s0 land between `li v0,120` and `sh v0,26(sp)`. In retail those
stall cycles were filled by the centre stores instead, so the saves stayed
on top. Brute force over the order of the centre stores, the packet pointer
and the D_8009CDD8 update (6 orders) is at best 24.

## Rescore and sched2 dependence analysis (agent 14, 2026-10-04): lev 14

lev 14 (231 words both). All 14 edits are the prologue order.

- Survey: of all retail functions in main and the overlays whose prologue
  loads stack arguments, only this one, Render_SetupEntityPrims and
  fx_common func_8018F55C keep every save above every stack-argument load;
  the other 83 interleave them, and every matched C function with stack
  arguments interleaves too. So the "saves first" shape is an outlier.
- In stock sched2 the five saves (ra, s4, s2, s1, s0) depend only on the
  packet-table load `lw v1,0xE58(at)` (insn 46, a register-based address
  that may alias the stack), so they are ready from T-6. At T-8..T-13 the
  D_8009CDDC/CDD8 loads are blocked by the memory unit after a store, and
  among the remaining ready insns a store always wins on potential hazard
  over `li v0,120` (equal priority 1). Retail picked `li v0,120` at T-8,
  which under these rules needs either the saves not ready or `li` with
  priority >= 2. Neither is reachable from the C: the saves are created
  after sched1, so their only dependences are sched2's own, and sp-relative
  saves never conflict with sp-relative argument loads or symbol accesses.
- Also observed: the stack-argument loads keep stale sched1 dependences
  (argp-based addresses conflict with the centre stores and the
  D_8009CDD8 store), which is why they sit above `lw D_8009CDDC` in both
  builds.
- No source change that keeps the body intact was found; parked at lev 14.
