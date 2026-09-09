# RotMatrixZYX reconstruction

New ordinary-C source for the 652-byte retail function at 0x800799E4.
Previously this proposal directory contained target metadata without a C candidate.
It reads three signed 16-bit angles, uses the packed retail sine/cosine table,
and writes nine rotation halfwords while preserving padding and translation.
Mixed-axis products preserve the original intermediate Q12 rounding.

**Semantic reconstruction, not an exact match:** stock GCC281 with unsplit
addresses scores 11.662577% against the expected object. Register allocation
and instruction scheduling still differ substantially. No pins, barriers or
instruction assembly are used. This does not replace the production ASM or
increase the matching-function count.

```sh
tools/scripts/cc.sh proposals/RotMatrixZYX/candidate.c /tmp/rot-zyx.o
python proposals/RotMatrixZYX/verify_behavior.py /tmp/rot-zyx.o
```

The C and SHA-1-checked retail body passed 8,798 cases against each other and
an independent integer model: all triples from 15 boundary angles plus 1,024
random signed-halfword triples, each with separate and identical input/output
base addresses. Checks cover staged rounding, matrix bytes, preserved
translation/padding, input preservation, returned pointer, SP and canaries.
A mutant reversing the sign of m[2][0] is rejected by assertion.
No external APIs or GTE operations are modeled. These finite tests do not
establish exact instruction timing or all partial-overlap layouts.
