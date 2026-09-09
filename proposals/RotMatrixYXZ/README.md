# RotMatrixYXZ reconstruction

New ordinary-C reconstruction of the 652-byte retail LIBGTE function at
0x80079754. Previously this proposal directory contained target metadata
without a C candidate. The function reads three signed 16-bit angles,
writes nine rotation halfwords and returns the matrix pointer. Padding and
translation remain unchanged. Inputs are captured before output stores.

The packed retail sine/cosine table uses 12-bit angle wrap and negative-angle
sine inversion. Mixed-axis products retain intermediate Q12 rounding.
The negative terms in m[0][1] and m[2][0] negate the rounded product; moving
negation before the shift changes some results by one.

**Semantic reconstruction, not an exact instruction match.** Stock GCC272
scores 20.349693% (580 candidate bytes versus 652 retail). Default GCC281
scores 13.85276%, and GCC281 with unsplit addresses scores 3.7852762%.
The maintained candidate uses GCC272, with no pins, barriers or instruction
assembly. Register allocation and multiply scheduling differ substantially.
No production ASM replacement or matching-function credit is claimed.

```sh
tools/scripts/cc.sh proposals/RotMatrixYXZ/candidate.c /tmp/rot-yxz.o
python proposals/RotMatrixYXZ/verify_behavior.py /tmp/rot-yxz.o
```

The verifier compares C and the SHA-1-checked retail body with an independent
integer model across 8,798 cases: all triples from 15 boundary angles and
1,024 random signed-halfword triples, each with separate and identical
input/output base addresses. It checks matrix bytes, staged rounding,
translation/padding, input preservation, returned pointer, SP and canaries.
A mutant moving m[0][1] negation before rounding is rejected by assertion.
No external API bodies or GTE operations are modeled. This finite coverage
does not prove exact timing or all partially overlapping input layouts.
