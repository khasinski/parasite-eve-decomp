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

The initial clean source under stock GCC 2.7.2 gave **94.57447%** against
`expected/build/USA/asm/USA/main/psyq/libapi/VSync.s.o` (376 retail bytes).
That initial version used no pins or barriers. No instruction ASM or
toolchain modifications are used.
GCC281 with unsplit addresses gives 91.32979%, adding no-schedule-insns gives
92.180855%, and default split addresses gives 78.85107%. Experimental mode
and target-register pins did not outperform the clean GCC272 source and
were discarded. Remaining differences include argument computation,
register allocation and return scheduling. The production promotion described below supersedes this initial result.

The candidate now includes the static `v_wait` helper in the same source.
Its initial GCC272 version had 95.789474% match without the production helper's global a0 register
variable or manually assembled error-string address. It uses a volatile
stack timeout, reports expiration, and clears the PAD/root-counter settings.
Its remaining instruction difference is the expanded error-string address
before the puts call instead of in the call delay slot, adding one nop.
Production still retains its existing helper and constraints.

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

## Original VSYNC translation unit

Psy-Q 4.6 LIBETC.LIB's VSYNC.OBJ proves the combined boundary. Its 528 text
bytes match retail 0x80073A44..0x80073C54 modulo exactly 42 relocations;
all non-relocation bits are identical. VSync is at offset zero and calls the
local helper at offset 0x178 twice. VSync occupies 376 bytes, v_wait 152.
The public definitions are VSync and the Hcount data symbol; v_wait is local.

The SDK also contains a 16-byte timeout string and 32 initialized data bytes.
The register pointers, Hcount and previous VBlank counter occupy data offsets
8, 12, 16 and 20. The candidate references existing retail storage; it does
not yet reconstruct all initialized-data ownership. In particular, the first
eight data bytes are not represented by invented padding fields.

The initial combination preserved VSync's 94.57447% score and all 80
modeled VSync cases. The checker resolves the helper's actual candidate entry
instead of assuming the retail address. These tests still model v_wait;
they do not claim runtime coverage of the helper body. Its reconstruction
is currently checked by full instruction comparison.

## Matched VSync production source

`src/main/psyq/libapi/VSync.c` now replaces the existing VSync ASM segment
without changing its boundaries. All **376 linked bytes match retail** under
stock GCC 2.8.1 with unsplit addresses. The full-TU candidate is updated to
the same VSync source strategy; its local v_wait is still only 92.63158%
under this compiler, so production retains the existing GCC272 helper.
This is not yet a matched merged VSYNC translation unit.

One v0 pin retains the target-frame calculation. Two empty barriers keep the
second call's frame argument after the counter load and prevent reuse of
its register as the increment constant. Each retained constraint was tested
individually on the final source: removing the pin gives 94.84042%, removing
the counter/argument barrier 97.87234%, and removing the tied frame-argument
barrier 98.297874%. An extra a1 pin and an earlier arithmetic barrier were
removed without affecting the match. Unresolved symbol aliases make objdiff
show 99.57447%; comparing all 376 linked bytes establishes the exact result.
The 80 modeled behavior cases also pass.

Production validation: main retail SHA-1, all 191 overlay SHA-1 checks,
source/organization/debt gates, and fresh progress/report audits pass.
The audited PsyQ result is 141/254 functions and 44.75% code.
