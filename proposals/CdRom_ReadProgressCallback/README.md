# CD read-progress callback

Exact C reconstruction of the 376-byte function at 0x80080F98, using the
recovered `CdReadProgressState` and shared API declarations. The project
categorizes this symbol as main-game; upstream SDK attribution is not
established here.

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

Production now matches all **376/376 linked bytes** using native GCC 2.8.1,
unsplit addressing and unmodified GNU as. GNU as places the shared branch
label after the load-delay nop, resolving the remaining linked jump-target
difference. The candidate has no pins or barriers. The recovered state view
remains anchored at `currentVsync`, preserving the retail field offsets.

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
