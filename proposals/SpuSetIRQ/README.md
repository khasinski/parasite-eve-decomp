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
