# RotMatrixZ scalar reconstruction

New C reconstruction of the 408-byte LIBGTE function at 0x80079E14. The
previous directory had target metadata but no candidate source. This function
uses ordinary CPU multiplication and a packed sine/cosine table; no COP2/GTE
operations are needed. The candidate has no pins, barriers or instruction asm.

The packed word table at 0x800966EC stores signed sine in the low halfword and
signed cosine in the high halfword. The angle selects a 12-bit index; negative
angles negate the selected sine. Unsigned magnitude calculation also handles
INT_MIN without signed negation overflow. The six original entries from the
first two matrix rows are loaded before writing their Q12 rotation about Z.
The third row, padding and translation are preserved, and the input matrix
pointer is returned. The local 32-byte matrix type has a size assertion.
Arithmetic uses unsigned products/sums to retain low-word MIPS behavior, then
the stock compiler's signed conversion/arithmetic shift for the Q12 result.

This is currently a **semantic reconstruction, not a close instruction
match**. Stock GCC281 unsplit scores 22.539215%; default GCC281 19.215687%,
GCC272 18.666666%. Disabling expensive optimizations does not improve default
GCC281. The compiler's allocation and scheduling differ substantially from
the leaf routine's explicit temporary registers and multiply latency NOPs.
No instruction-matching or production-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/RotMatrixZ/candidate.c /tmp/rotate-z.o
python proposals/RotMatrixZ/verify_behavior.py /tmp/rotate-z.o
```

Both final C and the SHA-1-checked retail body pass **9,257 cases** against each
other and an independent integer model using the retail table. Cases cover
-4095..4095, wrap boundaries, INT_MIN/MAX, 1,024 deterministic random 32-bit
angles, random signed matrix entries and uniform extreme matrices. Checks
include all 32 output bytes, return pointer, SP and surrounding canaries.
There are no modeled external API bodies or GTE instructions in this test.
Removing negative-angle sine inversion or shifting by 11 instead of 12 fails
assertions. These finite checks do not establish matching instruction timing.
