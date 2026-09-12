# Math_Add64WithShift

Integrated in `src/main/math/Math_Add64WithShift.c`: all **188 linked bytes**
at `0x80072EA4` match retail using stock native GCC 2.8.1 and stock MASPSX.
One output-pointer register binding and one empty memory barrier are recorded
in debt. No CPU instruction assembly or toolchain modifications are used.

Despite the historical name, this is a 64-bit shift helper: direction zero
shifts left; any nonzero direction shifts right with sign extension. Counts
at or below zero copy the value. Counts above 64 continue shifting one bit
at a time. `MathU64` describes the low and high halves; the explicit output
pointer arrives in `a0` and returns in `v0`. The shared declaration and
`Math_DoubleToInt32` caller now use that signature directly, removing its
local assembly-name adapter.

Validation:

- `tools/tests/test_arithmetic_shift_firstfile.py` guards all linked bytes
  with a fixed retail SHA-256 digest.
- `../Sys_FirstFileHookCallback/verify_behavior.py` passes 3,692 shift cases
  against both retail execution and Python signed/unsigned arithmetic:
  boundary and seeded random operands, four direction values, and counts
  from -2 through 128 including the 31/32 and 63/64 boundaries. It also
  checks output canaries, returned pointer, count argument, SP and saved
  registers. Extremely large positive counts are not executed by the test.
- All 315 CI tests pass, and the full EXE preserves retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
