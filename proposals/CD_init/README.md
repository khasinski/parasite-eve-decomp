# LIBCD CD_init

Complete C reconstruction of retail `0x8007BBFC` (480 bytes), using the
shared callback prototypes, word-sized CD status and `CdInterruptEvents`.
No register pins, barriers or instruction assembly are needed to express
its behavior. Stock GCC 2.8.1 with unsplit addresses scores 95.958336% in
objdiff. Production remains assembly pending a byte-exact match.

The function resets software state and callbacks, registers the CD
interrupt dispatcher, acknowledges pending controller interrupts, initializes
the event state and hardware control, then sends commands 1, 10 and 12.
Command 1 is repeated if the updated status has bit 0x10 set. Its return
value is ignored; failures of command 10 or 12 abort initialization.
Success additionally requires CD_sync(0, NULL) to return 2.

```sh
tools/scripts/cc.sh proposals/CD_init/candidate.c /tmp/CD_init.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/CD_init.s.o \
  -2 /tmp/CD_init.o -o /tmp/CD_init.json
python proposals/CD_init/verify_behavior.py /tmp/CD_init.o
```

The verifier requires Unicorn and pyelftools. It runs 240 retail/candidate
comparisons across pending-interrupt lengths, low/high status bits, failed
commands (including ignored command-1 errors), and synchronization results.
It checks return values, cleared state, dispatcher registration, every MMIO
access, and command arguments/order. External calls and interrupt hardware
are modeled. Calls are serviced outside emulation to avoid Unicorn restarting
a code hook when a modeled command writes emulated RAM. Removing the
command-10 failure check is rejected by the negative control.

Remaining differences are diagnostic/status-store delay slots, independent zero
argument registers and stack restoration in the epilogue. The final
comparison now matches by using an explicit non-success test and early
return, rather than a conditional expression (previously 89.833336%). GCC 2.7.2 scores
82.60833% for the earlier conditional-expression source. With the selected
source, disabling either scheduling pass scores 90.625%; disabling peepholes
or CSE follow-jumps/skip-blocks leaves 94.625% unchanged. Explicit constrained
zero-argument helpers regressed the earlier source, and a constrained return
was worse than this ordinary C branch. No constraints were retained.

The candidate now uses the same u32 word-control access type as CD_flush.
This moves the single 0x1325 store into the CD_cw call delay slot and removes
one extra NOP, raising the score from 94.625% to 95.958336% without pins or
barriers. The same score holds in the nine-function combined source. The
240-case suite passes there, along with 24 integrated initialization cases
executing real CD_cw, CD_sync and getintr; see ../libcd_commands/README.md.
