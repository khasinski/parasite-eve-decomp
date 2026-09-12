# Sys_FirstFileHookCallback

Integrated in `src/main/main/Sys_FirstFileHookCallback.c`: all **256 linked
bytes** at `0x80072950` match retail. The compiler is stock native GCC 2.8.1
with `-mno-split-addresses`, using the existing GNU assembler path. Two
register bindings and three empty barriers are recorded in debt. No CPU
instruction assembly or toolchain changes are used. The older `candidate.c`
is retained as the unmatched, unconstrained reconstruction.

This is a Square hook, classified under `main`, rather than a Psy-Q routine.
The shared `include/pe1/bios_firstfile.h` describes an 80-byte BIOS device
entry: name pointer at offset zero, first-file callback at offset `0x34`,
and opaque bytes elsewhere. The table pointer comes from `0x150`; `0x154`
contains its byte count. Only complete entries are scanned.

The function initializes a zero state word to one and restores the first
matching device's callback. Restoration uses the callback captured before
`strcmp`; forwarding reloads the global callback afterward. All three
arguments and the callback's return value are preserved.

Validation:

- `tools/tests/test_arithmetic_shift_firstfile.py` checks all linked bytes
  against a fixed retail digest, without requiring retail assets in CI.
- `verify_behavior.py` covers 504 hook cases: empty tables, null names,
  absent and duplicate matches, partial final entries, four initial states,
  three return values, and callback mutation during `strcmp`. It checks
  record bytes, output canaries, comparison order, first-match restoration,
  the reloaded callback, forwarded arguments, return value, SP and saved
  registers. External APIs are modeled with emulator hooks; malformed
  pointers and overflowing table extents are outside this test's scope.
- The combined oracle also covers 3,692 arithmetic shift cases.
- All 315 CI tests pass; the complete EXE retains retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

Run from the repository root:

```sh
.venv/bin/python proposals/Sys_FirstFileHookCallback/verify_behavior.py \
  build/USA/src/main/math/Math_Add64WithShift.c.o \
  build/USA/src/main/main/Sys_FirstFileHookCallback.c.o
```
