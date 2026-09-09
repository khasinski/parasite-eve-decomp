# LIBCD CD_initintr

Ordinary C reconstruction of the 76-byte routine at `0x8007BBB0`. It clears
the ready/sync callbacks and both full status words, resets callback handling,
and registers the CD dispatcher on interrupt channel 2. It uses no register
pin, barrier, synthetic page, or instruction assembly.

The common stock GCC 2.8.1 unsplit/no-expensive-optimizations configuration
scores 84.47369% against the current expected object (which retains legacy
status-symbol aliases). Remaining instruction differences are the last status
store moving out of ResetCallback's delay slot and stack restoration moving
into the return delay slot. The candidate is 80 bytes rather than 76.

The legacy production bios.c remains intact except for replacing the obsolete
g_CdResultByte declaration/use with the shared u32 D_8009AFC8 declaration.
getintr and CD_initintr now agree on this full-word CD_status1 storage. Its
retail word load/store evidence is in getintr; using a byte declaration here
would leave the upper bytes uncleared.

This candidate is included after the clean CD_initvol in the eight-function
combined source. CD_initvol remains at 100%. Attempts to select common
scheduling flags for the existing production pair did not preserve both
matches, so production is not split or replaced by a partially matching TU.

```sh
tools/scripts/cc.sh proposals/CD_initintr/candidate.c /tmp/CD_initintr.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/src/main/psyq/libcd/bios.c.o \
  -2 /tmp/CD_initintr.o -o /tmp/CD_initintr.json
python proposals/CD_initintr/verify_behavior.py /tmp/CD_initintr.o
```

The verifier requires Unicorn and pyelftools. Its 24 cases compare retail
and candidate for initial zero/nonzero/full-bit-pattern words, callback API
return values and state changes made by ResetCallback. It checks that all four
words are cleared before ResetCallback, installation uses channel 2 and the
correct dispatcher, calls remain ordered, and the stack is restored. Replacing
the status1 word store with a byte store is rejected. Callback APIs are modeled.
