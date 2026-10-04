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

## lev rescore and retry (agent 16, 2026-10-04): lev 14

- lev.py gives lev 14 (231 words each); all edits are in the prologue.
- sched2 dependence view (`-dR`): the `D_800B0E38.packets[D_8009CDDC]` load
  (insn 46) has a true memory dependence on every prologue save, because a
  `reg + symbol` address conflicts with any `sp + const` slot. The saves
  therefore become ready (backward) as soon as that load is placed. From then
  on they have the same priority (1) as the centre `li`/`sh` insns and win
  every tie in schedule_select as stores ("greater potential hazard"), so
  they fill the load-delay cycles of the D_8009CDDC/D_8009CDD8 loads. Retail's
  shape needs the competing body insns to outrank the saves (priority 2,
  for example a value fed by a load) or the saves to become ready only after
  the centre stores.
- 96 orderings of centre init / packet allocation / colour block (both branch
  senses, centre moved after the packet or down to the GTE block) are at best
  lev 14 (the current order).

## Retry (agent 19, 2026-10-04): not retried in code

The newest lessons (folded extra references, multi-set constants,
scalar outputs) act on sched1 and register allocation; this diff is the
sched2 placement of the prologue saves against the stack argument loads,
which needs a block boundary or barrier no plain-C construct provides.
Still lev 14.
