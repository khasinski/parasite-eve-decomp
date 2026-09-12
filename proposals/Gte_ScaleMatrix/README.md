# Gte_ScaleMatrix

Production matches **312/312 linked bytes** at `0x80078CC4`, using native
GCC 2.7.2 and unmodified MASPSX with the default flags. CPU multiplication,
packing, shifts and stores are C; no instruction asm or postpasses are used.

The three scale values multiply columns of the `GteMatrix` rotation block.
All scale inputs are read before output writes. Each product is truncated
to its low 32 bits before the signed Q12 shift, retaining retail overflow
behavior. The last store writes a full word, including the matrix padding;
translation remains unchanged.

Seven register pins and 42 empty barriers are recorded in debt. Nine of
those barriers bind a C unsigned 64-bit product in HI/LO (`x`) to its low
word in LO (`=l`). This retains MULTU while extracting only the low word;
it emits no instruction itself. Other barriers retain packed-word access
widths and the original mask/shift order. Six redundant barriers were removed.

`tools/tests/test_scale_long_vector.py` checks the SHA-256 of every linked
text byte at the retail address. The full main executable also retains its
retail SHA-1.

```sh
tools/scripts/cc.sh src/main/gte/Gte_ScaleMatrix.c /tmp/scale-matrix.o
.venv/bin/python proposals/Gte_ScaleMatrix/verify_behavior.py /tmp/scale-matrix.o
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
