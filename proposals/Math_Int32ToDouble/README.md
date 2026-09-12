# Math_Int32ToDouble

Integrated in `src/main/math/Math_Int32ToDouble.c`: all **256 linked bytes**
at `0x80073454` match retail. Stock native GCC 2.8.1 and stock MASPSX produce
the match without instruction assembly or toolchain patches.

The function normalizes an integer magnitude, shifts the 64-bit intermediate
through `Math_Shift64`, clears the implicit significand bit and packs the sign
and exponent into an IEEE-754 double. Unsigned shifts preserve MIPS word
operations. Selected volatile accesses preserve the original temporary loads
and stores. Four register bindings and three empty barriers are recorded in
debt, including a binding to architectural `$zero` on the zero-input path.

The retail code loops indefinitely for `INT_MIN`: negation leaves the high
bit set, the signed comparison enters normalization, and the first shift
turns the value into zero. This reconstruction preserves that path. The
behavioral test verifies a bounded execution remains in the same loop; it
does not treat `INT_MIN` as a successful numeric conversion.

Validation:

- `tools/tests/test_integer_math.py` checks every linked text byte against
  its fixed retail SHA-256, without requiring retail assets in CI.
- `../Math_Mul32To64/verify_behavior.py` covers 2,230 conversion cases:
  zero, signed limits, powers of two and adjacent values, seeded random
  inputs, and the bounded `INT_MIN` path. Every returning result is also
  compared bit-for-bit with Python's double conversion.
- The original `Math_Shift64` body runs unchanged in both executions.
  Helper arguments, SP, saved registers and output canaries are checked.
- The complete EXE retains SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.
