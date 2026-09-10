# LIBCD CD_sync

Promoted to `src/main/psyq/libcd/bios_poll.c`: all 640 bytes at retail
`0x8007B010` match with stock native GCC 2.7.2, GNU assembly and
`-fno-expensive-optimizations`. No register pins, barriers, instruction ASM,
compiler patches or assembly postpasses are used.

CD_sync and CD_ready occupy the contiguous retail range `0x6B810..0x6BD58`
and share timeout, interrupt-dispatch and response-copy helpers in this TU.
The SDK BIOS_1.OBJ exports them at `0x564` and `0x7E4`, with CD_cw next at
`0xAAC`; their instruction words agree with retail outside relocation fields.
This is a matched portion of BIOS_1.OBJ, not a claim that the whole object has
been reconstructed.

This function initializes the timeout state, checks timeout, drains callbacks
when required, then consumes a completion/error status (2 or 5). It resets
that status to 2, copies eight response bytes if requested, and returns the
consumed status. Other statuses continue waiting in mode 0, or return 0 in
nonzero mode. Even an already-completed or nonblocking call performs timeout
and callback handling before examining the event byte.

The poll-count helper preserves the pre-increment value and commits the
incremented counter separately. Together with explicit event-byte views and
the descending eight-byte copy loop, this produces the retail register
allocation and instruction order. The adjacent functions also match when
compiled together. The older local `candidate.c` and shared proposal helper
are retained as historical experiments; they are not the production source.

```sh
tools/scripts/cc.sh src/main/psyq/libcd/bios_poll.c /tmp/bios_poll.o
.venv/bin/python proposals/CD_sync/verify_behavior.py /tmp/bios_poll.o
make verify-clean
```

The behavior verifier requires Unicorn and pyelftools. Its 840 cases compare
retail and candidate MIPS for blocking/nonblocking modes (including negative
mode), five initial statuses, nullable output, callback combinations, polled
completion, disk error and both timeout mechanisms including their thresholds.
It compares status consumption, all result buffers, timeout state, external
calls and MMIO traces. Resetting a consumed status to 5 instead of 2 is rejected
as a negative control. Hardware and external calls are modeled; counter
overflow and infinite waits are outside this finite test set.

Validation: the linked function bytes match retail exactly, and
`make -j4 verify-clean` passes all 292 repository tests and the full main
SHA-1 check (`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`).
