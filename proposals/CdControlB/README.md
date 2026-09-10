# LIBCD CdControlB

`candidate.c` is a new complete C reconstruction of the 332-byte retail
function at `0x8007A740`. It temporarily suppresses the completion callback,
issues command 1 when the drive reports the shell-open bit, optionally sends
command 2 for commands whose table entry requires parameters, and retries the
requested command up to four times. On successful submission it calls
`CD_sync(0, result)` and returns one only for completion event 2. Failure
restores the callback and returns zero.

Stock GCC 2.8.1 with `-mno-split-addresses -fno-schedule-insns` emits 328 bytes
and scores **86.75903%** in objdiff. The candidate contains no register pins,
barriers, or instruction assembly. Assigning the shared command result on both
the optional command-2 path and the final command path is necessary: it keeps
all retry exits defined and raises the initial reconstruction from 85.13253%.

The target's register layout closely follows adjacent `CdControl`, but keeps
constant one in `$fp`, the command byte in `$s3`, the command in `$s4`, the
saved callback in `$s5`, the table slot in `$s6`, and minus one in `$s7`.
Remaining work is to recover the source lifetime that produces that allocation
and the common post-loop `$v0` path without constraints.

Reproduce the comparison with:

```sh
tools/scripts/cc.sh proposals/CdControlB/candidate.c /tmp/CdControlB.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/libcd/CdControlB.s.o \
  -2 /tmp/CdControlB.o CdControlB -o /tmp/CdControlB.json
```

Production remains the retail assembly until the function matches exactly.
