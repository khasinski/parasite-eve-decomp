# Seek completion callback

This full `cdrom/misc9.c` candidate uses an explicit unsigned-byte event
parameter and a direct `CdRom_AsyncCallback` function reference. Event 2
registers that function as the DS synchronization callback. Other events do
nothing. The second callback parameter is unused. `DsRead_IsBusy` remains
in the same translation unit and returns the original busy word.

The source has no global register variable, numeric callback address,
register pin, barrier or instruction assembly. Stock GCC 2.8.1 with `-O1`
produces all original instructions through the final RA load, including the
callback address low half in the DsSyncCallback call delay slot.

The remaining difference is the epilogue:

```text
Target:                 Candidate:
lw ra,16(sp)            lw ra,16(sp)
addiu sp,sp,24          nop
jr ra                   jr ra
nop                     addiu sp,sp,24
```

Both callback bodies are 48 bytes. Upstream objdiff reports 90% for the
callback and 100% for DsRead_IsBusy. This is not a matching production unit.

Stock GCC 2.7.2 restores the expected epilogue but treats the callback
address as a single `la` macro. It emits that address before the call and
leaves a nop in the call slot, scoring 86.666664%. GCC 2.8.1 with address
splitting disabled also fails that slot. Turning either scheduler off,
using `-mdebugd` or `-mdebugf`, or disabling expensive optimizations does
not change the 2.8.1 epilogue. Disabling delayed branches loses the call
slot and also fails the comparison.

Moving the existing volatile global a0 variable to a local pinned variable
is not an improvement: it grows the stack frame from 24 to 32 bytes and
changes the low address operation to ORI. That trial is not retained.
Production remains unchanged; no tool patch or postpass is proposed.

```sh
tools/scripts/cc.sh proposals/CdRom_SeekDoneCallback/candidate.c /tmp/seek-candidate.o
tools/objdiff/objdiff-cli diff -1 expected/build/USA/src/main/cdrom/misc9.c.o -2 /tmp/seek-candidate.o -o /tmp/seek-candidate.json
```
