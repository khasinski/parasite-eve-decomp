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

## Current reconstruction

`candidate.c` uses stock GCC 2.8.1 with
`-mno-split-addresses -fno-expensive-optimizations` and no pins or barriers.
Objdiff reports **94.666664%** for the standalone candidate. A distinct
`savedMode` local recovers the retail `$s2` parameter lifetime and prologue.
Passing the interrupt-event state explicitly, initialized before the event-name
table, recovers the retail `$s1`/`$s0` allocation. It uses shared CD
callback/status types and the correct
void CD_flush prototype. The former two-byte nonvolatile event array is now
the shared CdInterruptEvents structure with volatile sync/ready members.

Command/event-name pointers are initialized after the first VSync. A local
poll limit is passed into the timeout helper, retaining its lifetime across
subsequent calls. Shared fixed-limit timeout code first gave 86.666664%; the
explicit limit raised that to 93.577774% without constraints. The additional
lifetimes raise the current result to 94.666664%. The remaining differences
are instruction scheduling and symbolic delay slots.

The parameterized helper stays local: replacing the common fixed-limit helper
with it regressed CD_sync (89.90625%), CD_ready (85.6236%) and CD_cw (90.586876%).
The other functions keep their existing source and scores. All 15 combinations
of pins on mode/limit/command names/event names scored at most 94.22222%; none
was retained. Earlier event-state-argument trials without the recovered mode
lifetime scored 88.45556%; the combined lifetime and initialization order is
the retained improvement.

## Earlier experiments

The old split-address candidate scored 72.48889%. The following measurements
refer to that earlier source, before the shared types and revised lifetimes.

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

The oracle checks the reference EXE SHA-1, links the candidate in separate emulated RAM at 0x80180000, and runs both versions as MIPS code. It models VSync, puts, printf,
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

The verifier also accepts the combined LIBCD object, resolves its entry and
modeled CD_flush address from ELF symbols, and imports only undefined retail
symbols. All 45 cases and the four negative controls pass with the new source;
combined-object initialization (24 cases) and CD_cw/CD_sync/getintr interaction
(3072 cases) also pass. CD_flush remains modeled for this timeout check.

## SDK provenance

`python proposals/CD_cw/verify_sdk.py /path/to/BIOS_1.OBJ CD_datasync`
verifies the complete SDK range 0x1330..0x1498 against retail 0x8007BDDC:
360 bytes, 54 identical words and 36 differences confined to relocation
fields. The following export is CD_set_test_parmnum. This is direct evidence
for keeping CD_datasync in the same BIOS_1 reconstruction, not a claim that
the reconstructed C already matches those bytes.
