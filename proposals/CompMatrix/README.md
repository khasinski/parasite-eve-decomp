# CompMatrix CPU-side reconstruction

New C reconstruction of the 352-byte retail function at 0x800787D4.
It loads the left rotation into GTE control registers, transforms the three
right rotation columns, then transforms the low signed halfwords of the
right translation. The last operation reads MAC1/2/3, not saturated IR1/2/3,
and adds the full signed 32-bit left translation.

Output stores are staged as in retail: rotation words at offsets 0, 12 and
16 are written before reading the right translation. Words at offsets 4 and
8 follow the fourth GTE command and precede reads of the left translation.
All three left translation components are read before any translation store.
This ordering matters for partial overlaps. The last rotation word overwrites
padding with the high half of the signed IR3 result.

CPU indexing, packing, addition and stores are C. Only hardware transfers,
commands and hazard NOPs remain behind the pe1/gte.h interface plus an MTC2
register-1 transfer. No pins, empty barriers, CPU algorithm instruction
assembly or postpasses are added.

**Not an instruction match:** stock GCC272 and GCC281 each produce 420 bytes
versus 352 retail and objdiff reports 0.0%. The candidate uses GCC272.
Register allocation, access widths, packing, scheduling and hazard spacing
still differ. Production ASM is unchanged; no matching-function credit is
claimed.

```sh
tools/scripts/cc.sh proposals/CompMatrix/candidate.c /tmp/comp-matrix.o
python proposals/CompMatrix/verify_behavior.py /tmp/comp-matrix.o
```

**CPU-side/interface verification only:** the verifier executes the retail
and C CPU instructions while replacing COP2 operations with NOPs and handling
their effects through code hooks. It supplies scripted IR/MAC results rather
than implementing GTE arithmetic, flags or timing. VZ and rotation register 4
are normalized to signed low halfwords. The last modeled IR values are clipped
from the scripted MAC values; this is a test input generator, not a complete
GTE implementation.

All **12,303 cases** pass: 343 boundary IR-result combinations and 1,024 random
matrix/result cases, each in nine separate/equal/partially overlapping memory
layouts. MAC inputs include zero, unit, signed-halfword boundaries, signed
32-bit extrema and random magnitudes. Their signs are selected to keep all
three final signed additions representable. Assertions check rotation control
writes, four input vectors, read order, the complete memory arena before each
command and at return, translation sums, final modeled registers, padding,
return pointer, SP and canaries. Mutants using the wrong translation source,
truncating the left translation or advancing a rotation store fail assertions.

**Unresolved overflow behavior:** retail uses trapping MIPS ADD instructions
for the final sums. The C uses signed addition and has undefined behavior on
signed overflow; these tests deliberately do not exercise that domain.
Overflow exceptions, full GTE hardware equivalence, timing, access widths,
concurrent observers and arbitrary memory layouts remain unverified.
