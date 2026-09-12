# Math_Mul32To64

Integrated in `src/main/math/Math_Mul32To64.c`: all **236 linked bytes**
at `0x800738D8` match retail using stock native GCC 2.8.1 and stock MASPSX.
The C prototype explicitly describes the output buffer in `a0` and the
returned pointer in `v0`, alongside the two unsigned operands in `a1`/`a2`.

Four 16-bit partial products form the 64-bit result. The two cross terms
are accumulated through the shared `Math_Add64` helper. Four register pins
and six empty barriers preserve register allocation, temporary accesses and
multiply scheduling; all are recorded in debt. There is no CPU instruction
assembly or compiler/assembler patch.

Validation:

- `tools/tests/test_integer_math.py` guards the entire linked text against
  its fixed retail digest, without requiring retail assets in CI.
- `verify_behavior.py` passes 3,216 multiplication cases: cross-products of
  boundary values, every pair of single-bit operands and seeded random pairs.
  Results also match Python's full-width unsigned multiplication. The original
  `Math_Add64` body executes unchanged. Helper arguments, output bounds,
  returned pointer, SP and saved registers are checked.
- Together with `Math_Int32ToDouble`, the oracle covers 5,446 cases, including
  bounded verification of the conversion helper's nonreturning `INT_MIN` path.
- The complete EXE retains retail SHA-1
  `452fb033f2eaa4b18aa20a5bca60b8125af3a37b`.

Run from the repository root:

```sh
.venv/bin/python proposals/Math_Mul32To64/verify_behavior.py \
  build/USA/src/main/math/Math_Int32ToDouble.c.o \
  build/USA/src/main/math/Math_Mul32To64.c.o
```
