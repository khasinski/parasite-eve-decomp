# room_m256 func_80195728 (0x6740, 0x5A0 bytes): pulsing model ring controller, parked

Yaml line: `[0x6740, asm, func_80195728]` in configs/USA/overlays/room_m256.yaml.
Twin of the room_lib ModelBurstController template (room_m245, 0.76).

To score: copy room_model_pulse.h to include/pe1/ and
RoomEffect_ModelPulseController.c to src/overlays/room_m256/, then
`sc.sh <wt> src/overlays/room_m256/RoomEffect_ModelPulseController.c mp room_m256 6740 5A0`.
Best build: 0x59C (retail 0x5A0, one saved register short). The palette
constant now lands in a saved register (`palette3`); the remaining problem is
the tile page table base that retail keeps in s3.

## Why the base register is missing (agent 5, 2026-10-04, cc1 -dS/-dR)

- The base is the `reg = D_800E2850` set created by the first parameter
  block's `D_800E2850[D_800E11FA]`; combine folds that load back to the array
  form but keeps the set because the if-body uses it (retail does the same).
- sched2 has a bug in sched_analyze_1 (`call_used_regs[i]` with the loop
  counter instead of `regno + i`): EVERY hard register set gets an anti
  dependence on the preceding call. So after reload the base load can only
  rise to just after the last call before it. Retail's `la s3` sits right
  after func_800CE8F0, so retail's sched1 output had the set between
  func_800CE8F0 and func_80077DC4.
- Our sched1 marks the set as a birthing insn (single set, LAUNCH priority)
  and, with no consumer in the block, puts it at the end of the block (before
  the `D_800E27EC >= 4` branch); sched2 then lifts it only to after
  func_800CE9D4, where it can share s1 with &position: one saved register
  fewer.
- For retail's placement the set needs either a scheduling barrier between
  func_800CE8F0 and func_80077DC4 (a block end, loop note or volatile asm:
  none is in retail's code and do/while(0) is not allowed), or a
  non-birthing (multi-set) base pseudo that cse does not fold into the first
  block's register. Moving `tpages` after func_800CE8F0, making it outlive the
  join (final block via tpages), a frame local, and a do/while diagnostic all
  stay at 0x598..0x5A4.
- A decomp-permuter run (darwine scratch/a4ring, 12.5k iterations) bottomed
  out at score 260.

## Retry (agent 5, 2026-10-04, near-miss pass 2)

Still 0x59C, but the align2 count (s-registers normalised, ctc2 spelling
ignored) went from 43 to 22 lines. Plain C changes, all kept in the
candidate:
- First parameter block in the order parameter00, parameter02, extent_x,
  extent_y, `tpage = D_800E2850[D_800E11FA]` stored directly, palette,
  parameter06, parameter0A, depth (brute force over the 120 tail orders;
  this is the order that also matched room_m023 func_8018F710 and room_m273
  func_80194E6C).
- `i = D_800E27EC - 4; angle = (i << 10) / 12;` in the model branch: the
  loop counter `i` crosses calls in mode 1, so it is the global pseudo that
  gets s1 and fills the bnez delay slot (`addiu s1,v1,-4`) as retail.
- `D_80196094 = *state;` before the anchor copy gives the early
  `lhu a3,0(s1)`.

Left: the block after func_800CE8F0. Retail loads D_800E27EC into a0,
computes spin.z (negu) before the `<< 10` of the division and keeps
`la s3, D_800E2850` right after the call; here the magic constant and the
multiply are scheduled above the spin stores (mult latency fill) and the
base set still sinks to the end of the block (birthing). Moving the
`tpages` statement anywhere in the block, all 120 orders of the spin
stores and the tpages line, a block-local frame copy and `i` as the frame
copy (s5 save comes back but the order breaks, 27) did not help.

## Retry (agent 5, 2026-10-04, near-miss pass 3)

Tried a multi-set `tpages` (two `tpages = D_800E2850;` sets so sched1 no
longer treats the base as a birthing insn): 16 combinations of a set at
the top of case 2, `tpages[D_800E11FA]` in the first block, a second set
inside the `D_800E27EC >= 4` block and `tpages[D_800E11E8]` in the final
block. Only the variant with a set at the top plus a set in the model block
has the right size, at 88 diffs (frame 8 bytes smaller, tpages in a saved
register from the start). Kept the 0x59C candidate unchanged.
