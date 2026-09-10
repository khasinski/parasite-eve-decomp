# CdControlB — matched

The production source is now
[`src/main/psyq/libcd/CdControlB.c`](../../src/main/psyq/libcd/CdControlB.c).
Stock GCC 2.7.2 with `-fno-schedule-insns` and the GNU assembler profile
reproduces all **332 retail bytes** after linking. The inline retry helper
retains the callback and command-table lifetimes, retries at most four times,
and polls `CD_sync` only after successful submission. No register pins, empty
barriers, instruction ASM, or compiler patches are used.

The full `main.exe` retains retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
The candidates and measurements below are historical experiments.

# Synchronous LIBCD command retry wrapper

`candidate.c` reconstructs the 332-byte assembly function at 0x8007A740 in
ordinary C using the shared LIBCD callback, status and command declarations.
The old `variant_fp_stack.c` is a historical pinned experiment with unsupported
postpass flags; it is not the current candidate or validation baseline.

Psy-Q 4.6 `LIBCD.LIB` confirms the public name and TU: `S_016.OBJ` exports
`CdControlB` at text offset 0x270 after `CdControl` and `CdControlF`. The
object's final zero word is section padding and is not part of the 332-byte
retail function body.

The wrapper allows four attempts (counter 3 through 0), suppressing the sync
callback during optional setup commands. Unless the low command byte is one,
a status byte with bit 0x10 triggers command one; that command's return value
is ignored. A nonnull parameter plus a nonzero command-table entry triggers
command two, whose failure retries without issuing the requested command.
Before issuing the requested command the saved callback is restored. A zero
command result leads to CD_sync(0, result), with success only for sync result
two. Exhaustion restores the callback and returns zero. The full command is
truncated to its low byte for the table lookup and command call.

The retained source follows the byte-exact public PsyQ `CdControlB` source
shape from decomp.me scratch B5sWY: the retry loop is an inline helper and the
outer function converts its result through `CD_sync`. After adapting only the
retail symbols, a named `-1` retry boundary reproduces the retail `$s7`
lifetime and loop delay slot. Stock GCC272 now produces the exact 332-byte
size and scores **91.86747%** against this game's body. This replaces the
earlier 71.53012% candidate. It has no pins, barriers or instruction asm and
is not yet a production replacement.

```sh
tools/scripts/cc.sh proposals/func_8007A740/candidate.c /tmp/cd-retry.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/CdControlB.s.o \
  -2 /tmp/cd-retry.o -o /tmp/cd-retry.json
```

`verify_behavior.py` now passes **384 retail-comparison cases**, covering low-byte
command truncation, null/non-null parameters, lid status, optional location
commands, four command-result patterns, and three sync results. It compares
callback-clear/restore instructions, arguments and callback state at every
modeled API call, return values and result-buffer effects. Mutants with only
three attempts, the wrong sync success code, or missing callback restoration
are rejected by assertions. CD_cw and CD_sync remain modeled external APIs;
this does not yet validate the wrapper with their real bodies.

```sh
python proposals/func_8007A740/verify_behavior.py /tmp/cd-retry.o
```

The initial retail CPU exception was isolated to Unicorn 2.1.4's memory-write
hook while executing SW in a branch delay slot. The same cases pass with that
hook removed. The final verifier observes the callback SW via an instruction
hook, records its computed destination and source register, and executes the
original store unchanged. It also checks stored callback state at API entries
and at return. No retail function instructions or branches are patched. Only
the modeled external API entries use the documented NOP stubs. This replaces
the former incomplete verifier; the retained source remains at 91.86747%.

A compiler audit rejects the version-only explanation for the remaining gap.
The same source produces 332 bytes at 82.686745% with native GCC 2.8.0 and
324 bytes at 81.24097% with GCC 2.8.1; GCC 2.7.2 remains decisively closest.
Disabling the first scheduler pass gives 91.24097%, the second gives
71.80723%, and disabling delayed branches gives 69.26506%. A named ordinary
`one` local does recover the retail `$fp = 1` lifetime and the exact `$v0`
loop-exit values, but changes initialization order and grows the function to
340 bytes (90.89156% at best), so it is retained only as source-shape evidence.
`register volatile` instead spills the value and is rejected.
