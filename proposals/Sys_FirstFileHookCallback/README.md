# Sys_FirstFileHookCallback

New C reconstruction of the 256-byte retail function at 0x80072950. The
previous proposal directory contained target/build artifacts but no candidate
C. Production remains assembly until an exact match is obtained.

The function initializes a zero state word to one, reads the table byte count
from address 0x154 and its base pointer from 0x150, and scans complete 80-byte
records. Each record has a name pointer at offset zero and a first-file
callback at offset 0x34; the other bytes are left opaque. The recovered
`FirstFileDevice` includes a PSX size assertion. The first nonnull name equal
to D_800A32D8 has its callback replaced with the previously captured
D_800A32D0 value. The function then reloads D_800A32D0 and calls it with all
three original arguments, forwarding its return value. The field name follows
the existing hook name; no additional opaque record fields are inferred.

The candidate uses ordinary C with no pins, barriers or instruction asm.
Stock GCC281 with unsplit addresses scores **90.5625%**, producing 264 bytes
against retail's 256. Default GCC281 scores 86.171875%, GCC272 88.71875%, and
GCC281 with its first scheduling pass disabled 74.859375%. Disabling expensive
optimizations in the unsplit profile does not improve its score.

```sh
tools/scripts/cc.sh proposals/Sys_FirstFileHookCallback/candidate.c /tmp/first-hook.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/psyq/Sys_FirstFileHookCallback.s.o \
  -2 /tmp/first-hook.o -o /tmp/first-hook.json
python proposals/Sys_FirstFileHookCallback/verify_behavior.py /tmp/first-hook.o
```

The SHA-1-checked retail oracle passes 108 cases, crossing three initial state
values, six table byte counts (including partial records), three name layouts
and optional callback mutation during strcmp. It checks all record bytes,
state initialization, comparison order, first-match selection, the captured
callback stored in the record, reloaded callback invocation, forwarded
arguments, returned value and SP restoration. External strcmp and callbacks
are modeled through NOP entry stubs; their bodies are not executed. Removing
the first-match break or calling the captured callback instead of reloading
the global is rejected by assertions. Tests cover valid mapped tables; they
do not establish behavior for malformed pointers or overflowing table extents.
