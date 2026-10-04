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
