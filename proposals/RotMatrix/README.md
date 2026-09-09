# Three-angle rotation matrix

New scalar-C reconstruction of the 652-byte LIBGTE function at 0x800794C4.
The former directory contained target metadata but no candidate C. The
function consumes three signed 16-bit angles, writes all nine rotation
halfwords, preserves padding/translation and returns the matrix pointer.
All angle inputs are captured before output writes, preserving the tested
case where input starts at the output matrix address.

Each angle uses the packed retail sine/cosine table with 12-bit wrap and
negative-angle sine inversion. Products retain the MIPS low word and use
signed Q12 shifts. The source preserves intermediate Q12 rounding in the
mixed-axis terms; it also negates the two negative product terms before
shifting. Algebraically combining those products or negating an already
rounded result can change low bits. The inline helpers contain ordinary C
only and are inlined by the tested compiler.

**Semantic reconstruction, not a close instruction match:** stock GCC281
unsplit gives 11.325153% (616 candidate bytes versus 652 retail), default
GCC281 7.588957%, and GCC272 10.404908%. Register allocation and multiply
scheduling differ substantially. There are no pins, barriers or instruction
asm, and no production replacement or matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/RotMatrix/candidate.c /tmp/rot-matrix.o
python proposals/RotMatrix/verify_behavior.py /tmp/rot-matrix.o
```

Both C and the SHA-1-checked retail body pass **8,798 cases** against each other
and an independent integer model. They cover all triples from 15 boundary
angles, 1,024 random signed-halfword triples, and both separate and identical
input/output base addresses. Checks include staged Q12 results, all matrix
bytes, preserved translation/padding, separate input preservation, return,
SP and canaries. Replacing negative-before-shift with negative-after-shift is
rejected by assertion. No external API bodies or GTE operations are modeled;
this does not prove exact instruction timing or all partial-overlap layouts.
