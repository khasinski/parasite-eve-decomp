# Task_GpuPackPrimColor: unmatched custom-ABI routine

The retained C candidate scores **1930** with the project's asm-differ
Levenshtein configuration (13 target instructions, 22 candidate instructions).
It is not integrated. Stock native GCC 2.7.2 and stock MASPSX 2.56 were used on
darwine with `-O2 -G0 -funsigned-char -mips1 -mcpu=3000`.

The previous candidate scored 2405 and expressed the scaling with a 32-bit
product. The original combines HI and LO from signed `mult`, then selects bits
16 through 47. The retained candidate therefore uses a signed 64-bit product,
shifts it, and truncates the result before adding the starting value. It also
uses the existing callee's no-argument declaration instead of inventing two
formal arguments. The routine computes a scaled value from the low 16 bits of
the queue result; its historical GPU-related name is not evidence of purpose.

The original at `0x80070DD0` has a nonstandard calling sequence:

- Copies the return address into `v1` using `or` before calling
  `Task_GpuFlushPrimQueue` (`0x80070D6C`).
- Keeps the start and delta in `a0` and `a1` across that call. The current callee
  implementation preserves them, but ordinary C call-clobber rules do not.
- Restores `ra` from `v1` and uses no stack frame.
- Uses trapping `sub` for the delta and trapping `add` for the final result.

The stock GCC 2.7.2 MIPS backend's `addsi3_internal` and `subsi3_internal`
patterns emit `addu` and `subu` (`config/mips/mips.md`, lines 376–384 and
613–621 in the inspected source). Register allocation alone cannot change
those opcode choices. `compute_frame_size` also accounts for outgoing call
arguments. Global `ra`/`v1` bindings remove the normal RA spill in an experiment,
but still leave a 16-byte frame and do not solve the arithmetic or argument
lifetime requirements.

A tempting pinned variant scores 1325, but is **rejected**: GCC eliminates the
required delta computation and reuses argument registers across the call.
The lower score does not make it a valid implementation. No compiler patches,
CPU inline assembly, or special ABI flags were added to the retained candidate.

Research and reproducible scoring commands are in `scratch/gpu_pack_score`
and `/home/hasik/fx-search-archives/gpu_pack_score` on darwine. `score.py`
links to the retail addresses and reports the Levenshtein score; it does not
use byte distance as an acceptance metric. No permuter was started for this
structural ABI investigation.
