# CD_datasync reconstruction

Retail: `0x8007BDDC`, 360 bytes. The candidate remains outside the production
manifest and has not passed a behavioral oracle or full binary match.

The body consumes `mode` in a0 and returns -1 on timeout, 0 when DMA is idle,
or 1 when DMA remains busy and mode is nonzero. Mode zero repeats the wait.
The SDK LIBCD.H declaration is `int CdDataSync(int mode)`; both production
wrappers now share the correct low-level prototype.

The timeout initializes a VSync deadline to current time + 960 and a poll
counter to zero. Each iteration checks the signed deadline first, then tests
the previous counter value against 0x3c0000 while incrementing it. Timeout
prints the operation and command/status names, calls CD_flush, and fails.
The DMA busy test reads bit 24 through the volatile pointer D_8009B2C0.
These statements are reconstructed directly from the retail disassembly.

## Current experiment

`candidate.c` uses stock GCC 2.8.1 with its default split addresses and no
pins or barriers. objdiff against
`expected/build/USA/asm/USA/main/psyq/libcd/CD_datasync.s.o` gives **72.48889%**.
The outstanding differences include address hoisting, saved-register
allocation, symbolic stores, and timeout printf argument scheduling.

Measured alternatives (2026-09-09):

- GCC 2.7.2 default: 69.21111%.
- GCC 2.8.1 unsplit: 55.5%.
- Default split with no expensive optimizations: 72.03333%.
- Default split with no second scheduling pass: 62.488888%.
- All 16 unsplit combinations of disabling strength reduction, force-mem,
  expensive optimizations and cse-follow-jumps: at most 62.077778%.
- Explicit local command/status/name pointers and timeout limit kept live by
  one input-only empty asm inside the loop: 73.72222% unsplit.
- Pinning those values and mode to the retail saved registers and moving
  initialization after VSync: 73.45556% unsplit, 72.51111% split. Pins alone
  do not resolve the scheduling differences, so these variants are not kept.

Do not promote this candidate based on its percentage. Next work should
reconstruct the timeout helper's data accesses and lifetimes, then verify
its boundary conditions against retail before considering production use.
