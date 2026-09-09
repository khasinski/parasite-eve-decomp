# CD_datasync reconstruction

Retail: `0x8007BDDC`, 360 bytes. The candidate remains outside the production
manifest. It passes the finite behavioral checks below but does not match
the full binary.

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

## Differential behavior check

Run with a Python environment containing unicorn and pyelftools:

```
tools/scripts/cc.sh proposals/CD_datasync/candidate.c /tmp/cd-datasync.o
python proposals/CD_datasync/verify_behavior.py /tmp/cd-datasync.o
```

The oracle checks the reference EXE SHA-1, links the candidate at the retail
address, and runs both versions as MIPS code. It models VSync, puts, printf,
and CD_flush and supplies a simulated DMA register through the retail global
pointer. It compares the return value, ordered external-call and DMA-read
traces (including printf's fifth stack argument), and all three timeout globals.

All **45 cases pass**: five mode values, immediate/delayed DMA completion,
deadline offsets 959/960/961, and counter values around 0x3c0000. Counter
boundary scenarios inject a counter value on the second modeled VSync call
to avoid millions of iterations; they do not model real interrupt behavior.
This is a finite function-level comparison, not hardware/timing verification
or a substitute for exact linked bytes.

Negative controls were compiled and rejected: changing deadline `>` to `>=`,
postincrement to preincrement, reversing the mode test, and using DMA bit 23
instead of bit 24. These establish that the exercised boundaries and DMA
hook can detect the corresponding reconstruction errors.

Further compiler experiments: disabling force-addr or rerun-cse-after-loop
did not improve the unsplit candidate. Expanding the constrained helper body
at its call site reduced the saved frame from 0x40 to 0x38 but still differed
from retail's 0x30 and scored 71.566666%; disabling force-mem, strength
reduction or expensive optimizations did not fix it. Do not repeat those
variants as an unexplored route.
