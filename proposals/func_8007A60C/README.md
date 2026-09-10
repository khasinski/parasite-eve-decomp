# CD command retry with mode 1

Retail range: 0x8007A60C..0x8007A740 (308 bytes).

Psy-Q 4.6 `LIBCD.LIB` identifies this as `CdControlF`: `S_016.OBJ` exports it
at text offset 0x13C, immediately after `CdControl` and before `CdControlB`.

The function has two arguments: command and parameter pointer. Like
CdControl, it temporarily clears the completion callback, optionally
issues status command 1 and location command 2, restores the callback, and
retries up to four times. Unlike that wrapper, every result-buffer argument
is null and the final CD_cw call uses mode 1. Exhaustion returns 0; successful
submission returns 1. The callback is restored on both exits.

`candidate.c` now keeps the control-flow and temporary shape of the adjacent,
fully matched `CdControl` wrapper. It removes that function's result-buffer
parameter and changes only the final `CD_cw` mode to one. Stock GCC 2.8.1 with
`-mno-split-addresses -fno-schedule-insns` emits the target's exact 308-byte
size and scores 97.72727% upstream objdiff. Making the constant one a normal
loop invariant removes the false long-lived `0xFF` mask, two extra loop
instructions, and two empty constraints from the previous candidate. GCC then
assigns every saved register exactly as retail. The remaining differences are
zero argument register selection and the callback-clear store not entering the
comparison branch's delay slot. The candidate retains two register bindings
and one empty constraint; it is not a production match.

`constrained_registers.c` is an experiment derived from the adjacent matched
wrapper. Upstream objdiff scores it 89.72727%. Its remaining differences
include construction of the command-table pointer, callback-clear scheduling,
and copying zero from a2 into a3 at the location-command call. The target
places the callback clear in the comparison branch's delay slot; the candidate
emits the symbolic store before the branch and leaves a nop. Enabling split
addresses changes callback-address hoisting and allocation, rather than
providing a drop-in fix. Testing all 64 subsets of its six pins did not match.

Production remains ASM. Do not merge this into CdControl.c until all three
functions match together using the same stock pipeline. The linker script
allows standalone relocated byte comparison at the retail address.

Reproduce the constrained comparison:

```sh
tools/scripts/cc.sh proposals/func_8007A60C/constrained_registers.c /tmp/cd-mode1.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/asm/USA/main/psyq/libcd/CdControlF.s.o -2 /tmp/cd-mode1.o -o /tmp/cd-mode1.json CdControlF
mipsel-none-elf-ld -T proposals/func_8007A60C/diagnostic.ld /tmp/cd-mode1.o -o /tmp/cd-mode1.elf
```

A follow-up check of the simple candidate with only the failure-status
barrier, and of the constrained candidate's CSE/scheduler flags, did not
produce an exact match. Moving the table-pointer constraint into a temporary
or changing pointer-expression syntax likewise failed. These results do not
establish an impossibility; the next attempt needs a different source shape.
