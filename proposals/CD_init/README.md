# LIBCD CD_init

Complete C reconstruction of retail `0x8007BBFC` (480 bytes), using the
shared callback prototypes, word-sized CD status and `CdInterruptEvents`.
No register pins, barriers or instruction assembly are needed to express
its behavior. Stock GCC 2.8.1 with unsplit addresses scores 89.833336% in
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

Remaining differences are diagnostic/store delay slots, independent zero
argument registers and the final comparison/epilogue. GCC 2.7.2 scores
82.60833%. Disabling GCC 2.8.1's first or second scheduling pass scores
85.73333% or 87%; disabling expensive optimizations does not improve the
selected source.
