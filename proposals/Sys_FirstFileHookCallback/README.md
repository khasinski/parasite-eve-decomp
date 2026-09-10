# Sys_FirstFileHookCallback

New C reconstruction of the 256-byte retail function at 0x80072950. The
previous proposal directory contained target/build artifacts but no candidate
C. Production remains assembly until an exact match is obtained.

This is a Square first-file hook rather than a Psy-Q library routine. The
preceding function at 0x800727B4 copies the device name, saves the matching
kernel device's original first-file callback in D_800A32D0 and installs this
entry in its place. This callback restores the saved entry before forwarding
the request. The production manifest therefore classifies it under `main`.

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
Stock GCC281 with unsplit addresses scores **95.28125%** and produces the
retail function's complete 256-byte size. Reading the kernel table entries as
ordinary memory gives the target's direct zero-based loads at `0x150` and
`0x154`; the previous unjustified volatile qualifiers generated two extra
address-materialization instructions. A local state alias also recovers the
target allocation of that long-lived argument to `$s2` without a register pin.
Remaining differences are prologue scheduling, preservation of the computed
table end across `strcmp`, and the string-address call delay slot. Default
GCC281 scores 86.171875%, GCC272 88.71875%, and GCC281 with its first
scheduling pass disabled 74.859375%.

```sh
tools/scripts/cc.sh proposals/Sys_FirstFileHookCallback/candidate.c /tmp/first-hook.o
tools/objdiff/objdiff-cli diff \
  -1 expected/build/USA/asm/USA/main/main/Sys_FirstFileHookCallback.s.o \
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
