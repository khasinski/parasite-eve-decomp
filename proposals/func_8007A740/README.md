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
  -1 expected/build/USA/asm/USA/main/psyq/libcd/func_8007A740.s.o \
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
the former incomplete verifier; source match remains 71.53012%.
