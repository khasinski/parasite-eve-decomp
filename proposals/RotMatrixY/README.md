# RotMatrixY scalar reconstruction

New ordinary-C reconstruction of the 408-byte LIBGTE function at 0x80079C74.
The previous proposal directory contained no candidate C. There are no pins,
barriers or instruction asm; the routine uses CPU multiplication, not COP2.

The angle lookup uses the same packed sine/cosine table as RotMatrixZ, but
retail forms the opposite sine sign and rotates rows zero and two. In standard
sine notation, each column becomes x'=(cos*x+sin*z)>>12 and
z'=(cos*z-sin*x)>>12, with low-word arithmetic and halfword output. All six
original row entries are captured before stores. Row one, matrix padding and
translation remain untouched. The original pointer is returned. Unsigned
magnitude computation handles INT_MIN without signed negation overflow.

Stock GCC281 unsplit scores **25.656862%**, default GCC281 22.32353%, and GCC272
15.107843%. This is a semantic reconstruction, not a close instruction match
or a production replacement. Register allocation and multiply scheduling
remain substantially different from the retail leaf body.

```sh
tools/scripts/cc.sh proposals/RotMatrixY/candidate.c /tmp/rotate-y.o
python proposals/RotMatrixY/verify_behavior.py /tmp/rotate-y.o
```

Both bodies pass **9,257 cases** against each other and an independent integer
model using the SHA-1-checked retail table. Cases span the complete positive
and negative angle cycle, wrap boundaries, INT_MIN/MAX, 1,024 random 32-bit
angles, random matrices and uniform extreme halfwords. Checks cover the full
32-byte matrix, preserved data, pointer return, SP and canaries. No external
APIs or GTE operations are modeled. Mutants reversing the negative-angle sign
or rotating the second row instead of the third fail assertions. The model
uses the target's signed right shift and halfword truncation; timing and
linked-byte equivalence are not claimed.
