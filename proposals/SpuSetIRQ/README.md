# SpuSetIRQ C reconstruction

`candidate.c` reconstructs all of SpuSetIRQ using the shared `SpuRegs.spucnt`
field and a 32-bit unsigned timeout counter. Mode 0 clears IRQ enable, mode 1
sets it, and mode 3 clears then sets it. Other modes are returned unchanged.
Each wait reports an error after the 3841st unsuccessful control-bit read.
The apparently reversed IRQ/ON and IRQ/OFF diagnostic labels are retained
through the original string symbols.

Stock GCC 2.7.2 gives **97.46835%** against
`expected/build/USA/asm/USA/main/psyq/libspu/s_si.s.o` (316 retail bytes).
All register accesses and control-flow operations match except the first
error-string address: its ADDIU is before the jump rather than in the jump's
delay slot. An extra nop shifts the remainder of the function by four bytes.
The `_spu_RXX` name is the existing linker alias for D_8009B3FC.
There are no pins, empty barriers, instruction ASM, or toolchain patches.

GCC281 with unsplit addresses gives 95.94936%; the split-address configuration
with `-fcall-used-$1 -fno-schedule-insns2` gives 77.3924%. The 2.21 assembler
option does not improve the GCC272 candidate. Production remains ASM.

`verify_behavior.py` compares retail MIPS and the compiled C object with
modeled SPU control-bit transitions. Sixty cases cover modes 0/1/3 and invalid
modes, both initial register patterns, immediate and delayed acknowledgments,
and both sides of the timeout boundary. It compares return values, every
16-bit register read/write and diagnostic arguments, and separately asserts
expected timeout results. A negative control changing the timeout limit
from 0xF00 to 0xF01 is rejected. These are finite model tests, not physical
SPU hardware verification.

```sh
tools/scripts/cc.sh proposals/SpuSetIRQ/candidate.c /tmp/SpuSetIRQ.o
python3 proposals/SpuSetIRQ/verify_behavior.py /tmp/SpuSetIRQ.o
```

The checker requires Unicorn, pyelftools and MIPS binutils. The existing
`target_source.txt` refers to an older ASM path; the current target is the
`s_si` path above. The source algorithm was cross-checked with the local
psy-q-decomp-square `src/spu/s_si.c` reconstruction and retail instructions.

## Scheduling follow-up (2026-09-10)

`scheduling-audit.json` records 40 successful stock compilations and two
rejected option combinations. None is exact; production remains ASM and the
maintained source is unchanged. The best remains GCC272 at 97.46835%, 320
bytes against 316 retail bytes. This audit adds no decompiled-function credit.

Besides the existing source, the common-error-label shape assigns a local
`char *error` to the appropriate message, jumps to `timeout`, then calls
`printf(D_8001208C, error)` and returns -1. It reduces text to 284 bytes with
GCC272 or 268 with GCC281, but changes control flow and register allocation
relative to retail. It is not a scheduling-only fix.

The GCC281 split-address source hoists format/message address high halves
before each wait loop and retains the register-block address high half in a2.
Pinning the mode copy to a0 improves register allocation slightly but does not
prevent that hoisting. O1, scheduler/CSE/strength-reduction options, and the
stock AT allocation option do not close the gap. Their percentages describe
object alignment only; these variants were not behavior-validated or accepted.
The two rejected option combinations are recorded as errors, not test results.

Flags in the audit were supplied through CC1_FLAGS markers for the base/goto
matrix and PE_CC1_EXTRA_FLAGS for the mode-pin matrix. No compiler or MASPSX
source was changed. Further work needs a source lifetime/scheduling solution;
repeating these option combinations alone does not advance the match.
