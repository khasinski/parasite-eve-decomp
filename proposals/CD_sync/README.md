# LIBCD CD_sync

Complete C reconstruction of retail `0x8007B010` (640 bytes). Stock GCC 2.8.1
with unsplit addresses scores 88.58125%, without pins, barriers or instruction
ASM. Production remains assembly pending a byte-exact match.

This function initializes the timeout state, checks timeout, drains callbacks
when required, then consumes a completion/error status (2 or 5). It resets
that status to 2, copies eight response bytes if requested, and returns the
consumed status. Other statuses continue waiting in mode 0, or return 0 in
nonzero mode. Even an already-completed or nonblocking call performs timeout
and callback handling before examining the event byte.

`../libcd_bios_helpers.h` shares the reconstructed timeout and response-copy
implementations with CD_cw. Initializing the diagnostic table pointers after
the initial VSync call improves match from 82.2875% to 88.58125%. A barrier
keeping constant 2 alive worsens match to 85.3875% and was rejected. Remaining
differences concern register allocation, status masking and call scheduling.

The SDK BIOS_1.OBJ export range 0x564–0x7E4 matches all 640 retail bytes except
52 relocation fields; the other 108 words are identical. This proves its
LIBCD membership, not whole-object equivalence. Its manifest is still under
main/main pending the remaining driver classification work.

```sh
tools/scripts/cc.sh proposals/CD_sync/candidate.c /tmp/CD_sync.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/main/CD_sync.s.o \
  -2 /tmp/CD_sync.o -o /tmp/CD_sync.json
python proposals/CD_sync/verify_behavior.py /tmp/CD_sync.o
python proposals/CD_cw/verify_sdk.py /path/to/BIOS_1.OBJ CD_sync
```

The behavior verifier requires Unicorn and pyelftools. Its 840 cases compare
retail and candidate MIPS for blocking/nonblocking modes (including negative
mode), five initial statuses, nullable output, callback combinations, polled
completion, disk error and both timeout mechanisms including their thresholds.
It compares status consumption, all result buffers, timeout state, external
calls and MMIO traces. Resetting a consumed status to 5 instead of 2 is rejected
as a negative control. Hardware and external calls are modeled; counter
overflow and infinite waits are outside this finite test set.
