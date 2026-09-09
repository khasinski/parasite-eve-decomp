# LIBCD CD_ready

Complete C reconstruction of retail `0x8007B290` (712 bytes). Stock GCC 2.8.1
with unsplit addresses and `-fno-expensive-optimizations` scores 89.15169%.
There are no register pins, barriers or instruction ASM. Production remains
assembly until the entire function matches.

After checking timeout and dispatching pending callbacks, CD_ready consumes
the end event first, clears that byte and copies the end-response buffer.
Only if no end event exists does it consume and clear the ready byte, copying
the ready-response buffer. The other event remains pending. Mode 0 waits;
nonzero mode returns 0 when neither event exists. The consumed event value is
returned unchanged. Timeout precedes event consumption, including nonblocking
calls and already-pending events.

The timeout, callback dispatcher and eight-byte copy are shared with CD_sync
and CD_cw through `../libcd_bios_helpers.h`. All three preserve their previous
match percentages after factoring the dispatcher. Disabling expensive
optimizations improves CD_ready from 86.146065% to 89.15169%. A byte-sized
local status or explicit end-event pointer regresses it (86.42135% and
88.505615%). Remaining differences include status masking, event-pointer
register allocation and call scheduling.

LIBCD BIOS_1.OBJ exports CD_ready at 0x7E4 and CD_cw at 0xAAC. The intervening
712 bytes match retail: 123 identical words, 55 differences confined to
relocation fields. Together with the verified CD_sync range, this establishes
their LIBCD classification. The manifest now places both under psyq/libcd;
no new boundary or production C match is introduced.

```sh
tools/scripts/cc.sh proposals/CD_ready/candidate.c /tmp/CD_ready.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/CD_ready.s.o \
  -2 /tmp/CD_ready.o -o /tmp/CD_ready.json
python proposals/CD_ready/verify_behavior.py /tmp/CD_ready.o
python proposals/CD_cw/verify_sdk.py /path/to/BIOS_1.OBJ CD_ready
```

The verifier requires Unicorn and pyelftools. Its 1008 cases compare retail
and candidate for mode 0/1/-1, combinations of ready/end events, nullable
output, callback combinations, polled completion, disk error and both timeout
thresholds. It compares consumed events, all response buffers, timeout state,
external calls and MMIO traces. Reading the ready byte in the end-event branch
is rejected as a negative control. Hardware and external functions are modeled;
infinite waits and signed-counter overflow are outside this finite test set.

The same match is retained when compiling with the other two command/wait
functions in `../libcd_commands/candidate.c`. This verifier resolves the
candidate entry from its ELF symbol, and its full suite passes on that
combined object as well.
