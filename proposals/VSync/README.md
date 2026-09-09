# VSync C reconstruction

`candidate.c` reconstructs retail VSync at 0x80073A44 using named aliases
for the GPU-status pointer, HBlank-counter pointer and previous counter
samples. MMIO and sampled HBlank storage are volatile. The elapsed result
is a 16-bit wrapped counter difference. Negative modes return the global
VBlank count; mode 1 returns elapsed HBlanks without waiting; other modes
perform the two observed v_wait calls and update both saved counters.

The retail function repeatedly samples HBlank until two consecutive reads
agree, both before computing elapsed time and after waiting. It checks GPU
status bit **0x400000** before waiting for bit 31 to change. These details
differ from the local psy-q-decomp-square `src/etc/vsync.c` reconstruction,
which uses a direct counter read and checks 0x80000. Retail instructions
are authoritative for this candidate.

Stock GCC 2.7.2 gives **94.57447%** against
`expected/build/USA/asm/USA/main/psyq/libapi/VSync.s.o` (376 retail bytes).
No pins, barriers, instruction ASM or toolchain modifications are used.
GCC281 with unsplit addresses gives 91.32979%, adding no-schedule-insns gives
92.180855%, and default split addresses gives 78.85107%. Experimental mode
and target-register pins did not outperform the clean GCC272 source and
were discarded. Remaining differences include argument computation,
register allocation and return scheduling. Production remains ASM.

The adjacent v_wait is already C in the production tree, but has its own
compiler constraints. This candidate does not claim to restore the complete
original VSYNC object or to remove those constraints.

`verify_behavior.py` compares retail MIPS with the compiled C using modeled
GPU/counter reads and v_wait calls. Eighty cases cover query and wait modes,
steady and unstable counter reads, 16-bit subtraction wraparound, and GPU
status patterns including 0x400000 and 0x80000. Return values, saved state,
every modeled register read and wait-call arguments agree. The checker also
asserts expected return values and wait counts. Changing the GPU gate to the
donor's 0x80000 is rejected. Physical display timing and v_wait internals
are outside this finite model.

```sh
tools/scripts/cc.sh proposals/VSync/candidate.c /tmp/VSync.o
python3 proposals/VSync/verify_behavior.py /tmp/VSync.o
```

The checker requires Unicorn, pyelftools and MIPS binutils.
