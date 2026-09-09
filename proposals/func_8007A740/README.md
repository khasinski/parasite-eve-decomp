# Synchronous LIBCD command retry wrapper

`candidate.c` reconstructs the 332-byte assembly function at 0x8007A740 in
ordinary C using the shared LIBCD callback, status and command declarations.
The old `variant_fp_stack.c` is a historical pinned experiment with unsupported
postpass flags; it is not the current candidate or validation baseline.

The wrapper allows four attempts (counter 3 through 0), suppressing the sync
callback during optional setup commands. Unless the low command byte is one,
a status byte with bit 0x10 triggers command one; that command's return value
is ignored. A nonnull parameter plus a nonzero command-table entry triggers
command two, whose failure retries without issuing the requested command.
Before issuing the requested command the saved callback is restored. A zero
command result leads to CD_sync(0, result), with success only for sync result
two. Exhaustion restores the callback and returns zero. The full command is
truncated to its low byte for the table lookup and command call.

Stock GCC281 default addressing gives **71.53012%** objdiff. GCC272 scores
69%, GCC281 unsplit 70.69879%, with no improvement from disabling expensive
optimizations in the unsplit profile. An earlier direct-return source scored
64.90362% under GCC281 unsplit. The retained source uses a shared completion
path. It has no pins, barriers or instruction asm and is not a production
replacement.

```sh
tools/scripts/cc.sh proposals/func_8007A740/candidate.c /tmp/cd-retry.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/func_8007A740.s.o \
  -2 /tmp/cd-retry.o -o /tmp/cd-retry.json
```

**Behavioral verification is incomplete.** `verify_behavior_incomplete.py`
contains the intended retail comparison with CD_cw/CD_sync modeled through
NOP entry stubs. It currently fails while executing the unmodified retail
function on its first case: after the callback-clear store, Unicorn reaches
0x8007A7D4 and raises a CPU exception with PC zero before any modeled API call.
Thus no passing-case count or semantic equivalence is claimed. A diagnostic
attempt to stop/resume at that branch target did not resolve it and was not
retained. The next verification step must resolve that emulator failure or
use an independent executor, then exercise all retry and restoration paths.
