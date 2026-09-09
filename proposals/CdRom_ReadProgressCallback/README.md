# CD read-progress callback

New ordinary-C reconstruction of the 376-byte function at 0x80080F98. Its
previous proposal directory contained target/build artifacts but no candidate
C. It uses the recovered `CdReadProgressState` and shared API declarations.
The current project categorizes this symbol as main-game; no upstream SDK
object attribution or progress-category change is made here.

Every invocation records VSync(-1). With flag bit zero set, positive remaining
sectors start CdRom_IsBusy2 and save the incoming data word; nonpositive counts
run cleanup and notify the completion callback, preserving the low byte of
the incoming event unless the count is negative (event five). With that flag
clear, positive counts execute CdRom_IsBusy, advance the destination by four
times the sector size and decrement the count. A clock strictly greater than
start+1200 marks failure. Zero remaining sectors or a second expired clock
check causes cleanup and event two/five notification. State and callback are
read again after cleanup, so its changes are honored. The third argument in
the existing callback ABI is unused by retail.

Stock GCC281 unsplit addressing scores **99.07447%**; default addressing gives
94.606384% and GCC272 70.5%. Disabling expensive optimizations in the default
profile does not improve it. There are no pins, barriers or instruction asm.
This is not an exact match: differences include prologue ordering, one jump
location and the structure base/field displacements. Objdiff's percentage does
not establish linked-byte equivalence, and production remains assembly.

```sh
tools/scripts/cc.sh proposals/CdRom_ReadProgressCallback/candidate.c /tmp/cd-progress.o
python proposals/CdRom_ReadProgressCallback/verify_behavior.py /tmp/cd-progress.o
```

The SHA-1-checked retail oracle passes 384 cases crossing both flag branches,
four sector counts, three incoming events (including 0x102), four clock
sequences, present/absent completion callbacks, and cleanup that optionally
changes the count and callback. It compares state at each external API call,
API arguments/order and final state. Clock cases include exactly start+1200
and crossing the boundary between the two queries. Deadline >=, a wrong
buffer increment and a wrong asynchronous failure event are rejected by
assertions. VSync, transfer helpers, cleanup and completion callbacks are
modeled with NOP API entry stubs; their bodies are not executed. Tests use
representable signed time/address arithmetic and do not prove behavior across
signed overflow or physical CD hardware timing.
