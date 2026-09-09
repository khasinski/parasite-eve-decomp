# Gte_Atan2 C reconstruction

`candidate.c` reconstructs the integer angle calculation at 0x80079FB4.
It records coordinate signs, chooses a ratio no greater than one, indexes
the signed 16-bit angle table, then applies quarter-turn, half-turn and
sign corrections. When scaling the smaller coordinate by 1024 could
cross the signed range, it instead shifts the denominator down by ten.
The original 0x7FE00000 test and truncating signed divisions are preserved.
Unsigned intermediate negation/left shifts retain the MIPS bit operations
without relying on signed overflow for those expressions.

Stock GCC 2.7.2 with the pinned MASPSX `--expand-div` option gives
**93.548386%** against the 372-byte retail function. Division expansion is
necessary to retain the original divide-by-zero/overflow checks and breaks.
No pins, barriers, instruction ASM or toolchain changes are used. Remaining
mismatches include the zero-vector return path, indexed loads through AT
instead of v1 and instruction spacing. GCC281 unsplit gives 91.129036%;
GCC281 split gives 85.32258%. Production remains ASM.

`verify_behavior.py` runs retail and candidate MIPS with the real table.
It compares 2489 pairs: Cartesian products of axis, scaling-boundary and
large signed coordinates plus 2048 deterministic random pairs. Five explicit
axis results are also checked. A negative control changing the half-turn
correction from 2048 to 2047 is rejected.

The verified input domain excludes INT_MIN. Its magnitude cannot be
represented as a positive signed 32-bit value; the original wrapped negation
can subsequently reach exceptional division or out-of-range table accesses.
This reconstruction does not introduce a new mathematical result for those
inputs, and the finite test suite makes no equivalence claim for them.

```sh
tools/scripts/cc.sh proposals/Gte_Atan2/candidate.c /tmp/Gte_Atan2.o
python3 proposals/Gte_Atan2/verify_behavior.py /tmp/Gte_Atan2.o
```

The verifier requires Unicorn, pyelftools and MIPS binutils. The authoritative
comparison target is `expected/build/USA/asm/USA/main/psyq/libgte/Gte_Atan2.s.o`.
