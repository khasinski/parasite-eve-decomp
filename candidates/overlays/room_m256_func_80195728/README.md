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

## Rescore (agent 6, 2026-10-04, lev.py)

lev 43 (retail 360 words, mine 358). Score with
`lev.py <obj> room_m256 0x6740 0x5A0 -v 60`. Besides the missing s5 save
(tpages, see above) the visible edits are the block after func_800CE8F0.
cc1 -dS for that block: insn 293 (the mult of `/ 40`) carries LAUNCH
priority and is placed with 5 stalls only after the spin stores
(269/272/283/286, priority 4-5) have filled its shadow; retail keeps all
four spin stores before the magic constant and the mult (no latency fill)
and puts `la s3, D_800E2850` right after `lw a0, D_800E27EC`. Both look
like the region was split into its own scheduling unit in retail, but
there is no label, loop note or asm there. Note that spin is never read
(its stores are dead), so the original may have passed &spin somewhere
that was dropped from this draft. Tried without effect (all lev 43): a
block-local frame copy, spin.flags before spin.z, the angle computed
into `i` before or after the stores, tpages set after the division or
inside the model branch; `u16 *tpages = D_800E2850;` at declaration is
lev 58. `-fno-schedule-insns` (diagnostic only) is lev 122, so retail did
run sched1.
Also: room_model_pulse.h declares `func_800CE610_pulse
__asm__("func_800CE610")`, an alias counted as crutch debt; switch it to
the shared `void *func_800CE610(void *)` prototype before landing.

## Retry (agent 13, 2026-10-04): still lev 43

- sched.c `birthing_insn_p` gives the max priority only to a set whose
  pseudo has `reg_n_sets == 1`, so retail's early `la s3, D_800E2850` means
  the tpages pseudo was set more than once after flow. Every second constant
  set tried is folded away before flow (cse knows the value, or the set is
  single-use and combine folds it into the absolute address): a second
  `tpages = D_800E2850` in the final block, and reusing tpages for
  `D_800E1204` in the model block both stay at lev 43 with the same edits.
  The pointer reuse that matched scene_e08 func_8019104C needs a second
  value that survives (a non-constant, or one used twice).
- `GteRotation spin = {0, 0, -D_800E27EC << 6, 0};` in a block scope after
  func_800CE8F0 reproduces retail's zero-first store order but costs 8 frame
  bytes and moves the division (lev 70).
- When landing: room_spark.h already declares `RoomDampedSpark
  *func_800CE610(void *pool)`, so dropping the `func_800CE610_pulse` alias
  needs the particle record to be reachable without a cast (the README's
  "shared void * prototype" does not exist in that header).

## Type decision (agent 13, round 2): alias removed, still lev 43

room_spark.h (included by room_model_pulse.h) declares
`RoomDampedSpark *func_800CE610(void *pool)`, but room_m256 uses the slot as
a 6-byte ring particle (frame, offset, scale). The candidate now calls the
shared declaration and adapts the view through a `void *slot` local
(`slot = func_800CE610(pool); child = slot;`), no cast and no alias; the
`func_800CE610_pulse` line is gone from room_model_pulse.h. Code is
unchanged (lev 43).

## Multi-set diagnostic (agent 16, 2026-10-04): lev 43, diagnostic lev 32

- Confirmed the birthing explanation with a throwaway second set that
  survives flow: `tpages = (u16 *)D_800F33E0; D_801960A8 = tpages;` in case 1
  (nonsense code, not a candidate) makes the tpages pseudo multi-set, and
  sched1 then leaves `la s3, D_800E2850` right after func_800CE8F0 as in
  retail (lev 32 despite three extra words of junk). So a plain-C source
  needs the same pointer variable to be assigned a second, non-constant
  value somewhere in the function (a constant second set is folded by cse
  and the dead set is dropped by flow before reg_n_sets is counted).
  The only other pointer local that takes a non-constant value is the
  case 1 particle slot (`void *slot`), whose type differs.
- Left after that: the /40 division chain is scheduled above the four spin
  stores (retail stores spin first, then loads the magic constant), and the
  s1/s2 swap between `state` and &position.
