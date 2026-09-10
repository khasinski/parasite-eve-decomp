# CD command retry with mode 1

Retail range: 0x8007A60C..0x8007A740 (308 bytes).

The function has two arguments: command and parameter pointer. Like
func_8007A4D0, it temporarily clears the completion callback, optionally
issues status command 1 and location command 2, restores the callback, and
retries up to four times. Unlike that wrapper, every result-buffer argument
is null and the final CD_cw call uses mode 1. Exhaustion returns 0; successful
submission returns 1. The callback is restored on both exits.

`candidate.c` now keeps the control-flow and temporary shape of the adjacent,
fully matched `func_8007A4D0` wrapper. It removes that function's result-buffer
parameter and changes only the final `CD_cw` mode to one. Stock GCC 2.8.1 with
`-mno-split-addresses -fno-schedule-insns` emits the target's complete 0x134
bytes and scores 95.7013% upstream objdiff. A shared 16-bit `0xFF` mask for
the table index and final command conversion produces the target frame and
register lifetime. Five local instruction-difference groups remain.
The candidate retains three register bindings and five
empty constraints; it is not a production match.

`constrained_registers.c` is an experiment derived from the adjacent matched
wrapper. Upstream objdiff scores it 89.72727%. Its remaining differences
include construction of the command-table pointer, callback-clear scheduling,
and copying zero from a2 into a3 at the location-command call. The target
places the callback clear in the comparison branch's delay slot; the candidate
emits the symbolic store before the branch and leaves a nop. Enabling split
addresses changes callback-address hoisting and allocation, rather than
providing a drop-in fix. Testing all 64 subsets of its six pins did not match.

Production remains ASM. Do not merge this into command_retry.c until both
functions match together using the same stock pipeline. The linker script
allows standalone relocated byte comparison at the retail address.

Reproduce the constrained comparison:

```sh
tools/scripts/cc.sh proposals/func_8007A60C/constrained_registers.c /tmp/cd-mode1.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/asm/USA/main/psyq/libcd/func_8007A60C.s.o -2 /tmp/cd-mode1.o -o /tmp/cd-mode1.json func_8007A60C
mipsel-none-elf-ld -T proposals/func_8007A60C/diagnostic.ld /tmp/cd-mode1.o -o /tmp/cd-mode1.elf
```

A follow-up check of the simple candidate with only the failure-status
barrier, and of the constrained candidate's CSE/scheduler flags, did not
produce an exact match. Moving the table-pointer constraint into a temporary
or changing pointer-expression syntax likewise failed. These results do not
establish an impossibility; the next attempt needs a different source shape.
