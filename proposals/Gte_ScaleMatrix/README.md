# Gte_ScaleMatrix reconstruction

Ordinary-C reconstruction of the 312-byte retail function at 0x80078CC4.
The production unit remains ASM. A historical source under local/postpassed
requires an instruction-rewriting postpass, register pins and barriers; this
maintained candidate uses none of those mechanisms and returns the matrix
pointer as the original does.

The three signed 32-bit scale components multiply columns of the signed
16-bit rotation matrix. Each multiply retains its low 32 bits before an
arithmetic shift by 12. All three scale inputs are loaded before matrix
writes, including when the scale array overlaps the matrix.

The original final store is a full word: it writes m[2][2] and overwrites the
two padding bytes with the high half of the shifted 32-bit result. The C
preserves that observable behavior explicitly. Translation is preserved.
Using a full 64-bit product changes the padding for overflowing products;
simply preserving padding also differs from retail.

**Semantic reconstruction, not an exact instruction match:** stock GCC272,
default GCC281 and GCC281 with unsplit addresses all score 21.346153%
(240 candidate bytes versus 312 retail). The candidate uses GCC272.
Retail packs pairs into word stores; the typed C uses halfword accesses.
No production replacement or matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/Gte_ScaleMatrix/candidate.c /tmp/scale-matrix.o
python proposals/Gte_ScaleMatrix/verify_behavior.py /tmp/scale-matrix.o
```

The C and SHA-1-checked retail body pass **19,852 cases** against each other
and an independent integer model. Inputs include all triples from 12 scale
boundary values, 1,024 random scale/matrix pairs and 84 uniform-coefficient
cases. Each runs with separate scale storage and six word-aligned overlaps
inside the matrix. Checks cover all matrix bytes, translation preservation,
padding overwrite, separate scale preservation, return pointer, SP and
canaries. Mutants preserving padding or using a full 64-bit product are
rejected by assertion. No external API or GTE operations are modeled.
These tests do not prove identical instruction timing, access widths,
concurrent observers or every possible memory overlap.
