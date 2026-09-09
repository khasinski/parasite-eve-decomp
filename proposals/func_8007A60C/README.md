# CD command retry with mode 1

Retail range: 0x8007A60C..0x8007A740 (308 bytes).

The function has two arguments: command and parameter pointer. Like
func_8007A4D0, it temporarily clears the completion callback, optionally
issues status command 1 and location command 2, restores the callback, and
retries up to four times. Unlike that wrapper, every result-buffer argument
is null and the final CD_cw call uses mode 1. Exhaustion returns 0; successful
submission returns 1. The callback is restored on both exits.

`candidate.c` expresses that behavior without pins, barriers or instruction
ASM. Stock GCC 2.8.1 with -mno-split-addresses emits 0x128 bytes, twelve
bytes shorter than the target. This is not a production match.

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
