# All retail RotMatrix variants

Five full linked byte matches replace the remaining named RotMatrix ASM units:

| Symbol | PsyQ 4.3 LIBGTE object | Address | Bytes |
|---|---|---|---:|
| RotMatrix | FGO_01.OBJ | 0x800794C4 | 652 |
| RotMatrixYXZ | FGO_02.OBJ | 0x80079754 | 652 |
| RotMatrixZYX | FGO_03.OBJ | 0x800799E4 | 652 |
| RotMatrixY | FGO_05.OBJ | 0x80079C74 | 408 |
| RotMatrixZ | FGO_06.OBJ | 0x80079E14 | 408 |

Total: **2772 bytes**, excluding inter-object padding. RotMatrixX and the other
SDK variants are not named units in this executable. Existing MulRotMatrix and
SetRotMatrix are unchanged. SDK identities and nonrelocation signature checks
are recorded in provenance.json; the regression checks every linked text byte,
including relocation operands, against the retail SHA-256.

All arithmetic is C compiled by unmodified native GCC 2.7.2 and GNU as through
the existing ASSEMBLER: GNU route. No compiler, assembler or maspsx changes,
ABI flags, instruction rewrites, or arithmetic ASM are used. Matrix accesses
use the shared 32-byte GteMatrix layout; angle vectors use GteShortVector.
Packed sine/cosine lookup, 32-bit wrap, and each intermediate signed Q12 shift
preserve the original rounding order. Y/Z modify two rows in place, leaving
the remaining row, padding and translation untouched.

## Authorized matching debt

The user explicitly authorized **only NOP** as an exception to the CPU
instruction ASM prohibition. include/pe1/psyq_nop.h wraps each instruction in
one macro; operand dependencies preserve the intended scheduling slot. The
nop_barriers debt counter counts individual source invocations, including
alternative branch paths, rather than unique or dynamically executed slots.
Raw CPU instruction ASM remains excluded by the source classifier.

Register pins and empty operand/clobber barriers remain recorded separately.
Unsigned wide C multiplication plus the `x` constraint retains the HI/LO pair;
an empty `=l` output exposes its LO word, without an ASM arithmetic operation.
The angle-negation branch uses empty register clobbers to reproduce the retail
BGEZ with ANDI in its delay slot. Only INT_MIN remains negative after unsigned
negation; its unsigned shift by two is zero, the same table offset as masking
it by 4095. Short-vector inputs cannot reach that case. Tests include INT_MIN
for the single-angle variants.

Validation commands:

```sh
.venv/bin/python -m unittest tools.tests.test_psyq_rotation_matrix tools.tests.test_crutch_debt
make -j8 check
make ci
make report
```

The existing proposals/RotMatrix*/verify_behavior.py programs independently
compare the final compiled C objects and retail code against integer models.
They cover boundary/random angles, complete matrix output, preserved fields,
return values, stack and canaries; three-angle tests include identical input
and output addresses. The older candidate.c files in those directories remain
historical semantic-only reconstructions; production sources are in
src/main/psyq/libgte/RotMatrix*.c.

Final source debt (after two byte-checked removal passes):

| Function | Pins | Empty barriers | NOP macros | Gotos |
|---|---:|---:|---:|---:|
| RotMatrix | 10 | 71 | 34 | 6 |
| RotMatrixY | 12 | 41 | 18 | 0 |
| RotMatrixYXZ | 10 | 70 | 34 | 6 |
| RotMatrixZ | 12 | 42 | 18 | 0 |
| RotMatrixZYX | 10 | 70 | 36 | 6 |

Validation result: the complete rebuilt EXE has retail SHA-1
`452fb033f2eaa4b18aa20a5bca60b8125af3a37b`. All five final objects pass the
independent behavior checks: 3 × 8798 + 2 × 9257 = **44,908 cases**. The Y/Z
checks explicitly include INT_MIN and INT_MAX. Full linked-byte regression
also passes for each function.

`make ci` passes all **331 tests**, source policy and debt/organization checks.

`make report` and its audit pass. Each of these five SDK-named FGO units is
100% byte-matched in main-psyq. That category now reports 445/516 matched
functions and 72,172/78,296 matched code bytes (92.1784%). This includes the
existing, separately authorized BIOS trampoline credit; this change adds
only the five C functions and 2772 bytes above.
