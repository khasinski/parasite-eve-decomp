# LIBCD CD_ready

Promoted to `src/main/psyq/libcd/bios_poll.c`: all 712 bytes at retail
`0x8007B290` match with stock native GCC 2.7.2, GNU assembly and
`-fno-expensive-optimizations`. No register pins, barriers, instruction ASM,
compiler patches or assembly postpasses are used.

CD_sync and CD_ready occupy the contiguous retail range `0x6B810..0x6BD58`
and share timeout, interrupt-dispatch and response-copy helpers in this TU.
The SDK BIOS_1.OBJ exports them at `0x564` and `0x7E4`, with CD_cw next at
`0xAAC`; their instruction words agree with retail outside relocation fields.
This is a matched portion of BIOS_1.OBJ, not a claim that the whole object has
been reconstructed.

After checking timeout and dispatching pending callbacks, CD_ready consumes
the end event first, clears that byte and copies the end-response buffer.
Only if no end event exists does it consume and clear the ready byte, copying
the ready-response buffer. The other event remains pending. Mode 0 waits;
nonzero mode returns 0 when neither event exists. The consumed event value is
returned unchanged. Timeout precedes event consumption, including nonblocking
calls and already-pending events.

The poll-count helper preserves the pre-increment value and commits the
incremented counter separately. Together with explicit event-byte views and
the descending eight-byte copy loop, this produces the retail register
allocation and instruction order. The adjacent functions also match when
compiled together. The older local `candidate.c` and shared proposal helper
are retained as historical experiments; they are not the production source.

```sh
tools/scripts/cc.sh src/main/psyq/libcd/bios_poll.c /tmp/bios_poll.o
.venv/bin/python proposals/CD_ready/verify_behavior.py /tmp/bios_poll.o
make verify-clean
```

The verifier requires Unicorn and pyelftools. Its 1008 cases compare retail
and candidate for mode 0/1/-1, combinations of ready/end events, nullable
output, callback combinations, polled completion, disk error and both timeout
thresholds. It compares consumed events, all response buffers, timeout state,
external calls and MMIO traces. Reading the ready byte in the end-event branch
is rejected as a negative control. Hardware and external functions are modeled;
infinite waits and signed-counter overflow are outside this finite test set.

Validation: the linked function bytes match retail exactly, and
`make -j4 verify-clean` passes all 292 repository tests and the full main
SHA-1 check (`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`).
